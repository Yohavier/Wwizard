#include "Dockspace.h"
#include <stdio.h>

namespace wwizard
{
	Dockspace::Dockspace(cWwizardWwiseClient* wwizardWwiseClient)
        : m_currentLayout(Layout::HOME)
        , m_wwizarWwiseClient(wwizardWwiseClient)
	{ 
        //Init all modules
        m_queryEditorModule.Init(m_wwizarWwiseClient);
        std::cout << "Initialized Dockspace" << std::endl;
	}

	void Dockspace::Render(bool* p_open)
	{
        // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
        // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
        // In this specific demo, we are not using DockSpaceOverViewport() because:
        // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
        // - we allow the host window to have padding (when opt_padding == true)
        // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
        // TL;DR; this demo is more complicated than what you would normally use.
        // If we removed all the options we are showcasing, this demo would become:
        //     void ShowExampleAppDockSpace()
        //     {
        //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        //     }

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

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

        if (opt_fullscreen)
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
        
        if (m_currentLayout == Layout::QUERYEDITOR)
        {
            CreateQueryEditor(p_open);
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
                ImGui::MenuItem("Settings", NULL);
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
            ImGui::EndMenuBar();
        }
    }

    void Dockspace::SetLayout(Layout newLayout)
    {
        m_currentLayout = newLayout;
    }

    void Dockspace::CreateQueryEditor(bool* p_open)
    {       
        //Available Queries Field
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Available Queries", p_open))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::BeginTable("availableWwiseQueries", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            ShowPlaceholderObject(m_queryEditorModule.m_wwiseQueryHierarchy);
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
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
            //ShowPlaceholderObject(m_queryEditorModule.m_wwiseQueries);
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::End();

        //Details window
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Details", p_open))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::PopStyleVar();
        ImGui::End();
    }

    void Dockspace::ShowPlaceholderObject(BaseQueryStructure* queryObject)
    {
        // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
        ImGui::PushID(0);

        // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        static int item_current_idx = 0;

        if (queryObject->m_structureType == QueryType::FOLDER)
        {
            bool node_open = ImGui::TreeNode("", queryObject->m_name.c_str());

            if (node_open)
            {
                for (int i = 0; i < queryObject->subDir.size(); i++)
                {
                    ImGui::PushID(i); // Use field index as identifier.

                    ShowPlaceholderObject(queryObject->subDir[i]);

                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
        else
        {
            const bool is_selected = (item_current_idx == queryObject->m_guuid);
            if (ImGui::Selectable(queryObject->m_name.c_str(), is_selected)) 
            {
                //if (is_selected)
                    //m_queryEditorModule.AddToActiveQueryList(queryObject->m_guuid);

                item_current_idx = queryObject->m_guuid;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::PopID();
    }

    void Dockspace::CreateHomeLayout()
    {
        
    }
}