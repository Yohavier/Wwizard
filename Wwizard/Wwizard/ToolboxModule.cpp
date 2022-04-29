#include "ToolboxModule.h"

ToolboxModule::ToolboxModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	:wwizardClient(wwizardClient)
{

}

void ToolboxModule::GatherEmptyEvents()
{
	eventQueryResultFiles.clear();
	AkJson waapiResult; 
	std::vector<std::string> optionList = { "id", "name", "category", "path", "color", "childrenCount", "type" };

	if (deleteEmptyEventsForAllEvents)
	{
		AkJson result = wwizardClient->GetObjectFromPath("\\Events", optionList);
		if (!result["return"].IsEmpty())
		{
			for (const auto& results : result["return"].GetArray())
			{
				IterateGatherEmptyEvents(results["id"].GetVariant().GetString(), optionList);
			}
		}
	}
	else
	{
		waapiResult = wwizardClient->GetSelectedObjectsInWwise(optionList);
		if (!waapiResult["objects"].IsEmpty())
		{
			if (waapiResult["objects"].GetArray()[0]["category"].GetVariant().GetString() == "Events")
			{
				for (const auto& results : waapiResult["objects"].GetArray())
				{
					IterateGatherEmptyEvents(results["id"].GetVariant().GetString(), optionList);
				}
			}
		}
	}
}

void ToolboxModule::IterateGatherEmptyEvents(const std::string& guid, const std::vector<std::string>& optionList)
{
	AkJson result = wwizardClient->GetChildrenFromGuid(guid, optionList);
	for (const auto& evt : result["return"].GetArray())
	{
		if (evt["type"].GetVariant().GetString() != "Event")
		{
			IterateGatherEmptyEvents(evt["id"].GetVariant().GetString(), optionList);
		}
		else
		{
			if (IsEventEmptyOrInvalid(evt["childrenCount"].GetVariant().GetUInt8(), evt["id"].GetVariant().GetString()))
			{
				eventQueryResultFiles.insert({ evt["id"].GetVariant().GetString(), QueryResultFile(
					evt["name"].GetVariant().GetString(),
					evt["id"].GetVariant().GetString(),
					evt["path"].GetVariant().GetString(),
					evt["type"].GetVariant().GetString(),
					evt["color"].GetVariant().GetUInt8()) });
			}
		}
	}
}

bool ToolboxModule::IsEventEmptyOrInvalid(const int& count, const std::string& guid)
{
	return count == 0 || AreAllActionsEmpty(guid);
}

bool ToolboxModule::AreAllActionsEmpty(const std::string& parentGuid)
{
	std::vector < std::string> optionList = { "id" };
	AkJson result = wwizardClient->GetChildrenFromGuid(parentGuid, optionList);
	
	for (const auto& evt : result["return"].GetArray())
	{
		AkJson actionresult = wwizardClient->GetPropertyFromGuid(evt["id"].GetVariant().GetString());

		for (auto& a : actionresult["return"].GetArray())
		{
			if (a["Target"]["id"].GetVariant().GetString() != "{00000000-0000-0000-0000-000000000000}")
			{
				return false;
			}
		}
	}
	return true;
}

void ToolboxModule::DeleteEmptyEvent()
{
	for (const auto& evt : eventQueryResultFiles)
	{
		wwizardClient->DeleteObjectInWwise(evt.second.guid);
	}
	eventQueryResultFiles.clear();
}


void ToolboxModule::GatherFadersInHierarchy()
{
	faderQueryResultFiles.clear();

	std::vector<std::string> optionList = { "id", "path", "type", "name", "color", "category", "childrenCount", "notes"};
	AkJson result = wwizardClient->GetSelectedObjectsInWwise(optionList);

	if (!result["objects"].IsEmpty())
	{
		std::string category = result["objects"].GetArray()[0]["category"].GetVariant().GetString();
		if (category == "Actor-Mixer Hierarchy" || category == "Master-Mixer Hierarchy" || category == "Interactive Music Hierarchy")
		{
			for (const auto& fader : result["objects"].GetArray())
			{
				IterateResetFaders(fader["id"].GetVariant().GetString(), optionList);

				if (CheckObjectType(fader["type"].GetVariant().GetString()))
				{
					if (fader["notes"].GetVariant().GetString().find(ignoreFaderNote) == std::string::npos)
					{
						faderQueryResultFiles.insert({ fader["id"].GetVariant().GetString(), QueryResultFile(fader["name"].GetVariant().GetString(),
						fader["id"].GetVariant().GetString(),
						fader["path"].GetVariant().GetString(),
						fader["type"].GetVariant().GetString(),
						fader["color"].GetVariant().GetUInt8()) });
					}
				}
			}
		}
	}
}

void ToolboxModule::IterateResetFaders(const std::string& guid, const std::vector<std::string>& optionList)
{
	AkJson result = wwizardClient->GetChildrenFromGuid(guid, optionList);
	for (const auto& fader : result["return"].GetArray())
	{
		if (CheckObjectType(fader["type"].GetVariant().GetString()))
		{
			if (fader["notes"].GetVariant().GetString().find(ignoreFaderNote) == std::string::npos)
			{
				faderQueryResultFiles.insert({ fader["id"].GetVariant().GetString(), QueryResultFile(fader["name"].GetVariant().GetString(),
						fader["id"].GetVariant().GetString(),
						fader["path"].GetVariant().GetString(),
						fader["type"].GetVariant().GetString(),
						fader["color"].GetVariant().GetUInt8()) });
			}
		}
		IterateResetFaders(fader["id"].GetVariant().GetString(), optionList);
	}
}

bool ToolboxModule::CheckObjectType(const std::string& type)
{
	if (faderContainers.find(type) != faderContainers.end())
	{
		return true;
	}
	return false;
}

void ToolboxModule::ResetFader()
{
	for (const auto& fader : faderQueryResultFiles)
	{
		std::string property = "Volume";
		if (fader.second.type == "Bus" || fader.second.type == "AuxBus")
			property = "BusVolume";

		wwizardClient->SetProperty<int>(fader.second.path, property, 0);
	}
	faderQueryResultFiles.clear();
}

const std::map<std::string, QueryResultFile>& ToolboxModule::GetEventResultFiles()
{
	return eventQueryResultFiles;
}

const std::map<std::string, QueryResultFile>& ToolboxModule::GetFaderResultFiles()
{
	return faderQueryResultFiles;
}