#pragma once
#include "NamingConventionLayout.h"
#include "GuiHelper.h"

NamingConventionLayout::NamingConventionLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<NamingConventionModule>& namingConventionModule)
    : BaseLayout(wwizardWwiseClient)
    , namingConventionModule(namingConventionModule)
{
}

void NamingConventionLayout::RenderLayout()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Naming Convention", NULL, ImGuiWindowFlags_NoMove))
    {
        ImGui::End();
        return;
    }
    if (ImGui::BeginTabBar("NamingConventionTabBar"))
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
                ColoredHeadline("CurrentName");
                ImGui::Separator();
                ImGui::TableNextColumn();
                ColoredHeadline("Desired Name");
                ImGui::Separator();
                ImGui::TableNextRow();


                for (auto& issue : namingConventionModule->GetNamingIssues())
                {
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(("##", issue.second.name.c_str()), false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        if (ImGui::IsMouseDoubleClicked(0))
                        {
                            wwizardWwiseClient->FocusObjectInWwise(issue.second.guid);
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
        if (ImGui::BeginTabItem("Workunit Settings"))
        {
            ImGui::Separator();
            ImGui::BeginColumns("workunit", 4);
            int i = 0;
            ColoredHeadline("Workunit");
            ImGui::NextColumn();
            ColoredHeadline("Space");
            ImGui::NextColumn();
            ColoredHeadline("Uppercase");
            ImGui::NextColumn();
            ColoredHeadline("Prefix");
            ImGui::Separator();
            ImGui::NextColumn();
            for (const auto& wwuType : namingConventionModule->GetWhiteListedWwuTypes())
            {
                ImGui::PushID(i);
                ImGui::Checkbox(namingConventionModule->GetStringToReplace(wwuType).substr(1).c_str(), &(namingConventionModule->wwuSettings[wwuType].applyNamingConventionCheck));
                ImGui::NextColumn();

                ImGui::Checkbox(("##allowSpace" + namingConventionModule->GetStringToReplace(wwuType).substr(1)).c_str(), &(namingConventionModule->wwuSettings[wwuType].allowSpace));
                ImGui::NextColumn();

                ImGui::Checkbox(("##allowUpperCase" + namingConventionModule->GetStringToReplace(wwuType).substr(1)).c_str(), &(namingConventionModule->wwuSettings[wwuType].allowUpperCase));
                ImGui::NextColumn();

                ImGui::Checkbox("##Prefix", &(namingConventionModule->wwuSettings[wwuType].applyPrefix));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::InputText("##PrefixInput", &(namingConventionModule->wwuSettings[wwuType].prefixToApply));
                ImGui::PopItemWidth();
                ImGui::Separator();
                ImGui::NextColumn();
                ImGui::PopID();
                i++;
            }
            ImGui::EndColumns();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Container Settings"))
        {
            ImGui::BeginColumns("suffix", 4);
            ColoredHeadline("Container");
            ImGui::NextColumn();
            ColoredHeadline("String suffix");
            ImGui::NextColumn();
            ColoredHeadline("Number suffix");
            ImGui::NextColumn();
            ColoredHeadline("Multiple suffix");
            ImGui::Separator();
            ImGui::NextColumn();

            int i = 0;
            for (auto& containerType : namingConventionModule->GetWhiteListedContainers())
            {
                ImGui::PushID(i);
                ImGui::Text(containerType.c_str());
                std::string currentContainerID = "##" + containerType;
                ImGui::NextColumn();


                ImGui::Checkbox("##ApplyString", &(namingConventionModule->containerSettings[containerType].allowStringSuffix));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::InputText("##Layers", &(namingConventionModule->containerSettings[containerType].stringSuffixes));
                ImGui::PopItemWidth();
                ImGui::NextColumn();

                ImGui::Checkbox("##ApplyNumber", &(namingConventionModule->containerSettings[containerType].allowNumberSuffix));
                ImGui::SameLine();
                ImGui::Text("Max Number");
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::InputInt("##MaxNumber", &(namingConventionModule->containerSettings[containerType].maxNumberAllowed),1);
                ImGui::PopItemWidth();
                ImGui::NextColumn();

                ImGui::Text("Max Layers");
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::InputInt("##MaxSuffixLayers", &(namingConventionModule->containerSettings[containerType].suffixLayers), 1);  
                ImGui::PopItemWidth();
                ImGui::Separator();
                ImGui::NextColumn();

                ImGui::PopID();
                i++;
            }
            ImGui::EndColumns();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

