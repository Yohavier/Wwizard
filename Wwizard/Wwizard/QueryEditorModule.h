#pragma once
#include <vector>
#include <string>
#include "WwizardWwiseClient.h"
#include <map>
#include <guiddef.h>

/*
* TODO :
*-after restart pc first open of app doesnt connect to wwise
*/

enum class QueryType
{
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
	QueryType structureType = QueryType::FOLDER;
	std::vector<BaseQueryStructure> subHierarchy;
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
	int color;

	QueryResult(std::string name, std::string guid, std::string path, std::string type, int color)
		: name(name)
		, guid(guid)
		, path(path)
		, type(type)
		, color(color)
	{}
};


class QueryEditorModule
{
public:
	QueryEditorModule(const std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	~QueryEditorModule();

	void AddToActiveQueryList(std::string guid);
	void RemoveFromActiveQueryList(const std::string guid);
	const std::map<std::string, BaseQueryStructure&>& GetActiveQueryList();
	void SetQuerySelection(const std::string& guid);
	const std::string& GetCurrentSelectionGuid();
	void RunActiveQueries();
	void ResetQueryModule(const std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	const std::string ConvertQueryTypeToString(QueryType& queryType)
	{
		switch (queryType)
		{
			case QueryType::WAAPIQUERY:
				return "Waapi";
			case QueryType::WAQLQUERY:
				return "Waql";
			case QueryType::WWISEQUERY:
				return "Wwise";
			default:
				return "";
		}
	}
	
	template<typename TReturn, typename TMap>
	TReturn FindInMap(const TMap& findMap)
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
	void CreateNewQuery(const std::string name, const QueryType type, const std::string arg);
	const std::string GetCurrentArgAsString();
	void SaveChangedQuery(std::string newName, std::string newArg, std::string guid);
	void DeleteQuery(std::string guid);

private:
	const std::string GenerateGuid();
	void FetchWwiseQueries();
	void FetchWwiseFolderchildren(BaseQueryStructure& parentStructureFolder, const AkJson options);
	const BaseQueryStructure* const GetCurrentSelectionQuery();
	const QueryResult* const GetCurrentSelectionFile();
	void LoadWaapiQueriesFromJson();
	void LoadWaqlQueriesFromJson();
	void SaveCustomQueriesToJson();
	void AddQueryToAllQueriesMap(BaseQueryStructure& newQuery);


public:
	std::map<std::string, BaseQueryStructure> allQueries;
	std::unique_ptr<BaseQueryStructure> wwiseQueryHierarchy;
	std::map<std::string, QueryResult> queryResultFiles;

	std::map<std::string, const BaseQueryStructure&> waapiQueries;
	std::map<std::string, const BaseQueryStructure&> waqlQueries;
	std::map<std::string, const BaseQueryStructure&> wwiseQueries;
	std::string selectedGuid = "";


private:
	const std::unique_ptr<WwizardWwiseClient>& wwizardClient;
	std::map<std::string, BaseQueryStructure&> activeQueryDictionary;
};


