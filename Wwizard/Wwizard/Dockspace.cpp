#include "Dockspace.h"
#include <stdio.h>
#include "imgui_stdlib.h"

static std::string projectPathSetting;
static std::string sdkPathSetting;
static std::string waapiIPSetting;
static int waapiPortSetting;

Dockspace::Dockspace(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler)
    : currentLayout(Layout::HOME)
    , wwizarWwiseClient(wwizardWwiseClient)
    , settingHandler(settingHandler)
{ 
    //Init all modules
    queryEditorModule.reset(new QueryEditorModule(wwizardWwiseClient));
    sortOriginalsModule.reset(new SortOriginalsModule());
    namingConventionModule.reset(new NamingConventionModule(settingHandler->GetWwisProjectPathRef())); 

    std::cout << "Initialized Dockspace" << std::endl;
    ImGuiIO& io = ImGui::GetIO();
    float fontSize = 18.0f;// *2.0f;
    auto fontpath = static_cast<std::string>(SOLUTION_DIR) + "assets/fonts/OpenSans-Bold.ttf";
    io.FontDefault = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontSize);
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

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

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
    else if (currentLayout == Layout::SORTORIGINALS)
    {
        ShowSortOriginalsModule();
    }
    else if (currentLayout == Layout::NAMINGCONVENTION)
    {
        ShowNamingConventionModule();
    }
    else //Default Home Layout
    {
        ShowHome();
    }

    ImGui::End();
}

