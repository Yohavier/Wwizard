#include "HomeLayout.h"

HomeLayout::HomeLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient)
    :BaseLayout(wwizardWwiseClient)
{
}

void HomeLayout::RenderLayout()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Home"))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Roadmap");
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ConvertWwiseColorToRGB(5)));
    ImGui::Text("---- Release Version 1.0 ----");
    ImGui::Indent(15);
    ImGui::Text("First stable version");
    ImGui::Text("Main Features:");
    ImGui::Indent(15);
    ImGui::BulletText("Naming convention checking");
    ImGui::BulletText("Creating color coding");
    ImGui::BulletText("More advanced query editor");
    ImGui::BulletText("Sort original files");
    ImGui::BulletText("Toolbox with helpers:");
    ImGui::Indent(15);
    ImGui::BulletText("Delete invalid events");
    ImGui::BulletText("Reset Fader");
    ImGui::Unindent(45);
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::BulletText("Dig into ImGui and create proper UI setup");
    ImGui::BulletText("Create Nodebase queryeditor for combining queries");
    ImGui::BulletText("Sort Originals P4 integration");
    ImGui::BulletText("QueryEditor: open and import wav file to reaper shortcut select file and ctrl+r");
    ImGui::BulletText("Remove Unreferenced Sounds");
    ImGui::BulletText("Remove unused Game syncs");
    ImGui::BulletText("Analyse audio spectrum of whole project");

    ImGui::End();
}
