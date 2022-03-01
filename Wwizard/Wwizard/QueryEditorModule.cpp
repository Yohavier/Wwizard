#include "QueryEditorModule.h"

QueryEditorModule::QueryEditorModule()
{
	std::cout << "Queryeditor Module loaded!" << std::endl;
}

void QueryEditorModule::Init(cWwizardWwiseClient* wwizardClient)
{
	m_wwizardClient = wwizardClient;
    FetchWwiseQueries();
}

void QueryEditorModule::FetchWwiseQueries()
{
    using namespace AK::WwiseAuthoringAPI;

    AkJson args(AkJson::Map{
        { "from", AkJson::Map{
            { "path", AkJson::Array{ AkVariant("\\Queries") } } } }
        });

    // Connect to Wwise Authoring on localhost.
    AkJson options(AkJson::Map{
        { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type")}} });

    m_wwiseQueries.clear();

    m_wwizardClient->WalkProject(args, options, m_wwiseQueries);
}

const std::vector<std::string>& QueryEditorModule::GetWwiseQueries()
{
    return m_wwiseQueries;
}