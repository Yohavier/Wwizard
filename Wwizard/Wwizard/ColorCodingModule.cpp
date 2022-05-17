#pragma once
#include "ColorCodingModule.h"

ColorCodingModule::ColorCodingModule(std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	: wwizardClient(wwizardClient)
{
	LoadColorSettings();
}

ColorCodingModule::~ColorCodingModule()
{
	SaveColorSettings();
}

void ColorCodingModule::BeginColorCodingProcess()
{
	ClearPreviousData();
	FindObjectsAffectedByColorSettings();
	ApplyColorSettings();
}

void ColorCodingModule::FindObjectsAffectedByColorSettings()
{
	for (const auto& colorSetting : colorSettings)
	{
		std::vector<std::string> optionList = { "path", "name", "type", "id", "color", "classId" };
		AkJson result = wwizardClient->GetObjectsByPartName(colorSetting.second.name, optionList);

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
						if (CheckIfWwiseObjectHasColorProperty(colorObject["classId"].GetVariant().GetUInt32()))
						{
							CollectObjectsInColorHierarchy(colorObject["id"].GetVariant().GetString(), "", colorSetting.second.settingMode, colorSetting.second.colorCode, colorObject["path"].GetVariant().GetString(), colorObject["color"].GetVariant().GetInt8(), colName);
						}
					}
				}
			}
		}
	}
}

void ColorCodingModule::CollectObjectsInColorHierarchy(std::string currentID, std::string parentID, int mode, int applyableColorID, std::string path, int actualColor, std::string name)
{	
	std::vector<std::string> optionList = { "path", "color", "name", "type", "id" };

	AkJson result = wwizardClient->GetChildrenFromGuid(currentID, optionList);

	std::vector<std::string> childIDs;

	for (const auto& child : result["return"].GetArray())
	{
		if (child["type"].GetVariant().GetString() != "Action" && child["type"].GetVariant().GetString() != "AudioFileSource")
		{
			childIDs.push_back(child["id"].GetVariant().GetString());
		}
	}

	ColorSettingMode newMode = (ColorSettingMode)mode;

	if (colorHierarchy.find(currentID) != colorHierarchy.end())
	{
		colorHierarchy.at(currentID).parentID = parentID;
		ColorSettingMode currentMode = (ColorSettingMode)colorHierarchy.at(currentID).colorSettingMode;

		if (newMode == ColorSettingMode::SingleHard)
		{
			colorHierarchy.at(currentID).colorSettingMode = (int)ColorSettingMode::SingleHard;
			colorHierarchy.at(currentID).applyableColorID = applyableColorID;
		}
		else if (newMode == ColorSettingMode::HierarchyHard)
		{
			if (currentMode != ColorSettingMode::SingleSoft)
			{
				colorHierarchy.at(currentID).colorSettingMode = (int)ColorSettingMode::HierarchyHard;
				colorHierarchy.at(currentID).applyableColorID = applyableColorID;
			}
		}
		else if (newMode == ColorSettingMode::SingleSoft)
		{
			if (currentMode == ColorSettingMode::HierarchySoft)
			{
				colorHierarchy.at(currentID).colorSettingMode = (int)ColorSettingMode::SingleSoft;
				colorHierarchy.at(currentID).applyableColorID = applyableColorID;
			}
		}
		else if (colorHierarchy.at(currentID).colorSettingMode == -1)
		{
			colorHierarchy.at(currentID).colorSettingMode = (int)newMode;
			colorHierarchy.at(currentID).applyableColorID = applyableColorID;
		}
	}
	else
	{
		colorHierarchy.emplace(currentID, ColorResultFile(currentID, parentID, mode, applyableColorID, path, name));
	}

	for (const auto& col : blockedColors)
	{
		if (col == actualColor)
		{
			colorHierarchy.at(currentID).applyableColorID = actualColor;
		}
	}

	for (const auto& child : result["return"].GetArray())
	{
		if (child["type"].GetVariant().GetString() != "Action" && child["type"].GetVariant().GetString() != "AudioFileSource")
		{
			if (newMode == ColorSettingMode::HierarchyHard || newMode == ColorSettingMode::HierarchySoft)
			{
				CollectObjectsInColorHierarchy(child["id"].GetVariant().GetString(), currentID, mode, applyableColorID, child["path"].GetVariant().GetString(), child["color"].GetVariant().GetInt8(), name);
			}
			else
			{
				if (child.HasKey("color"))
				{
					CollectObjectsInColorHierarchy(child["id"].GetVariant().GetString(), currentID, -1, child["color"].GetVariant().GetInt8(), child["path"].GetVariant().GetString(), child["color"].GetVariant().GetInt8(), name);
				}
				else
				{
					CollectObjectsInColorHierarchy(child["id"].GetVariant().GetString(), currentID, -1, 0, child["path"].GetVariant().GetString(), 0, name);
				}
			}
		}
	}
}

