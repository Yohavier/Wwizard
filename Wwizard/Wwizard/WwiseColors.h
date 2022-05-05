#pragma once
#include <map>
#include "imgui.h"

static std::map<const int, const ImColor> wwiseColors = { {0, ImColor(83, 83, 83)}, {1, ImColor(54, 62, 200)}, {2, ImColor(54, 62, 200)},
									  {3, ImColor(25, 85, 203)}, {4, ImColor(7, 104, 104)}, {5, ImColor(86, 115, 12)},
									  {6, ImColor(120, 113, 16)}, {7,ImColor(121, 87, 21)}, {8, ImColor(120, 66, 12)},
									  {9, ImColor(114, 56, 43)}, {10, ImColor(137, 35, 36)}, {11, ImColor(124, 38, 125)},
									  {12, ImColor(115, 42, 151)}, {13, ImColor(88, 54, 174)}, {14, ImColor(135, 135, 135)},
									  {15, ImColor(106, 111, 194)}, {16, ImColor(99, 131, 197)},{17, ImColor(67, 137, 137)},
									  {18, ImColor(83, 147, 83)}, {19, ImColor(128, 152, 61)}, {20, ImColor(160, 151, 38)},
									  {21, ImColor(171, 135, 62)}, {22, ImColor(174, 121, 65)}, {23, ImColor(174, 100, 85)},
									  {24, ImColor(185, 91, 91)}, {25, ImColor(169, 80, 170)}, {26, ImColor(187, 85, 189)},
									  {27, ImColor(134, 96, 226)} };

static const ImColor& ConvertWwiseColorToRGB(const int& wwiseColorID)
{
	return wwiseColors[wwiseColorID];
}