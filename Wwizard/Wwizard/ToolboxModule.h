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
	void GetEmptyEvents();
	void GatherFadersInHierarchy(); 

	void DeleteEmptyEvent();
	void ResetFader();


	const std::map<std::string, QueryResultFile>& GetEventResultFiles();
	const std::map<std::string, QueryResultFile>& GetFaderResultFiles();

private:
	void GetEmptyEventsInHierarchy(const std::string& guid, const std::vector<std::string>& optionList);
	bool IsEventEmptyOrInvalid(const int& count, const std::string& guid);
	bool AreAllActionsEmpty(const std::string& parentGuid);

	void IterateResetFaders(const std::string& guid, const std::vector<std::string>& optionList);
	bool CheckObjectType(const std::string& type);


public:
	bool deleteEmptyEventsForAllEvents = false;
	std::string ignoreFaderNote = "@FIgnore";

private:
	std::map<std::string, QueryResultFile> eventQueryResultFiles;
	std::map<std::string, QueryResultFile> faderQueryResultFiles;
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::set<std::string> faderContainers = { "ActorMixer", "Sound", "BlendContainer", "SwitchContainer", "RandomSequenceContainer", "AuxBus", "Bus", "MusicTrack", "MusicPlaylistContainer", "MusicSegment", "MusicSwitchContainer"};
};

