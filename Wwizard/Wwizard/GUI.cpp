#include "GUI.h"

static std::string projectPathSetting;
static std::string sdkPathSetting;
static std::string waapiIPSetting;
static int waapiPortSetting;

static ID3D10Device* g_pd3dDevice = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D10RenderTargetView* g_mainRenderTargetView = NULL;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GUI::GUI(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler, std::unique_ptr<QueryEditorModule>& queryEditorModule, std::unique_ptr<SortOriginalsModule>& sortOriginalsModule, std::unique_ptr<NamingConventionModule>& namingConventionModule, std::unique_ptr<ToolboxModule>& toolboxModule, std::unique_ptr<ColorCodingModule>& colorCodingModule)
    : currentLayout(Layout::HOME)
    , wwizarWwiseClient(wwizardWwiseClient)
    , settingHandler(settingHandler)
    , queryEditorModule(queryEditorModule)
    , sortOriginalsModule(sortOriginalsModule)
    , namingConventionModule(namingConventionModule)
    , toolboxModule(toolboxModule)
    , colorCodingModule(colorCodingModule)
{ 
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    hwnd = ::CreateWindow(wc.lpszClassName, _T("Wwizard"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX10_Init(g_pd3dDevice);

    float fontSize = 18.0f;
    auto fontpath = static_cast<std::string>(SOLUTION_DIR) + "assets/fonts/OpenSans-Bold.ttf";
    io->FontDefault = io->Fonts->AddFontFromFileTTF(fontpath.c_str(), fontSize);
    SetDefaultStyle();
}

GUI::~GUI()
{
    ImGui_ImplDX10_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void GUI::Render(bool& isRunning)
{
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            isRunning = false;
    }
    
    // Start the Dear ImGui frame
    ImGui_ImplDX10_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

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
    ImGui::Begin("DockSpace Demo", (bool*)1, window_flags);

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

    CreateTaskBar(isRunning);
        
    if (layouts.find(currentLayout) != layouts.end())
    {
        func_ptr fp= layouts[currentLayout];
        (this->*fp)();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDevice->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDevice->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    g_pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync
}

void GUI::CreateTaskBar(bool& isRunning)
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
                wwizarWwiseClient->StartReconnectionThread();
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

            if (ImGui::MenuItem("Close", NULL))
            {
                isRunning = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Module"))
        {
            bool enabled = wwizarWwiseClient->IsConnected();
            if (ImGui::MenuItem("Naming Convention", NULL, false, enabled))
            {
                SetLayout(Layout::NAMINGCONVENTION);
            }

            if (ImGui::MenuItem("Sort Originals", NULL, false, enabled))
            {
                SetLayout(Layout::SORTORIGINALS);
            }

            if (ImGui::MenuItem("Query Editor", NULL, false, enabled))
            {
                SetLayout(Layout::QUERYEDITOR);
                queryEditorModule->ResetQueryModule(wwizarWwiseClient);
            }

            if (ImGui::MenuItem("Color Coding", NULL, false, enabled))
            {
                SetLayout(Layout::COLORCODING);
            }

            if (ImGui::MenuItem("Toolbox", NULL, false, enabled))
            {
                SetLayout(Layout::TOOLBOX);
            }
            
            ImGui::EndMenu();
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

void GUI::SetLayout(const Layout newLayout)
{
    currentLayout = newLayout;
}

//Settings Layout
void GUI::RenderLayoutSettings()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Settings", (bool*)1))
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
            wwizarWwiseClient->StartReconnectionThread();
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
void GUI::RenderLayoutQueryEditor()
{
    if (!wwizarWwiseClient->IsConnected())
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

void GUI::ShowActiveQueries()
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

void GUI::ShowWwiseQueries(const BaseQueryStructure& queryObject)
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

void GUI::ShowWaapiQueries()
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

void GUI::ShowWaqlQueries()
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

void GUI::ShowQueryResults()
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
                wwizarWwiseClient->FocusObjectInWwise(object.second.guid);
            }  
        }   

        ImGui::Separator();
    }
    ImGui::PopID();
}

void GUI::RenderLayoutHome()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Home"))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Roadmap");
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ConvertWwiseColorToRGB(5)));
    ImGui::Text("---- Release Version 1.0 ----");
    ImGui::Indent(15);
    ImGui::Text("First stable version");
    ImGui::Text("Main Features:");
    ImGui::Indent(15);
    ImGui::BulletText("Naming convention checking");
    ImGui::BulletText("Creating color coding");
    ImGui::BulletText("More advanced query editor");
    ImGui::BulletText("Sort original files");
    ImGui::BulletText("Toolbox with helpers:");
    ImGui::Indent(15);
    ImGui::BulletText("Delete invalid events");
    ImGui::BulletText("Reset Fader");
    ImGui::Unindent(45);
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::BulletText("Dig into ImGui and create proper UI setup");
    ImGui::BulletText("Create Nodebase queryeditor for combining queries");
    ImGui::BulletText("Sort Originals P4 integration");
    ImGui::BulletText("QueryEditor: open and import wav file to reaper shortcut select file and ctrl+r");
    ImGui::BulletText("Remove Unreferenced Sounds");
    ImGui::BulletText("Remove unused Game syncs");
    ImGui::BulletText("Analyse audio spectrum of whole project");

    ImGui::End();
}

