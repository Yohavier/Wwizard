#pragma once
#include <iostream>
#include <tchar.h>
#include <memory>
#include <stdio.h>

#include "WwizardWwiseClient.h"
#include "QueryEditorModule.h"
#include "SettingHandler.h"
#include "SortOriginalsModule.h"
#include "NamingConventionModule.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include "imgui_stdlib.h"


enum class Layout
{
	HOME,
	QUERYEDITOR,
	NAMINGCONVENTION,
	SORTORIGINALS,
	SETTINGS
};

class GUI
{
public:
	GUI(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler, std::unique_ptr<QueryEditorModule>&, std::unique_ptr<SortOriginalsModule>&, std::unique_ptr<NamingConventionModule>&);
	~GUI();
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

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	HWND hwnd;
	WNDCLASSEX wc;
	ImGuiIO* io = nullptr;
};



