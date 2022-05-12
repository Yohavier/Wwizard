#pragma once
#include "ColorCodingLayout.h"
#include "WwiseColors.h"

ColorCodingLayout::ColorCodingLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<ColorCodingModule>& colorCodingModule)
	: BaseLayout(wwizardWwiseClient)
	, colorCodingModule(colorCodingModule)
{
}

void ColorCodingLayout::RenderLayout()
{
    ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Color Coding", NULL, ImGuiWindowFlags_NoMove))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Single always stronger than hierarchy");
    if (ImGui::Button("Add  Setting"))
    {
        colorCodingModule->CreateColorSetting("", 0);
    }

    int id = 0;
    for (auto& colorSetting : colorCodingModule->colorSettings)
    {
        ImGui::PushID(id);
        ImGui::Text("Keywords: ");
        ImGui::SameLine();
        ImGui::InputText("##Keywords: ", &(colorSetting.second.name));

        if (ImGui::BeginPopupModal("ColorPicker"))
        {
            ImGui::BeginColumns("Color Pop Up", 7);
            int column = 0;
            for (const auto& color : wwiseColors)
            {
                if (ImGui::ColorButton(std::to_string(color.first).c_str(), color.second, 0, ImVec2(50, 50)))
                {
                    colorSetting.second.colorCode = color.first;
                    ImGui::CloseCurrentPopup();
                }
                column++;
                if (column == 4)
                {
                    ImGui::NextColumn();
                    column = 0;
                }
            }
            ImGui::EndColumns();

            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.25f);
            if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Text("Color ");
        ImGui::SameLine();
        if (ImGui::ColorButton("Select Color", ConvertWwiseColorToRGB(colorSetting.second.colorCode)))
        {
            ImGui::OpenPopup("ColorPicker");
        }
        ImGui::Text("Mode: ");
        ImGui::SameLine();
        ImGui::Combo("##mode", &colorSetting.second.settingMode, colorCodingModule->colorSettingModes, 4);
        if (ImGui::Button("Delete Color setting"))
        {
            colorCodingModule->DeleteColorSetting(colorSetting.second);
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
        id++;
        ImGui::Separator();
    }

    ImGui::Text("Colors that should never change");
    if (ImGui::BeginPopupModal("ColorSelector"))
    {
        ImGui::BeginColumns("Color Pop Up", 7);
        int column = 0;
        for (const auto& color : wwiseColors)
        {
            if (ImGui::ColorButton(std::to_string(color.first).c_str(), color.second, 0, ImVec2(50, 50)))
            {
                colorCodingModule->blockedColors.insert(color.first);
                ImGui::CloseCurrentPopup();
            }
            column++;
            if (column == 4)
            {
                ImGui::NextColumn();
                column = 0;
            }
        }
        ImGui::EndColumns();

        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x * 0.25f);
        if (ImGui::Button("Close", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::Button("+"))
    {
        ImGui::OpenPopup("ColorSelector");
    }
    int idd = 0;
    for (const auto& blockedColor : colorCodingModule->GetBlockedColors())
    {
        ImGui::PushID(idd);
        ImGui::SameLine();
        if (ImGui::ColorButton("blocked Color", ConvertWwiseColorToRGB(blockedColor)))
        {
            colorCodingModule->blockedColors.erase(blockedColor);
        }
        ImGui::PopID();
        idd++;
    }

    if (ImGui::Button("Apply color coding to Wwise"))
    {
        colorCodingModule->BeginColorCodingProcess();
    }
    ImGui::End();
}
