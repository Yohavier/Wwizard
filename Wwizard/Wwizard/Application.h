#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <tchar.h>
#include <memory>
#include "Dockspace.h"
#include "WwizardWwiseClient.h"

class Application
{
public:
	Application();
	void Loop();
	void ShutDown();

private:
	cWwizardWwiseClient* m_wwizardWwiseClient = nullptr;
	std::unique_ptr<wwizard::Dockspace> myDock = nullptr;
	
	bool m_isRunning = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	HWND hwnd;
	WNDCLASSEX wc;
	ImGuiIO* io = nullptr;
};

