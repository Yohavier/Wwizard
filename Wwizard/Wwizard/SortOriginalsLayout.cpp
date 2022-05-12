#pragma once
#include "imgui.h"
#include "SortOriginalsLayout.h"

SortOriginalsLayout::SortOriginalsLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SortOriginalsModule>& sortOriginalsModule)
    :BaseLayout(wwizardWwiseClient)
    ,sortOriginalsModule(sortOriginalsModule)
{

}

void SortOriginalsLayout::RenderLayout()
{
    if (!wwizardWwiseClient->IsConnected())
        return;
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Sort Originals", NULL, ImGuiWindowFlags_NoMove))
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
    if (!ImGui::Begin("Sorting Settings", NULL, ImGuiWindowFlags_NoMove))
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
