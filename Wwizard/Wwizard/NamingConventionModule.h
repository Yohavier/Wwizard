#pragma once
#include <string>
#include <filesystem>
#include "pugixml-1.12/src/pugixml.hpp"
#include "WwuLookUpData.h"
#include <set>
#include <map>


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
};

class NamingConventionModule
{ 
public:
	NamingConventionModule();
	~NamingConventionModule();
	void CheckNamingConvention();
	NamingConventionModule(std::string wwiseProjPath);

private:
	void ScanWorkUnitData(std::string directory);
	void FetchWwuData(std::string path);
	void ApplyPrefix(std::string& namePath, std::string fullFolderName, const WwuSettings& newPrefix);
	void ScanWorkUnitXMLByPath(std::string wwuPath, std::string namePath);
	void IterateFolder(std::string path, std::string namePath);

	void ModularResolve(pugi::xml_node wwuNode, std::string namePath, std::string wwuType);
	std::string AddLastNamePathLayer(const std::string& currentNamePath, std::string newNodeName, std::string containerName);
	void SaveNamingConvention();
	void LoadNamingConvention();

	void CheckNameForSpace(std::string currentName, bool allowSpace);
	void CheckForMultipleSeparatorsPerLayer(std::string newNameLayer, std::string currentName, std::string containerName);

	bool IsCorrectSuffix(std::string currentName, std::string newNameLayer, std::string containerName);
	bool IsNumber(const std::string& s);

public:
	std::string levelSeparator = "_";

	std::map<std::string, WwuSettings> wwuSettings;
	std::map<std::string, ContainerSettings> containerSettings;

	std::string projectPath;	
	std::vector<WwuLookUpData> wwuData;

	std::map<std::string, std::string> namingIssueResults;


	std::set<std::string> whitelistedContainers = { "Folder", "Switch", "AudioDevice", "SwitchGroup", 
		"SoundBank", "Event", "DialogueEvent", "Bus", "AuxBus", "MusicSegment", "MusicTrack", "MusicSwitchContainer", 
		"MusicPlaylistContainer", "ActorMixer", "BlendContainer", "RandomSequenceContainer",
		"SwitchContainer", "Sound", "AcousticTexture", "Trigger", "State", "StateGroup", "Query"};

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

