#pragma once
#include "WwizardWwiseClient.h"
#include <windows.h>
#include <atlstr.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <mutex>

using namespace AK::WwiseAuthoringAPI;
typedef std::basic_string<TCHAR> tstring;

#if defined( AK_ENABLE_ASSERTS )

void SampleAssertHook(const char* in_pszExpression, const char* in_pszFileName, int in_lineNumber)
{}

AkAssertHook g_pAssertHook = SampleAssertHook;

#endif

WwizardWwiseClient::~WwizardWwiseClient()
{
    SaveCurrentProject();
    if (wwiseClient.IsConnected())
    {
        std::cout << "Disconnect from Wwise Instance" << std::endl;
        wwiseClient.Disconnect();
        KillWwiseConsole();
    }
    std::cout << "End Wwizard Wwise Client" << std::endl;
}

bool WwizardWwiseClient::Connect(const std::unique_ptr<SettingHandler>& settings)
{
    if (wwiseClient.IsConnected())
    {
        wwiseClient.Disconnect();
    }

    if (wwiseClient.Connect(settings->GetWaapiIP().c_str(),settings->GetWaaapiPort()))
    {
        connectionSuffixInfo = " with Wwise Instance";
        SubscribeToSelectionChanged();
        SubscribeToPreDeleted();
        return true;
    }
    else if (ForceOpenWwiseInstance(settings))
    {
        connectionSuffixInfo = " with WwiseConsole";
        return true;
    }
    else
    {
        std::cout << "Failed to Connect to Wwise Instance!" << std::endl;
        return false;
    }
}

void WwizardWwiseClient::SubscribeToSelectionChanged()
{
    AkJson waapiOption(AkJson::Map{ { "return", AkJson::Array{ AkVariant("name"), AkVariant("id")}}});
    AkJson waapiResult;
    uint64_t i = 0;


    auto selectionChanged = [this](uint64_t, const JsonProvider& in_rJson)
    {
        auto result = in_rJson.GetAkJson();
        currentSelectedName = result["objects"].GetArray()[0]["name"].GetVariant().GetString();
        currentSelectedGuid = result["objects"].GetArray()[0]["id"].GetVariant().GetString();
    };

    wwiseClient.Subscribe("ak.wwise.ui.selectionChanged", waapiOption, selectionChanged, i, waapiResult);
}

void WwizardWwiseClient::SubscribeToPreDeleted()
{
    AkJson waapiOption(AkJson::Map{ { "return", AkJson::Array{ AkVariant("name"), AkVariant("id")}} });
    AkJson waapiResult;
    uint64_t i = 0;


    auto preDeletedObject = [this](uint64_t, const JsonProvider& in_rJson)
    {
        auto result = in_rJson.GetAkJson();
        auto a = result["object"]["id"].GetVariant().GetString();
        if (currentSelectedGuid ==a)
        {
            currentSelectedName = "";
            currentSelectedGuid = "";
        }
    };

    wwiseClient.Subscribe("ak.wwise.core.object.preDeleted", waapiOption, preDeletedObject, i, waapiResult);
}

bool WwizardWwiseClient::ForceOpenWwiseInstance(const std::unique_ptr<SettingHandler>& settings)
{
    std::string commandline = settings->GetSDKPathHyphen() + " waapi-server " + settings->GetWwiseProjectPathRefHyphen() + " --allow-migration --wamp-port " + std::to_string(settings->GetWaaapiPort());
    
    std::vector<TCHAR> buffer(commandline.begin(), commandline.end());
    buffer.push_back(_T('\0'));
    TCHAR* p = &buffer[0];

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, p, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return false;
    }
    
    WaitForSingleObject(pi.hProcess, 2000);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (wwiseClient.Connect(settings->GetWaapiIP().c_str(), settings->GetWaaapiPort()))
    {
        return true;
    }
    else
        return false;
}

const AkJson WwizardWwiseClient::GetChildrenFromGuid(const std::string& guid, const std::vector<std::string>& optionList)
{
    AkJson waapiArg(AkJson::Map{
    {
        {"from", AkJson::Map{{ "id", AkJson::Array{ AkVariant(guid) }}}},
        {"transform", AkJson::Array{ AkJson::Map {{"select", AkJson::Array{ AkVariant("children")}}}}}
    } });

    AkJson waapiOption = ConvertVectorToAkJsonOption(optionList);

    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::core::object::get, waapiArg, waapiOption, waapiResult, 500);

    return waapiResult;
}

const AkJson WwizardWwiseClient::GetSelectedObjectsInWwise(const std::vector<std::string>& optionList)
{
    AkJson waapiArg(AkJson::Map{ });
    AkJson waapiOption = ConvertVectorToAkJsonOption(optionList);
    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::ui::getSelectedObjects, waapiArg, waapiOption, waapiResult, 500);
    return waapiResult;
}

const AkJson WwizardWwiseClient::GetObjectFromPath(const std::string& path, std::vector<std::string>& optionList)
{
    AkJson waapiArg(AkJson::Map{
        { "from", AkJson::Map{
            { "path", AkJson::Array{ AkVariant(path) } } } }
        });

    AkJson waapiOption = ConvertVectorToAkJsonOption(optionList);
    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::core::object::get, waapiArg, waapiOption, waapiResult, 500);

    return waapiResult;
}

const AkJson WwizardWwiseClient::RunQueryFromGuuid(const std::string& guid)
{
    AkJson waapiArg(AkJson::Map{
       { "from", AkJson::Map{{ "query", AkJson::Array{AkVariant(guid)}}}}});

    AkJson waapiOption(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path"), AkVariant("color")}}});

    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::core::object::get, waapiArg, waapiOption, waapiResult, 500);

    return waapiResult;
}

