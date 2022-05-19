#pragma once
#include "GUI.h"
#include "WwiseColors.h"

static ID3D10Device* g_pd3dDevice = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D10RenderTargetView* g_mainRenderTargetView = NULL;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GUI::GUI(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, 
    std::unique_ptr<SettingHandler>& settingHandler, 
    std::unique_ptr<QueryEditorModule>& queryEditorModule, 
    std::unique_ptr<SortOriginalsModule>& sortOriginalsModule, 
    std::unique_ptr<NamingConventionModule>& namingConventionModule, 
    std::unique_ptr<ToolboxModule>& toolboxModule, 
    std::unique_ptr<ColorCodingModule>& colorCodingModule)
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
    ImGui::LoadIniSettingsFromDisk((static_cast<std::string>(SOLUTION_DIR) + "/Wwizard/SavedData/imgui.ini").c_str());
    (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX10_Init(g_pd3dDevice);

    float fontSize = 18.0f;
    auto fontpath = static_cast<std::string>(SOLUTION_DIR) + "/Wwizard/assets/fonts/OpenSans-Bold.ttf";
    io->FontDefault = io->Fonts->AddFontFromFileTTF(fontpath.c_str(), fontSize);
    SetDefaultStyle();

    colorCodingLayout.reset(new ColorCodingLayout(wwizardWwiseClient, colorCodingModule));
    queryEditorLayout.reset(new QueryEditorLayout(wwizardWwiseClient, queryEditorModule));
    settingLayout.reset(new SettingLayout(wwizardWwiseClient, settingHandler));
    homeLayout.reset(new HomeLayout(wwizarWwiseClient));
    sortOriginalsLayout.reset(new SortOriginalsLayout(wwizarWwiseClient, sortOriginalsModule));
    namingConventionLayout.reset(new NamingConventionLayout(wwizardWwiseClient, namingConventionModule));
    toolboxLayout.reset(new ToolboxLayout(wwizardWwiseClient, toolboxModule));
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

    //ImGui::ShowDemoWindow();
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
    if (wwizarWwiseClient->DidConnectionStatusChange() && settingHandler->IsProjectPathValid())
    {
        namingConventionModule->OnConnectionStatusChange(wwizarWwiseClient->IsConnected());
        colorCodingModule->OnConnectionStatusChange(wwizarWwiseClient->IsConnected());
        toolboxModule->OnConnectionStatusChange(wwizarWwiseClient->IsConnected());
        queryEditorModule->OnConnectionStatusChange(wwizarWwiseClient->IsConnected());
        sortOriginalsModule->OnConnectionStatusChange(wwizarWwiseClient->IsConnected());
    }
    wwizarWwiseClient->SyncPreviousConnectionStatus();
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
            }
            if (ImGui::MenuItem("Settings", NULL))
            {
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
            if (ImGui::MenuItem("Naming Convention", NULL, false, wwizarWwiseClient->IsConnected()))
            {
                SetLayout(Layout::NAMINGCONVENTION);
            }

            if (ImGui::MenuItem("Sort Originals", NULL, false, wwizarWwiseClient->IsConnected()))
            {
                SetLayout(Layout::SORTORIGINALS);
            }

            if (ImGui::MenuItem("Query Editor", NULL, false, wwizarWwiseClient->IsConnected()))
            {
                SetLayout(Layout::QUERYEDITOR);
            }

            if (ImGui::MenuItem("Color Coding", NULL, false, wwizarWwiseClient->IsConnected()))
            {
                SetLayout(Layout::COLORCODING);
            }

            if (ImGui::MenuItem("Toolbox", NULL, false, wwizarWwiseClient->IsConnected()))
            {
                SetLayout(Layout::TOOLBOX);
            }
            
            ImGui::EndMenu();
        }

        if (wwizarWwiseClient->IsConnected() && settingHandler->IsProjectPathValid())
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

void GUI::RenderLayoutSettings()
{
    settingLayout->RenderLayout();
}

void GUI::RenderLayoutQueryEditor()
{
    if (settingHandler->IsProjectPathValid())
        queryEditorLayout->RenderLayout();
}

void GUI::RenderLayoutHome()
{
    homeLayout->RenderLayout();
}

void GUI::RenderColorCodingModule()
{
    if (settingHandler->IsProjectPathValid())
        colorCodingLayout->RenderLayout();
}

void GUI::RenderLayoutSortOriginals()
{
    if (settingHandler->IsProjectPathValid())
        sortOriginalsLayout->RenderLayout();
}

void GUI::RenderLayoutNamingConvention()
{
    if (settingHandler->IsProjectPathValid())
        namingConventionLayout->RenderLayout();
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
    toolboxLayout->RenderLayout();
}


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

