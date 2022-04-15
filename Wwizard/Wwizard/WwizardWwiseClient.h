#pragma once
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/WwiseAuthoringAPI/waapi.h"
#include <string>
#include <iostream>
#include <vector>
#include "SettingHandler.h"
#include <memory>
#include <thread>

using namespace AK::WwiseAuthoringAPI;

class WwizardWwiseClient
{ 
public:
	WwizardWwiseClient() = delete;
	WwizardWwiseClient(std::unique_ptr<SettingHandler>& settings)
		: settings(settings)
	{
		StartReconnectionThread();
	}

	~WwizardWwiseClient();
	
	void StartReconnectionThread();

	bool ForceOpenWwiseInstance(const std::unique_ptr<SettingHandler>& settings);

	void WalkProjectPath(const AkJson& arg, const AkJson& opt, std::vector<AkJson>& outputList);

	AkJson GetObjectFromPath(const std::string path, AkJson option);

	AkJson RunQueryFromGuuid(const std::string guuid);
	
	AkJson RunCustomQuery(const AkJson arg);

	void GetProjectInfo();

	bool IsConnected()
	{
		return wwiseClient.IsConnected();
	}

	void OpenPropertyInWwise(const std::string& guid);

	const AkJson GetChildrenFromGuid(const std::string guid, const AkJson option);

private:
	void WalkChildren(const std::string& guid, const AkJson& opt, std::vector<AkJson>& outputList);
	void ReconnectionThread();
	bool Connect(const std::unique_ptr<SettingHandler>& settings);

private:
	Client wwiseClient;
	std::unique_ptr<SettingHandler>& settings;
	std::thread* currentConnectionThread = nullptr;
};

