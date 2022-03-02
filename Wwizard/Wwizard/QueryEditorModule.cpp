#include "QueryEditorModule.h"

QueryEditorModule::QueryEditorModule()
{
    m_wwiseQueries = new BaseQueryStructure();
	std::cout << "Queryeditor Module loaded!" << std::endl;
}

void QueryEditorModule::Init(cWwizardWwiseClient* wwizardClient)
{
    std::cout << "Init QueryModule" << std::endl;
	m_wwizardClient = wwizardClient;
    FetchWwiseQueries();
}

void QueryEditorModule::FetchWwiseQueries()
{
    using namespace AK::WwiseAuthoringAPI;
    AkJson options(AkJson::Map{
    { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type"), AkVariant("path")}} });

    AkJson parentObject;
    parentObject = m_wwizardClient->GetObjectFromPath("\\Queries", options);
    BaseQueryStructure parentStructureFolder = BaseQueryStructure(parentObject["return"].GetArray()[0]["name"].GetVariant().GetString(), parentObject["return"].GetArray()[0]["id"].GetVariant().GetInt32(), parentObject["return"].GetArray()[0]["path"].GetVariant().GetString(), QueryType::FOLDER);

    m_wwiseQueries->m_guuid = parentStructureFolder.m_guuid;
    m_wwiseQueries->m_name = parentStructureFolder.m_name;
    m_wwiseQueries->m_path = parentStructureFolder.m_path;

    FetchWwiseFolderchildren(m_wwiseQueries, options);
}

void QueryEditorModule::FetchWwiseFolderchildren(BaseQueryStructure* parentStructureFolder, AkJson options)
{
    AkJson queryResult = m_wwizardClient->GetChildrenFromPath(parentStructureFolder->m_path, options);

    for (const auto& object : queryResult["return"].GetArray())
    {
        if (object["type"].GetVariant().GetString() == "Query")
        {
            BaseQueryStructure* newQuery = new BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetInt32(), object["path"].GetVariant().GetString(), QueryType::WWISEQUERY);
            parentStructureFolder->subDir.push_back(newQuery);
        }
        else
        {
            BaseQueryStructure* newFolder = new BaseQueryStructure(object["name"].GetVariant().GetString(), object["id"].GetVariant().GetInt32(), object["path"].GetVariant().GetString(), QueryType::FOLDER);
            parentStructureFolder->subDir.push_back(newFolder);
            FetchWwiseFolderchildren(newFolder, options);
        }
    }
}