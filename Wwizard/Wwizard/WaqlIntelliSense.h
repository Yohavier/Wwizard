#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "memory"
#include "WwizardWwiseClient.h"

class WaqlIntelliSense
{
public:
	WaqlIntelliSense() = delete;
	WaqlIntelliSense(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
		:wwizardClient(wwizardClient)
	{		
	}

	void FindFittingCommands(std::string input);

	void OnConnected();

private:
	void SearchMatches(std::string inputCommand, const std::string* arrayBegin, int size);

	bool IsInArray(std::string* arrayBegin, int size, std::string searchWord);
	bool iequals(const std::string& a, const std::string& b);
	void SplitStringInVector(std::string& input);
	bool IsEnclosedString(const std::string& part);
	bool IsNumber(const std::string& part, bool allowNegative);
	void SearchMatchesInValueExpression(std::string inputCommand);
	bool IsInSet(std::set<std::string>& mySet, std::string& searchWord);
	bool IsRegEx(int& vectorCommandIndex);

	bool HandleSource();
	bool HandleSourceObject();
	bool HandleSourceType();
	bool HandleSourceQuery();
	bool HandleSourceSearch();
	bool HandleSourceProject();
	
	bool HandleTransform();
	bool HandleTransformWhere();
	bool HandleTransformSkip();
	bool HandleTransformTake();
	bool HandleTransformSelect();
	bool HandleTransformOrderBy();
	bool HandleTransformDistinct();


	bool HandleValueExpression();
	bool HandleBooleanExpression();
	bool HandleObjectExpression(const bool allowConcatenation, const bool allowComma);

	bool IsBufferReadable();
	bool IsWordInValueExpressions(const std::string& firstWordPart);
	bool MatchingWordA(const std::string A, const std::string B);

public:
	std::set<std::string> fittingCommands;
	std::set<std::string> suggestions;

private:
	int readIndex = 0;
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

	std::set<std::string> valueProperties;

	std::string valueExpressions[32] = { "id", "shortId", "name", "notes", "type", "classId", "category", "filePath", "path", "activeSource", "isPlayable", "childrenCount",
									   "pluginName", "convertedWemFilePath", "originalWavFilePath", "soundbankBnkFilePath", "workunitIsDefault", "workunitType",
									   "workunitIsDirty", "isExplicitMute", "isExplicitSolo", "isImplicitMute", "isImplicitSolo", "maxDurationSource.is",
									   "maxDurationSource.trimmedDuration", "duration.min", "duration.max", "duration.type", "audioSourceTrimValues.trimBegin",
									   "audioSourceTrimValues.trimEnd", "maxRadiusAttenuation.id", "maxRadiusAttenuation.radius" };


	std::string concatenableReferences[38] = { "Attenuation", "AttachableMixerInput", "AudioDevice", "Conversion", "ControlInput", "DefaultSwitchOrState", "DestinationContextObject",
								   "Effect0", "Effect1", "Effect2", "Effect3", "HdrPeakOutputGameParameter", "LayerCrossFadeControlInput", "Mixer", "MidiTarget", "OutputBus",
								   "PostEventTarget", "ReflectionsAuxSend", "SwitchGroupOrStateGroup", "StartObject", "SourceContextObject", "Target", "TargetObject",
								   "TrackSequenceSwitch", "UserAuxSend0", "UserAuxSend1", "UserAuxSend2", "UserAuxSend3", "notes", "parent", "owner","workunit", 
								   "musicTransitionRoot", "musicPlaylistRoot", "maxDurationSource", "maxRadiusAttenuation", "audioSourceLanguage", "switchContainerChildContext"
	};

	std::string nonConcatenableReferences[5] = { "children", "descendants", "this", "ancestors", "referencesTo" };

	std::string logicalOperator[2] = { "and", "or" };
};