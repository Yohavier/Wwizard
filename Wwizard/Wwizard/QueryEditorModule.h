#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "WwizardWwiseClient.h"

class QueryEditorModule
{
public:
	QueryEditorModule();
	void Init(cWwizardWwiseClient* wwizardClient);
	void FetchWwiseQueries();
	const std::vector<std::string>& GetWwiseQueries();

private:
	std::vector<std::string> m_wwiseQueries;
	cWwizardWwiseClient* m_wwizardClient;
};

