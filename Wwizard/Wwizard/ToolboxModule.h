#pragma once
#include <string>
#include "WwizardWwiseClient.h"


class ToolboxModule
{
public:
	ToolboxModule() = delete;
	ToolboxModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	void GatherEmptyEvents();
	void DeleteEmptyEvent(const std::string& guid);
	void ResetFadersInHierarchy();
	void EnableStreamingAfterThreshold();

private:
	void IterateGatherEmptyEvents(const std::string& guid, const AkJson& options);
	bool IsEventEmptyOrInvalid(const int& count, const std::string& guid);
	bool AreAllActionsEmpty(const std::string& parentGuid);

public:
	bool deleteEmptyEventsForAllEvents = false;
	double streamingThreshold = 5;
private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;
};

