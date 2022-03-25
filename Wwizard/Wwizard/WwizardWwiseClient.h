#pragma once
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/WwiseAuthoringAPI/waapi.h"
#include <string>
#include <iostream>
#include <vector>
#include "SettingHandler.h"

using namespace AK::WwiseAuthoringAPI;



class WwizardWwiseClient
{ 
public:
	WwizardWwiseClient();

	~WwizardWwiseClient();

	bool Connect(const std::unique_ptr<SettingHandler>& settings);

	bool ForceOpenWwiseInstance(const std::unique_ptr<SettingHandler>& settings);

	void WalkProjectPath(const AkJson& arg, const AkJson& opt, std::vector<AkJson>& outputList);

	const AkJson GetChildrenFromPath(const std::string path, const AkJson option);

	AkJson GetObjectFromPath(const std::string path, AkJson option);

	AkJson RunQueryFromGuuid(const std::string guuid);
	
	AkJson RunCustomQuery(const AkJson arg);

	void GetProjectInfo();

	bool IsConnected()
	{
		return wwiseClient.IsConnected();
	}

	void OpenPropertyInWwise(std::string& guid);

private:
	void WalkChildren(const std::string& guid, const AkJson& opt, std::vector<AkJson>& outputList);

private:
	Client wwiseClient;
};

