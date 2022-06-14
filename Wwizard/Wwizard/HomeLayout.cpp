#pragma once
#include "HomeLayout.h"
#include "helper.h"

HomeLayout::HomeLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient)
    :BaseLayout(wwizardWwiseClient)
{
}

void HomeLayout::RenderLayout()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Home", NULL, ImGuiWindowFlags_NoMove))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Here is the project documentation: ");
    ImGui::SameLine();
    if (ImGui::Button("View on Github'"))
    {
        OpenGithubInBrowser();
    }
    ImGui::End();
}

void HomeLayout::OpenGithubInBrowser()
{
    OpenURL("http://github.com/Yohavier/Wwizard");
}