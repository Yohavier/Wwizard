#pragma once
#include <iostream>
#include "WwizardWwiseClient.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "QueryEditorModule.h"
#include "SettingHandler.h"
#include "SortOriginalsModule.h"
#include "NamingConventionModule.h"

enum class Layout
{
	HOME,
	QUERYEDITOR,
	NAMINGCONVENTION,
	SORTORIGINALS,
	SETTINGS
};

class Dockspace
{
public:
	Dockspace(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler, std::unique_ptr<QueryEditorModule>&, std::unique_ptr<SortOriginalsModule>&, std::unique_ptr<NamingConventionModule>&);
	void Render(bool* p_open);

private:
	void CreateMenuBar();
	void CreateQueryEditor(bool* p_open);
	void ShowHome();
	void SetLayout(Layout newLayout);

	//Settings
	void ShowSettings(bool* p_open);

	//Query Module
	void ShowActiveQueries();
	void ShowQueryResults();
	void ShowWaapiQueries();
	void ShowWaqlQueries();
	void ShowWwiseQueries(const BaseQueryStructure& queryObject);
	void ShowQueryCreator();
	void ShowDetails(bool* p_open);

	//Sort Originals
	void ShowSortOriginalsModule();

	//Naming conventions
	void ShowNamingConventionModule();

	//Misc
	void SetDefaultStyle();
	ImColor ConvertWwiseColorToRGB(int wwiseColor);

private:
	Layout currentLayout;

	std::unique_ptr<WwizardWwiseClient>& wwizarWwiseClient;
	std::unique_ptr<SettingHandler>& settingHandler;

	std::unique_ptr<QueryEditorModule>& queryEditorModule;
	std::unique_ptr<SortOriginalsModule>& sortOriginalsModule;
	std::unique_ptr<NamingConventionModule>& namingConventionModule;
};