void GUI::ShowQueryCreator()
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
    
void GUI::ShowDetails()
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
                wwizarWwiseClient->FocusObjectInWwise(possibleSelectedQuery->guid);
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

void GUI::RenderColorCodingModule()
{
    ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Color Coding", (bool*)1))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Single always stronger than hierarchy");
    if (ImGui::Button("Add  Setting"))
    {
        colorCodingModule->CreateColorSetting("", 0);
    }

    int id = 0;
    for (auto& colorSetting : colorCodingModule->colorSettings)
    {
        ImGui::PushID(id);
        ImGui::Text("Keywords: ");
        ImGui::SameLine();
        ImGui::InputText("##Keywords: ", &(colorSetting.second.name));

        if (ImGui::BeginPopupModal("ColorPicker"))
        {
            ImGui::BeginColumns("Color Pop Up", 7);
            int column = 0;
            for (const auto& color : wwiseColors)
            {
                if (ImGui::ColorButton(std::to_string(color.first).c_str(), color.second, 0, ImVec2(50, 50)))
                {
                    colorSetting.second.colorCode = color.first;
                    ImGui::CloseCurrentPopup();
                }
                column++;
                if (column == 4)
                {
                    ImGui::NextColumn();
                    column = 0;
                }
            }
            ImGui::EndColumns();
            
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.25f);
            if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
        ImGui::Text("Color ");
        ImGui::SameLine();
        if (ImGui::ColorButton("Select Color", ConvertWwiseColorToRGB(colorSetting.second.colorCode)))
        {
            ImGui::OpenPopup("ColorPicker");
        }
        ImGui::Text("Mode: ");
        ImGui::SameLine();
        ImGui::Combo("##mode", &colorSetting.second.settingMode, colorCodingModule->colorSettingModes, 4);
        if (ImGui::Button("Delete Color setting"))
        {
            colorCodingModule->DeleteColorSetting(colorSetting.second);
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
        id++;
        ImGui::Separator();
    }

    ImGui::Text("Colors that should never change");
    if (ImGui::BeginPopupModal("ColorSelector"))
    {
        ImGui::BeginColumns("Color Pop Up", 7);
        int column = 0;
        for (const auto& color : wwiseColors)
        {
            if (ImGui::ColorButton(std::to_string(color.first).c_str(), color.second, 0, ImVec2(50, 50)))
            {
                colorCodingModule->blockedColors.insert(color.first);
                ImGui::CloseCurrentPopup();
            }
            column++;
            if (column == 4)
            {
                ImGui::NextColumn();
                column = 0;
            }
        }
        ImGui::EndColumns();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.25f);
        if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::Button("+"))
    {
        ImGui::OpenPopup("ColorSelector");
    }
    int idd = 0;
    for (const auto& blockedColor : colorCodingModule->GetBlockedColors())
    {
        ImGui::PushID(idd);
        ImGui::SameLine();
        if (ImGui::ColorButton("blocked Color", ConvertWwiseColorToRGB(blockedColor)))
        {
            colorCodingModule->blockedColors.erase(blockedColor);
        }
        ImGui::PopID();
        idd++;
    }

    if (ImGui::Button("Apply color coding to Wwise"))
    {
        colorCodingModule->BeginColorCodingProcess();
    }
    ImGui::End();
}

