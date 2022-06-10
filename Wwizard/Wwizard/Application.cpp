#pragma once
#include "Application.h" 

Application::Application()
{
    settingsHandler.reset(new SettingHandler());
    wwizardWwiseClient.reset(new WwizardWwiseClient(settingsHandler));
    queryEditorModule.reset(new QueryEditorModule(wwizardWwiseClient));
    sortOriginalsModule.reset(new SortOriginalsModule(settingsHandler->GetWwiseProjectPathRefHyphen()));
    namingConventionModule.reset(new NamingConventionModule(settingsHandler->GetWwiseProjectPathRefHyphen(), wwizardWwiseClient));
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
            namingConventionModule->OnSettingsChange(settingsHandler->GetWwiseProjectPathRefHyphen(), settingsHandler->GetSDKPathHyphen());
            sortOriginalsModule->OnSettingsChange(settingsHandler->GetWwiseProjectPathRefHyphen(), settingsHandler->GetSDKPathHyphen());
        }
        myGUI->Render(m_isRunning);
    }
}



