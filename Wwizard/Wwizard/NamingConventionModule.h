#pragma once
#include <string>
#include <filesystem>
#include "pugixml-1.12/src/pugixml.hpp"
#include "WwuLookUpData.h"
#include <set>
#include <map>
#include "Issues.h"
#include "ResultFile.h"
#include "WwizardWwiseClient.h"

struct WwuSettings
{
	WwuSettings() = default;
	WwuSettings(std::string prefixToApply, bool applyPrefix, bool applyNamingConventionCheck, bool allowSpace)
		: prefixToApply(prefixToApply)
		, applyPrefix(applyPrefix)
		, applyNamingConventionCheck(applyNamingConventionCheck)
		, allowSpace(allowSpace)
	{}

	std::string prefixToApply = "";	

	bool applyPrefix = false;
	bool applyNamingConventionCheck = false;
	bool allowSpace = false;
	bool allowUpperCase = false;
};

struct ContainerSettings
{
	ContainerSettings() = default;
	ContainerSettings(bool allowNumberSuffix, bool allowStringSuffix, int suffixLayers, int maxNumberAllowed, std::string stringSuffixes)
		: allowNumberSuffix(allowNumberSuffix)
		, allowStringSuffix(allowStringSuffix)
		, suffixLayers(suffixLayers)
		, maxNumberAllowed(maxNumberAllowed)
		, stringSuffixes(stringSuffixes)
	{

	}

	bool allowNumberSuffix = false;
	bool allowStringSuffix = false;

	int suffixLayers = 0;
	int maxNumberAllowed =  0;

	std::string stringSuffixes ="";

	bool IsStringInSuffixList(std::string layer) 
	{
		std::string newSuffix;
		for (auto& c : stringSuffixes)
		{
			if (c != ' ')
			{
				if (c == ',')
				{
					if (newSuffix == layer)
					{
						return true;
					}
					newSuffix = "";
				}
				else
				{
					newSuffix += c;
				}
			}
		}
		if (newSuffix != "")
		{
			if (newSuffix == layer)
			{
				return true;
			}
		}

		return false;
	}

	bool IsNumberInRange(std::string number)
	{

		if (std::to_string(maxNumberAllowed).size() == number.size())
		{
			int numLayer = std::stoi(number);
			if (numLayer <= maxNumberAllowed)
			{
				return true;
			}
		}
		return false;
	}

	bool IsSuffixCountInRange(int layerCount)
	{
		if (layerCount <= maxNumberAllowed)
		{
			return true;
		}
		return false;
	}

	bool IsNumber(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}
};

class NamingConventionModule
{ 
public:
	NamingConventionModule(const std::string& wwiseProjPath, std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	~NamingConventionModule();

	bool CheckNamingConvention();

	const std::string& GetErrorMessageFromIssue(const Issue& issue);
	const std::map<std::string, NamingResultFile>& GetNamingIssues();
	const std::set<std::string>& GetWhiteListedContainers();
	const std::set<std::string>& GetWhiteListedWwuTypes();
	const std::string& GetStringToReplace(const std::string& wwuType);

private:
	void PreFetchAllWwuData(const std::string& directory);
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

	bool DetermineResult();
	void ClearOldData();
	void AddIssueToList(const std::string& guid, const std::string& name, const Issue& issue);


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

	std::map<Issue, std::string> issueMessages = { {Issue::HIERARCHY, "Hierarchy doesnt match"},
		{Issue::SEPARATOR, "Multiple Separators or suffix is wrong"}, {Issue::SPACE, "Space is not allowed"},
		{Issue::UPPERCASE, "Uppercase is not allowed"}, {Issue::PREFIX, "Wrong Prefix"} };

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

