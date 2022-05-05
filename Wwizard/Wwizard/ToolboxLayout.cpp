#include "ToolboxLayout.h"
#include "WwiseColors.h"

ToolboxLayout::ToolboxLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<ToolboxModule>& toolboxModule)
	: BaseLayout(wwizardWwiseClient)
	, toolboxModule(toolboxModule)
{
}

void ToolboxLayout::RenderLayout()
{
    if (!wwizardWwiseClient->IsConnected())
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

                if (ImGui::Selectable(evt.second.name.c_str()))
                {
                    wwizardWwiseClient->FocusObjectInWwise(evt.second.guid);
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
