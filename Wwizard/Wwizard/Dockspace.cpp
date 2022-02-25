#include "Dockspace.h"
#include <stdio.h>

namespace wwizardGUI
{
	Dockspace::Dockspace()
	{
		
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
        
        if (currentLayout == Layout::QUERYEDITOR)
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
        currentLayout = newLayout;
    }

    void Dockspace::CreateQueryEditor(bool* p_open)
    { 
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("All Queries", p_open))
        {
            ImGui::End();
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::BeginTable("split", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            // Iterate placeholder objects (all the same data)
            for (int obj_i = 0; obj_i < 4; obj_i++)
            {
                ShowPlaceholderObject("Object", obj_i);
                //ImGui::Separator();
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }

    void Dockspace::CreateHomeLayout()
    {
        
    }

    void Dockspace::ShowPlaceholderObject(const char* prefix, int uid)
    {
        // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
        ImGui::PushID(uid);

        // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        bool node_open = ImGui::TreeNode("Object", prefix, uid);


        if (node_open)
        {
            static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
            for (int i = 0; i < 8; i++)
            {
                ImGui::PushID(i); // Use field index as identifier.
                if (i < 2)
                {
                    ShowPlaceholderObject("Child", 424242);
                }
                else
                {
                    // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::AlignTextToFramePadding();
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                    ImGui::Selectable("Field");
                    //ImGui::Selectable(, flags, "Field_%d", i);
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}