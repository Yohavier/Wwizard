#pragma once
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/WwiseAuthoringAPI/waapi.h"
#include <string>
#include <iostream>
#include <vector>
#include "SettingHandler.h"
#include <memory>
#include <thread>
#include <set>

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

	const std::string GetSelectedObjectNameInWwise()
	{
		return currentSelectedName;
	}

	const std::string GetSelectedObjectGuidInWwise()
	{
		return currentSelectedGuid;
	}

	const std::string& GetConnectionSuffixInfo()
	{
		return connectionSuffixInfo;
	}

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

	const std::set<std::string> GetAllWwiseProperties();

private:
	void ReconnectionThread();

	bool Connect(const std::unique_ptr<SettingHandler>& settings);

	const AkJson ConvertVectorToAkJsonOption(const std::vector<std::string>& optionList);

	bool ForceOpenWwiseInstance(const std::unique_ptr<SettingHandler>& settings);

	void KillWwiseConsole();

	void SubscribeToSelectionChanged();

	void SubscribeToPreDeleted();

private:
	Client wwiseClient;
	std::unique_ptr<SettingHandler>& settings;
	std::thread* currentConnectionThread = nullptr;

	bool previousFrameConnectionStatus = false;

	std::string currentSelectedGuid = "";
	std::string currentSelectedName = "";
	std::string connectionSuffixInfo = "";

	std::vector<int> wwiseClassIDs = { 4718608, 327696, 4980752, 524304, 2686992, 4653072, 16, 3997712, 1900560, 1966096, 1376272, 4390928, 4456464, 4325392, 3604496, 917520, 5177360, 3014672,
									   1114128, 262160, 3735568, 3670032, 131088, 1507344, 4915216, 5308432, 4128784, 3473424, 983056, 4259856, 4194320, 5111824, 655360016, 3932176, 4063248,
									   3866640, 5046288, 2555920, 2228240, 2359312, 1769488, 2490384, 2293776, 1835024, 3801104, 2424848, 1572880, 2752528, 1441808, 720912, 4522000, 3538960,
									   786448, 196624, 2097168, 589840, 2162704, 65552, 1179664, 1703952, 393232, 458768, 1310736, 655376, 1245200, 2621456, 3342352, 1638416, 12189699, 7602179,
									   7077891, 7143427, 8192003, 9109507, 8257539, 9043971, 7536643, 8454147, 6881283, 7208963, 8912899, 8650755, 11206659, 7733251, 8847363, 8519683,
									   8585219, 7798786, 7864322, 26542082, 26804226, 6619138, 9699330 };
};