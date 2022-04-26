#pragma once
#include <map>
#include <string>
#include "WwizardWwiseClient.h"
#include <memory>
#include <set>

struct ColorSettings
{
	ColorSettings() = delete;
	ColorSettings(std::string name, int colorCode, int id)
		: name(name)
		, colorCode(colorCode)
		, id(id)
	{}

	std::string name = "";
	int colorCode = -1;
	int id;
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

	void FindNamesInWwise();
	void AddColorSettings(std::string name, int colorCode);
	std::set<int> GetBlockedColors()
	{
		return blockedColors;
	}

private:
	void CollectColorHierarchy(std::string currentID, std::string parentID, int mode, int applyableColorID, std::string path, int actualColor);
	void ApplyColors();

public:
	std::map<int, ColorSettings> colorSettings;
	std::set<int> blockedColors;

	const char* items[4] = { "Single - soft", "Single - hard", "Hierarchy - soft", "Hierarchy - hard"};

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::map<std::string, ColorResult> colorHierarchy = {};
};