void Dockspace::CreateMenuBar()
{
    if (ImGui::BeginMenuBar())
    {   
        if (ImGui::BeginMenu("Wwizard"))
        {
            if (ImGui::MenuItem("Home", NULL))
            {
                SetLayout(Layout::HOME);
            }
            if (ImGui::MenuItem("Reconnect", NULL))
            {
                wwizarWwiseClient->Connect(settingHandler);
                queryEditorModule->ResetQueryModule(wwizarWwiseClient);
            }
            if (ImGui::MenuItem("Settings", NULL))
            {
                projectPathSetting = settingHandler->GetWwisProjectPathRef();
                sdkPathSetting = settingHandler->GetSDKPath();
                waapiIPSetting = settingHandler->GetWaapiIP();
                waapiPortSetting = settingHandler->GetWaaapiPort();
                SetLayout(Layout::SETTINGS);
            }
            ImGui::Separator();

            ImGui::MenuItem("Close", NULL);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Module"))
        {
            if (ImGui::MenuItem("Naming Convention", NULL))
            {
                SetLayout(Layout::NAMINGCONVENTION);
            }
    
            if (ImGui::MenuItem("Sort Originals", NULL))
            {
                sortOriginalsModule->LoadModule(settingHandler->GetWwisProjectPathRef());
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
            if (wwizarWwiseClient->IsConnected())
            {
                if (ImGui::BeginMenu("Add Query"))
                {
                    ShowQueryCreator();
                    ImGui::EndMenu();
                }
            }       
        }

        if (wwizarWwiseClient->IsConnected())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 153, 0, 255));
            ImGui::Text("Connected");
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(153, 0, 0, 255));
            ImGui::Text("Disconnected");
            ImGui::PopStyleColor();
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

    ImGui::Text("SDK Path       : ");
    ImGui::SameLine();
    ImGui::InputText("##2", &sdkPathSetting);

    ImGui::Text("Waapi IP        : ");
    ImGui::SameLine();
    ImGui::InputText("##3", &waapiIPSetting);
       
    ImGui::Text("Waapi Port   : ");
    ImGui::SameLine();
    ImGui::InputInt("##4", &waapiPortSetting);

    if (ImGui::Button("Save Settings", ImVec2(200,50)))
    {
        if (waapiIPSetting != settingHandler->GetWaapiIP() || waapiPortSetting != settingHandler->GetWaaapiPort())
        {
            settingHandler->SaveSettings(projectPathSetting, sdkPathSetting, waapiIPSetting, waapiPortSetting);
            wwizarWwiseClient->Connect(settingHandler);
            queryEditorModule->ResetQueryModule(wwizarWwiseClient);
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
        ShowWwiseQueries(*queryEditorModule->wwiseQueryHierarchy);
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
        ShowActiveQueries();
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::End();


    //Details window
    ShowDetails(p_open);
        

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
        ShowQueryResults();
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void Dockspace::ShowActiveQueries()
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

void Dockspace::ShowWwiseQueries(const BaseQueryStructure& queryObject)
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
            
        for (auto& object : queryEditorModule->waapiQueries)
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

        for (auto& object : queryEditorModule->waqlQueries)
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

void Dockspace::ShowQueryResults()
{
    ImGui::PushID(0);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();

    for (auto& object : queryEditorModule->queryResultFiles)
    {
        auto col = ConvertWwiseColorToRGB(object.second.color);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(col.Value.x, col.Value.y, col.Value.z, 1));
        ImGui::Text("|=|");
        ImGui::SameLine();
        ImGui::PopStyleColor();
        const bool is_selected = (queryEditorModule->GetCurrentSelectionGuid() == object.second.guid && ImGui::IsWindowFocused());
        if (ImGui::Selectable(object.second.name.c_str(), is_selected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (is_selected)
                ImGui::SetItemDefaultFocus();

            queryEditorModule->SetQuerySelection(object.second.guid);

            if (ImGui::IsMouseDoubleClicked(0))
            {
                wwizarWwiseClient->OpenPropertyInWwise(object.second.guid);
            }
        }  
        ImGui::Separator();
    }
    ImGui::PopID();
}

void Dockspace::ShowHome()
{
        
}

void Dockspace::ShowQueryCreator()
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
            queryEditorModule->CreateNewQuery(nameText, QueryType::WAAPIQUERY, argText);
        else if (waql)
            queryEditorModule->CreateNewQuery(nameText, QueryType::WAQLQUERY, argText);
        else
        {
            ImGui::OpenPopup("Options");
        }
            
    }    
}
    
void Dockspace::ShowDetails(bool* p_open)
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Details", p_open))
    {
        ImGui::End();
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

    auto possibleSelectedQuery = queryEditorModule->FindInMap<BaseQueryStructure*, std::map<std::string, BaseQueryStructure>&>(queryEditorModule->allQueries);
    auto possibleSelectedFile = queryEditorModule->FindInMap<QueryResult*, std::map<std::string, QueryResult>&>(queryEditorModule->queryResultFiles);

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
        ImGui::Text(("Type : " + queryEditorModule->ConvertQueryTypeToString(possibleSelectedQuery->structureType)).c_str());

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
                wwizarWwiseClient->OpenPropertyInWwise(possibleSelectedQuery->guid);
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

//Sort Originals
void Dockspace::ShowSortOriginalsModule()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Sort Originals"))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Originals Count: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(sortOriginalsModule->GetOriginalsCount()).c_str());
    ImGui::Text("OriginalsPath: ");
    ImGui::SameLine();
    ImGui::Text(sortOriginalsModule->GetOriginalPath().c_str());
    ImGui::Separator();
    ImGui::Separator();
    if (ImGui::Button("Sort Originals", ImVec2(200, 50)))
    {
        sortOriginalsModule->SortOriginals();
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete unused Originals", ImVec2(200, 50)))
    {
        sortOriginalsModule->CreateUnusedOriginalsList();
        ImGui::OpenPopup("Unused wav in Originals Path");
    }

    if (ImGui::BeginPopup("Unused wav in Originals Path", ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::Button("Abort"))
        {
            sortOriginalsModule->FinalizeDeleteUnusedOriginals(false);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            sortOriginalsModule->FinalizeDeleteUnusedOriginals(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();

        if (ImGui::BeginTable("Unused .wav", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX, ImVec2(600, 300)))
        {
            ImGui::PushID(0);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();

            for (auto& object : sortOriginalsModule->unusedOriginalsPaths)
            {
                ImGui::Selectable(object.c_str(), false);
            }
            ImGui::PopID();
            ImGui::EndTable();
        }
        ImGui::EndPopup();
    }

    ImGui::End();


    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Sorting Settings"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Containers that can create Folders");
    ImGui::Separator();
    ImGui::Text("Universal");
    ImGui::Checkbox("Physical Folder", &sortOriginalsModule->physicalFolderFlag);
    ImGui::Checkbox("Work Unit", &sortOriginalsModule->workUnitFlag);
    ImGui::Checkbox("Virtual Folder", &sortOriginalsModule->virtualFolderFlag);
    ImGui::Separator();
    ImGui::Text("Actor-Mixer Hierarchy");
    ImGui::Checkbox("Actor-Mixer", &sortOriginalsModule->actorMixerFlag);
    ImGui::Checkbox("Random Container", &sortOriginalsModule->randomContainerFlag);
    ImGui::Checkbox("Sequence Container", &sortOriginalsModule->sequenceContainerFlag);
    ImGui::Checkbox("Switch Container", &sortOriginalsModule->switchContainerFlag);
    ImGui::Checkbox("Blend Container", &sortOriginalsModule->blendContainerFlag);
    ImGui::Checkbox("Sound SFX", &sortOriginalsModule->soundSFXFlag);
    ImGui::Checkbox("Voice Sound", &sortOriginalsModule->voiceSoundFlag);
    ImGui::Separator();
    ImGui::Text("Interactive Music Hierarchy");
    ImGui::Checkbox("Music Switch Container", &sortOriginalsModule->musicSwitchContainerFlag);
    ImGui::Checkbox("Music Playlist Container", &sortOriginalsModule->musicPlaylistContainerFlag);
    ImGui::Checkbox("Music Segment", &sortOriginalsModule->musicSegmentFlag);
    ImGui::Checkbox("Music Track", &sortOriginalsModule->musicTrackFlag);
    ImGui::End();
}

//Naming Conventions
void Dockspace::ShowNamingConventionModule()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Naming Convention"))
    {
        ImGui::End();
        return;
    }
    if(ImGui::BeginTabBar("NamingConventionTabBar"))
    {
        if (ImGui::BeginTabItem("Execute"))
        {
            if (ImGui::Button("Check Naming"))
            {
                namingConventionModule->CheckNamingConvention();
            }
            if (ImGui::BeginTable("Naming Issues", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Current Name");
                ImGui::TableNextColumn();
                ImGui::Text("Desired Name");
                ImGui::TableNextRow();

                for (auto& issue : namingConventionModule->namingIssueResults)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(issue.first.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(issue.second.c_str());
                    ImGui::TableNextRow();
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Prefix Settings"))
        {
            for (auto& wwuType : namingConventionModule->whitelistedWwuTypes)
            {
                ImGui::Checkbox(namingConventionModule->stringToReplace[wwuType].c_str(), &(namingConventionModule->wwuSpaceSettings[wwuType].applyPrefix));
                ImGui::SameLine();
                std::string identificationName = "##" + namingConventionModule->stringToReplace[wwuType];
                ImGui::InputText(identificationName.c_str(), &(namingConventionModule->wwuSpaceSettings[wwuType].prefixToApply));
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Apply for..."))
        {
            for (auto& wwuType : namingConventionModule->whitelistedWwuTypes)
            {
                ImGui::Checkbox(namingConventionModule->stringToReplace[wwuType].c_str(), &(namingConventionModule->wwuSpaceSettings[wwuType].applyNamingConventionCheck));
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Space Settigns"))
        {
            for (auto& wwuType : namingConventionModule->whitelistedWwuTypes)
            {
                ImGui::Checkbox(namingConventionModule->stringToReplace[wwuType].c_str(), &(namingConventionModule->wwuSpaceSettings[wwuType].allowSpace));
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

//Misc
ImColor Dockspace::ConvertWwiseColorToRGB(int wwiseColor)
{
    switch (wwiseColor)
    {
        case 0:
            return ImColor(83, 83, 83);
        case 1:
            return ImColor(54, 62, 200);
        case 3:
            return ImColor(25, 85, 203);
        case 4:
            return ImColor(7, 104, 104);
        case 5:
            return ImColor(86, 115, 12);
        case 6:
            return ImColor(120, 113, 16);
        case 7:
            return ImColor(121, 87, 21);
        case 8:
            return ImColor(120, 66, 12);
        case 9:
            return ImColor(114, 56, 43);
        case 10:
            return ImColor(137, 35, 36);
        case 11:
            return ImColor(124, 38, 125);
        case 12:
            return ImColor(115, 42, 151);
        case 13:
            return ImColor(88, 54, 174);
        case 14:
            return ImColor(135, 135, 135);
        case 15:
            return ImColor(106, 111, 194);
        case 16:
            return ImColor(99, 131, 197);
        case 17:
            return ImColor(67, 137, 137);
        case 18:
            return ImColor(83, 147, 83);
        case 19:
            return ImColor(128, 152, 61);
        case 20:
            return ImColor(160, 151, 38);
        case 21:
            return ImColor(171, 135, 62);
        case 22:
            return ImColor(174, 121, 65);
        case 23:
            return ImColor(174, 100, 85);
        case 24:
            return ImColor(185, 91, 91);
        case 25:
            return ImColor(169, 80, 170);
        case 26:
            return ImColor(187, 85, 189);
        case 27:
            return ImColor(134, 96, 226);
        default:
            return ImColor(83, 83, 83, 83);
    }
}

void Dockspace::SetDefaultStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(10, 10);
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImColor(219, 152, 80);
    colors[ImGuiCol_HeaderHovered] = ImColor(219, 152, 80);
    colors[ImGuiCol_HeaderActive] = ImColor(219, 152, 80);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImColor(219, 152, 80);
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_TabHovered] = ImColor(219, 152, 80);
    colors[ImGuiCol_TabActive] = ImColor(219, 152, 80);
    colors[ImGuiCol_TabUnfocused] = ImColor(219, 152, 80);
    colors[ImGuiCol_TabUnfocusedActive] = ImColor(219, 152, 80);

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}