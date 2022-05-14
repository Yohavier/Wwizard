#pragma once
#include <map>
#include <string>
#include "WwizardWwiseClient.h"
#include <memory>
#include <set>
#include "BaseModule.h"
#include "ResultFile.h"
#include "ColorSetting.h"
#include "ColorSettingMode.h"

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
	void CollectObjectsInColorHierarchy(std::string currentID, std::string parentID, int mode, int applyableColorID, std::string path, int actualColor, std::string name);
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

	std::map<std::string, ColorResultFile> colorHierarchy = {};
};

