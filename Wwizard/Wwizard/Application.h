#pragma once
#include "GUI.h"
#include "WwizardWwiseClient.h"
#include "SettingHandler.h"
#include "NamingConventionModule.h"
#include "QueryEditorModule.h"
#include "SortOriginalsModule.h"
#include "ToolboxModule.h"
#include "ColorCodingModule.h"
#include "WaqlIntelliSense.h"

#include <memory>

class Application
{
public:
	Application();


private:
	void Loop();

private:
	std::unique_ptr<WwizardWwiseClient> wwizardWwiseClient = nullptr;
	std::unique_ptr<SettingHandler> settingsHandler = nullptr;
	std::unique_ptr<GUI> myGUI = nullptr;

	std::unique_ptr<QueryEditorModule> queryEditorModule = nullptr;
	std::unique_ptr<SortOriginalsModule> sortOriginalsModule = nullptr;
	std::unique_ptr<NamingConventionModule> namingConventionModule = nullptr;
	std::unique_ptr<ToolboxModule> toolboxModule = nullptr;
	std::unique_ptr<ColorCodingModule> colorCodingModule = nullptr;

	bool m_isRunning = true;
};

