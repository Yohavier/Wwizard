#pragma once
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <memory>

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
	SETTINGS,
	TOOLBOX
};

class GUI
{
public:
	GUI(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler, std::unique_ptr<QueryEditorModule>&, std::unique_ptr<SortOriginalsModule>&, std::unique_ptr<NamingConventionModule>&);
	~GUI();
	void Render(bool* p_open);

private:
	void RenderLayoutToolBox();
	void RenderLayoutQueryEditor();
	void RenderLayoutHome();
	void RenderLayoutSettings();
	void RenderLayoutSortOriginals();
	void RenderLayoutNamingConvention();

	void CreateTaskBar();
	void SetLayout(Layout newLayout);

	void ShowActiveQueries();
	void ShowQueryResults();
	void ShowWaapiQueries();
	void ShowWaqlQueries();
	void ShowWwiseQueries(const BaseQueryStructure& queryObject);
	void ShowQueryCreator();
	void ShowDetails();

	void SetDefaultStyle();
	const ImColor& ConvertWwiseColorToRGB(const int& wwiseColorID);

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
	typedef void(GUI::* func_ptr) (void);
	std::map<Layout, func_ptr> layouts = { {Layout::TOOLBOX, &GUI::RenderLayoutToolBox},
												{Layout::NAMINGCONVENTION, &GUI::RenderLayoutNamingConvention },
												{Layout::HOME, &GUI::RenderLayoutHome},
												{Layout::QUERYEDITOR, &GUI::RenderLayoutQueryEditor},
												{Layout::SETTINGS, &GUI::RenderLayoutSettings},
												{Layout::SORTORIGINALS, &GUI::RenderLayoutSortOriginals} 
	};


	std::map<int, ImColor> wwiseColors = { {0, ImColor(83, 83, 83)}, {1, ImColor(54, 62, 200)}, {2, ImColor(54, 62, 200)},
										  {3, ImColor(25, 85, 203)}, {4, ImColor(7, 104, 104)}, {5, ImColor(86, 115, 12)},
										  {6, ImColor(120, 113, 16)}, {7,ImColor(121, 87, 21)}, {8, ImColor(120, 66, 12)},
										  {9, ImColor(114, 56, 43)}, {10, ImColor(137, 35, 36)}, {11, ImColor(124, 38, 125)},
										  {12, ImColor(115, 42, 151)}, {13, ImColor(88, 54, 174)}, {14, ImColor(135, 135, 135)},
										  {15, ImColor(106, 111, 194)}, {16, ImColor(99, 131, 197)},{17, ImColor(67, 137, 137)},
										  {18, ImColor(83, 147, 83)}, {19, ImColor(128, 152, 61)}, {20, ImColor(160, 151, 38)},
										  {21, ImColor(171, 135, 62)}, {22, ImColor(174, 121, 65)}, {23, ImColor(174, 100, 85)},
										  {24, ImColor(185, 91, 91)}, {25, ImColor(169, 80, 170)}, {26, ImColor(187, 85, 189)},
										  {27, ImColor(134, 96, 226)} };
};



