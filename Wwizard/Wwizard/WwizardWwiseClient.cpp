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
    if (wwiseClient.IsConnected())
    {
        std::cout << "Disconnect from Wwise Instance" << std::endl;
        wwiseClient.Disconnect();
        system("taskkill /F /T /IM WwiseConsole.exe");
    }
    std::cout << "End Wwizard Wwise Client" << std::endl;
}

bool WwizardWwiseClient::Connect(const std::unique_ptr<SettingHandler>& settings)
{
    if (wwiseClient.IsConnected())
    {
        wwiseClient.Disconnect();
    }

    if (wwiseClient.Connect(settings->waapiIP.c_str(),settings->waapiPort))
    {
        std::cout << "Connected to Wwise Instance!" << std::endl;
        return true;
    }
    else if (ForceOpenWwiseInstance(settings))
    {
        std::cout << "Force open Wwise Instance!" << std::endl;
        return true;
    }
    else
    {
        std::cout << "Failed to Connect to Wwise Instance!" << std::endl;
        return false;
    }
}

bool WwizardWwiseClient::ForceOpenWwiseInstance(const std::unique_ptr<SettingHandler>& settings)
{
    std::string commandline = settings->GetSDKPath() + " waapi-server " + settings->GetWwisProjectPathRef() + " --allow-migration --wamp-port " + std::to_string(settings->waapiPort);
    
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

    if (wwiseClient.Connect(settings->waapiIP.c_str(), settings->waapiPort))
    {
        return true;
    }
    else
        return false;
}

void WwizardWwiseClient::WalkProjectPath(const AkJson& arg, const AkJson& opt, std::vector<AkJson>& outputList)
{
 	AkJson queryResult;
	wwiseClient.Call(ak::wwise::core::object::get, arg, opt, queryResult, 500);

    const auto& objects = queryResult["return"].GetArray();
    for (const auto& object : objects)
    {
        outputList.push_back(object);
        WalkChildren(object["id"].GetVariant().GetString().c_str(), opt, outputList);
    }
}

void WwizardWwiseClient::WalkChildren(const std::string& guid, const AkJson& opt, std::vector<AkJson>& outputList)
{
    AkJson queryResult;

    AkJson arg(AkJson::Map{
            {
                {"from", AkJson::Map{{ "id", AkJson::Array{ AkVariant(guid) }}}},
                {"transform", AkJson::Array{ AkJson::Map {{"select", AkJson::Array{ AkVariant("children")}}}}}
            }});

    wwiseClient.Call(ak::wwise::core::object::get, arg, opt, queryResult, 500);

    const auto& objects = queryResult["return"].GetArray();
    for (const auto& object : objects)
    {
        outputList.push_back(object);
        WalkChildren(object["id"].GetVariant().GetString().c_str(), opt, outputList);
    }
}

const AkJson WwizardWwiseClient::GetChildrenFromGuid(const std::string guid, const AkJson option)
{
    AkJson arg(AkJson::Map{
    {
        {"from", AkJson::Map{{ "id", AkJson::Array{ AkVariant(guid) }}}},
        {"transform", AkJson::Array{ AkJson::Map {{"select", AkJson::Array{ AkVariant("children")}}}}}
    } });
    AkJson queryResult;

    wwiseClient.Call(ak::wwise::core::object::get, arg, option, queryResult, 500);

    return queryResult;
}

void WwizardWwiseClient::GetCurrentSelectedObjectsInWwise(AkJson& result, const AkJson& option)
{
    AkJson arg(AkJson::Map{ });
    wwiseClient.Call(ak::wwise::ui::getSelectedObjects, arg, option, result, 500);
}

AkJson WwizardWwiseClient::GetObjectFromPath(const std::string path, AkJson option)
{
    AkJson arg(AkJson::Map{
        { "from", AkJson::Map{
            { "path", AkJson::Array{ AkVariant(path) } } } }
        });

    AkJson queryResult;

    wwiseClient.Call(ak::wwise::core::object::get, arg, option, queryResult, 500);

    return queryResult;
}

AkJson WwizardWwiseClient::RunQueryFromGuuid(const std::string guid)
{
    AkJson arg(AkJson::Map{
       { "from", AkJson::Map{{ "query", AkJson::Array{AkVariant(guid)}}}}});

    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path"), AkVariant("color")}}});

    AkJson queryResult;
    wwiseClient.Call(ak::wwise::core::object::get, arg, options, queryResult, 500);

    return queryResult;
}

AkJson WwizardWwiseClient::RunCustomQuery(const AkJson arg)
{
    AkJson queryResult;
    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path"), AkVariant("color")}}});

    wwiseClient.Call(ak::wwise::core::object::get, arg, options, queryResult, 500);

    return queryResult;
}

const AkJson WwizardWwiseClient::GetProjectInfo()
{
    AkJson queryResult;
    AkJson arg(AkJson::Map{});
    AkJson opt(AkJson::Map{});

    wwiseClient.Call(ak::wwise::core::getInfo, arg, opt, queryResult, 500);

    return queryResult;
}

void WwizardWwiseClient::OpenPropertyInWwise(const std::string& guid)
{
    AkJson openHierarchy(AkJson::Map{ {{"command", AkVariant("FindInProjectExplorerSyncGroup1")}, {"objects", AkJson::Array{AkVariant(guid)}}} });
    AkJson openProperty(AkJson::Map{ {{"command", AkVariant("Inspect")}, {"objects", AkJson::Array{AkVariant(guid)}}} });

    AkJson options(AkJson::Map{ });
    AkJson result;

    wwiseClient.Call(ak::wwise::ui::commands::execute, openProperty, options, result, 500);
    wwiseClient.Call(ak::wwise::ui::commands::execute, openHierarchy, options, result, 500);
}

void WwizardWwiseClient::StartReconnectionThread()
{
    if (currentConnectionThread != nullptr)
    {
        return;
    }

    std::cout << "Start New Connection Threat" << std::endl;
    wwiseClient.Disconnect();
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
    AkJson empty(AkJson::Map{});
    AkJson arg(AkJson::Map{ {"object", AkVariant(guid)} });
    wwiseClient.Call(ak::wwise::core::object::delete_, arg, empty, empty, 500);
}

const AkJson WwizardWwiseClient::GetPropertyFromGuid(const std::string& parentGuid)
{
    AkJson options(AkJson::Map{ { "return", AkJson::Array{ AkVariant("Target")}} });
    AkJson result;
    AkJson arg(AkJson::Map{
       {
           {"from", AkJson::Map{{ "id", AkJson::Array{ AkVariant(parentGuid) }}}}
       } });
    wwiseClient.Call(ak::wwise::core::object::get, arg, options, result);
    return result;
}

void WwizardWwiseClient::SetProperty(const AkJson& arg)
{
    AkJson empty(AkJson::Map{});
    wwiseClient.Call(ak::wwise::core::object::setProperty, arg, empty, empty, 500);
}

const AkJson WwizardWwiseClient::GetObjectsByPartName(const std::string& name, const AkJson& option)
{
    AkJson result;
    AkJson arg(AkJson::Map{
           {
               {"waql", AkVariant("where name :\"" + name + "\"")}
           } });

    wwiseClient.Call(ak::wwise::core::object::get, arg, option, result);
    return result;
}

const AkJson WwizardWwiseClient::GetObjectPropertyList(const int& classID)
{
    AkJson result;
    AkJson empty(AkJson::Map{});
    AkJson arg(AkJson::Map{
           {
               {"classId", AkVariant(classID)}
           } });

    wwiseClient.Call(ak::wwise::core::object::getPropertyAndReferenceNames, arg, empty, result);
    return result;
}
