#pragma once
#include <iostream>
#include "WwizardWwiseClient.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "QueryEditorModule.h"
#include "SettingHandler.h"

namespace wwizard
{
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
		Dockspace(WwizardWwiseClient* wwizardWwiseClient, SettingHandler* settingHandler);
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
		void ShowWwiseQueries(BaseQueryStructure* queryObject);
		void SetAddQueryPopUp();
		void HandleDetailsWindow(bool* p_open);
		void SetDefaultStyle();

	private:
		Layout currentLayout;
		WwizardWwiseClient* wwizarWwiseClient;
		SettingHandler* settingHandler;

		QueryEditorModule queryEditorModule = QueryEditorModule();
	};
}


