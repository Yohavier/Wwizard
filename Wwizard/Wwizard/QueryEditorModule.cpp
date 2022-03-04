#include "QueryEditorModule.h"

QueryEditorModule::QueryEditorModule()
{
    wwiseQueryHierarchy = new BaseQueryStructure();
	std::cout << "Queryeditor Module loaded!" << std::endl;
}

void QueryEditorModule::Init(cWwizardWwiseClient* wwizardClient)
{
    std::cout << "Init QueryModule" << std::endl;
	QueryEditorModule::wwizardClient = wwizardClient;
    FetchWwiseQueries();
}

void QueryEditorModule::FetchWwiseQueries()
{
    using namespace AK::WwiseAuthoringAPI;
    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path")}} });

    AkJson parentObject;
    parentObject = wwizardClient->GetObjectFromPath("\\Queries", options);
    BaseQueryStructure parentStructureFolder = BaseQueryStructure(parentObject["return"].GetArray()[0]["name"].GetVariant().GetString(), parentObject["return"].GetArray()[0]["id"].GetVariant().GetString(), parentObject["return"].GetArray()[0]["path"].GetVariant().GetString(), QueryType::FOLDER);

    wwiseQueryHierarchy->guid = parentStructureFolder.guid;
    wwiseQueryHierarchy->name = parentStructureFolder.name;
    wwiseQueryHierarchy->path = parentStructureFolder.path;

    FetchWwiseFolderchildren(wwiseQueryHierarchy, options);
}

void QueryEditorModule::FetchWwiseFolderchildren(BaseQueryStructure* parentStructureFolder, AkJson options)
{
    AkJson queryResult = wwizardClient->GetChildrenFromPath(parentStructureFolder->path, options);

    for (const auto& object : queryResult["return"].GetArray())
    {
        if (object["type"].GetVariant().GetString() == "Query")
        {
            BaseQueryStructure* newQuery = new BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), QueryType::WWISEQUERY);
            parentStructureFolder->subHierarchy.push_back(newQuery);
            allQueryDictionary.insert({ object["id"].GetVariant().GetString(), newQuery });
        }
        else
        {
            BaseQueryStructure* newFolder = new BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), QueryType::FOLDER);
            parentStructureFolder->subHierarchy.push_back(newFolder);
            FetchWwiseFolderchildren(newFolder, options);
        }
    }
}

void QueryEditorModule::AddToActiveQueryList(std::string guid)
{
    activeQueryDictionary.insert({ guid, allQueryDictionary.find(guid)->second });
}

void QueryEditorModule::RemoveFromActiveQueryList(const std::string guid)
{
    auto it = activeQueryDictionary.find(guid);
    if (it != activeQueryDictionary.end()) 
    {
        activeQueryDictionary.erase(it);
    }
}

std::map<std::string, BaseQueryStructure*> QueryEditorModule::GetActiveQueryList()
{
    return activeQueryDictionary;
}

const BaseQueryStructure* QueryEditorModule::GetCurrentSelection()
{
    auto it = allQueryDictionary.find(selectedGuid);
    if (it != allQueryDictionary.end())
    {
        return it->second;
    }
    return nullptr;
}


void QueryEditorModule::SetQuerySelection(std::string& guid)
{
    selectedGuid = guid;
}

const std::string& QueryEditorModule::GetCurrentSelectionGuid()
{
    return selectedGuid;
}

void QueryEditorModule::RunActiveQueries()
{
    for (auto it = activeQueryDictionary.begin(); it != activeQueryDictionary.end(); ++it)
    {
        AkJson queryResult = wwizardClient->RunQueryFromGuuid(it->second->guid);

        for (const auto& object : queryResult["return"].GetArray())
        {
            queryResultFiles.push_back(object["name"].GetVariant().GetString());
        }
    }
}
