#include "QueryEditorLayout.h"
#include "WwiseColors.h"


QueryEditorLayout::QueryEditorLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<QueryEditorModule>& queryEditorModule)
	: BaseLayout(wwizardWwiseClient)
	, queryEditorModule(queryEditorModule)
{
    auto outputNodeCreator = available_nodes.find("Output");
    outputNode = outputNodeCreator->second();
    nodes.insert({ outputNode->nodeGuid, outputNode });
    outputNode->Pos = ImVec2(500,200);
}

void QueryEditorLayout::RenderLayout()
{
    if (!wwizardWwiseClient->IsConnected())
        return;

    SettingWidget();

    AvailableQueriesWidget();

    ActiveQueriesWidget();

    DetailsWidget();
    
    ResultWidget();
}

void QueryEditorLayout::SettingWidget()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Config", NULL, ImGuiWindowFlags_NoMove))
    {
        ImGui::End();
        return;
    }
    if (ImGui::BeginCombo("##QueryMode", current_item))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (current_item == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
                current_item = items[n];
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (current_item == "Standard")
    {
        SetUseQueryNodeEditor(false);
    }
    else if (current_item == "Nodegraph")
    {
        SetUseQueryNodeEditor(true);
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
}

void QueryEditorLayout::AvailableQueriesWidget()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Available Queries", NULL, ImGuiWindowFlags_NoMove))
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
    ImGui::End();
}

void QueryEditorLayout::ActiveQueriesWidget()
{
    if (useQueryNodeEditor)
    {
        ShowQueryNodeEditor();
    }
    else
    {
        if (!ImGui::Begin("Active Queries", NULL, ImGuiWindowFlags_NoMove))
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
    }
}

void QueryEditorLayout::ResultWidget()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Results", NULL, ImGuiWindowFlags_NoMove))
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

void QueryEditorLayout::DetailsWidget()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Details", NULL, ImGuiWindowFlags_NoMove))
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
            if (ImGui::IsMouseDoubleClicked(0) && !useQueryNodeEditor)
            {
                queryEditorModule->AddToActiveQueryList(queryObject.guid);
                queryEditorModule->RunActiveQueries();
            }

            queryEditorModule->SetQuerySelection(queryObject.guid);
        }

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("Query", &queryObject.guid, queryObject.guid.size());
            ImGui::Text(queryObject.name.c_str());
            ImGui::EndDragDropSource();
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
                if (ImGui::IsMouseDoubleClicked(0) && !useQueryNodeEditor)
                {
                    queryEditorModule->AddToActiveQueryList(object.second.guid);
                    queryEditorModule->RunActiveQueries();
                }

                queryEditorModule->SetQuerySelection(object.second.guid);
            }

            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("Query", &object.second.guid, sizeof(object.second.guid));
                ImGui::Text(object.second.name.c_str());
                ImGui::EndDragDropSource();
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

        for (const auto& object : queryEditorModule->GetWaqlQueries())
        {
            ImGui::PushID(counter);
            const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());

            if (ImGui::Selectable(object.second.name.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0) && !useQueryNodeEditor)
                {
                    queryEditorModule->AddToActiveQueryList(object.second.guid);
                    queryEditorModule->RunActiveQueries();
                }

                queryEditorModule->SetQuerySelection(object.second.guid);
            }

            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("Query", &object.second.guid, sizeof(object.second.guid));
                ImGui::Text(object.second.guid.c_str());
                ImGui::EndDragDropSource();
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

