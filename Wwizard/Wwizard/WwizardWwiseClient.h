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

	const AkJson GetObjectFromPath(const std::string& path, std::vector<std::string>& returnValues);

	const AkJson RunQueryFromGuuid(const std::string& guuid);
	
	const AkJson RunCustomQuery(const AkJson arg);

	const AkJson GetProjectInfo();

	bool IsConnected()
	{
		return wwiseClient.IsConnected();
	}

	void FocusObjectInWwise(const std::string& guid);

	const AkJson GetChildrenFromGuid(const std::string& guid, const std::vector<std::string>& optionList);

	const AkJson GetSelectedObjectsInWwise(const std::vector<std::string>& option);

	void DeleteObjectInWwise(const std::string& guid);

	const AkJson GetPropertyFromGuid(const std::string& parentGuid);

	template<typename PropertyValue>
	void SetProperty(const std::string& objectPath, const std::string& propertyToSet, const PropertyValue& value)
	{
		AkJson waapiArg(AkJson::Map{
			   {
				   {"object", AkVariant(objectPath)},
				   {"property", AkVariant(propertyToSet)},
				   {"value", AkVariant(value)}
			   } });

		AkJson empty(AkJson::Map{});
		wwiseClient.Call(ak::wwise::core::object::setProperty, waapiArg, empty, empty, 500);
	}
	
	const AkJson GetObjectsByPartName(const std::string& name, const std::vector<std::string>& optionList);

	const AkJson GetObjectPropertyList(const int& classID);

	bool DidConnectionStatusChange();

	void SyncPreviousConnectionStatus();

	void SaveCurrentProject();

private:
	void ReconnectionThread();

	bool Connect(const std::unique_ptr<SettingHandler>& settings);

	const AkJson ConvertVectorToAkJsonOption(const std::vector<std::string>& optionList);

	bool ForceOpenWwiseInstance(const std::unique_ptr<SettingHandler>& settings);

	void KillWwiseConsole();

private:
	Client wwiseClient;
	std::unique_ptr<SettingHandler>& settings;
	std::thread* currentConnectionThread = nullptr;

	bool previousFrameConnectionStatus = false;
};

