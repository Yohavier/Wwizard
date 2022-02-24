#include "QueryHelper.h"

using namespace AK::WwiseAuthoringAPI;

void QueryHelper::WalkProject(const AkJson& arg, const AkJson& opt, std::vector<std::string>& outputList)
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

void QueryHelper::ChangeSettings(const std::string ip, const int port)
{
	m_ip = ip;
	m_port = port;
}

void QueryHelper::WalkChildren(const std::string& guid, std::vector<std::string>& outputList, const AkJson& opt)
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
