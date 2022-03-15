#pragma once
#include <iostream>
#include "WwizardWwiseClient.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "QueryEditorModule.h"
#include "SettingHandler.h"

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
	Dockspace(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr < SettingHandler>& settingHandler);
	void Render(bool* p_open);

private:
	void CreateMenuBar();
	void CreateQueryEditor(bool* p_open);
	void CreateHomeLayout();
	void SetLayout(Layout newLayout);

	//Settings
	void ShowSettings(bool* p_open);

	//Query Module
	void ShowActiveList();
	void ShowResultList();
	void ShowWaapiQueries();
	void ShowWaqlQueries();
	void ShowWwiseQueries(const BaseQueryStructure& queryObject);
	void SetAddQueryPopUp();
	void HandleDetailsWindow(bool* p_open);
	void SetDefaultStyle();

private:
	Layout currentLayout;

	std::unique_ptr<WwizardWwiseClient>& wwizarWwiseClient;
	std::unique_ptr<SettingHandler>& settingHandler;

	std::unique_ptr<QueryEditorModule> queryEditorModule = nullptr;
};



