#pragma once
#include <string>
#include <set>
#include "WwizardWwiseClient.h"
#include "ResultFile.h"

class ToolboxModule
{
public:
	ToolboxModule() = delete;
	ToolboxModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	void GatherEmptyEvents();

	void GatherFadersInHierarchy(); 
	void DeleteEmptyEvent();
	void ResetFader();

private:
	
	void IterateGatherEmptyEvents(const std::string& guid, const AkJson& options);
	bool IsEventEmptyOrInvalid(const int& count, const std::string& guid);
	bool AreAllActionsEmpty(const std::string& parentGuid);

	void IterateResetFaders(const std::string& guid, const AkJson& options);
	bool CheckObjectType(const std::string& type);


public:
	bool deleteEmptyEventsForAllEvents = false;
	double streamingThreshold = 5;
	std::map<std::string, QueryResultFile> eventQueryResultFiles;
	std::map<std::string, QueryResultFile> faderQueryResultFiles;

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::set<std::string> faderContainers = { "ActorMixer", "Sound", "BlendContainer", "SwitchContainer", "RandomSequenceContainer", "AuxBus", "Bus", "MusicTrack", "MusicPlaylistContainer", "MusicSegment", "MusicSwitchContainer"};
	
	
};

