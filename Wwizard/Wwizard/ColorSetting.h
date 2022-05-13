#pragma once
#include <string>

struct ColorSetting
{
	ColorSetting() = delete;
	ColorSetting(std::string name, std::string settingID, int colorCode)
		: name(name)
		, colorCode(colorCode)
		, settingID(settingID)
	{}

	ColorSetting(std::string name, int colorCode, std::string settingID, int settingMode)
		: name(name)
		, colorCode(colorCode)
		, settingID(settingID)
		, settingMode(settingMode)
	{}

	std::string name = "";
	int colorCode = -1;
	std::string settingID;
	int settingMode = 0;
};