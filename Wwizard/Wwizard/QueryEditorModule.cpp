#include "QueryEditorModule.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <random>

#include <chrono>

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "rapidjson/RapidJsonUtils.h"


QueryEditorModule::~QueryEditorModule()
{
    SaveCustomQueriesToJson();
}

QueryEditorModule::QueryEditorModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
    : wwizardClient(wwizardClient)
{
    if (wwizardClient->IsConnected())
    {
        wwiseQueryHierarchy.reset( new BaseQueryStructure());
        FetchWwiseQueries();
        LoadWaapiQueriesFromJson();
        LoadWaqlQueriesFromJson();
    }
}

void QueryEditorModule::FetchWwiseQueries()
{
    std::vector<std::string> optionList = { "id", "name", "type", "path" };
    AkJson parentObject = wwizardClient->GetObjectFromPath("\\Queries", optionList);
    BaseQueryStructure parentStructureFolder = BaseQueryStructure(parentObject["return"].GetArray()[0]["name"].GetVariant().GetString(), parentObject["return"].GetArray()[0]["id"].GetVariant().GetString(), parentObject["return"].GetArray()[0]["path"].GetVariant().GetString(), QueryType::FOLDER);

    wwiseQueryHierarchy->guid = parentStructureFolder.guid;
    wwiseQueryHierarchy->name = parentStructureFolder.name;
    wwiseQueryHierarchy->path = parentStructureFolder.path;

    FetchWwiseFolderchildren(*wwiseQueryHierarchy, optionList);
}

void QueryEditorModule::FetchWwiseFolderchildren(BaseQueryStructure& parentStructureFolder, const std::vector<std::string>& optionList)
{
    AkJson queryResult = wwizardClient->GetChildrenFromGuid(parentStructureFolder.guid, optionList);

    for (const auto& object : queryResult["return"].GetArray())
    {
        if (object["type"].GetVariant().GetString() == "Query")
        {
            //created on the stack of that function is just not fading, because its a memory leak        
            BaseQueryStructure newQuery = BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), QueryType::WWISEQUERY);
            
            AddQueryToAllQueriesMap(newQuery);
            

            auto it = allQueries.find(object["id"].GetVariant().GetString());
            if (it != allQueries.end())
            {
                wwiseQueries.insert({ it->second.guid, it->second });      
                parentStructureFolder.subHierarchy.emplace_back(it->second);
            }
        }
        else
        {
            BaseQueryStructure newFolder = BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), QueryType::FOLDER);
            parentStructureFolder.subHierarchy.emplace_back(newFolder);
            FetchWwiseFolderchildren(parentStructureFolder.subHierarchy.back(), optionList);
        }
    }
}

void QueryEditorModule::AddToActiveQueryList(const std::string& guid)
{
    activeQueryDictionary.insert({ allQueries.find(guid)->second.guid, allQueries.find(guid)->second });
}

void QueryEditorModule::RemoveFromActiveQueryList(const std::string& guid)
{
    auto it = activeQueryDictionary.find(guid);
    if (it != activeQueryDictionary.end()) 
    {
        activeQueryDictionary.erase(it);
    }
}

const std::map<std::string, BaseQueryStructure&>& QueryEditorModule::GetActiveQueryList()
{
    return activeQueryDictionary;
}

const BaseQueryStructure* const QueryEditorModule::GetCurrentSelectionQuery()
{
    auto it = allQueries.find(selectedGuid);
    if (it != allQueries.end())
    {
        return &(it->second);
    }

    return nullptr;
}

const QueryResultFile* const QueryEditorModule::GetCurrentSelectionFile()
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
                    if (object["color"].IsEmpty())
                    {
                        queryResultFiles.insert({ object["id"].GetVariant().GetString(), QueryResultFile(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), object["type"].GetVariant().GetString(), 0) });
                    }
                    else
                    {
                        queryResultFiles.insert({ object["id"].GetVariant().GetString(), QueryResultFile(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), object["type"].GetVariant().GetString(), object["color"].GetVariant().GetInt32()) });
                    } 
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
                        if (object["color"].IsEmpty())
                        {
                            queryResultFiles.insert({ object["id"].GetVariant().GetString(), QueryResultFile(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), object["type"].GetVariant().GetString(), 0) });
                        }
                        else
                        {
                            queryResultFiles.insert({ object["id"].GetVariant().GetString(), QueryResultFile(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetString(), object["path"].GetVariant().GetString(), object["type"].GetVariant().GetString(), object["color"].GetVariant().GetInt32()) });
                        }
                    }
                }
            }
        }
    }
}

