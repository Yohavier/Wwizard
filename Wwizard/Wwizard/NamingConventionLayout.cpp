#include "NamingConventionLayout.h"

NamingConventionLayout::NamingConventionLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<NamingConventionModule>& namingConventionModule)
    : BaseLayout(wwizardWwiseClient)
    , namingConventionModule(namingConventionModule)
{
}

void NamingConventionLayout::RenderLayout()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Naming Convention"))
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
                ImGui::Checkbox((currentContainerID + "ApplyNumber").c_str(), &(namingConventionModule->containerSettings[containerType].allowNumberSuffix));

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
