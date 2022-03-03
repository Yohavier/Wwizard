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
	std::string m_name;
	std::string m_guid;
	std::string m_path;
	QueryType m_structureType = FOLDER;
	std::vector<BaseQueryStructure*> subDir;

	BaseQueryStructure() = default;

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType)
		: m_name(name)
		, m_guid(guid)
		, m_path(path)
		, m_structureType(queryType)
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
	const BaseQueryStructure* GetCurrentSelection();
	void SetQuerySelection(std::string& guid);
	const std::string& GetCurrentSelectionGuid();

	void RunActiveQueries();

	BaseQueryStructure* m_wwiseQueryHierarchy;
	std::vector<std::string> resultList;
private:
	cWwizardWwiseClient* m_wwizardClient;
	std::string selectedGuid = "";
	std::map<std::string, BaseQueryStructure*> activeQueryDictionary;
	std::map<std::string, BaseQueryStructure*> allQueryDictionary;


};

