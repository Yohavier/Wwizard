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
#include "ContainerSetting.h"
#include "WwuSetting.h"

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

	void OnConnectionStatusChange(const bool newConnectionStatus) override;

private:
	void BeginNamingConventionProcess();

	void FetchWwuDataInDirectory(const std::string& directory);
	void FetchSingleWwuData(const std::string& path);
	void ScanWorkUnitXMLByPath(const std::string& wwuPath, std::string& namePath);
	void StartCheckingNamingConvention(const std::string& path, std::string reconstructedPath);
	void IterateThroughWwu(const pugi::xml_node& wwuNode, std::string namePath, const std::string& wwuType);
	
	bool RunChecks(const std::string& nodeName, const std::string& nodeID, const std::string& wwuType, const std::string & lastAddedLayer, const std::string& reconstructedNamePath, const std::string& containerName);
	std::string AddLastNamePathLayer(std::string& currentNamePath, const std::string& newName, const std::string& containerName);
	bool CheckNameForSpace(const std::string& nodeName, const std::string& nodeID, bool& allowSpace);
	bool CheckForMultipleSeparatorsPerLayer(const std::string& nodeName, const std::string& nodeID, const std::string& newNameLayer, const std::string& containerName);
	bool IsCorrectSuffix(const std::string& currentName, const std::string& newNameLayer, const std::string& containerName);
	void ApplyPrefix(std::string& constructedNamePath, const WwuSettings& newPrefix);
	bool CheckUppercaseRule(const std::string& nodeName, const std::string& nodeID, const bool& allowUppercase);
	bool CheckRightPrefix(const std::string& nodeName, const std::string& nodeID, const std::string& wwuType);
	bool CheckHierarchy(const std::string& currentName, const std::string& constructedName, const std::string& nameID);

	void ClearOldData();
	void AddIssueToList(const std::string& guid, const std::string& name, const NamingIssue& issue);

	void SaveNamingConventionSettings();
	void LoadNamingConventionSettings();

public:
	std::map<std::string, WwuSettings> wwuSettings;
	std::map<std::string, ContainerSettings> containerSettings;

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::map<std::string, NamingResultFile> namingIssueResults;
	std::string levelSeparator = "_";

	std::string projectPath;
	std::vector<WwuLookUpData> prefetchedWwuData;

	std::thread* currentNamingConventionThread = nullptr;

	std::map<NamingIssue, std::string> issueMessages = { {NamingIssue::HIERARCHY, "Hierarchy doesnt match"},
		{NamingIssue::SEPARATOR, "Multiple Separators or suffix is wrong"}, {NamingIssue::SPACE, "Space is not allowed"},
		{NamingIssue::UPPERCASE, "Uppercase is not allowed"}, {NamingIssue::PREFIX, "Wrong Prefix"} };

	std::set<std::string> whitelistedContainers = { "Folder", "Switch", "AudioDevice", "SwitchGroup",
		"SoundBank", "Event", "DialogueEvent", "Bus", "AuxBus", "MusicSegment", "MusicTrack", "MusicSwitchContainer",
		"MusicPlaylistContainer", "ActorMixer", "BlendContainer", "RandomSequenceContainer",
		"SwitchContainer", "Sound", "AcousticTexture", "Trigger", "State", "StateGroup", "Query" };

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

