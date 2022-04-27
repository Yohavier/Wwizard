#include "ColorCodingModule.h"

ColorCodingModule::ColorCodingModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	: wwizardClient(wwizardClient)
{
	LoadColorSettings();
}

void ColorCodingModule::FindNamesInWwise()
{
	colorHierarchy.clear();
	for (const auto& colorSetting : colorSettings)
	{
		AkJson option(AkJson::Map{ { "return", AkJson::Array{ AkVariant("path"), AkVariant("name"), AkVariant("type"), AkVariant("id"), AkVariant("color"), AkVariant("classId")}}});
		AkJson result = wwizardClient->GetObjectsByPartName(colorSetting.second.name, option);

		if (colorSetting.second.name == "")
		{
			continue;
		}

		for (const auto& colorObject : result["return"].GetArray())
		{
			std::string colName = colorObject["name"].GetVariant().GetString();
			int keywordStart = static_cast<int>(colName.find(colorSetting.second.name));

			if (keywordStart != -1)
			{
				if (keywordStart == 0 || colName[keywordStart - 1] == '_')
				{
					if (colName.size() == keywordStart + colorSetting.second.name.size() || colName[keywordStart + colorSetting.second.name.size()] == '_')
					{
						if (CheckIfHasColorProperty(colorObject["classId"].GetVariant().GetUInt32()))
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

void ColorCodingModule::DeleteColorSetting(ColorSetting setting)
{
	if (colorSettings.find(setting.settingID) != colorSettings.end())
	{
		colorSettings.erase(setting.settingID);
	}
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
	std::string newGUID = GenerateGuid();
	colorSettings.emplace(newGUID, ColorSetting(name, newGUID, colorCode));
}

bool ColorCodingModule::CheckIfHasColorProperty(int classID)
{
	AkJson result = wwizardClient->GetObjectPropertyList(classID);

	for (const auto& res : result["return"].GetArray())
	{
		if(res.GetVariant().GetString() == "Color")
		{
			return true;
		}
	}
	return false;
}

void ColorCodingModule::SaveColorSettings()
{
	rapidjson::Document d;
	d.SetObject();

	rapidjson::Value rapidColorSettings;
	rapidColorSettings.SetArray();

	for (const auto& setting : colorSettings)
	{
		rapidjson::Value rapidSetting(rapidjson::kObjectType);
		{
			rapidjson::Value name;
			name = rapidjson::StringRef(setting.second.name.c_str());
			rapidSetting.AddMember("name", name, d.GetAllocator());

			rapidjson::Value colorCode;
			colorCode = setting.second.colorCode;
			rapidSetting.AddMember("colorCode", colorCode, d.GetAllocator());

			rapidjson::Value id;
			id = rapidjson::StringRef(setting.second.settingID.c_str());
			rapidSetting.AddMember("settingID", id, d.GetAllocator());

			rapidjson::Value settingMode;
			settingMode = setting.second.settingMode;
			rapidSetting.AddMember("settingMode", settingMode, d.GetAllocator());
		}
		rapidColorSettings.PushBack(rapidSetting, d.GetAllocator());
	}
	d.AddMember("ColorSettings", rapidColorSettings, d.GetAllocator());
	
	rapidjson::Value rapidBlockedColors;
	rapidBlockedColors.SetObject();
	rapidjson::Value rapidBlockedList(rapidjson::kArrayType);

	for (const auto& excludeColor : blockedColors)
	{
		rapidBlockedList.PushBack(excludeColor, d.GetAllocator());
	}

	rapidBlockedColors.AddMember("colorID", rapidBlockedList, d.GetAllocator());
	d.AddMember("BlockedColors", rapidBlockedColors, d.GetAllocator());
	
	auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/ColorSettings.json";
	FILE* fp = fopen(path.c_str(), "wb");
	if (fp != 0)
	{
		char* writeBuffer = new char[65536];
		rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		d.Accept(writer);

		fclose(fp);
	}
}

void ColorCodingModule::LoadColorSettings()
{
	auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/ColorSettings.json";
	FILE* fp = fopen(path.c_str(), "rb");
	
	if (fp != 0)
	{
		char* readBuffer = new char[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

		rapidjson::Document d;
		d.ParseStream(is);
		fclose(fp);

		for (int i = 0; i < static_cast<int>(d["ColorSettings"].Size()); i++)
		{
			ColorSetting newColorSetting = ColorSetting(d["ColorSettings"][i]["name"].GetString(), d["ColorSettings"][i]["colorCode"].GetInt(), d["ColorSettings"][i]["settingID"].GetString(), d["ColorSettings"][i]["settingMode"].GetInt());
			colorSettings.emplace(d["ColorSettings"][i]["settingID"].GetString(), newColorSetting);
		}

		for (int i = 0; i < static_cast<int>(d["BlockedColors"]["colorID"].Size()); i++)
		{
			blockedColors.emplace(d["BlockedColors"]["colorID"][i].GetInt());
		}
	}
}

const std::string ColorCodingModule::GenerateGuid()
{
	return std::to_string(((long long)rand() << 32) | rand());
}