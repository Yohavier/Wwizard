#pragma once
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/WwiseAuthoringAPI/waapi.h"
#include <string>
#include <iostream>
#include <vector>

using namespace AK::WwiseAuthoringAPI;

class cWwizardWwiseClient
{ 
public:
	cWwizardWwiseClient();

	~cWwizardWwiseClient();

	//Connect to a wwise instance
	bool Connect(const std::string& ip, const int& port);

	bool IsWwiseConnected() { return m_isConnected; }

	void WalkProject(const AkJson& arg, const AkJson& opt, std::vector<std::string>& outputList);
	
private:
	void WalkChildren(const std::string& guid, std::vector<std::string>& outputList, const AkJson& opt);

private:
	Client m_wwiseClient;

	bool m_isConnected = false;
	int m_port;
	std::string m_ip;
};

