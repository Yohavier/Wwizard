#include "QueryEditorModule.h"
#include "rapidjson/document.h"

QueryEditorModule::QueryEditorModule()
{
    wwiseQueryHierarchy = new BaseQueryStructure();
	std::cout << "Queryeditor Module loaded!" << std::endl;

    LoadWaapiQueriesFromJson();
}

QueryEditorModule::~QueryEditorModule()
{
    SaveWaapiQueriesToJson();
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
            wwiseQueries.insert({ object["id"].GetVariant().GetString(), newQuery });
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
    activeQueryDictionary.insert({ guid, wwiseQueries.find(guid)->second });
}

void QueryEditorModule::RemoveFromActiveQueryList(const std:: string guid)
{
    auto it = activeQueryDictionary.find(guid);
    if (it != activeQueryDictionary.end()) 
    {
        activeQueryDictionary.erase(it);
    }
    RunActiveQueries();
}

std::map<std::string, BaseQueryStructure*> QueryEditorModule::GetActiveQueryList()
{
    return activeQueryDictionary;
}

const BaseQueryStructure* QueryEditorModule::GetCurrentSelectionQuery()
{
    auto it = wwiseQueries.find(selectedGuid);
    if (it != wwiseQueries.end())
    {
        return it->second;
    }

    return nullptr;
}

const QueryResult* QueryEditorModule::GetCurrentSelectionFile()
{
    auto it = queryResultFiles.find(selectedGuid);
    if (it != queryResultFiles.end())
    {
        return &(it->second);
    }
    return nullptr;
}


void QueryEditorModule::SetQueryModuleSelection(std::string& guid)
{
    selectedGuid = guid;
}

const std::string& QueryEditorModule::GetCurrentSelectionGuid()
{
    return selectedGuid;
}

void QueryEditorModule::RunActiveQueries()
{
    queryResultFiles.clear();
    for (auto it = activeQueryDictionary.begin(); it != activeQueryDictionary.end(); ++it)
    {
        if (it->second->structureType == QueryType::WWISEQUERY)
        {
            AkJson queryResult = wwizardClient->RunQueryFromGuuid(it->second->guid);

            if (!queryResult["return"].IsEmpty())
            {
                for (const auto& object : queryResult["return"].GetArray())
                {
                    queryResultFiles.insert({ object["id"].GetVariant().GetString(), QueryResult(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), object["type"].GetVariant().GetString()) });
                }
            }
        }
    }
}

void QueryEditorModule::LoadWaapiQueriesFromJson()
{
    using json = nlohmann::json;
    

    std::ifstream i("..\\SavedData\\test.json", std::ifstream::binary);
    json j;
    i >> j;

    for (auto& object : j["WaapiQueries"])
    {
        waapiQueries.insert({ object["guid"], BaseQueryStructure(object["name"], object["guid"], object["path"], QueryType::WAAPIQUERY) });

        auto it = waapiQueries.find(object["guid"]);
        if (it != waapiQueries.end())
        {
            wwiseQueries.insert({ it->second.guid, &it->second });
        }     
    }

    std::ofstream MyFile("..\\SavedData\\test.json");
    MyFile << std::setw(4) << j << std::endl;
}

void QueryEditorModule::SaveWaapiQueriesToJson()
{
    using json = nlohmann::json;
    json savedWaapiQueries;

    std::ofstream SavingJsonFile("..\\SavedData\\test.json");

    for (const auto& object : waapiQueries)
    {
        savedWaapiQueries["WaapiQueries"].push_back({ {"name", object.second.name},{"guid", object.second.guid}, {"path", object.second.path} });
    }

    SavingJsonFile << std::setw(4) << savedWaapiQueries << std::endl;
}
