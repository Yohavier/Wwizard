#include "Dockspace.h"
#include <stdio.h>
#include "imgui_stdlib.h"

namespace wwizard
{
    static std::string projectPathSetting;
    static std::string sdkPathSetting;
    static std::string waapiIPSetting;
    static int waapiPortSetting;

	Dockspace::Dockspace(WwizardWwiseClient* wwizardWwiseClient, SettingHandler* settingHandler)
        : currentLayout(Layout::HOME)
        , wwizarWwiseClient(wwizardWwiseClient)
        , settingHandler(settingHandler)
	{ 
        //Init all modules
        queryEditorModule.Init(wwizarWwiseClient);
        std::cout << "Initialized Dockspace" << std::endl;
        SetDefaultStyle();
	}

	void Dockspace::Render(bool* p_open)
	{
        static bool opt_padding = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", p_open, window_flags);
        if (!opt_padding)
        ImGui::PopStyleVar();
   
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id;
        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        CreateMenuBar();
        
        if (currentLayout == Layout::QUERYEDITOR)
        {
            if(wwizarWwiseClient->IsConnected())
                CreateQueryEditor(p_open);
        }
        else if (currentLayout == Layout::SETTINGS)
        {
            ShowSettings(p_open);
        }
        else //Default Home Layout
        {
            CreateHomeLayout();
        }

        ImGui::End();
    }

