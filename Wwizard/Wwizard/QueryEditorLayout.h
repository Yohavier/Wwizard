#pragma once
#include "BaseLayout.h"
#include "QueryEditorModule.h"

class QueryEditorLayout : public BaseLayout
{
public:
	QueryEditorLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<QueryEditorModule>& queryEditorModule);
	void RenderLayout() override;

private:
	void ShowActiveQueries();
	void ShowQueryResults();
	void ShowWaapiQueries();
	void ShowWaqlQueries();
	void ShowWwiseQueries(const BaseQueryStructure& queryObject);
	void ShowQueryCreator();
	void ShowDetails();
	std::unique_ptr<QueryEditorModule>& queryEditorModule;
};