void QueryEditorModule::LoadWaqlQueriesFromJson()
{
    auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/CustomQueries.json";
    FILE* fp = fopen(path.c_str(), "rb");
    if (fp != 0)
    {
        char* readBuffer = new char[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document d;
        d.ParseStream(is);
        fclose(fp);
        if (d.HasMember("WaqlQueries"))
        {
            for (int i = 0; i < static_cast<int>(d["WaqlQueries"].Size()); i++)
            {
                if (d["WaqlQueries"][i].HasMember("arg") && d["WaqlQueries"][i].HasMember("name") && d["WaqlQueries"][i].HasMember("guid") && d["WaqlQueries"][i].HasMember("path"))
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
        }    
    }
}

void QueryEditorModule::LoadWaapiQueriesFromJson()
{
    auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/CustomQueries.json";
    FILE* fp = fopen(path.c_str(), "rb"); 
    if (fp != 0)
    {
        char* readBuffer = new char[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document d;
        d.ParseStream(is);
        fclose(fp);

        if (d.HasMember("WaapiQueries"))
        {
            for (int i = 0; i < static_cast<int>(d["WaapiQueries"].Size()); i++)
            {
                if (d["WaapiQueries"][i].HasMember("name") && d["WaapiQueries"][i].HasMember("guid") && d["WaapiQueries"][i].HasMember("path") && d["WaapiQueries"][i].HasMember("arg"))
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

    auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/CustomQueries.json";
    FILE* fp = fopen(path.c_str(), "wb");
    if (fp != 0)
    {
        char* writeBuffer = new char[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        d.Accept(writer);

        fclose(fp);
    }
}

void QueryEditorModule::CreateNewQuery(const std::string name, const QueryType type, const std::string arg)
{
    unsigned long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::string guid = std::to_string(now);
    
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

const std::string QueryEditorModule::GetCurrentArgAsString()
{
    auto query = GetCurrentSelectionQuery();
    if (query != nullptr)
    {
        rapidjson::Document doc;

        if (query->structureType == QueryType::WAQLQUERY)
        {
            RapidJsonUtils::ToRapidJson(query->arg["waql"], doc, doc.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);
    
            std::string rawArg = buffer.GetString();
    
            //remove " in the front and back
            rawArg.erase(0, 1);
            rawArg.erase(rawArg.size() - 1);

            std::string processedArg = "";
            for (int i = 0; i < rawArg.size(); i++)
            {
                if (rawArg.at(i) == '\\')
                {
                    if (rawArg.at(i + 1) == '\\')
                    {
                        processedArg += "\\";
                        i++;
                    }
                }
                else
                {
                    processedArg += rawArg.at(i);
                }
            }

            return processedArg;
        }
        else if (query->structureType == QueryType::WAAPIQUERY)
        {
            RapidJsonUtils::ToRapidJson(query->arg, doc, doc.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);
            return buffer.GetString();
        }
    }
    return "";
}

void QueryEditorModule::OnConnectionStatusChange(const bool newConnectionStatus)
{
    if (newConnectionStatus)
    {
        ResetQueryModule();
    } 
}

void QueryEditorModule::AddQueryToAllQueriesMap(BaseQueryStructure& newQuery)
{
    allQueries.insert({ newQuery.guid, newQuery });
}

void QueryEditorModule::ResetQueryModule()
{
    queryResultFiles.clear();
    activeQueryDictionary.clear();
    waapiQueries.clear();
    waqlQueries.clear();
    wwiseQueries.clear();
    allQueries.clear();
    selectedGuid = "";
    

    if (wwizardClient->IsConnected())
    {
        wwiseQueryHierarchy.reset(new BaseQueryStructure());
        FetchWwiseQueries();
        LoadWaapiQueriesFromJson();
        LoadWaqlQueriesFromJson();
    }
    else
    {
        wwiseQueryHierarchy.release();
    }
}

void QueryEditorModule::SaveChangedQuery(const std::string newName, const std::string newArg, const std::string guid)
{
    auto it = allQueries.find(guid);
    if (it != allQueries.end())
    {
        it->second.name = newName;

        rapidjson::Document a;
        if (it->second.structureType == QueryType::WAQLQUERY)
        {
            AkJson query(AkJson::Map{ {{"waql", AkVariant(newArg)}} });
            it->second.arg = query;
        }
        else if (it->second.structureType == QueryType::WAAPIQUERY)
        {
            a.Parse(newArg.c_str());
            AkJson query;
            AkJson::FromRapidJson(a, query);
            it->second.arg = query;
        }
    }
}

void QueryEditorModule::DeleteQuery(const std::string& guid)
{
    waqlQueries.erase(guid);
    waapiQueries.erase(guid);
    activeQueryDictionary.erase(guid);
    allQueries.erase(guid);
}

const std::string& QueryEditorModule::GetQueryTypeAsString(const QueryType& queryType)
{
    return queryTypeAsString.find(queryType)->second;
}

const std::map<std::string, const BaseQueryStructure&>& QueryEditorModule::GetWaapiQueries()
{
    return waapiQueries;
}

const std::map<std::string, const BaseQueryStructure&>& QueryEditorModule::GetWaqlQueries()
{
    return waqlQueries;
}

const std::map<std::string, const BaseQueryStructure&>& QueryEditorModule::GetWwiseQueries()
{
    return wwiseQueries;
}

const std::map<std::string, QueryResultFile>& QueryEditorModule::GetQueryResultFiles()
{
    return queryResultFiles;
}

const std::map<std::string, BaseQueryStructure>& QueryEditorModule::GetAllQueries()
{
    return allQueries;
}
 
const std::unique_ptr<BaseQueryStructure>& QueryEditorModule::GetWwiseQueryHierarchy()
{
    return wwiseQueryHierarchy;
}

void QueryEditorModule::ResetQueryResults()
{
    queryResultFiles.clear();
}

void QueryEditorModule::RunNodeGraph(MyNode* node)
{
    ResetQueryResults();
    queryResultFiles = CalculateNode(node);
}

std::map<std::string, QueryResultFile> QueryEditorModule::CalculateNode(MyNode* node)
{
    std::map<std::string, QueryResultFile> emptyDefault;
    if (node->Title == "And")
    {
        bool firstAnd = true;
        std::map<std::string, QueryResultFile> tempAnd;
        for (const auto& connection : node->Connections)
        {
            MyNode* nextNode = static_cast<MyNode*>(connection.OutputNode);
            if (nextNode->nodeGuid != node->nodeGuid)
            {
                auto newResults = CalculateNode(nextNode);
                if (firstAnd)
                {
                    for (auto& result : newResults)
                    {
                        tempAnd.insert({ result.first, result.second });
                    }
                }
                else
                {
                    auto tempRemoveAnd = tempAnd;
                    for (auto& alreadyCaptured : tempRemoveAnd)
                    {
                        if (newResults.find(alreadyCaptured.first) == newResults.end())
                        {
                            tempAnd.erase(alreadyCaptured.first);
                        }
                    }
                }
                firstAnd = false;
            }
        }
        return tempAnd;
    }
    else if (node->Title == "Or")
    {
        std::map<std::string, QueryResultFile> tempOr;
        for (const auto& connection : node->Connections)
        {
            MyNode* nextNode = static_cast<MyNode*>(connection.OutputNode);
            if (nextNode->nodeGuid != node->nodeGuid)
            {
                for (auto& result : CalculateNode(nextNode))
                {
                    tempOr.insert({ result.first, result.second });
                }
            }
        }
        return tempOr;
    }
    else if (node->Title == "Output")
    {
        for (const auto& connection : node->Connections)
        {
            MyNode* nextNode = static_cast<MyNode*>(connection.OutputNode);
            if (nextNode->nodeGuid != node->nodeGuid)
            {
                return CalculateNode(nextNode);
            }
        }
    }
    else if (node->Title == "Query")
    {
        static_cast<MyQueryNode*>(node)->RerunQuery();
        return static_cast<MyQueryNode*>(node)->queryResults;
    }

    return emptyDefault;
}