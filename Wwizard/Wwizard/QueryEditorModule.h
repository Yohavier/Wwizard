#pragma once
#include <vector>
#include <string>
#include <map>

#include "WwizardWwiseClient.h"
#include "ResultFile.h"

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

class QueryEditorModule
{
public:
	QueryEditorModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	~QueryEditorModule();

	void AddToActiveQueryList(const std::string& guid);
	void RemoveFromActiveQueryList(const std::string& guid);
	void SetQuerySelection(const std::string& guid);
	void RunActiveQueries();
	void ResetQueryModule(const std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	void CreateNewQuery(const std::string name, const QueryType type, const std::string arg);
	void SaveChangedQuery(const std::string newName, const std::string newArg, const std::string guid);
	void DeleteQuery(const std::string& guid);

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

	//Getter
	const std::map<std::string, const BaseQueryStructure&>& GetWaapiQueries();
	const std::map<std::string, const BaseQueryStructure&>& GetWaqlQueries();
	const std::map<std::string, const BaseQueryStructure&>& GetWwiseQueries();
	const std::map<std::string, QueryResultFile>& GetQueryResultFiles();
	const std::map<std::string, BaseQueryStructure>& GetAllQueries();
	const std::unique_ptr<BaseQueryStructure>& GetWwiseQueryHierarchy();
	const std::string& GetQueryTypeAsString(const QueryType& queryType);
	const std::string& GetCurrentSelectionGuid();
	const std::map<std::string, BaseQueryStructure&>& GetActiveQueryList();
	const std::string GetCurrentArgAsString();

private:
	const std::string GenerateGuid();
	void FetchWwiseQueries();
	void FetchWwiseFolderchildren(BaseQueryStructure& parentStructureFolder, const std::vector<std::string>& optionList);
	const BaseQueryStructure* const GetCurrentSelectionQuery();
	const QueryResultFile* const GetCurrentSelectionFile();
	void LoadWaapiQueriesFromJson();
	void LoadWaqlQueriesFromJson();
	void SaveCustomQueriesToJson();
	void AddQueryToAllQueriesMap(BaseQueryStructure& newQuery);

private:
	std::map<std::string, BaseQueryStructure> allQueries;
	std::map<std::string, const BaseQueryStructure&> waapiQueries;
	std::map<std::string, const BaseQueryStructure&> waqlQueries;
	std::map<std::string, const BaseQueryStructure&> wwiseQueries;

	std::map<std::string, QueryResultFile> queryResultFiles;
	std::unique_ptr<BaseQueryStructure> wwiseQueryHierarchy;

	std::unique_ptr<WwizardWwiseClient>& wwizardClient;
	std::map<std::string, BaseQueryStructure&> activeQueryDictionary;
	std::string selectedGuid = "";

	const std::map<const QueryType, const std::string> queryTypeAsString = { {QueryType::WAAPIQUERY, "Waapi"},
																				{QueryType::WAQLQUERY, "Waql"},
																				{QueryType::WWISEQUERY, "Wwise"} };
};

