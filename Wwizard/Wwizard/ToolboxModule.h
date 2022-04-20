#pragma once
#include <string>
#include <set>
#include "WwizardWwiseClient.h"


class ToolboxModule
{
public:
	ToolboxModule() = delete;
	ToolboxModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	void GatherEmptyEvents();

	void ResetFadersInHierarchy();
	void EnableStreamingAfterThreshold();

private:
	void DeleteEmptyEvent(const std::string& guid);
	void IterateGatherEmptyEvents(const std::string& guid, const AkJson& options);
	bool IsEventEmptyOrInvalid(const int& count, const std::string& guid);
	bool AreAllActionsEmpty(const std::string& parentGuid);

	void IterateResetFaders(const std::string& guid, const AkJson& options);
	bool CheckObjectType(const std::string& type);
	void ResetFader(const std::string& guid);

public:
	bool deleteEmptyEventsForAllEvents = false;
	double streamingThreshold = 5;
private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::set<std::string> faderContainers = { "ActorMixer", "Sound", "BlendContainer", "SwitchContainer", "RandomSequenceContainer"};
};

