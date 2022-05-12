#include "HomeLayout.h"

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

    ImGui::End();
}
