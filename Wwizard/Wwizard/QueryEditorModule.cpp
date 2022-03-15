#include "QueryEditorModule.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "rapidjson/RapidJsonUtils.h"
#include <random>

QueryEditorModule::~QueryEditorModule()
{
    SaveCustomQueriesToJson();
}

QueryEditorModule::QueryEditorModule(std::unique_ptr<WwizardWwiseClient>& exwwizardClient)
    : wwizardClient(exwwizardClient)
{
    if (exwwizardClient->IsConnected())
    {
        wwiseQueryHierarchy = new BaseQueryStructure();
        FetchWwiseQueries();
        LoadWaapiQueriesFromJson();
        LoadWaqlQueriesFromJson();
    }
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

void QueryEditorModule::FetchWwiseFolderchildren(BaseQueryStructure* parentStructureFolder, const AkJson options)
{
    AkJson queryResult = wwizardClient->GetChildrenFromPath(parentStructureFolder->path, options);

    for (const auto& object : queryResult["return"].GetArray())
    {
        if (object["type"].GetVariant().GetString() == "Query")
        {
            BaseQueryStructure* newQuery = new BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), QueryType::WWISEQUERY);
            
            AddQueryToAllQueriesMap(*newQuery);
            parentStructureFolder->subHierarchy.push_back(newQuery);

            auto it = allQueries.find(object["id"].GetVariant().GetString());
            if (it != allQueries.end())
            {
                wwiseQueries.insert({ it->second.guid, it->second });      
            }
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
    activeQueryDictionary.insert({ guid, allQueries.find(guid)->second });
}

void QueryEditorModule::RemoveFromActiveQueryList(const std::string guid)
{
    auto it = activeQueryDictionary.find(guid);
    if (it != activeQueryDictionary.end()) 
    {
        activeQueryDictionary.erase(it);
    }
    RunActiveQueries();
}

std::map<std::string, BaseQueryStructure&> QueryEditorModule::GetActiveQueryList()
{
    return activeQueryDictionary;
}

const BaseQueryStructure* QueryEditorModule::GetCurrentSelectionQuery()
{
    auto it = wwiseQueries.find(selectedGuid);
    if (it != wwiseQueries.end())
    {
        return &(it->second);
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

void QueryEditorModule::SetQuerySelection(const std::string& guid)
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
        if (it->second.structureType == QueryType::WWISEQUERY)
        {
            AkJson queryResult = wwizardClient->RunQueryFromGuuid(it->second.guid);

            if (!queryResult["return"].IsEmpty())
            {
                for (const auto& object : queryResult["return"].GetArray())
                {
                    queryResultFiles.insert({ object["id"].GetVariant().GetString(), QueryResult(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), object["type"].GetVariant().GetString()) });
                }
            }
        } 
        else
        {
            if (!it->second.arg.IsEmpty())
            {
                AkJson queryResult = wwizardClient->RunCustomQuery(it->second.arg);
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
}

void QueryEditorModule::LoadWaqlQueriesFromJson()
{
    FILE* fp = fopen("../SavedData/test.json", "rb");
    char* readBuffer = new char[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    assert(d["WaqlQueries"].IsArray());

    for (int i = 0; i < static_cast<int>(d["WaqlQueries"].Size()); i++)
    {
        AkJson test;
        test.FromRapidJson(d["WaqlQueries"][i]["arg"], test);

        BaseQueryStructure newQuery = BaseQueryStructure(d["WaqlQueries"][i]["name"].GetString(), d["WaqlQueries"][i]["guid"].GetString(), d["WaqlQueries"][i]["path"].GetString(), QueryType::WAQLQUERY, test);
        AddQueryToAllQueriesMap(newQuery);

        auto it = allQueries.find(d["WaqlQueries"][i]["guid"].GetString());
        if (it != allQueries.end())
        {
            waqlQueries.insert({ it->second.guid, it->second });
        }
    }
}

void QueryEditorModule::LoadWaapiQueriesFromJson()
{
    using namespace rapidjson;

    FILE* fp = fopen("../SavedData/test.json", "rb"); 
    char* readBuffer = new char[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    
    Document d;
    d.ParseStream(is);
    fclose(fp);

    assert(d["WaapiQueries"].IsArray());

    for (int i = 0; i < static_cast<int>(d["WaapiQueries"].Size()); i++)
    {
        AkJson test;
        test.FromRapidJson(d["WaapiQueries"][i]["arg"], test);
        
        BaseQueryStructure newQuery = BaseQueryStructure(d["WaapiQueries"][i]["name"].GetString(), d["WaapiQueries"][i]["guid"].GetString(), d["WaapiQueries"][i]["path"].GetString(), QueryType::WAAPIQUERY, test);
        AddQueryToAllQueriesMap(newQuery);

        auto it = allQueries.find(d["WaapiQueries"][i]["guid"].GetString());
        if (it != allQueries.end())
        {
            waapiQueries.insert({ it->second.guid, it->second });
        }
    }
}

void QueryEditorModule::SaveCustomQueriesToJson()
{   
    rapidjson::Document d;
    d.SetObject();

    rapidjson::Value waapiQueries;  
    waapiQueries.SetArray();
    
    for (const auto& AkJsonQuery : QueryEditorModule::waapiQueries)
    {
        rapidjson::Value rapidJsonQuery(rapidjson::kObjectType);
        {
            rapidjson::Value name;
            name = rapidjson::StringRef(AkJsonQuery.second.name.c_str());
            rapidJsonQuery.AddMember("name", name, d.GetAllocator());

            rapidjson::Value guid; 
            guid = rapidjson::StringRef(AkJsonQuery.second.guid.c_str());
            rapidJsonQuery.AddMember("guid", guid, d.GetAllocator());

            rapidjson::Value path;
            path = rapidjson::StringRef(AkJsonQuery.second.path.c_str());
            rapidJsonQuery.AddMember("path", path, d.GetAllocator());

            rapidjson::Value waapiValue;
            RapidJsonUtils::ToRapidJson(AkJsonQuery.second.arg, waapiValue, d.GetAllocator());
            rapidJsonQuery.AddMember("arg", waapiValue, d.GetAllocator());
        }
        waapiQueries.PushBack(rapidJsonQuery, d.GetAllocator());
    }
    d.AddMember("WaapiQueries", waapiQueries, d.GetAllocator());
 

    rapidjson::Value waqlQueries;
    waqlQueries.SetArray();

    for (const auto& AkJsonQuery : QueryEditorModule::waqlQueries)
    {
        rapidjson::Value rapidJsonQuery(rapidjson::kObjectType);
        {
            rapidjson::Value name;
            name = rapidjson::StringRef(AkJsonQuery.second.name.c_str());
            rapidJsonQuery.AddMember("name", name, d.GetAllocator());

            rapidjson::Value guid;
            guid = rapidjson::StringRef(AkJsonQuery.second.guid.c_str());
            rapidJsonQuery.AddMember("guid", guid, d.GetAllocator());

            rapidjson::Value path;
            path = rapidjson::StringRef(AkJsonQuery.second.path.c_str());
            rapidJsonQuery.AddMember("path", path, d.GetAllocator());

            rapidjson::Value waqlValue;
            RapidJsonUtils::ToRapidJson(AkJsonQuery.second.arg, waqlValue, d.GetAllocator());
        
            rapidJsonQuery.AddMember("arg", waqlValue, d.GetAllocator());
        }
        waqlQueries.PushBack(rapidJsonQuery, d.GetAllocator());
    }
    d.AddMember("WaqlQueries", waqlQueries, d.GetAllocator());


    FILE* fp = fopen("../SavedData/test.json", "wb");
    char* writeBuffer = new char[65536];
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    d.Accept(writer);

    fclose(fp);
}

void QueryEditorModule::CreateNewQuery(std::string name, QueryType type, std::string arg)
{
    std::string guid = GenerateGuid();
    if (type == QueryType::WAAPIQUERY)
    {
        rapidjson::Document a;
        a.Parse(arg.c_str());
        AkJson waapiQuery;
        AkJson::FromRapidJson(a, waapiQuery);

        BaseQueryStructure newQuery=BaseQueryStructure(name, guid, "", type, waapiQuery);
        AddQueryToAllQueriesMap(newQuery);

        auto it = allQueries.find(guid);
        if (it != allQueries.end())
        {
            waapiQueries.insert({ it->second.guid, it->second });
        }
    } 
    else if (type == QueryType::WAQLQUERY)
    {
        AkJson argJson(AkJson::Map{ {{"waql", AkVariant(arg)}}});
        BaseQueryStructure newQuery = BaseQueryStructure(name, guid, "", type, argJson);
        AddQueryToAllQueriesMap(newQuery);

        auto it = allQueries.find(guid);
        if (it != allQueries.end())
        {
            waqlQueries.insert({ it->second.guid, it->second });
        }
    }
}

void QueryEditorModule::AddQueryToAllQueriesMap(BaseQueryStructure newQuery)
{
    allQueries.insert({ newQuery.guid, newQuery });
}

std::string QueryEditorModule::GenerateGuid()
{
    return std::to_string(((long long)rand() << 32) | rand());
}

void QueryEditorModule::ResetQueryModule(const std::unique_ptr<WwizardWwiseClient>& wwizardClient)
{
    waapiQueries.clear();
    waqlQueries.clear();
    wwiseQueries.clear();
    allQueries.clear();
    selectedGuid = "";
    queryResultFiles.clear();

    if (wwizardClient->IsConnected())
    {
        delete wwiseQueryHierarchy;
        FetchWwiseQueries();
        LoadWaapiQueriesFromJson();
        LoadWaqlQueriesFromJson();
    }
}