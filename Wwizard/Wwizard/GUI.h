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
#include "ToolboxModule.h"
#include "ColorCodingModule.h"

#include "SortOriginalsLayout.h"
#include "NamingConventionLayout.h"
#include "ColorCodingLayout.h"
#include "QueryEditorLayout.h"
#include "ToolboxLayout.h"
#include "SettingLayout.h"
#include "HomeLayout.h"

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
	TOOLBOX,
	COLORCODING
};

class GUI
{
public:
	GUI(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, 
		std::unique_ptr<SettingHandler>& settingHandler, 
		std::unique_ptr<QueryEditorModule>& queryModule, 
		std::unique_ptr<SortOriginalsModule>& sortOriginalsModule, 
		std::unique_ptr<NamingConventionModule>& namingConventionModule, 
		std::unique_ptr<ToolboxModule>& toolboxModule, 
		std::unique_ptr<ColorCodingModule>& colorCodingModule);

	~GUI();
	void Render(bool& isRunning);

private:
	void RenderLayoutToolbox();
	void RenderLayoutQueryEditor();
	void RenderLayoutHome();
	void RenderLayoutSettings();
	void RenderLayoutSortOriginals();
	void RenderLayoutNamingConvention();
	void RenderColorCodingModule();

	void CreateTaskBar(bool& isRunning);
	void SetLayout(const Layout newLayout);

	void SetDefaultStyle(); 

private:
	Layout currentLayout;

	std::unique_ptr<WwizardWwiseClient>& wwizarWwiseClient;
	std::unique_ptr<SettingHandler>& settingHandler;
	std::unique_ptr<QueryEditorModule>& queryEditorModule;
	std::unique_ptr<SortOriginalsModule>& sortOriginalsModule;
	std::unique_ptr<NamingConventionModule>& namingConventionModule;
	std::unique_ptr<ToolboxModule>& toolboxModule;
	std::unique_ptr<ColorCodingModule>& colorCodingModule;

	std::unique_ptr<ColorCodingLayout> colorCodingLayout;
	std::unique_ptr<QueryEditorLayout> queryEditorLayout;
	std::unique_ptr<SettingLayout> settingLayout;
	std::unique_ptr<HomeLayout> homeLayout;
	std::unique_ptr<SortOriginalsLayout> sortOriginalsLayout;
	std::unique_ptr<NamingConventionLayout> namingConventionLayout;
	std::unique_ptr<ToolboxLayout> toolboxLayout;

	HWND hwnd;
	WNDCLASSEX wc;
	ImGuiIO* io = nullptr;

	typedef void(GUI::* func_ptr) (void);
	std::map<Layout, func_ptr> layouts = { {Layout::TOOLBOX, &GUI::RenderLayoutToolbox},
												{Layout::NAMINGCONVENTION, &GUI::RenderLayoutNamingConvention },
												{Layout::HOME, &GUI::RenderLayoutHome},
												{Layout::QUERYEDITOR, &GUI::RenderLayoutQueryEditor},
												{Layout::SETTINGS, &GUI::RenderLayoutSettings},
												{Layout::SORTORIGINALS, &GUI::RenderLayoutSortOriginals},
												{Layout::COLORCODING, &GUI::RenderColorCodingModule}
	};

	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};



