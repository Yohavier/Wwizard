#include "Application.h" 

Application::Application()
{
    
    settingsHandler.reset(new SettingHandler());
    wwizardWwiseClient.reset(new WwizardWwiseClient(settingsHandler));
    queryEditorModule.reset(new QueryEditorModule(wwizardWwiseClient));
    sortOriginalsModule.reset(new SortOriginalsModule(settingsHandler->GetWwisProjectPathRef()));
    namingConventionModule.reset(new NamingConventionModule(settingsHandler->GetWwisProjectPathRef(), wwizardWwiseClient));

    myGUI.reset(new GUI(wwizardWwiseClient, settingsHandler, queryEditorModule, sortOriginalsModule, namingConventionModule));

    Loop();
}

void Application::Loop()
{
    m_isRunning = true;
    while (m_isRunning)
    {
        myGUI->Render(m_isRunning);
    }
}


