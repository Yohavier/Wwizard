#pragma onceo
#include <string>
#include <filesystem>
#include "pugixml-1.12/src/pugixml.hpp"
#include "WwuLookUpData.h"
#include <set>
#include <map>


struct WwuSpaceSettings
{
public:
	std::string prefixToApply = "";	
	bool applyPrefix = false;
	bool applyNamingConventionCheck = false;
};

class NamingConventionModule
{ 
public:
	NamingConventionModule();
	void CheckNamingConvention();
	NamingConventionModule(std::string wwiseProjPath)
	{
		wwiseProjPath.erase(0, 1);
		wwiseProjPath.erase(wwiseProjPath.size() - 1);
		for (int i = static_cast<int>(wwiseProjPath.size()) - 1; i > 0; i--)
		{
			if (wwiseProjPath.at(i) == '\\')
				break;
			else
				wwiseProjPath.erase(i);
		}
		projectPath = wwiseProjPath;
	}

private:
	void ScanWorkUnitData(std::string directory);
	void FetchWwuData(std::string path);
	void ApplyPrefix(std::string& namePath, std::string fullFolderName, const WwuSpaceSettings& newPrefix);
	void ScanWorkUnitXMLByPath(std::string wwuPath, std::string namePath);
	void IterateFolder(std::string path, std::string namePath);

	void ModularResolve(pugi::xml_node wwuNode, std::string namePath);
	std::string AddLastNamePathLayer(const std::string& currentNamePath, std::string newNodeName);


public:
	std::string levelSeparator = "_";

	bool applyActorMixerHierarchyPrefix = false;
	std::map<std::string, WwuSpaceSettings> wwuSpaceSettings;

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

