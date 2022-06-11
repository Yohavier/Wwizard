#pragma once
#include "imgui.h"
#include <string>

static void ColoredHeadline(std::string text)
{
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(99, 131, 197, 255));
	ImGui::Text(text.c_str());
	ImGui::PopStyleColor();
}