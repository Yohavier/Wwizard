#include "ToolboxModule.h"

ToolboxModule::ToolboxModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	:wwizardClient(wwizardClient)
{

}

void ToolboxModule::GatherEmptyEvents()
{
	AkJson result; 
	AkJson options(AkJson::Map{{ "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("category"), AkVariant("childrenCount")}} });
	
	if (deleteEmptyEventsForAllEvents)
	{
		result = wwizardClient->GetObjectFromPath("\\Events", options);
		if (!result["return"].GetArray().empty())
		{
			for (const auto& results : result["return"].GetArray())
			{
				IterateGatherEmptyEvents(results["id"].GetVariant().GetString(), options);
			}
		}
	}
	else
	{
		wwizardClient->GetCurrentSelectedObjectsInWwise(result, options);
		if (!result["objects"].GetArray().empty())
		{
			if (result["objects"].GetArray()[0]["category"].GetVariant().GetString() == "Events")
			{
				for (const auto& results : result["objects"].GetArray())
				{
					IterateGatherEmptyEvents(results["id"].GetVariant().GetString(), options);
				}
			}
		}
	}
}

void ToolboxModule::IterateGatherEmptyEvents(const std::string& guid, const AkJson& options)
{
	AkJson result = wwizardClient->GetChildrenFromGuid(guid, options);
	for (const auto& evt : result["return"].GetArray())
	{
		if (evt["type"].GetVariant().GetString() != "Event")
		{
			IterateGatherEmptyEvents(evt["id"].GetVariant().GetString(), options);
		}
		else
		{
			if (IsEventEmptyOrInvalid(evt["childrenCount"].GetVariant().GetUInt8(), evt["id"].GetVariant().GetString()))
			{
				DeleteEmptyEvent(evt["id"].GetVariant().GetString());
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
	AkJson options(AkJson::Map{{ "return", AkJson::Array{ AkVariant("id")}}});
	AkJson result = wwizardClient->GetChildrenFromGuid(parentGuid, options);
	
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

void ToolboxModule::DeleteEmptyEvent(const std::string& guid)
{
	wwizardClient->DeleteObjectInWwise(guid);
}

void ToolboxModule::ResetFadersInHierarchy()
{
}

void ToolboxModule::EnableStreamingAfterThreshold()
{
}