void QueryEditorLayout::ShowQueryNodeEditor()
{
    // Canvas must be created after ImGui initializes, because constructor accesses ImGui style to configure default colors.
    static ImNodes::Ez::Context* context = ImNodes::Ez::CreateContext();
    
    IM_UNUSED(context);

    if (ImGui::Begin("Querynode Editor", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove))
    {
        // We probably need to keep some state, like positions of nodes/slots for rendering connections.
        ImNodes::Ez::BeginCanvas();

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Query"))
            {
                std::string droppedGuid = *static_cast<std::string*> (payload->Data);
                auto newNode = available_nodes.find("Query")->second();
                nodes.emplace(newNode->nodeGuid, newNode);
     
                ImNodes::AutoPositionNode(newNode);
                auto a = queryEditorModule->GetAllQueries().find(droppedGuid);
                static_cast<MyQueryNode*>(newNode)->SetQueryDetails(a->second.name, droppedGuid);
                static_cast<MyQueryNode*>(newNode)->RerunQuery();
            }
            ImGui::EndDragDropTarget();
        }
        for (auto it = nodes.begin(); it != nodes.end();)
        {
            MyNode* node = it->second;

            // Start rendering node
            if (ImNodes::Ez::BeginNode(node, node->Title, &node->Pos, &node->Selected))
            {
                // Render input nodes first (order is important)
                ImNodes::Ez::InputSlots(node->InputSlots.data(), static_cast<int>(node->InputSlots.size()));

                // Custom node content may go here
                ImGui::Text(node->nodeContent.c_str(), node->Title);

                // Render output nodes first (order is important)
                ImNodes::Ez::OutputSlots(node->OutputSlots.data(), static_cast<int>(node->OutputSlots.size()));

                // Store new connections when they are created
                Connection new_connection;
                if (ImNodes::GetNewConnection(&new_connection.InputNode, &new_connection.InputSlot,
                    &new_connection.OutputNode, &new_connection.OutputSlot))
                {
                    if (((MyNode*)new_connection.InputNode)->Title == "Output")
                    {
                        if (!((MyNode*)new_connection.InputNode)->Connections.empty())
                        {
                            Connection currentOutputConnection = ((MyNode*)new_connection.InputNode)->Connections[0];
                            ((MyNode*)currentOutputConnection.InputNode)->DeleteConnection(currentOutputConnection);
                            ((MyNode*)currentOutputConnection.OutputNode)->DeleteConnection(currentOutputConnection);
                        }
                    }

                    ((MyNode*)new_connection.InputNode)->Connections.push_back(new_connection);
                    ((MyNode*)new_connection.OutputNode)->Connections.push_back(new_connection);
                    RecalculateNodeGraph();
                }

                // Render output connections of this node
                for (const Connection& connection : node->Connections)
                {
                    // Node contains all it's connections (both from output and to input slots). This means that multiple
                    // nodes will have same connection. We render only output connections and ensure that each connection
                    // will be rendered once.
                    if (connection.OutputNode != node)
                        continue;

                    if (!ImNodes::Connection(connection.InputNode, connection.InputSlot, connection.OutputNode,
                        connection.OutputSlot))
                    {
                        // Remove deleted connections
                        ((MyNode*)connection.InputNode)->DeleteConnection(connection);
                        ((MyNode*)connection.OutputNode)->DeleteConnection(connection);
                        RecalculateNodeGraph();
                    }
                }

                if (node->Selected)
                {
                    if (node->Title == "Query")
                    {
                        if (node != currentSelectedNode)
                        {
                            UpdateSelectedNode(node);
                        }
                    } 
                }
            }
            // Node rendering is done. This call will render node background based on size of content inside node.
            ImNodes::Ez::EndNode();

            if (node->Selected && ImGui::IsKeyPressedMap(ImGuiKey_Delete) && node->Title != "Output")
            {
                // Deletion order is critical: first we delete connections to us
                for (auto& connection : node->Connections)
                {
                    if (connection.OutputNode == node)
                    {
                        ((MyNode*)connection.InputNode)->DeleteConnection(connection);
                    }
                    else
                    {
                        ((MyNode*)connection.OutputNode)->DeleteConnection(connection);
                    }
                }
                // Then we delete our own connections, so we don't corrupt the list
                node->Connections.clear();

                delete node;
                it = nodes.erase(it);
                RecalculateNodeGraph();
            }
            else
                ++it;
        }

        if (ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered() && !ImGui::IsMouseDragging(1))
        {
            ImGui::FocusWindow(ImGui::GetCurrentWindow());
            ImGui::OpenPopup("NodesContextMenu");
        }

        if (ImGui::BeginPopup("NodesContextMenu"))
        {
            for (const auto& desc : available_nodes)
            {
                if (desc.first != "Query" && desc.first != "Output")
                {
                    if (ImGui::MenuItem(desc.first.c_str()))
                    {
                        auto newNode = desc.second();
                        nodes.emplace(newNode->nodeGuid, newNode);

                        ImNodes::AutoPositionNode(newNode);
                    }
                }
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Reset Zoom"))
                ImNodes::GetCurrentCanvas()->Zoom = 1;

            if (ImGui::IsAnyMouseDown() && !ImGui::IsWindowHovered())
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImNodes::Ez::EndCanvas();
    }
    ImGui::End();

}

void QueryEditorLayout::RecalculateNodeGraph()
{
    queryEditorModule->RunNodeGraph(outputNode);
}

void QueryEditorLayout::SetUseQueryNodeEditor(bool newState)
{
    useQueryNodeEditor = newState;

    if (newState)
    {
        RecalculateNodeGraph();
    }
    else
    {
        queryEditorModule->ResetQueryResults();
        queryEditorModule->RunActiveQueries();
    }
}

void QueryEditorLayout::UpdateSelectedNode(MyNode* newCurrentSelectedNode)
{
    currentSelectedNode = newCurrentSelectedNode;
    queryEditorModule->SetQuerySelection(static_cast<MyQueryNode*>(newCurrentSelectedNode)->queryGuid);
}