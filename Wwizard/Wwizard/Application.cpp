#pragma once
#include "Application.h" 

Application::Application()
{
    settingsHandler.reset(new SettingHandler());
    wwizardWwiseClient.reset(new WwizardWwiseClient(settingsHandler));
    queryEditorModule.reset(new QueryEditorModule(wwizardWwiseClient));
    sortOriginalsModule.reset(new SortOriginalsModule(settingsHandler->GetWwisProjectPathRef()));
    namingConventionModule.reset(new NamingConventionModule(settingsHandler->GetWwisProjectPathRef(), wwizardWwiseClient));
    colorCodingModule.reset(new ColorCodingModule(wwizardWwiseClient));
    toolboxModule.reset(new ToolboxModule(wwizardWwiseClient));
    myGUI.reset(new GUI(wwizardWwiseClient, settingsHandler, queryEditorModule, sortOriginalsModule, namingConventionModule, toolboxModule, colorCodingModule));

    Loop();
}

void Application::Loop()
{
    m_isRunning = true;
    while (m_isRunning)
    {
        if (settingsHandler->settingChangeFlag)
        {
            settingsHandler->settingChangeFlag = false;
            namingConventionModule->OnSettingsChange(settingsHandler->GetWwisProjectPathRef(), settingsHandler->GetSDKPath());
            sortOriginalsModule->OnSettingsChange(settingsHandler->GetWwisProjectPathRef(), settingsHandler->GetSDKPath());
        }
        myGUI->Render(m_isRunning);
    }
}



