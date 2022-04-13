#pragma once
#include <string>
#include <filesystem>
#include "pugixml-1.12/src/pugixml.hpp"
#include "WwuLookUpData.h"
#include <set>
#include <map>
#include "Issues.h"
#include "ResultFile.h"

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
	NamingConventionModule(const std::string& wwiseProjPath);
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
	void StartCheckingNamingConvention(const std::string& path, std::string namePath);
	void IterateThroughWwu(const pugi::xml_node& wwuNode, const std::string& namePath, const std::string& wwuType);
	

	std::string AddLastNamePathLayer(const std::string& currentNamePath, const pugi::xml_node& newNode, const std::string& containerName);
	void CheckNameForSpace(const pugi::xml_node& currentNode, bool& allowSpace);
	void CheckForMultipleSeparatorsPerLayer(const std::string& newNameLayer, const pugi::xml_node& currentNode, const std::string& containerName);
	bool IsCorrectSuffix(const std::string& currentName, const std::string& newNameLayer, const std::string& containerName);
	void ApplyPrefix(std::string& namePath, const std::string& fullFolderName, const WwuSettings& newPrefix);
	bool CheckUppercaseRule(const pugi::xml_node& currentNode, const bool& allowUppercase);

	bool DetermineResult();
	void ClearOldData();
	void AddIssueToList(const std::string& guid, const std::string& name, const Issue& issue);


	void SaveNamingConventionSettings();
	void LoadNamingConventionSettings();

public:
	std::map<std::string, WwuSettings> wwuSettings;
	std::map<std::string, ContainerSettings> containerSettings;

private:
	std::map<std::string, NamingResultFile> namingIssueResults;
	std::string levelSeparator = "_";

	std::string projectPath;
	std::vector<WwuLookUpData> prefetchedWwuData;

	std::map<Issue, std::string> issueMessages = { {Issue::HIERARCHY, "Hierarchy doesnt match"},
		{Issue::SEPARATOR, "Multiple Separators or suffix is wrong"}, {Issue::SPACE, "Space is not allowed"},
		{Issue::UPPERCASE, "Uppercase is not allowed"} };

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
};

