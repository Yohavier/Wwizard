#pragma once
#include <iostream>
#include "imgui.h"
#include "imgui_internal.h"

namespace wwizardGUI
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
		Dockspace();
		void Render(bool* p_open);

	private:
		void CreateMenuBar();
		void CreateQueryEditor(bool* p_open);
		void CreateHomeLayout();
		void SetLayout(Layout newLayout);
		void ShowPlaceholderObject(const char* prefix, int uid);

	private:
		Layout currentLayout;
	};
}


