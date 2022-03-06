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

	bool ForceOpenWwiseInstance();

	bool IsWwiseConnected() { return isConnected; }

	void WalkProjectPath(const AkJson& arg, const AkJson& opt, std::vector<AkJson>& outputList);

	AkJson GetChildrenFromPath(const std::string path, AkJson option);

	AkJson GetObjectFromPath(const std::string path, AkJson option);

	AkJson RunQueryFromGuuid(const std::string guuid);
	
	AkJson RunWaapiQuery(const AkJson arg);

	AkJson RunWaqlQuery(const AkJson arg);

private:
	void WalkChildren(const std::string& guid, const AkJson& opt, std::vector<AkJson>& outputList);

private:
	Client wwiseClient;

	bool isConnected = false;
	int port;
	std::string ipAdresse;
};

