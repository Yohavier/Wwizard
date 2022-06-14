#pragma once
#include <string>
#include <filesystem>
#include "pugixml-1.12/src/pugixml.hpp"
#include "WwuLookUpData.h"
#include <set>
#include <map>
#include "NamingIssues.h"
#include "ResultFile.h"
#include <thread>
#include "WwizardWwiseClient.h"
#include "BaseModule.h"
#include "NamingSetting.h"

class NamingConventionModule : public BaseModule
{ 
public:
	NamingConventionModule(const std::string& wwiseProjPath, std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	~NamingConventionModule();

	
	void StartCheckNamingConventionThread();

	const std::string& GetErrorMessageFromIssue(const NamingIssue& issue);
	const std::map<std::string, NamingResultFile>& GetNamingIssues();
	const std::set<std::string>& GetWhiteListedContainers();
	const std::set<std::string>& GetWhiteListedWwuTypes();
	const std::string& GetStringToReplace(const std::string& wwuType);

	void SetProjectPath(std::string newProjectPath);
	void OnConnectionStatusChange(const bool newConnectionStatus) override;
	void OnSettingsChange(const std::string projectPath, const std::string sdkPath) override;
	void LoadAllNamingSettingsInDir(const std::string path);
	void SetDefaultActive();
	void SaveSettings(const std::string settingToSaveName, NamingSetting& saveSetting);
	void SaveAsNewSetting(const std::string settingToSaveName, NamingSetting& saveSetting);

	void ChangeNamingSetting(const std::string newSetting);

	bool NamingConventionNameAlreadyExists(const std::string newName);

	bool IsDefaultSetting(const std::string settingName)
	{
		if (settingName == "Default.json")
		{
			return true;
		}
		return false;
	}

	void ClearSetting(const std::string& settingName);

private:
	void BeginNamingConventionProcess();

	void ClearOldData();
	void AddIssueToList(const std::string& guid, const std::string& name, const NamingIssue& issue);

	void LoadNamingConventionSettings(const std::filesystem::path settingPath);

	void FindTopPhysicalFolders(const std::string& folderPath);
	void IteratePhysicalFolder(const std::filesystem::path& folderPath, const std::string& wwuSettingKey, const std::string& parentContainerKey);
	std::string GetWorkUnitIDFromXML(const std::string& wwuPath);
	void IterateThroughWwu(const std::string id, const std::string parentName, const std::string& wwuSettingKey, const std::string& parentContainerKey);

	bool CheckNamingSettings(const std::string& currentFileName, const std::string& parentFileName, const std::string& wwuSettingKey, const std::string& containerKey, const std::string& parentContainerKey, const std::string& currentID);
	bool IsUppercaseInPath(const std::string& fileName);
	bool IsSpaceInPath(const std::string& fileName);
	bool IsPrefixRight(const std::string& fileName, const std::string& prefix);
	bool IsParentHierarchyMatching(const std::string& fileName, const std::string& parentFileName, const std::string& prefix, const std::string& suffix, const std::string& parentContainerKey);
	bool IsOneUnderscorePerNewLayer(const std::string& fileName, const std::string& parentName);

	std::string RemovePrefixSuffix(const std::string& fileName, const std::string& wwuSettingKey, const std::string& containerSettingKey);
	std::string RemovePrefixFromName(const std::string& fileName, const std::string& wwuSettingKey);
	std::string RemoveSuffixFromName(const std::string& fileName, const std::string& containerSettingKey);

	bool IsContainerWhiteListed(const std::string& containerType);
	bool CheckSuffix(const std::string& fileName, const std::string& containerType, std::string& outSuffix);

	std::vector<std::string> ConvertStringToVector(const std::string& inputSetting);



public:
	NamingSetting* activeNamingSetting = nullptr;
	std::vector<std::string> allSettings;
	std::string activeSettingName;

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::map<std::string, NamingResultFile> namingIssueResults;
	std::vector<std::filesystem::path> legalTopFolderPaths;

	std::string projectPath;
	std::string openedNamingConventionSetting;



	std::map<std::string, NamingSetting> loadedNamingConventions;

	std::thread* currentNamingConventionThread = nullptr;

	std::map<NamingIssue, std::string> issueMessages = { {NamingIssue::HIERARCHY, "Hierarchy doesnt match"},
		{NamingIssue::SEPARATOR, "Multiple Separators or suffix is wrong"}, {NamingIssue::SPACE, "Space is not allowed"},
		{NamingIssue::UPPERCASE, "Uppercase is not allowed"}, {NamingIssue::PREFIX, "Wrong Prefix"}, {NamingIssue::Suffix, "Wrong Suffix"}};

	std::set<std::string> whitelistedContainers = { "Folder", "Switch", "AudioDevice", "SwitchGroup",
		"SoundBank", "Event", "DialogueEvent", "Bus", "AuxBus", "MusicSegment", "MusicTrack", "MusicSwitchContainer",
		"MusicPlaylistContainer", "ActorMixer", "BlendContainer", "RandomSequenceContainer",
		"SwitchContainer", "Sound", "AcousticTexture", "Trigger", "State", "StateGroup", "Query", "WorkUnit"};

	std::set<std::string> whitelistedWwuTypes = { "AudioObjects", "Attenuations", "AudioDevices",
		"ControlSurfaceSessions", "Conversions", "DynamicDialogue", "Effects", "Events", "GameParameters",
		"InteractiveMusic", "Busses", "Metadatas", "MixingSessions", "Modulators", "Queries", "SoundBanks",
		"SoundcasterSessions", "States", "Switches", "Triggers", "VirtualAcoustics" };

	std::map<std::string, std::string> stringToReplace = { {"AudioObjects","_Actor-Mixer Hierarchy"},
		{"Attenuations","_Attenuations"}, {"AudioDevices","_Audio Devices"},{"ControlSurfaceSessions","_Control Surface Session"},
		{"Conversions", "_Conversion Settings"},{"DynamicDialogue","_Dynamic Dialogue"},{"Effects", "_Effects"},
		{"Events","_Events"},{"GameParameters", "_Game Parameters"}, {"InteractiveMusic","_Interactive Music Hierarchy"},
		{"Busses","_Master-Mixer Hierarchy"},{"Metadatas","_Metadata"},{"MixingSessions","_Mixing Session"},
		{"Modulators","_Modulators"},{"Queries","_Queries"},{"SoundBanks","_SoundBanks"},{"SoundcasterSessions","_Soundcaster Sessions"},
		{"States","_States"},{"Switches","_Switches"},{"Triggers","_Triggers"},{"VirtualAcoustics","_Virtual Acoustics"}
	};

	std::map<std::string, std::string> wwiseWorkFoldersToWwuType = { {"Actor-Mixer Hierarchy", "AudioObjects"},
		{"Attenuations","Attenuations"}, {"Audio Devices","AudioDevices"},{"Control Surface Sessions","ControlSurfaceSession"},
		{"Conversion Settings", "Conversions"},{"Dynamic Dialogue","DynamicDialogue"},{"Effects", "Effects"},
		{"Events","Events"},{"Game Parameters", "GameParameters"}, {"Interactive Music Hierarchy","InteractiveMusic"},
		{"Master-Mixer Hierarchy","Busses"},{"Metadata","Metadatas"},{"Mixing Sessions","Mixing Session"},
		{"Modulators","Modulators"},{"Queries","Queries"},{"SoundBanks","SoundBanks"},{"Soundcaster Sessions","Soundcaster Sessions"},
		{"States","States"},{"Switches","Switches"},{"Triggers","Triggers"},{"Virtual Acoustics","VirtualAcoustics"} 
	};
};

