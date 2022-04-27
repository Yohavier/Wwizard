#pragma once
#include <map>
#include <string>
#include "WwizardWwiseClient.h"
#include <memory>
#include <set>

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

enum class SettingMode
{
	SingleSoft, 
	SingleHard, 
	HierarchySoft, 
	HierarchyHard
};

struct ColorResult
{
public:
	ColorResult() = delete;
	ColorResult(std::string id, std::string parentID, std::vector<std::string> childIDs, int mode, int applyableColorID, std::string path)
		:id(id)
		, parentID(parentID)
		, childIDs(childIDs)
		, mode (mode)
		, applyableColorID(applyableColorID)
		, path(path)
	{}

	std::string id;
	std::string parentID;
	std::vector<std::string> childIDs;
	int mode = 0;
	int applyableColorID = 0;
	std::string path;
};

class ColorCodingModule
{
public:
	ColorCodingModule() = delete;
	ColorCodingModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient);
	~ColorCodingModule()
	{
		SaveColorSettings();
	}

	void FindNamesInWwise();
	void AddColorSettings(std::string name, int colorCode);

	bool CheckIfHasColorProperty(int classID);

	std::set<int> GetBlockedColors()
	{
		return blockedColors;
	}
	void DeleteColorSetting(ColorSetting setting);

private:
	void CollectColorHierarchy(std::string currentID, std::string parentID, int mode, int applyableColorID, std::string path, int actualColor);
	void ApplyColors();
	void LoadColorSettings();
	void SaveColorSettings();
	const std::string GenerateGuid();

public:
	std::map<std::string, ColorSetting> colorSettings;
	std::set<int> blockedColors;

	const char* items[4] = { "Single - soft", "Single - hard", "Hierarchy - soft", "Hierarchy - hard"};

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::map<std::string, ColorResult> colorHierarchy = {};
};