    void Dockspace::CreateMenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Wwizard"))
            {
                ImGui::MenuItem("Reconnect", NULL);
                if (ImGui::MenuItem("Settings", NULL))
                {
                    projectPathSetting = *settingHandler->GetWwisProjectPathRef();
                    sdkPathSetting = *settingHandler->GetSDKPath();
                    waapiIPSetting = *settingHandler->GetWaapiIP();
                    waapiPortSetting = *settingHandler->GetWaaapiPort();
                    SetLayout(Layout::SETTINGS);
                }
                ImGui::Separator();

                ImGui::MenuItem("Close", NULL);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Module"))
            {
                if (ImGui::MenuItem("Home", NULL))
                {
                    SetLayout(Layout::HOME);
                }

                if (ImGui::MenuItem("Naming Convention", NULL))
                {
                    SetLayout(Layout::NAMINGCONVENTION);
                }
    
                if (ImGui::MenuItem("Sort Originals", NULL))
                {
                    SetLayout(Layout::SORTORIGINALS);
                }

                if (ImGui::MenuItem("Query Editor", NULL))
                {
                    SetLayout(Layout::QUERYEDITOR);
                }
                ImGui::EndMenu();
            }
            if (currentLayout == Layout::QUERYEDITOR)
            {
                if (ImGui::BeginMenu("Add Query"))
                { 
                    SetAddQueryPopUp();
                    ImGui::EndMenu();
                }                
            }
            ImGui::EndMenuBar();
        }
    }

    void Dockspace::SetLayout(Layout newLayout)
    {
        currentLayout = newLayout;
    }

    //Settings Layout
    void Dockspace::ShowSettings(bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Settings", p_open))
        {
            ImGui::End();
            return; 
        }

        ImGui::Text("Project Path : ");
        ImGui::SameLine();
        ImGui::InputText("##1", &projectPathSetting);

        ImGui::Text("SDK Path     : ");
        ImGui::SameLine();
        ImGui::InputText("##2", &sdkPathSetting);

        ImGui::Text("Waapi IP     : ");
        ImGui::SameLine();
        ImGui::InputText("##3", &waapiIPSetting);
       
        ImGui::Text("Waapi Port   : ");
        ImGui::SameLine();
        ImGui::InputInt("##4", &waapiPortSetting);

        if (ImGui::Button("Save Settings"))
        {
            if (waapiIPSetting != *settingHandler->GetWaapiIP() || waapiPortSetting != *settingHandler->GetWaaapiPort())
            {
                settingHandler->SaveSettings(projectPathSetting, sdkPathSetting, waapiIPSetting, waapiPortSetting);
                wwizarWwiseClient->Connect(*settingHandler);
                queryEditorModule.ResetQueryModule(wwizarWwiseClient);
            }
            else
            {
                settingHandler->SaveSettings(projectPathSetting, sdkPathSetting, waapiIPSetting, waapiPortSetting);
            }
        }
        ImGui::End();
    }

    //QueryLayouts
    void Dockspace::CreateQueryEditor(bool* p_open)
    {       
        //Available Queries Field
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Available Queries", p_open))
        {
            ImGui::End();
            return;
        }


        if (ImGui::BeginTable("availableWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            ShowWwiseQueries(queryEditorModule.wwiseQueryHierarchy);
            ShowWaapiQueries();
            ShowWaqlQueries();
            ImGui::EndTable();
        }
        ImGui::End();


        //Active Queries Field
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Active Queries", p_open))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::BeginTable("activeWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            ShowActiveList();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::End();


        //Details window
        HandleDetailsWindow(p_open);
        

        //Results
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Results", p_open))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::BeginTable("activeWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            ShowResultList();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }

    void Dockspace::ShowActiveList()
    {
        ImGui::PushID(0);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        for (auto& object : queryEditorModule.GetActiveQueryList())
        {
            const bool is_selected = (queryEditorModule.GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());
            if (ImGui::Selectable((object.second.name + "##" + object.second.guid).c_str(), is_selected))
            {
                if (is_selected)
                {
                    queryEditorModule.RemoveFromActiveQueryList(object.second.guid);
                    std::string resetSelection = "";
                    queryEditorModule.SetQuerySelection(resetSelection);
                    queryEditorModule.RunActiveQueries();
                }
                else 
                {
                    queryEditorModule.SetQuerySelection(object.second.guid);
                }
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::PopID();
    }

    void Dockspace::ShowWwiseQueries(BaseQueryStructure* queryObject)
    {
        // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
        ImGui::PushID(0);

        // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        if (queryObject->structureType == QueryType::FOLDER)
        {
            bool node_open = ImGui::TreeNode("", queryObject->name.c_str());

            if (node_open)
            {
                for (int i = 0; i < queryObject->subHierarchy.size(); i++)
                {
                    ImGui::PushID(i); // Use field index as identifier.

                    ShowWwiseQueries(queryObject->subHierarchy[i]);

                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
        else
        {
            const bool is_selected = (queryEditorModule.GetCurrentSelectionGuid() == queryObject->guid && ImGui::IsWindowFocused());

            if (ImGui::Selectable(queryObject->name.c_str(), is_selected)) 
            {
                
                if (is_selected)
                {
                    queryEditorModule.AddToActiveQueryList(queryObject->guid);
                    queryEditorModule.RunActiveQueries();
                }     

                queryEditorModule.SetQuerySelection(queryObject->guid);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::PopID();
    }

    void Dockspace::ShowWaapiQueries()
    {
        ImGui::PushID(1);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool node_open = ImGui::TreeNode("", "Waapi Queries");

        if (node_open)
        {
            int counter = 0;
            
            for (auto& object : queryEditorModule.waapiQueries)
            {
                ImGui::PushID(counter);
                const bool is_selected = (queryEditorModule.GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());

                if (ImGui::Selectable(object.second.name.c_str(), is_selected))
                {
                    if (is_selected)
                    {
                        queryEditorModule.AddToActiveQueryList(object.second.guid);
                        queryEditorModule.RunActiveQueries();
                    }

                    queryEditorModule.SetQuerySelection(object.second.guid);
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

    void Dockspace::ShowWaqlQueries()
    {
        ImGui::PushID(2);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        bool node_open = ImGui::TreeNode("", "Waql Queries");

        if (node_open)
        {
            int counter = 0;

            for (auto& object : queryEditorModule.waqlQueries)
            {
                ImGui::PushID(counter);
                const bool is_selected = (queryEditorModule.GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());

                if (ImGui::Selectable(object.second.name.c_str(), is_selected))
                {
                    if (is_selected)
                    {
                        queryEditorModule.AddToActiveQueryList(object.second.guid);
                        queryEditorModule.RunActiveQueries();
                    }

                    queryEditorModule.SetQuerySelection(object.second.guid);
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

    void Dockspace::ShowResultList()
    {
        ImGui::PushID(0);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        for (auto& object : queryEditorModule.queryResultFiles)
        {
            const bool is_selected = (queryEditorModule.GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());
            if (ImGui::Selectable(object.second.name.c_str(), false))
            {

                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                queryEditorModule.SetQuerySelection(object.second.guid);
            }          
        }

        ImGui::PopID();
    }

    void Dockspace::CreateHomeLayout()
    {
        
    }

    void Dockspace::SetAddQueryPopUp()
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
                queryEditorModule.CreateNewQuery(nameText, QueryType::WAAPIQUERY, argText);
            else if (waql)
                queryEditorModule.CreateNewQuery(nameText, QueryType::WAQLQUERY, argText);
            else
            {

                ImGui::OpenPopup("Options");
            }
            
        }    
    }
    
    void Dockspace::HandleDetailsWindow(bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Details", p_open))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        auto possibleSelectedQuery = queryEditorModule.FindInMap<BaseQueryStructure*, std::map<std::string, BaseQueryStructure>&>(queryEditorModule.allQueries);
        auto possibleSelectedFile = queryEditorModule.FindInMap<QueryResult*, std::map<std::string, QueryResult>&>(queryEditorModule.queryResultFiles);

        if (possibleSelectedQuery != nullptr)
        {
            ImGui::Text(("Name : " + possibleSelectedQuery->name).c_str());
            ImGui::Text(("Guid : " + possibleSelectedQuery->guid).c_str());
            ImGui::Text(("Type : " + queryEditorModule.ConvertQueryTypeToString(possibleSelectedQuery->structureType)).c_str());

            if (possibleSelectedQuery->structureType == QueryType::WAAPIQUERY || possibleSelectedQuery->structureType == QueryType::WAQLQUERY)
            {
                ImGui::Text("Arg : ");
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

    void Dockspace::SetDefaultStyle()
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        style->WindowPadding = ImVec2(10, 10);
    }
}