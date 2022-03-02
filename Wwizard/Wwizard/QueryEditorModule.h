#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "WwizardWwiseClient.h"

enum QueryType {
	WWISEQUERY,
	WAQLQUERY,
	WAAPIQUERY,
	FOLDER
};

struct BaseQueryStructure
{
	std::string m_name;
	int m_guuid;
	std::string m_path;
	QueryType m_structureType = FOLDER;
	std::vector<BaseQueryStructure*> subDir;

	BaseQueryStructure() = default;

	BaseQueryStructure(std::string name, int guuid, std::string path, QueryType queryType)
		: m_name(name)
		, m_guuid(guuid)
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

	BaseQueryStructure* m_wwiseQueries;

private:
	cWwizardWwiseClient* m_wwizardClient;
};