const AkJson WwizardWwiseClient::RunCustomQuery(const AkJson arg)
{
    AkJson queryResult;
    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path"), AkVariant("color")}}});

    wwiseClient.Call(ak::wwise::core::object::get, arg, options, queryResult, 500);

    return queryResult;
}

const AkJson WwizardWwiseClient::GetProjectInfo()
{
    AkJson waapiArg(AkJson::Map{});
    AkJson waapiOption(AkJson::Map{});
    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::core::getInfo, waapiArg, waapiOption, waapiResult, 500);

    return waapiResult;
}

void WwizardWwiseClient::FocusObjectInWwise(const std::string& guid)
{
    AkJson waapiHierarchyArg(AkJson::Map{ {{"command", AkVariant("FindInProjectExplorerSyncGroup1")}, {"objects", AkJson::Array{AkVariant(guid)}}} });
    AkJson waapiPropertyArg(AkJson::Map{ {{"command", AkVariant("Inspect")}, {"objects", AkJson::Array{AkVariant(guid)}}} });

    AkJson waapiOption(AkJson::Map{ });
    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::ui::commands::execute, waapiPropertyArg, waapiOption, waapiResult, 500);
    wwiseClient.Call(ak::wwise::ui::commands::execute, waapiHierarchyArg, waapiOption, waapiResult, 500);
}

void WwizardWwiseClient::StartReconnectionThread()
{
    if (currentConnectionThread != nullptr)
    {
        return;
    }

    std::cout << "Start New Connection Threat" << std::endl;
    wwiseClient.Disconnect();
    KillWwiseConsole();
    std::thread t1(&WwizardWwiseClient::ReconnectionThread, this);
    currentConnectionThread = &t1;
    t1.detach();
}

void WwizardWwiseClient::ReconnectionThread()
{
    while (!wwiseClient.IsConnected())
    {
        std::cout << "run thread" << std::endl;
        Connect(settings);
    }
    currentConnectionThread = nullptr;
}

void WwizardWwiseClient::DeleteObjectInWwise(const std::string& guid)
{
    AkJson waapiArg(AkJson::Map{ {"object", AkVariant(guid)} });
    AkJson empty(AkJson::Map{});

    wwiseClient.Call(ak::wwise::core::object::delete_, waapiArg, empty, empty, 500);
}

const AkJson WwizardWwiseClient::GetPropertyFromGuid(const std::string& parentGuid)
{
    AkJson waapiArg(AkJson::Map{
    {
        {"from", AkJson::Map{{ "id", AkJson::Array{ AkVariant(parentGuid) }}}}
    } });
    AkJson waapiOption(AkJson::Map{ { "return", AkJson::Array{ AkVariant("Target")}} });
    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::core::object::get, waapiArg, waapiOption, waapiResult);
    return waapiResult;
}

const AkJson WwizardWwiseClient::GetObjectsByPartName(const std::string& name, const std::vector<std::string> & optionList)
{
    AkJson waapiArg(AkJson::Map{
    {
        {"waql", AkVariant("where name :\"" + name + "\"")}
    } });

    AkJson waapiOption = ConvertVectorToAkJsonOption(optionList);

    AkJson waapiResult;

    wwiseClient.Call(ak::wwise::core::object::get, waapiArg, waapiOption, waapiResult);
    return waapiResult;
}

const AkJson WwizardWwiseClient::GetObjectPropertyList(const int& classID)
{
    AkJson waapiArg(AkJson::Map{
    {
        {"classId", AkVariant(classID)}
    } });

    AkJson waapiResult;
    AkJson waapiOption(AkJson::Map{});

    wwiseClient.Call(ak::wwise::core::object::getPropertyAndReferenceNames, waapiArg, waapiOption, waapiResult);
    return waapiResult;
}

const AkJson WwizardWwiseClient::ConvertVectorToAkJsonOption(const std::vector<std::string>& optionList)
{
    AkJson waapiOption(AkJson::Map{ { "return", AkJson::Array{ }} });

    for (const auto& option : optionList)
    {
        waapiOption["return"].GetArray().emplace_back(AkVariant(option));
    }

    return waapiOption;
}

void WwizardWwiseClient::KillWwiseConsole()
{
    system("taskkill /f /im WwiseConsole.exe");
}

bool WwizardWwiseClient::DidConnectionStatusChange()
{
    if (previousFrameConnectionStatus != wwiseClient.IsConnected())
    {
        return true;
    }
    return false;
}

void WwizardWwiseClient::SyncPreviousConnectionStatus()
{
    previousFrameConnectionStatus = IsConnected()   ;
}

void WwizardWwiseClient::SaveCurrentProject()
{
    AkJson empty(AkJson::Map{});

    wwiseClient.Call(ak::wwise::core::project::save, empty, empty, empty);
}

const std::set<std::string> WwizardWwiseClient::GetAllWwiseProperties()
{
    std::set<std::string> properties;
    AkJson waapiOption(AkJson::Map{});

    for (const auto& wwiseClass : wwiseClassIDs)
    {
        AkJson waapiArg(AkJson::Map{ {{"classId", AkVariant(wwiseClass)}} });

        AkJson waapiResult;


        wwiseClient.Call(ak::wwise::core::object::getPropertyAndReferenceNames, waapiArg, waapiOption, waapiResult);

        for (auto& a : waapiResult["return"].GetArray())
        {
            properties.insert(a.GetVariant().GetString());
        }
    }
    return properties;
}