#pragma once
#include "HomeLayout.h"
#include <Windows.h>

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
    if (ImGui::Button("github.com/Yohavier/Wwizard"))
    {
        OpenGithubInBrowser();
    }
    ImGui::End();
}

void HomeLayout::OpenGithubInBrowser()
{
    ShellExecute(NULL, "open", "http://github.com/Yohavier/Wwizard",NULL, NULL, SW_SHOWNORMAL);
}