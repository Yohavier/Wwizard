#pragma once
#include <map>
#include <string>
#include "WwizardWwiseClient.h"
#include <memory>
#include <set>
#include "BaseModule.h"

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

enum class ColorSettingMode
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
	ColorResult(std::string objectID, std::string parentID, int colorSettingMode, int applyableColorID, std::string objectPath)
		:objectID(objectID)
		, parentID(parentID)
		, colorSettingMode (colorSettingMode)
		, applyableColorID(applyableColorID)
		, objectPath(objectPath)
	{}

	std::string objectID;
	std::string parentID;
	int colorSettingMode = 0;
	int applyableColorID = 0;
	std::string objectPath;
};

class ColorCodingModule : public BaseModule
{
public:
	ColorCodingModule() = delete;
	ColorCodingModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient);

	~ColorCodingModule();

	void BeginColorCodingProcess();

	std::set<int> GetBlockedColors();

	void CreateColorSetting(std::string name, int colorCode);
	void DeleteColorSetting(ColorSetting setting);

	void OnConnectionStatusChange(const bool newConnectionStatus) override;

private:
	void FindObjectsAffectedByColorSettings();
	void CollectObjectsInColorHierarchy(std::string currentID, std::string parentID, int mode, int applyableColorID, std::string path, int actualColor);
	void ApplyColorSettings();
	void LoadColorSettings();
	void SaveColorSettings();
	const std::string GenerateGuid();
	void ClearPreviousData();
	bool CheckIfWwiseObjectHasColorProperty(int classID);

public:
	std::map<std::string, ColorSetting> colorSettings;
	std::set<int> blockedColors;

	const char* colorSettingModes[4] = { "Single - soft", "Single - hard", "Hierarchy - soft", "Hierarchy - hard"};

private:
	std::unique_ptr<WwizardWwiseClient>& wwizardClient;

	std::map<std::string, ColorResult> colorHierarchy = {};
};

