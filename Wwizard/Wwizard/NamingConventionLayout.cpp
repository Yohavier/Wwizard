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
            static std::string saveAsName = "";
            static bool existsFlag = false;
            if (ImGui::BeginPopupModal("Save As"))
            {
                ImGui::InputText("##save", &saveAsName);
                if (ImGui::Button("Abort"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Save As"))
                {
                    if (!namingConventionModule->NamingConventionNameAlreadyExists(saveAsName))
                    {
                        namingConventionModule->SaveAsNewSetting(saveAsName, *namingConventionModule->activeNamingSetting);
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        existsFlag = true;
                    }
                }
                if (existsFlag)
                {
                    ImGui::Text("Settingname already exists!");
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginCombo("##LoadedSettings", namingConventionModule->activeSettingName.c_str()))
            {
                for (int n = 0; n < namingConventionModule->allSettings.size(); n++)
                {
                    bool is_selected = (namingConventionModule->activeSettingName == namingConventionModule->allSettings.at(n));
                    if (ImGui::Selectable(namingConventionModule->allSettings.at(n).c_str(), is_selected))
                    {
                        namingConventionModule->ChangeNamingSetting(namingConventionModule->allSettings.at(n).c_str());
                    }               
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                namingConventionModule->SaveSettings(namingConventionModule->activeSettingName, *namingConventionModule->activeNamingSetting);
            }
            ImGui::SameLine();
            if (ImGui::Button("Save As"))
            {
                saveAsName = "";
                existsFlag = false;
                ImGui::OpenPopup("Save As");
            }

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
                ImGui::Text(namingConventionModule->GetStringToReplace(wwuType).substr(1).c_str());
                ImGui::NextColumn();

                ImGui::Checkbox(("##allowSpace" + namingConventionModule->GetStringToReplace(wwuType).substr(1)).c_str(), &(namingConventionModule->activeNamingSetting->wwuSettings[wwuType].allowSpace));
                ImGui::NextColumn();

                ImGui::Checkbox(("##allowUpperCase" + namingConventionModule->GetStringToReplace(wwuType).substr(1)).c_str(), &(namingConventionModule->activeNamingSetting->wwuSettings[wwuType].allowUpperCase));
                ImGui::NextColumn();

                ImGui::Checkbox("##Prefix", &(namingConventionModule->activeNamingSetting->wwuSettings[wwuType].applyPrefix));
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::InputText("##PrefixInput", &(namingConventionModule->activeNamingSetting->wwuSettings[wwuType].prefixToApply));
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
            static std::string newSuffix = "";

            ImGui::BeginColumns("suffix", 3);
            ColoredHeadline("Container");
            ImGui::NextColumn();
            ColoredHeadline("String suffix");
            ImGui::NextColumn();
            ColoredHeadline("Number suffix");
            ImGui::Separator();
            ImGui::NextColumn();

            int i = 0;
            for (auto& containerType : namingConventionModule->GetWhiteListedContainers())
            {
                ImGui::PushID(i);
                ImGui::Text(containerType.c_str());
                std::string currentContainerID = "##" + containerType;
                ImGui::NextColumn();


                ImGui::Checkbox("##ApplyString", &(namingConventionModule->activeNamingSetting->containerSettings[containerType].applyStringSuffix));
                ImGui::SameLine();

                if (ImGui::BeginPopupModal("Create new suffix"))
                {
                    ImGui::Text("Enter new suffix :");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(100);
                    ImGui::InputText("##NewSuffix", &newSuffix);
                    ImGui::PopItemWidth();
                    if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Create", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
                    {
                        namingConventionModule->activeNamingSetting->containerSettings[containerType].AddNewSuffix(newSuffix);
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (ImGui::Button("+"))
                {
                    newSuffix = "";
                    ImGui::OpenPopup("Create new suffix"); 
                }

                for (const auto& suffix : namingConventionModule->activeNamingSetting->containerSettings[containerType].stringSuffixVector)
                {
                    ImGui::SameLine();
                    if (ImGui::Button(suffix.c_str()))
                    {
                        namingConventionModule->activeNamingSetting->containerSettings[containerType].RemoveSuffix(suffix);
                    }
                }


                //ImGui::PushItemWidth(100);
                //ImGui::InputText("##Layers", &(namingConventionModule->containerSettings[containerType].stringSuffixes));
                //ImGui::PopItemWidth();
                ImGui::NextColumn();

                ImGui::Checkbox("##ApplyNumber", &(namingConventionModule->activeNamingSetting->containerSettings[containerType].applyNumberSuffix));
                ImGui::SameLine();
                ImGui::Text("Max Number");
                ImGui::SameLine();
                ImGui::PushItemWidth(100);
                ImGui::InputInt("##MaxNumber", &(namingConventionModule->activeNamingSetting->containerSettings[containerType].maxNumberAllowed),1);
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

