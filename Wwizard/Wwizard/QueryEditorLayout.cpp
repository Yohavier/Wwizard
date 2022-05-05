#include "QueryEditorLayout.h"
#include "WwiseColors.h"

QueryEditorLayout::QueryEditorLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<QueryEditorModule>& queryEditorModule)
	: BaseLayout(wwizardWwiseClient)
	, queryEditorModule(queryEditorModule)
{
}

void QueryEditorLayout::RenderLayout()
{
    if (!wwizardWwiseClient->IsConnected())
        return;
    //Available Queries Field
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Available Queries", (bool*)1))
    {
        ImGui::End();
        return;
    }


    if (ImGui::BeginTable("availableWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        ShowWwiseQueries(*queryEditorModule->GetWwiseQueryHierarchy());
        ShowWaapiQueries();
        ShowWaqlQueries();
        ImGui::EndTable();
    }

    if (ImGui::BeginPopupModal("Query Creator"))
    {
        ShowQueryCreator();
        ImGui::EndPopup();
    }

    if (ImGui::Button("Create new query"))
    {
        ImGui::OpenPopup("Query Creator");
    }


    ImGui::End();


    //Active Queries Field

    if (!ImGui::Begin("Active Queries", (bool*)1))
    {
        ImGui::End();
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (ImGui::BeginTable("activeWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        ShowActiveQueries();
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::End();

    //Details window
    ShowDetails();


    //Results
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Results", (bool*)1))
    {
        ImGui::End();
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (ImGui::BeginTable("activeWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        ShowQueryResults();
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void QueryEditorLayout::ShowActiveQueries()
{
    ImGui::PushID(0);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    for (auto& object : queryEditorModule->GetActiveQueryList())
    {
        const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());
        if (ImGui::Selectable((object.second.name + "##" + object.second.guid).c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseDoubleClicked(0))
            {
                queryEditorModule->RemoveFromActiveQueryList(object.second.guid);
                std::string resetSelection = "";
                queryEditorModule->SetQuerySelection(resetSelection);
                queryEditorModule->RunActiveQueries();
                break;
            }
            else
            {
                queryEditorModule->SetQuerySelection(object.second.guid);
            }
        }
        if (is_selected)
            ImGui::SetItemDefaultFocus();
    }

    ImGui::PopID();
}

void QueryEditorLayout::ShowWwiseQueries(const BaseQueryStructure& queryObject)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    ImGui::PushID(0);

    // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    if (queryObject.structureType == QueryType::FOLDER)
    {
        bool node_open = ImGui::TreeNode("", queryObject.name.c_str());

        if (node_open)
        {
            for (int i = 0; i < queryObject.subHierarchy.size(); i++)
            {
                ImGui::PushID(i); // Use field index as identifier.

                ShowWwiseQueries(queryObject.subHierarchy[i]);

                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }
    else
    {
        const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == queryObject.guid && ImGui::IsWindowFocused());

        if (ImGui::Selectable(queryObject.name.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseDoubleClicked(0))
            {
                queryEditorModule->AddToActiveQueryList(queryObject.guid);
                queryEditorModule->RunActiveQueries();
            }

            queryEditorModule->SetQuerySelection(queryObject.guid);
        }

        if (is_selected)
            ImGui::SetItemDefaultFocus();
    }

    ImGui::PopID();
}

void QueryEditorLayout::ShowWaapiQueries()
{
    ImGui::PushID(1);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    bool node_open = ImGui::TreeNode("", "Waapi Queries");

    if (node_open)
    {
        int counter = 0;

        for (auto& object : queryEditorModule->GetWaapiQueries())
        {
            ImGui::PushID(counter);
            const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());

            if (ImGui::Selectable(object.second.name.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    queryEditorModule->AddToActiveQueryList(object.second.guid);
                    queryEditorModule->RunActiveQueries();
                }

                queryEditorModule->SetQuerySelection(object.second.guid);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

            ImGui::PopID();
            counter++;
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void QueryEditorLayout::ShowWaqlQueries()
{
    ImGui::PushID(2);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    bool node_open = ImGui::TreeNode("", "Waql Queries");

    if (node_open)
    {
        int counter = 0;

        for (auto& object : queryEditorModule->GetWaqlQueries())
        {
            ImGui::PushID(counter);
            const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());

            if (ImGui::Selectable(object.second.name.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    queryEditorModule->AddToActiveQueryList(object.second.guid);
                    queryEditorModule->RunActiveQueries();
                }

                queryEditorModule->SetQuerySelection(object.second.guid);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();

            ImGui::PopID();
            counter++;
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void QueryEditorLayout::ShowQueryResults()
{
    ImGui::PushID(0);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    for (auto& object : queryEditorModule->GetQueryResultFiles())
    {
        const ImColor& col = ConvertWwiseColorToRGB(object.second.color);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(col.Value.x, col.Value.y, col.Value.z, 1));
        ImGui::Text("|=|");
        ImGui::SameLine();
        ImGui::PopStyleColor();
        const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());
        if (ImGui::Selectable(object.second.name.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }

            queryEditorModule->SetQuerySelection(object.second.guid);

            if (ImGui::IsMouseDoubleClicked(0))
            {
                wwizardWwiseClient->FocusObjectInWwise(object.second.guid);
            }
        }

        ImGui::Separator();
    }
    ImGui::PopID();
}

void QueryEditorLayout::ShowQueryCreator()
{
    static char argText[124] = "pls enter your query text here....";
    static char nameText[64] = "pls enter your query name here....";
    static bool waql = false;
    static bool waapi = false;

    if (ImGui::Checkbox("Waapi", &waapi))
    {
        waql = false;
    }
    if (ImGui::Checkbox("Waql", &waql))
    {
        waapi = false;
    }

    ImGui::Text("label");
    ImGui::SameLine();
    ImGui::InputText("##", nameText, IM_ARRAYSIZE(nameText));

    ImGui::Text("MultiArg");
    ImGui::SameLine();
    ImGui::InputTextMultiline("###", argText, IM_ARRAYSIZE(argText));

    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Text("Please select if its a Waapi or Waql Query!");
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (ImGui::Button("Submit Query", ImVec2(-FLT_MIN, 0.0f)))
    {
        if (waapi)
        {
            queryEditorModule->CreateNewQuery(nameText, QueryType::WAAPIQUERY, argText);
            ImGui::CloseCurrentPopup();
        }
        else if (waql)
        {
            queryEditorModule->CreateNewQuery(nameText, QueryType::WAQLQUERY, argText);
            ImGui::CloseCurrentPopup();
        }
        else
        {
            ImGui::OpenPopup("Options");
        }
    }
    if (ImGui::Button("Close"))
    {
        ImGui::CloseCurrentPopup();
    }
}

void QueryEditorLayout::ShowDetails()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Details", (bool*)1))
    {
        ImGui::End();
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

    auto possibleSelectedQuery = queryEditorModule->FindInMap<const BaseQueryStructure*, const std::map<std::string, BaseQueryStructure>&>(queryEditorModule->GetAllQueries());
    auto possibleSelectedFile = queryEditorModule->FindInMap<const QueryResultFile*, const std::map<std::string, QueryResultFile>&>(queryEditorModule->GetQueryResultFiles());

    if (possibleSelectedQuery != nullptr)
    {
        static std::string nameText;
        static std::string argText;
        if (possibleSelectedQuery->structureType == QueryType::WAAPIQUERY || possibleSelectedQuery->structureType == QueryType::WAQLQUERY)
        {
            if (ImGui::Button("Edit Query"))
            {
                ImGui::OpenPopup("QueryEditing");
                nameText = possibleSelectedQuery->name;
                argText = queryEditorModule->GetCurrentArgAsString().c_str();
            }
        }

        ImGui::Text(("Name : " + possibleSelectedQuery->name).c_str());
        ImGui::Text(("Guid : " + possibleSelectedQuery->guid).c_str());
        ImGui::Text(("Type : " + queryEditorModule->GetQueryTypeAsString(possibleSelectedQuery->structureType)).c_str());

        if (possibleSelectedQuery->structureType == QueryType::WAAPIQUERY || possibleSelectedQuery->structureType == QueryType::WAQLQUERY)
        {
            ImGui::Text(("Arg : " + queryEditorModule->GetCurrentArgAsString()).c_str());
        }
        else if (possibleSelectedQuery->structureType == QueryType::WWISEQUERY)
        {
            ImGui::Text("Arg : ");
            ImGui::SameLine();
            if (ImGui::Button("Inspect Wwise Query"))
            {
                wwizardWwiseClient->FocusObjectInWwise(possibleSelectedQuery->guid);
            }
        }
        if (ImGui::BeginPopup("QueryEditing"))
        {
            ImGui::Text("Edit Query");
            ImGui::Separator();

            ImGui::Text("name");
            ImGui::SameLine();
            ImGui::InputText("##1", &nameText);

            ImGui::Text("arg");
            ImGui::SameLine();
            ImGui::InputText("##2", &argText);

            ImGui::Separator();
            if (ImGui::Button("Delete Query"))
            {
                queryEditorModule->DeleteQuery(possibleSelectedQuery->guid);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                queryEditorModule->SaveChangedQuery(nameText, argText, possibleSelectedQuery->guid);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    else if (possibleSelectedFile != nullptr)
    {
        ImGui::Text(("Name : " + possibleSelectedFile->name).c_str());
        ImGui::Text(("Guid : " + possibleSelectedFile->guid).c_str());
    }
    ImGui::PopStyleVar();
    ImGui::End();
}
