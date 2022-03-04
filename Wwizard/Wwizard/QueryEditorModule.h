#pragma once
#include <vector>
#include <iostream>
#include <fstream>
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
	std::string arg;
	QueryType structureType = FOLDER;
	std::vector<BaseQueryStructure*> subHierarchy;

	BaseQueryStructure() = default;

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType)
		: name(name)
		, guid(guid)
		, path(path)
		, structureType(queryType)
	{}

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType, std::string arg)
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
	std::map<std::string, BaseQueryStructure*> GetActiveQueryList();

	const BaseQueryStructure* GetCurrentSelectionQuery();
	const QueryResult* GetCurrentSelectionFile();

	void SetQueryModuleSelection(std::string& guid);
	const std::string& GetCurrentSelectionGuid();

	void RunActiveQueries();

	void LoadWaapiQueriesFromJson();
	void SaveWaapiQueriesToJson();

	BaseQueryStructure* wwiseQueryHierarchy;
	std::map<std::string, QueryResult> queryResultFiles;
	std::map<std::string, BaseQueryStructure> waapiQueries;

private:
	cWwizardWwiseClient* wwizardClient;
	std::string selectedGuid = "";
	std::map<std::string, BaseQueryStructure*> activeQueryDictionary;
	std::map<std::string, BaseQueryStructure*> wwiseQueries;


};

