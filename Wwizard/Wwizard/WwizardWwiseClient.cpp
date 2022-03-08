#include "WwizardWwiseClient.h"
#include <windows.h>

using namespace AK::WwiseAuthoringAPI;

cWwizardWwiseClient::cWwizardWwiseClient() 
    :port(0)
    , ipAdresse("")
{
    std::cout << "Initialized Wwizard Wwise Client" << std::endl;
}

cWwizardWwiseClient::~cWwizardWwiseClient()
{
    if (isConnected)
    {
        std::cout << "Disconnect from Wwise Instance" << std::endl;
        wwiseClient.Disconnect();
    }
    std::cout << "End Wwizard Wwise Client" << std::endl;
}

bool cWwizardWwiseClient::Connect(const std::string& ip, const int& port)
{
    if (wwiseClient.Connect(ip.c_str(), port))
    {
        std::cout << "Connected to Wwise Instance!" << std::endl;
        isConnected = true;
        return true;
    }
    else if (ForceOpenWwiseInstance())
    {
        std::cout << "Force open Wwise Instance!" << std::endl;
        isConnected = true;
        return true;
    }
    else
    {
        std::cout << "Failed to Connect to Wwise Instance!" << std::endl;
        isConnected = false;
        return false;
    }
}

bool cWwizardWwiseClient::ForceOpenWwiseInstance()
{
    std::string commandline = "\"E:\\Wwise 2021.1.6.7774\\Authoring\\x64\\Release\\bin\\WwiseConsole.exe\" waapi-server \"D:\\ue\\BubbleSpace\\BubbleSpace_WwiseProject\\BubbleSpace_WwiseProject.wproj\" --allow-migration --wamp-port 8080";
    WinExec(commandline.c_str(), 1);
    if (wwiseClient.Connect("127.0.0.1", 8080))
    {
        isConnected = true;
        return true;
    }
    else
        return false;
}

void cWwizardWwiseClient::WalkProjectPath(const AkJson& arg, const AkJson& opt, std::vector<AkJson>& outputList)
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

void cWwizardWwiseClient::WalkChildren(const std::string& guid, const AkJson& opt, std::vector<AkJson>& outputList)
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

AkJson cWwizardWwiseClient::GetChildrenFromPath(const std::string path, AkJson option)
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

AkJson cWwizardWwiseClient::GetObjectFromPath(const std::string path, AkJson option)
{
    AkJson arg(AkJson::Map{
        { "from", AkJson::Map{
            { "path", AkJson::Array{ AkVariant(path) } } } }
        });

    AkJson queryResult;

    wwiseClient.Call(ak::wwise::core::object::get, arg, option, queryResult, 100);

    return queryResult;
}

AkJson cWwizardWwiseClient::RunQueryFromGuuid(const std::string guid)
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

AkJson cWwizardWwiseClient::RunCustomQuery(const AkJson arg)
{
    AkJson queryResult;
    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path")}} });

    wwiseClient.Call(ak::wwise::core::object::get, arg, options, queryResult, 100);

    return queryResult;
}