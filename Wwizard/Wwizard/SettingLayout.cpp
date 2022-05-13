#include "SettingLayout.h"

static std::string projectPathSetting;
static std::string sdkPathSetting;
static std::string waapiIPSetting;
static int waapiPortSetting;

SettingLayout::SettingLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler)
	: BaseLayout(wwizardWwiseClient)
	, settingHandler(settingHandler)
{
    projectPathSetting = settingHandler->GetWwisProjectPathRef();
    sdkPathSetting = settingHandler->GetSDKPath();
    waapiIPSetting = settingHandler->GetWaapiIP();
    waapiPortSetting = settingHandler->GetWaaapiPort();
}

void SettingLayout::RenderLayout()
{
    ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoMove))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Project Path : ");
    ImGui::SameLine();
    ImGui::InputText("##1", &projectPathSetting);

    ImGui::Text("SDK Path       : ");
    ImGui::SameLine();
    ImGui::InputText("##2", &sdkPathSetting);

    ImGui::Text("Waapi IP        : ");
    ImGui::SameLine();
    ImGui::InputText("##3", &waapiIPSetting);

    ImGui::Text("Waapi Port   : ");
    ImGui::SameLine();
    ImGui::InputInt("##4", &waapiPortSetting);

    if (ImGui::Button("Save Settings", ImVec2(200, 50)))
    {
        if (waapiIPSetting != settingHandler->GetWaapiIP() || waapiPortSetting != settingHandler->GetWaaapiPort())
        {
            settingHandler->SaveSettings(projectPathSetting, sdkPathSetting, waapiIPSetting, waapiPortSetting);
            wwizardWwiseClient->StartReconnectionThread();
            //queryEditorModule->ResetQueryModule(wwizarWwiseClient);
        }
        else
        {
            settingHandler->SaveSettings(projectPathSetting, sdkPathSetting, waapiIPSetting, waapiPortSetting);
        }
    }
    ImGui::End();
}
