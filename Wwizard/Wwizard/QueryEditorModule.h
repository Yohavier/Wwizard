#pragma once
#include <vector>
#include <string>
#include "WwizardWwiseClient.h"
#include <map>
#include <guiddef.h>
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
	QueryEditorModule(const std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	~QueryEditorModule();

	void AddToActiveQueryList(std::string guid);
	void RemoveFromActiveQueryList(const std::string guid);
	const std::map<std::string, BaseQueryStructure&>& GetActiveQueryList();
	void SetQuerySelection(const std::string& guid);
	const std::string& GetCurrentSelectionGuid();
	void RunActiveQueries();
	void ResetQueryModule(const std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	const std::string GetQueryTypeAsString(const QueryType& queryType);
	
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

	//Getter
	const std::map<std::string, const BaseQueryStructure&>& GetWaapiQueries();
	const std::map<std::string, const BaseQueryStructure&>& GetWaqlQueries();
	const std::map<std::string, const BaseQueryStructure&>& GetWwiseQueries();
	const std::map<std::string, QueryResultFile>& GetQueryResultFiles();
	const std::map<std::string, BaseQueryStructure>& GetAllQueries();
	const std::unique_ptr<BaseQueryStructure>& GetWwiseQueryHierarchy();

private:
	const std::string GenerateGuid();
	void FetchWwiseQueries();
	void FetchWwiseFolderchildren(BaseQueryStructure& parentStructureFolder, const AkJson options);
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

	const std::unique_ptr<WwizardWwiseClient>& wwizardClient;
	std::map<std::string, BaseQueryStructure&> activeQueryDictionary;
	std::string selectedGuid = "";
};


