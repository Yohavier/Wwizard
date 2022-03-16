#include "WwizardWwiseClient.h"
#include <windows.h>

using namespace AK::WwiseAuthoringAPI;

WwizardWwiseClient::WwizardWwiseClient() 
{
    std::cout << "Initialized Wwizard Wwise Client" << std::endl;
}

WwizardWwiseClient::~WwizardWwiseClient()
{
    if (wwiseClient.IsConnected())
    {
        std::cout << "Disconnect from Wwise Instance" << std::endl;
        wwiseClient.Disconnect();
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
    std::string commandline = "\"E:\\Wwise 2021.1.6.7774\\Authoring\\x64\\Release\\bin\\WwiseConsole.exe\" waapi-server \"D:\\ue\\BubbleSpace\\BubbleSpace_WwiseProject\\BubbleSpace_WwiseProject.wproj\" --allow-migration --wamp-port " + std::to_string(settings->waapiPort);
    WinExec(commandline.c_str(), 0);

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
	wwiseClient.Call(ak::wwise::core::object::get, arg, opt, queryResult, 100);

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

    wwiseClient.Call(ak::wwise::core::object::get, arg, opt, queryResult, 100);

    const auto& objects = queryResult["return"].GetArray();
    for (const auto& object : objects)
    {
        outputList.push_back(object);
        WalkChildren(object["id"].GetVariant().GetString().c_str(), opt, outputList);
    }
}

const AkJson WwizardWwiseClient::GetChildrenFromPath(const std::string path, const AkJson option)
{
    AkJson arg(AkJson::Map{
        {
            {"from", AkJson::Map{{ "path", AkJson::Array{ AkVariant(path) }}}},
            {"transform", AkJson::Array{ AkJson::Map {{"select", AkJson::Array{ AkVariant("children")}}}}}
        } });
    AkJson queryResult;
 
    wwiseClient.Call(ak::wwise::core::object::get, arg, option, queryResult, 100);
    
    return queryResult;
}

AkJson WwizardWwiseClient::GetObjectFromPath(const std::string path, AkJson option)
{
    AkJson arg(AkJson::Map{
        { "from", AkJson::Map{
            { "path", AkJson::Array{ AkVariant(path) } } } }
        });

    AkJson queryResult;

    wwiseClient.Call(ak::wwise::core::object::get, arg, option, queryResult, 100);

    return queryResult;
}

AkJson WwizardWwiseClient::RunQueryFromGuuid(const std::string guid)
{
    std::cout << guid << std::endl;
    AkJson arg(AkJson::Map{
       { "from", AkJson::Map{{ "query", AkJson::Array{AkVariant(guid)}}}}});

    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path")}} });

    AkJson queryResult;
    wwiseClient.Call(ak::wwise::core::object::get, arg, options, queryResult, 100);

    return queryResult;
}

AkJson WwizardWwiseClient::RunCustomQuery(const AkJson arg)
{
    AkJson queryResult;
    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path")}} });

    wwiseClient.Call(ak::wwise::core::object::get, arg, options, queryResult, 100);

    return queryResult;
}

void WwizardWwiseClient::OpenPropertyInWwise(std::string& guid)
{
    AkJson openHierarchy(AkJson::Map{ {{"command", AkVariant("FindInProjectExplorerSyncGroup1")}, {"objects", AkJson::Array{AkVariant(guid)}}} });
    AkJson openProperty(AkJson::Map{ {{"command", AkVariant("Inspect")}, {"objects", AkJson::Array{AkVariant(guid)}}} });

    AkJson options(AkJson::Map{ });
    AkJson result;

    wwiseClient.Call(ak::wwise::ui::commands::execute, openProperty, options, result, 100);
    wwiseClient.Call(ak::wwise::ui::commands::execute, openHierarchy, options, result, 100);
}