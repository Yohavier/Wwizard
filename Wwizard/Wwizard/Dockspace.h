#pragma once
#include <iostream>
#include "WwizardWwiseClient.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "QueryEditorModule.h"

namespace wwizard
{
	enum Layout
	{
		HOME,
		QUERYEDITOR,
		NAMINGCONVENTION,
		SORTORIGINALS
	};

	class Dockspace
	{
	public:
		Dockspace(cWwizardWwiseClient* wwizardWwiseClient);
		void Render(bool* p_open);

	private:
		void CreateMenuBar();
		void CreateQueryEditor(bool* p_open);
		void CreateHomeLayout();
		void SetLayout(Layout newLayout);
		void ShowPlaceholderObject(const char* prefix, int uid);

	private:
		Layout m_currentLayout;
		cWwizardWwiseClient* m_wwizarWwiseClient;

		QueryEditorModule m_queryEditorModule = QueryEditorModule();
	};
}


