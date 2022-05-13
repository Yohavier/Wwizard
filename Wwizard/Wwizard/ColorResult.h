#pragma once
#include <string>

struct ColorResult
{
public:
	ColorResult() = delete;
	ColorResult(std::string objectID, std::string parentID, int colorSettingMode, int applyableColorID, std::string objectPath)
		:objectID(objectID)
		, parentID(parentID)
		, colorSettingMode(colorSettingMode)
		, applyableColorID(applyableColorID)
		, objectPath(objectPath)
	{}

	std::string objectID;
	std::string parentID;
	int colorSettingMode = 0;
	int applyableColorID = 0;
	std::string objectPath;
};