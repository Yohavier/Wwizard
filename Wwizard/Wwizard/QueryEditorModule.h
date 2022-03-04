#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "WwizardWwiseClient.h"
#include <map>

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
	QueryType structureType = FOLDER;
	std::vector<BaseQueryStructure*> subHierarchy;

	BaseQueryStructure() = default;

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType)
		: name(name)
		, guid(guid)
		, path(path)
		, structureType(queryType)
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
	void Init(cWwizardWwiseClient* wwizardClient);
	void FetchWwiseQueries();
	void FetchWwiseFolderchildren(BaseQueryStructure* parentStructureFolder, AkJson options);
	
	void AddToActiveQueryList(std::string guid);
	void RemoveFromActiveQueryList(const std::string guid);
	std::map<std::string, BaseQueryStructure*> GetActiveQueryList();

	const BaseQueryStructure* GetCurrentSelectionQuery();
	const QueryResult* GetCurrentSelectionFile();

	void SetQueryModuleSelection(std::string& guid);
	const std::string& GetCurrentSelectionGuid();

	void RunActiveQueries();

	BaseQueryStructure* wwiseQueryHierarchy;
	std::map<std::string, QueryResult> queryResultFiles;
	
private:
	cWwizardWwiseClient* wwizardClient;
	std::string selectedGuid = "";
	std::map<std::string, BaseQueryStructure*> activeQueryDictionary;
	std::map<std::string, BaseQueryStructure*> allQueryDictionary;

};

