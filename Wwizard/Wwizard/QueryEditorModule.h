#pragma once
#include <vector>
#include <string>
#include "WwizardWwiseClient.h"
#include <map>
#include "nlohmann/json.hpp"
#include <guiddef.h>

enum QueryType {
	WWISEQUERY,
	WAQLQUERY,
	WAAPIQUERY,
	FOLDER
};

struct BaseQueryStructure
{
	std::string name;
	std::string guid;
	std::string path;
	AkJson arg;
	QueryType structureType = FOLDER;
	std::vector<BaseQueryStructure*> subHierarchy;

	BaseQueryStructure() = default;

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType)
		: name(name)
		, guid(guid)
		, path(path)
		, structureType(queryType)
	{}

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType, AkJson arg)
		: name(name)
		, guid(guid)
		, path(path)
		, structureType(queryType)
		, arg(arg)
	{}
};

struct QueryResult
{
	std::string name;
	std::string guid;
	std::string path;
	std::string type;

	QueryResult(std::string name, std::string guid, std::string path, std::string type)
		: name(name)
		, guid(guid)
		, path(path)
		, type(type)
	{}
};


class QueryEditorModule
{
public:
	QueryEditorModule();
	~QueryEditorModule();
	void Init(cWwizardWwiseClient* wwizardClient);
	void FetchWwiseQueries();
	void FetchWwiseFolderchildren(BaseQueryStructure* parentStructureFolder, AkJson options);
	
	void AddToActiveQueryList(std::string guid);
	void RemoveFromActiveQueryList(const std::string guid);
	std::map<std::string, BaseQueryStructure&> GetActiveQueryList();

	const BaseQueryStructure* GetCurrentSelectionQuery();
	const QueryResult* GetCurrentSelectionFile();

	void SetQuerySelection(std::string& guid);
	const std::string& GetCurrentSelectionGuid();

	void RunActiveQueries();

	void LoadWaapiQueriesFromJson();
	void LoadWaqlQueriesFromJson();

	void SaveWaapiQueriesToJson();

	template<typename TReturn, typename TMap>
	TReturn FindInMap(TMap findMap)
	{
		auto it = findMap.find(selectedGuid);
		if (it != findMap.end())
		{
			return &(it->second);
		}
		else
		{
			return nullptr;
		}
	}

	void CreateNewQuery(std::string name, QueryType type, std::string arg);

	BaseQueryStructure* wwiseQueryHierarchy;
	std::map<std::string, QueryResult> queryResultFiles;

	std::map<std::string, BaseQueryStructure&> waapiQueries;
	std::map<std::string, BaseQueryStructure&> waqlQueries;
	std::map<std::string, BaseQueryStructure&> wwiseQueries;
	std::string selectedGuid = "";

	void AddQueryToAllQueriesMap(BaseQueryStructure* newQuery);

	std::map<std::string, BaseQueryStructure> allQueries;
private:


	std::string GenerateGuid();

	cWwizardWwiseClient* wwizardClient;
	
	std::map<std::string, BaseQueryStructure&> activeQueryDictionary;
};


