#include "WwizardWwiseClient.h"

using namespace AK::WwiseAuthoringAPI;

cWwizardWwiseClient::cWwizardWwiseClient() 
    :m_port(0)
    , m_ip("")
{
    std::cout << "Initialized Wwizard Wwise Client" << std::endl;
}

cWwizardWwiseClient::~cWwizardWwiseClient()
{
    if (m_isConnected)
    {
        std::cout << "Disconnect from Wwise Instance" << std::endl;
        m_wwiseClient.Disconnect();
    }
    std::cout << "End Wwizard Wwise Client" << std::endl;
}

bool cWwizardWwiseClient::Connect(const std::string& ip, const int& port)
{
    if (m_wwiseClient.Connect(ip.c_str(), port))
    {
        std::cout << "Connected to Wwise Instance!" << std::endl;
        m_isConnected = true;
        return true;
    }
    else
    {
        std::cout << "Failed to Connect to Wwise Instance!" << std::endl;
        m_isConnected = false;
        return false;
    }
}

void cWwizardWwiseClient::WalkProject(const AkJson& arg, const AkJson& opt, std::vector<std::string>& outputList)
{
 	AkJson queryResult;
	m_wwiseClient.Connect(m_ip.c_str(), m_port);
	m_wwiseClient.Call(ak::wwise::core::object::get, arg, opt, queryResult, 100);

    const auto& objects = queryResult["return"].GetArray();
    for (const auto& object : objects)
    {
        outputList.push_back(object["name"].GetVariant().GetString());
        WalkChildren(object["id"].GetVariant().GetString().c_str(), outputList, opt);
    }
}

void cWwizardWwiseClient::WalkChildren(const std::string& guid, std::vector<std::string>& outputList, const AkJson& opt)
{
    AkJson queryResult;

    AkJson arg(AkJson::Map{
            {
                {"from", AkJson::Map{{ "id", AkJson::Array{ AkVariant(guid) }}}},
                {"transform", AkJson::Array{ AkJson::Map {{"select", AkJson::Array{ AkVariant("children")}}}}}
            }});

    m_wwiseClient.Call(ak::wwise::core::object::get, arg, opt, queryResult, 100);

    const auto& objects = queryResult["return"].GetArray();
    for (const auto& object : objects)
    {
        outputList.push_back(object["type"].GetVariant().GetString() + " " + object["name"].GetVariant().GetString());
        WalkChildren(object["id"].GetVariant().GetString().c_str(), outputList, opt);
    }
}
