#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "QueryEditorModule.h"

class WaqlIntelliSense
{
public:
	WaqlIntelliSense(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
		:wwizardClient(wwizardClient)
	{		
	}

	void FindFittingCommands(std::string input);

	void OnConnected();

private:
	void SearchMatches(std::string& inputCommand, const std::string* arrayBegin, int size);

	bool IsInArray(std::string* arrayBegin, int size, std::string searchWord);
	bool iequals(const std::string& a, const std::string& b);
	
	void CheckSource(int& vectorCommandIndex);
	void CheckTransform(int& vectorCommandIndex);
	void SplitStringInVector(std::string& input);
	bool IsEnclosedString(const std::string& part);
	bool IsNumber(const std::string& part);
	void SearchMatchesInValueExpression(std::string& inputCommand);
	bool IsInSet(std::set<std::string>& mySet, std::string& searchWord);
	bool IsRegEx(int& vectorCommandIndex);

	bool FindAllConcatenations(int& vectorCommandIndex, std::string* arrayToLook, int arraySize, std::string concatenator);
	bool FindAllStringConcatenationsByComma(int& vectorCommandIndex);
	bool HandleConcatenations(int& vectorCommandIndex, std::string* arrayToLook, int arraySize);
	bool HandleConditionFollowUpValue(int& vectorCommandIndex);

public:
	std::set<std::string> fittingCommands;
	bool isSourceFree;

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;
	std::vector<std::string> vectorCommand;

	std::string sourceInit[1] = {"from"};

	std::string transformInit[6] = { "skip", "take", "select", "orderby", "distinct", "where" };

	std::string objectExpression[14] = { "parent", "children", "this", "descendants", "ancestors", "referencesTo", "owner", "workunit", "musicTransitionRoot", 
										 "musicPlaylistRoot", "maxDurationSource", "maxRadiusAttenuation", "audioSourceLanguage", "switchContainerChildContext" };

	std::string concatenableObjectExpression[9] = { "parent", "owner", "workunit", "musicTransitionRoot", "musicPlaylistRoot", 
												   "maxDurationSource", "maxRadiusAttenuation", "audioSourceLanguage", "switchContainerChildContext" };


	std::string sourceNeedingString[3] = { "object", "query", "search" };
	std::string sourceForCheck[5] = { "object", "query", "search", "type", "project"};

	

	std::string specialOperation[2] = { ".", "," };

	std::string referenceObjects[61] = { "AcousticTexture", "Action", /*"ActionException",*/ "ActorMixer", "Attenuation", "AudioDevice", "AudioSource", "AuxBus",
											"BlendContainer", "BlendTrack", "Bus",
											"ControlSurfaceBinding", "ControlSurfaceBindingGroup", "ControlSurfaceSession", "Conversion", "Curve", "CustomState",
											"DialogueEvent", "Effect", "Event", "ExternalSource", /*"ExternalSourceFile",*/
											"Folder", "GameParameter", "Language",
											"Metadata", "MidiParameter", "MixingSession", "Modifier", "ModulatorEnvelope", "ModulatorLfo", "ModulatorTime", /*"MultiSwitchEntry",*/ "MusicClip",
											"MusicClipMidi", "MusicCue", "MusicEventCue", "MusicFade", "MusicPlaylistContainer", "MusicPlaylistItem", "MusicSegment", "MusicStinger",
											"MusicSwitchContainer", "MusicTrack", "MusicTrackSequence", "MusicTransition",
											/*"ObjectSettingAssoc",*/ "Panner", /*"ParamControl",*/ /*"Path",*/ "Platform", /*"PluginDataSource",*/ "Position", "Project", "Query", "RandomSequenceContainer",
											"SearchCriteria", "Sound", "SoundBank", "SoundcasterSession", "State", "StateGroup", "Switch", "SwitchContainer", "SwitchGroup", "Trigger",
											"UserProjectSettings", "WorkUnit"
	};

	std::string conditions[7] = { "=", "<", ">", ">=", "<=", ":", "!="};

	std::string categories[21] = { "Audio Devices", "Master-Mixer Hierarchy", "Actor-Mixer Hierarchy", "Interactive Music Hierarchy", "Events", "Dynamic Dialogue", "SoundBanks",
									  "Switches", "States", "Game Parameters", "Triggers", "Effects", "Attenuations", "Conversion Settings", "Modulators", "Virtual Acoustics", "Metadata",
									  "Soundcaster Session", "Mixing Session", "Control Surface Session", "Queries" };

	std::set<std::string> valueExpressions;
};