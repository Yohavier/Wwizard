#include "Application.h" 

Application::Application()
{
    wwizardWwiseClient.reset(new WwizardWwiseClient());
    settingsHandler.reset(new SettingHandler());

    //create loop thread if not connected, trying to reconnect and reopen wwise
    wwizardWwiseClient->Connect(settingsHandler);

    queryEditorModule.reset(new QueryEditorModule(wwizardWwiseClient));
    sortOriginalsModule.reset(new SortOriginalsModule(settingsHandler->GetWwisProjectPathRef()));
    namingConventionModule.reset(new NamingConventionModule(settingsHandler->GetWwisProjectPathRef()));

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


