#include "ColorCodingModule.h"

ColorCodingModule::ColorCodingModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	: wwizardClient(wwizardClient)
{

}

void ColorCodingModule::FindNamesInWwise()
{
	colorHierarchy.clear();
	for (const auto& colorSetting : colorSettings)
	{
		AkJson option(AkJson::Map{ { "return", AkJson::Array{ AkVariant("path"), AkVariant("name"), AkVariant("type"), AkVariant("id"), AkVariant("color")}}});
		AkJson result = wwizardClient->GetObjectsByPartName(colorSetting.second.name, option);

		for (const auto& colorObject : result["return"].GetArray())
		{
			std::string colName = colorObject["name"].GetVariant().GetString();
			int keywordStart = colName.find(colorSetting.second.name);

			if (keywordStart != -1)
			{
				if (keywordStart == 0 || colName[keywordStart - 1] == '_')
				{
					if (colName.size() == keywordStart + colorSetting.second.name.size() || colName[keywordStart + colorSetting.second.name.size()] == '_')
					{
						if (colorObject["type"].GetVariant().GetString() != "AudioFileSource")
						{
							CollectColorHierarchy(colorObject["id"].GetVariant().GetString(), "", colorSetting.second.settingMode, colorSetting.second.colorCode, colorObject["path"].GetVariant().GetString(), colorObject["color"].GetVariant().GetInt8());
						}
					}
				}
			}
		}
	}
	ApplyColors();
}

void ColorCodingModule::CollectColorHierarchy(std::string currentID, std::string parentID, int mode, int applyableColorID, std::string path, int actualColor)
{
	AkJson option(AkJson::Map{ { "return", AkJson::Array{ AkVariant("path"), AkVariant("name"), AkVariant("type"), AkVariant("id"), AkVariant("color")}} });
	AkJson result = wwizardClient->GetChildrenFromGuid(currentID, option);
	
	std::vector<std::string> childIDs;

	for (const auto& child : result["return"].GetArray())
	{
		if (child["type"].GetVariant().GetString() != "Action" && child["type"].GetVariant().GetString() != "AudioFileSource")
		{
			childIDs.push_back(child["id"].GetVariant().GetString());
		}
	}

	SettingMode newMode = (SettingMode)mode;

	if (colorHierarchy.find(currentID) != colorHierarchy.end())
	{
		colorHierarchy.at(currentID).parentID = parentID;
		SettingMode currentMode = (SettingMode)colorHierarchy.at(currentID).mode;

		if (newMode == SettingMode::SingleHard)
		{
			colorHierarchy.at(currentID).mode = (int)SettingMode::SingleHard;
			colorHierarchy.at(currentID).applyableColorID = applyableColorID;
		}
		else if (newMode == SettingMode::HierarchyHard)
		{
			if (currentMode != SettingMode::SingleSoft)
			{
				colorHierarchy.at(currentID).mode = (int)SettingMode::HierarchyHard;
				colorHierarchy.at(currentID).applyableColorID = applyableColorID;
			}
		}
		else if (newMode == SettingMode::SingleSoft)
		{
			if (currentMode == SettingMode::HierarchySoft)
			{
				colorHierarchy.at(currentID).mode = (int)SettingMode::SingleSoft;
				colorHierarchy.at(currentID).applyableColorID = applyableColorID;
			}
		}
		else if (colorHierarchy.at(currentID).mode == -1)
		{
			colorHierarchy.at(currentID).mode = (int)newMode;
			colorHierarchy.at(currentID).applyableColorID = applyableColorID;
		}
	}
	else
	{
		colorHierarchy.emplace(currentID, ColorResult(currentID, parentID, childIDs, mode, applyableColorID, path));
	}

	for (const auto& col : blockedColors)
	{
		if (col == actualColor)
		{
			colorHierarchy.at(currentID).applyableColorID = actualColor;
		}
	}

	for  (const auto& child : result["return"].GetArray())
	{
		if (child["type"].GetVariant().GetString() != "Action" && child["type"].GetVariant().GetString() != "AudioFileSource")
		{
			if (newMode == SettingMode::HierarchyHard || newMode == SettingMode::HierarchySoft)
			{
				CollectColorHierarchy(child["id"].GetVariant().GetString(), currentID, mode, applyableColorID, child["path"].GetVariant().GetString(), child["color"].GetVariant().GetInt8());
			}
			else
			{
				CollectColorHierarchy(child["id"].GetVariant().GetString(), currentID, -1, child["color"].GetVariant().GetInt8(), child["path"].GetVariant().GetString(), child["color"].GetVariant().GetInt8());
			}
		}
	}
}

void ColorCodingModule::ApplyColors()
{
	for (const auto& colorObject : colorHierarchy)
	{
		if (colorObject.second.mode == 2 || colorObject.second.mode == 3)
		{
			if (colorObject.second.parentID != "" && colorHierarchy.at(colorObject.second.parentID).mode != 0 && colorHierarchy.at(colorObject.second.parentID).mode != 1)
			{
				AkJson argEnable(AkJson::Map{
				{
					{"object", AkVariant(colorObject.second.path)},
					{"property", AkVariant("OverrideColor")},
					{"value", AkVariant(false)}
				} });
				wwizardClient->SetProperty(argEnable);
			}
			else
			{
				AkJson argEnable(AkJson::Map{
				{
					{"object", AkVariant(colorObject.second.path)},
					{"property", AkVariant("OverrideColor")},
					{"value", AkVariant(true)}
				} });
				wwizardClient->SetProperty(argEnable);

				AkJson argSetColor(AkJson::Map{
				{
					{"object", AkVariant(colorObject.second.path)},
					{"property", AkVariant("Color")},
					{"value", AkVariant(colorObject.second.applyableColorID)}
				} });
				wwizardClient->SetProperty(argSetColor);
			}
		}
		else
		{
			AkJson argEnable(AkJson::Map{
				{
					{"object", AkVariant(colorObject.second.path)},
					{"property", AkVariant("OverrideColor")},
					{"value", AkVariant(true)}
				} });
			wwizardClient->SetProperty(argEnable);
			AkJson argSetColor(AkJson::Map{
			{
				{"object", AkVariant(colorObject.second.path)},
				{"property", AkVariant("Color")},
				{"value", AkVariant(colorObject.second.applyableColorID)}
			} });
			wwizardClient->SetProperty(argSetColor);
		}
	}
}

void ColorCodingModule::AddColorSettings(std::string name, int colorCode)
{
	colorSettings.emplace(colorSettings.size() + 1, ColorSettings(name, colorCode, colorSettings.size() + 1));
}

