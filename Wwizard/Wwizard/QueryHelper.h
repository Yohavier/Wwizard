#pragma once
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/WwiseAuthoringAPI/waapi.h"
#include <string>
#include <iostream>
#include <vector>

using namespace AK::WwiseAuthoringAPI;

class QueryHelper
{ 
public:
	void WalkProject(const AkJson& arg, const AkJson& opt, std::vector<std::string>& outputList);

	void ChangeSettings(const std::string ip, const int port);
	
private:
	void WalkChildren(const std::string& guid, std::vector<std::string>& outputList, const AkJson& opt);

private:
	Client m_wwiseClient;
	int m_port;
	std::string m_ip;
};