void ColorCodingModule::ApplyColorSettings()
{
	for (const auto& colorObject : colorHierarchy)
	{
		if (colorObject.second.colorSettingMode == 2 || colorObject.second.colorSettingMode == 3)
		{
			if (colorObject.second.parentID != "" && colorHierarchy.at(colorObject.second.parentID).colorSettingMode != 0 && colorHierarchy.at(colorObject.second.parentID).colorSettingMode != 1)
			{
				wwizardClient->SetProperty<bool>(colorObject.second.objectPath, "OverrideColor", false);
			}
			else
			{
				wwizardClient->SetProperty<bool>(colorObject.second.objectPath, "OverrideColor", true);

				wwizardClient->SetProperty<int>(colorObject.second.objectPath, "Color", colorObject.second.applyableColorID);
			}
		}
		else
		{
			wwizardClient->SetProperty<int>(colorObject.second.objectPath, "OverrideColor", true);
			wwizardClient->SetProperty<int>(colorObject.second.objectPath, "Color", colorObject.second.applyableColorID);
		}
	}
}

void ColorCodingModule::CreateColorSetting(std::string name, int colorCode)
{
	std::string newGUID = GenerateGuid();
	colorSettings.emplace(newGUID, ColorSetting(name, newGUID, colorCode));
}

void ColorCodingModule::DeleteColorSetting(ColorSetting setting)
{
	if (colorSettings.find(setting.settingID) != colorSettings.end())
	{
		colorSettings.erase(setting.settingID);
	}
}

void ColorCodingModule::OnConnectionStatusChange(const bool newConnectionStatus)
{
}

bool ColorCodingModule::CheckIfWwiseObjectHasColorProperty(int classID)
{
	AkJson result = wwizardClient->GetObjectPropertyList(classID);

	for (const auto& res : result["return"].GetArray())
	{
		if (res.GetVariant().GetString() == "Color")
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

	auto path = static_cast<std::string>(SOLUTION_DIR) + "/SavedData/ColorSettings.json";
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
	auto path = static_cast<std::string>(SOLUTION_DIR) + "/SavedData/ColorSettings.json";
	FILE* fp = fopen(path.c_str(), "rb");

	if (fp != 0)
	{
		char* readBuffer = new char[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

		rapidjson::Document d;
		d.ParseStream(is);
		fclose(fp);

		if (d.HasMember("ColorSettings"))
		{
			for (int i = 0; i < static_cast<int>(d["ColorSettings"].Size()); i++)
			{
				if (d["ColorSettings"][i].HasMember("name") && d["ColorSettings"][i].HasMember("colorCode") && d["ColorSettings"][i].HasMember("settingID") && d["ColorSettings"][i].HasMember("settingMode"))
				{
					ColorSetting newColorSetting = ColorSetting(d["ColorSettings"][i]["name"].GetString(), d["ColorSettings"][i]["colorCode"].GetInt(), d["ColorSettings"][i]["settingID"].GetString(), d["ColorSettings"][i]["settingMode"].GetInt());
					colorSettings.emplace(d["ColorSettings"][i]["settingID"].GetString(), newColorSetting);
				}
			}
		}

		if (d.HasMember("BlockedColors"))
		{
			if (d["BlockedColors"].HasMember("colorID"))
			{
				for (int i = 0; i < static_cast<int>(d["BlockedColors"]["colorID"].Size()); i++)
				{
					blockedColors.emplace(d["BlockedColors"]["colorID"][i].GetInt());
				}
			}
		}
	}
}

const std::string ColorCodingModule::GenerateGuid()
{
	return std::to_string(((long long)rand() << 32) | rand());
}

void ColorCodingModule::ClearPreviousData()
{
	colorHierarchy.clear();
}

std::set<int> ColorCodingModule::GetBlockedColors()
{
	return blockedColors;
}