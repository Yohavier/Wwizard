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

	bool Connect(const std::string& ip, const int& port);

	bool IsWwiseConnected() { return m_isConnected; }

	void WalkProjectPath(const AkJson& arg, const AkJson& opt, std::vector<AkJson>& outputList);

	AkJson GetChildrenFromPath(const std::string path, AkJson option);

	AkJson GetObjectFromPath(const std::string path, AkJson option);

	AkJson RunQueryFromGuuid(const std::string guuid);
	
private:
	void WalkChildren(const std::string& guid, const AkJson& opt, std::vector<AkJson>& outputList);

private:
	Client m_wwiseClient;

	bool m_isConnected = false;
	int m_port;
	std::string m_ip;
};

