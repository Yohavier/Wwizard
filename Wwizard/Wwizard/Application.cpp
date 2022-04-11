#include "Application.h" 

#if defined( AK_ENABLE_ASSERTS )

void SampleAssertHook(const char* in_pszExpression, const char* in_pszFileName, int in_lineNumber)
{}

AkAssertHook g_pAssertHook = SampleAssertHook;

#endif

Application::Application()
{
    wwizardWwiseClient.reset(new WwizardWwiseClient());
    settingsHandler.reset(new SettingHandler());
    wwizardWwiseClient->Connect(settingsHandler);

    queryEditorModule.reset(new QueryEditorModule(wwizardWwiseClient));
    sortOriginalsModule.reset(new SortOriginalsModule());
    namingConventionModule.reset(new NamingConventionModule(settingsHandler->GetWwisProjectPathRef()));

    myGUI.reset(new GUI(wwizardWwiseClient, settingsHandler, queryEditorModule, sortOriginalsModule, namingConventionModule));
}

void Application::Loop()
{
    m_isRunning = true;
    while (m_isRunning)
    {
        myGUI->Render(&m_isRunning);
    }
}