//Sort Originals
void GUI::RenderLayoutSortOriginals()
{
    if (!wwizarWwiseClient->IsConnected())
        return;
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Sort Originals"))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Originals Count: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(sortOriginalsModule->GetOriginalsCount()).c_str());
    ImGui::Text("SFX Count: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(sortOriginalsModule->GetSFXCount()).c_str());
    ImGui::Text("music Count: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(sortOriginalsModule->GetMusicCount()).c_str());

    ImGui::Text("OriginalsPath: ");
    ImGui::SameLine();
    ImGui::Text(sortOriginalsModule->GetOriginalPath().c_str());

    if (ImGui::Button("Scan", ImVec2(75, 25)))
    {
        sortOriginalsModule->BeginScanProcess();
    }

    ImGui::Separator();
    ImGui::Separator();
    if (ImGui::Button("Sort Originals", ImVec2(200, 50)))
    {
        sortOriginalsModule->StartSortOriginalsThread();
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete unused Originals", ImVec2(200, 50)))
    {
        sortOriginalsModule->CollectUnusedOriginals();
        ImGui::OpenPopup("Unused wav in Originals Path");
    }

    if (ImGui::BeginPopup("Unused wav in Originals Path", ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::Button("Abort"))
        {
            sortOriginalsModule->ClearCollectedOriginalsList();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            sortOriginalsModule->DeleteUnusedOriginals();
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();

        if (ImGui::BeginTable("Unused .wav", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX, ImVec2(600, 300)))
        {
            ImGui::PushID(0);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();

            for (auto& object : sortOriginalsModule->GetUnusedOriginals())
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
void GUI::RenderLayoutNamingConvention()
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
                namingConventionModule->StartCheckNamingConventionThread();
            }
            if (ImGui::BeginTable("Naming Issues", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Current Name");
                ImGui::TableNextColumn();
                ImGui::Text("Desired Name");
                ImGui::TableNextRow();

                
                for (auto& issue : namingConventionModule->GetNamingIssues())
                {
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(("##", issue.second.name.c_str()), false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        if (ImGui::IsMouseDoubleClicked(0))
                        {
                            wwizarWwiseClient->FocusObjectInWwise(issue.second.guid);
                        }
                    }
            
                    ImGui::TableNextColumn();
                    ImGui::Text(namingConventionModule->GetErrorMessageFromIssue(issue.second.issue).c_str());
                    ImGui::TableNextRow();
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Activation Settings"))
        {
            ImGui::Text("Select in which field the naming convention should be checked.");
            ImGui::Separator();
            for (auto& wwuType : namingConventionModule->GetWhiteListedWwuTypes())
            {
                ImGui::Checkbox(namingConventionModule->GetStringToReplace(wwuType).substr(1).c_str(), &(namingConventionModule->wwuSettings[wwuType].applyNamingConventionCheck));
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Additional Settings"))
        {
            ImGui::Separator();                
            ImGui::BeginColumns("additional", 3);
            for (const auto& wwuType : namingConventionModule->GetWhiteListedWwuTypes())
            {
                ImGui::Text(namingConventionModule->GetStringToReplace(wwuType).substr(1).c_str());
                ImGui::NextColumn();
                ImGui::Checkbox(("## allowSpace" + namingConventionModule->GetStringToReplace(wwuType).substr(1)).c_str(), &(namingConventionModule->wwuSettings[wwuType].allowSpace));
                ImGui::SameLine();
                ImGui::Text("Allow Space");
                ImGui::NextColumn();
                ImGui::Checkbox(("## allowUpperCase" + namingConventionModule->GetStringToReplace(wwuType).substr(1)).c_str(), &(namingConventionModule->wwuSettings[wwuType].allowUpperCase));
                ImGui::SameLine();
                ImGui::Text("Allow Upper Case");
                ImGui::NextColumn();
            }
            ImGui::EndColumns();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Prefix Settings"))
        {
            ImGui::Text("This tab lets you add a prefix to each Physical Folder of Wwise (Type of Work Units).");
            ImGui::Separator();
            ImGui::BeginColumns("prefix", 2);
            for (auto& wwuType : namingConventionModule->GetWhiteListedWwuTypes())
            {
                ImGui::Checkbox(namingConventionModule->GetStringToReplace(wwuType).substr(1).c_str(), &(namingConventionModule->wwuSettings[wwuType].applyPrefix));
                ImGui::NextColumn();
                std::string identificationName = "##" + namingConventionModule->GetStringToReplace(wwuType);
                ImGui::InputText(identificationName.c_str(), &(namingConventionModule->wwuSettings[wwuType].prefixToApply));
                ImGui::NextColumn();
            }
            ImGui::EndColumns();
            ImGui::EndTabItem(); 
        }
        if (ImGui::BeginTabItem("Suffix Settings"))
        {
            ImGui::Text("This tab lets you add a suffix to each container. To the naming convention.");
            ImGui::Text("If you have multiple text suffixes you want to make possible, please separate them with a comma.");
            ImGui::Text("Max Layers lets you define how many layers the suffix has. 'tree_lp_01' has two suffix layers. You can only have one number suffixlayer.");
            ImGui::Separator();
            ImGui::BeginColumns("suffix", 2);
            int columnCounter = 0;
            for (auto& containerType : namingConventionModule->GetWhiteListedContainers())
            {
                columnCounter++;
                ImGui::PushItemWidth(200);
                ImGui::Text(containerType.c_str());
                std::string currentContainerID = "##" + containerType;
               
                ImGui::Text("Apply String");
                ImGui::SameLine();
                ImGui::Checkbox((currentContainerID + "ApplyString").c_str(), &(namingConventionModule->containerSettings[containerType].allowStringSuffix));
                
                ImGui::SameLine();
                ImGui::Text("Apply Number");
                ImGui::SameLine();
                ImGui::Checkbox((currentContainerID +"ApplyNumber").c_str(), &(namingConventionModule->containerSettings[containerType].allowNumberSuffix));
                
                ImGui::SameLine();
                ImGui::Text("Max Layers");
                ImGui::SameLine();
                ImGui::InputInt((currentContainerID + "MaxSuffixLayers").c_str(), &(namingConventionModule->containerSettings[containerType].suffixLayers));

                ImGui::Text("Layers");
                ImGui::SameLine();
                ImGui::InputText((currentContainerID + "Layers").c_str(), &(namingConventionModule->containerSettings[containerType].stringSuffixes));

                ImGui::SameLine();
                ImGui::Text("Max Number");
                ImGui::SameLine();
                ImGui::InputInt((currentContainerID + "MaxNumber").c_str(), &(namingConventionModule->containerSettings[containerType].maxNumberAllowed), ImGuiInputTextFlags_NoHorizontalScroll);
                ImGui::PopItemWidth();
                if (columnCounter == 2)
                {
                    ImGui::Separator();
                    columnCounter = 0;
                }
                ImGui::NextColumn();
            
            }
            ImGui::EndColumns();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Info"))
        {
            if (ImGui::CollapsingHeader("Issues messages"))
            {
                ImGui::Separator();
                ImGui::Text("Hierarchy doesnt match");
                ImGui::Text("muiltiple separators");
                ImGui::Text("Space is not allowed");
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("Naming Convention"))
            {
                ImGui::Separator();
                ImGui::Text("The separator to differentiate layers is '_'. Each underscore indicates that there is a new layer of description.");
                ImGui::Separator();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

//Misc
const ImColor& GUI::ConvertWwiseColorToRGB(const int& wwiseColorID)
{
    return wwiseColors[wwiseColorID];
}

void GUI::SetDefaultStyle()
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

void GUI::RenderLayoutToolbox()
{
    if (!wwizarWwiseClient->IsConnected())
        return;

    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Toolbox"))
    {
        ImGui::End();
        return;
    }
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(ConvertWwiseColorToRGB(15)));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(ConvertWwiseColorToRGB(15)));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(ConvertWwiseColorToRGB(16)));

    if (ImGui::CollapsingHeader("Invalid Events"))
    {
        ImGui::BeginColumns("events", 2);
        ImGui::Text("Controls");
        ImGui::Dummy(ImVec2(0, 10));
        if (ImGui::Button("Gather invalid events"))
        {
            toolboxModule->GetEmptyEvents();
        }
        ImGui::Checkbox("All Events", &toolboxModule->deleteEmptyEventsForAllEvents);
        if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f)
            ImGui::SetTooltip("If checked all events will be checked.");

        ImGui::Dummy(ImVec2(0, 5));
        if (!toolboxModule->GetEventResultFiles().empty())
        {
            if (ImGui::Button("Delete"))
            {
                toolboxModule->DeleteEmptyEvent();
            }
        }

        ImGui::NextColumn(); 
        ImGui::Text("Invalid Events");
        ImGui::Separator();
        if (ImGui::BeginTable("split", 1))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(ImColor(219, 152, 80)));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(ImColor(219, 152, 80)));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(ImColor(219, 152, 80)));
            for (const auto& evt : toolboxModule->GetEventResultFiles())
            {
                ImGui::TableNextColumn(); 
                
                if(ImGui::Selectable(evt.second.name.c_str()))
                {
                    wwizarWwiseClient->FocusObjectInWwise(evt.second.guid);
                }
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::EndTable();
        }
        ImGui::EndColumns();
        ImGui::Dummy(ImVec2(0, 10));
    }

    if (ImGui::CollapsingHeader("Reset Faders"))
    {
        ImGui::BeginColumns("Faders", 2);
        ImGui::Text("Controls");
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Text("Input");
        ImGui::SameLine();
        ImGui::InputText("##", &toolboxModule->ignoreFaderNote);
        if (ImGui::Button("Get Faders in Hierarchy"))
        {
            toolboxModule->GatherFadersInHierarchy();
        }
        ImGui::Dummy(ImVec2(0, 5));
        if (!toolboxModule->GetFaderResultFiles().empty())
        {
            if (ImGui::Button("Reset"))
            {
                toolboxModule->ResetFader();
            }
        }
        
        ImGui::NextColumn();
        ImGui::Text("Faders");
        ImGui::Separator();
        if (ImGui::BeginTable("split", 1))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(ImColor(219, 152, 80)));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(ImColor(219, 152, 80)));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(ImColor(219, 152, 80)));
            for (const auto& fader : toolboxModule->GetFaderResultFiles())
            {
                ImGui::TableNextColumn();
                ImGui::Text(fader.second.name.c_str());
            }
            ImGui::EndTable();

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        ImGui::EndColumns();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}

//Helper Functions for Iam GUi
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
    if (D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D10Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

