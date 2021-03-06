#pragma once
#include "NamingConventionModule.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "rapidjson/RapidJsonUtils.h"
#include <algorithm>
#include "helper.h"


NamingConventionModule::NamingConventionModule(const std::string& wwiseProjPath, std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	:wwizardClient(wwizardClient)
{
	SetProjectPath(wwiseProjPath);
	LoadAllNamingSettingsInDir(static_cast<std::string>(SOLUTION_DIR) + "/Wwizard/SavedData/NamingConventions");
	SetDefaultActive();
}

NamingConventionModule::~NamingConventionModule()
{
}

void NamingConventionModule::LoadAllNamingSettingsInDir(const std::string path)
{
	allSettings.clear();
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.path().extension() == ".json")
		{
			LoadNamingConventionSettings(entry.path());
		}
	}
	activeSettingName = allSettings.front();
}

void NamingConventionModule::SetProjectPath(std::string newProjectPath)
{
	newProjectPath.erase(0, 1);
	if (newProjectPath.size() > 0)
	{
		newProjectPath.erase(newProjectPath.size() - 1);
		for (int i = static_cast<int>(newProjectPath.size()) - 1; i > 0; i--)
		{
			if (newProjectPath.at(i) == '\\')
				break;
			else
				newProjectPath.erase(i);
		}
	}

	projectPath = newProjectPath;

}

void NamingConventionModule::BeginNamingConventionProcess()
{
	ClearOldData();

	FindTopPhysicalFolders(projectPath);

	for (const auto& legalFolder : legalTopFolderPaths)
	{
		IteratePhysicalFolder(legalFolder, wwiseWorkFoldersToWwuType[legalFolder.filename().string()], "");
	}

	currentNamingConventionThread = nullptr;
}

void NamingConventionModule::ClearOldData()
{
	namingIssueResults.clear();
	legalTopFolderPaths.clear();
}

void NamingConventionModule::AddIssueToList(const std::string& guid, const std::string& name, const NamingIssue& issue)
{
	if (namingIssueResults.find(guid) == namingIssueResults.end())
	{
		namingIssueResults.emplace(guid, NamingResultFile(guid, name, issue));
	}	
}

void NamingConventionModule::StartCheckNamingConventionThread()
{
	if (currentNamingConventionThread != nullptr)
	{
		return;
	}

	std::cout << "Start Naming Convention Thread" << std::endl;
	std::thread namingConventionThread(&NamingConventionModule::BeginNamingConventionProcess, this);
	currentNamingConventionThread = &namingConventionThread;
	namingConventionThread.detach();
}

const std::string& NamingConventionModule::GetErrorMessageFromIssue(const NamingIssue& issue)
{
	return issueMessages[issue];
}

const std::map<std::string, NamingResultFile>& NamingConventionModule::GetNamingIssues()
{
	return namingIssueResults;
}

const std::set<std::string>& NamingConventionModule::GetWhiteListedContainers()
{
	return whitelistedContainers;
}

const std::set<std::string>& NamingConventionModule::GetWhiteListedWwuTypes()
{
	return whitelistedWwuTypes;
}

const std::string& NamingConventionModule::GetStringToReplace(const std::string& wwuType)
{
	return stringToReplace[wwuType];
}

void NamingConventionModule::ClearSetting(const std::string& settingName)
{
	NamingSetting* setting = &loadedNamingConventions[settingName];
	
	setting->Reset();
}


void NamingConventionModule::OnConnectionStatusChange(const bool newConnectionStatus)
{
	namingIssueResults.clear();
}

void NamingConventionModule::LoadNamingConventionSettings(std::filesystem::path settingPath)
{
	FILE* fp = fopen(settingPath.string().c_str(), "rb");
	if (fp != 0)
	{
		openedNamingConventionSetting = settingPath.string();
		char* readBuffer = new char[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

		rapidjson::Document d;
		d.ParseStream(is);
		fclose(fp);

		NamingSetting newNamingSetting = NamingSetting();


		for (const auto& wwu : whitelistedWwuTypes)
		{
			if (d.HasMember("WwuSettings"))
			{
				if (d["WwuSettings"].HasMember(wwu.c_str()))
				{
					if (d["WwuSettings"][wwu.c_str()].HasMember("name") && d["WwuSettings"][wwu.c_str()].HasMember("prefixToApply") && d["WwuSettings"][wwu.c_str()].HasMember("applyPrefix") && d["WwuSettings"][wwu.c_str()].HasMember("allowUppercase") && d["WwuSettings"][wwu.c_str()].HasMember("allowSpace"))
					{
						newNamingSetting.wwuSettings.emplace(d["WwuSettings"][wwu.c_str()]["name"].GetString(), WwuSetting(d["WwuSettings"][wwu.c_str()]["prefixToApply"].GetString(),
							d["WwuSettings"][wwu.c_str()]["applyPrefix"].GetBool(),
							d["WwuSettings"][wwu.c_str()]["allowSpace"].GetBool(),
							d["WwuSettings"][wwu.c_str()]["allowUppercase"].GetBool()));
					}
				}

			}
		}
		for (const auto& container : whitelistedContainers)
		{
			if (d.HasMember("ContainerSettings"))
			{
				if (d["ContainerSettings"].HasMember(container.c_str()))
				{
					if (d["ContainerSettings"][container.c_str()].HasMember("allowNumberSuffix") && d["ContainerSettings"][container.c_str()].HasMember("allowStringSuffix") && d["ContainerSettings"][container.c_str()].HasMember("maxNumberAllowed") && d["ContainerSettings"][container.c_str()].HasMember("allowUseContainerAsEnumeration"))
					{
						newNamingSetting.containerSettings.emplace(container.c_str(), ContainerSetting(d["ContainerSettings"][container.c_str()]["allowNumberSuffix"].GetBool(),
							d["ContainerSettings"][container.c_str()]["allowStringSuffix"].GetBool(),
							d["ContainerSettings"][container.c_str()]["maxNumberAllowed"].GetInt(),
							d["ContainerSettings"][container.c_str()]["allowUseContainerAsEnumeration"].GetBool()));
					}
					if (d["ContainerSettings"][container.c_str()].HasMember("suffixes"))
					{
						for (int i = 0; i < d["ContainerSettings"][container.c_str()]["suffixes"].Size(); i ++)
						{
							std::string newSuffix = d["ContainerSettings"][container.c_str()]["suffixes"][i].GetString();
							newNamingSetting.containerSettings[container].AddNewSuffix(newSuffix);
						}	
					}
				}
			}
		}
		loadedNamingConventions.emplace(settingPath.filename().string(), newNamingSetting);
		allSettings.push_back(settingPath.filename().string());
	}
}

void NamingConventionModule::ChangeNamingSetting(const std::string newSetting)
{
	activeSettingName = newSetting;
	activeNamingSetting = &loadedNamingConventions[newSetting];
}

void NamingConventionModule::SetDefaultActive()
{
	activeNamingSetting = &loadedNamingConventions["Default.json"];
}

void NamingConventionModule::SaveSettings(const std::string settingToSaveName, NamingSetting& saveSetting)
{
	if (IsDefaultSetting(settingToSaveName))
		return;

	rapidjson::Document d;
	d.SetObject();

	rapidjson::Value rapidjsonWwuSettings(rapidjson::kObjectType);
	for (const auto& wwu : saveSetting.wwuSettings)
	{
		rapidjson::Value settings(rapidjson::kObjectType);

		rapidjson::Value name;
		name = rapidjson::StringRef(wwu.first.c_str());
		settings.AddMember("name", name, d.GetAllocator());

		rapidjson::Value applyPrefix;
		applyPrefix.SetBool(wwu.second.applyPrefix);
		settings.AddMember("applyPrefix", applyPrefix, d.GetAllocator());

		rapidjson::Value prefixToApply;
		prefixToApply = rapidjson::StringRef(wwu.second.prefixToApply.c_str());
		settings.AddMember("prefixToApply", prefixToApply, d.GetAllocator());

		rapidjson::Value allowSpace;
		allowSpace.SetBool(wwu.second.allowSpace);
		settings.AddMember("allowSpace", allowSpace, d.GetAllocator());

		rapidjson::Value allowUppercase;
		allowUppercase.SetBool(wwu.second.allowUpperCase);
		settings.AddMember("allowUppercase", allowUppercase, d.GetAllocator());

		rapidjsonWwuSettings.AddMember(rapidjson::StringRef(wwu.first.c_str()), settings, d.GetAllocator());
	}
	d.AddMember("WwuSettings", rapidjsonWwuSettings, d.GetAllocator());

	//container settings
	rapidjson::Value rapidjsonContainerSettings(rapidjson::kObjectType);
	for (const auto& container : saveSetting.containerSettings)
	{
		rapidjson::Value cSettings(rapidjson::kObjectType);

		rapidjson::Value name;
		name = rapidjson::StringRef(container.first.c_str());
		cSettings.AddMember("name", name, d.GetAllocator());

		rapidjson::Value allowNumberSuffix;
		allowNumberSuffix.SetBool(container.second.applyNumberSuffix);
		cSettings.AddMember("allowNumberSuffix", allowNumberSuffix, d.GetAllocator());

		rapidjson::Value allowStringSuffix;
		allowStringSuffix.SetBool(container.second.applyStringSuffix);
		cSettings.AddMember("allowStringSuffix", allowStringSuffix, d.GetAllocator());

		rapidjson::Value maxNumberAllowed;
		maxNumberAllowed.SetInt(container.second.maxNumberAllowed);
		cSettings.AddMember("maxNumberAllowed", maxNumberAllowed, d.GetAllocator());

		rapidjson::Value allowUseEnumeration;
		allowUseEnumeration.SetBool(container.second.allowUseContainerAsEnumeration);
		cSettings.AddMember("allowUseContainerAsEnumeration", allowUseEnumeration, d.GetAllocator());

		rapidjson::Value stringSuffixes;
		stringSuffixes.SetArray();
		for (const auto& suffix : container.second.stringSuffixVector)
		{
			rapidjson::Value newSuffix;
			newSuffix = rapidjson::StringRef(suffix.c_str());
			stringSuffixes.PushBack(newSuffix, d.GetAllocator());
		}
		cSettings.AddMember("suffixes", stringSuffixes, d.GetAllocator());
		rapidjsonContainerSettings.AddMember(rapidjson::StringRef(container.first.c_str()), cSettings, d.GetAllocator());
	}
	d.AddMember("ContainerSettings", rapidjsonContainerSettings, d.GetAllocator());

	FILE* fp = fopen((static_cast<std::string>(SOLUTION_DIR) + "/Wwizard/SavedData/NamingConventions/" + settingToSaveName + ".json").c_str(), "wb");
	if (fp != 0)
	{
		char* writeBuffer = new char[65536];
		rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		d.Accept(writer);

		fclose(fp);
	}
}

void NamingConventionModule::SaveAsNewSetting(const std::string settingToSaveName, NamingSetting& saveSetting)
{
	SaveSettings(settingToSaveName, saveSetting);
	LoadNamingConventionSettings(static_cast<std::string>(SOLUTION_DIR) + "/Wwizard/SavedData/NamingConventions/" + settingToSaveName + ".json");
}

void NamingConventionModule::OnSettingsChange(const std::string projectPath, const std::string sdkPath)
{
	ClearOldData();
	SetProjectPath(projectPath);
}

bool NamingConventionModule::NamingConventionNameAlreadyExists(const std::string newName)
{
	auto path = static_cast<std::string>(SOLUTION_DIR) + "/Wwizard/SavedData/NamingConventions";
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.path().filename().string() == newName + ".json")
		{
			return true;
		}
	}
	return false;
}

void NamingConventionModule::FindTopPhysicalFolders(const std::string& folderPath)
{
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (std::filesystem::is_directory(entry))
		{
			if (entry.path().filename().u8string() != ".cache" && entry.path().filename().u8string() != "GeneratedSoundBanks" && entry.path().filename().u8string() != "Originals" && entry.path().filename().u8string() != "Presets")
			{
				if (wwiseWorkFoldersToWwuType.find(entry.path().filename().string()) != wwiseWorkFoldersToWwuType.end())
				{
					legalTopFolderPaths.push_back(entry.path());
				}		
			}
		}
	}
}

void NamingConventionModule::IteratePhysicalFolder(const std::filesystem::path& folderPath, const std::string& wwuSettingKey, const std::string& parentContainerKey)
{
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		std::string passableParentName = folderPath.filename().string();
		if (passableParentName == wwuSettingKey)
		{
			passableParentName = "";
		}

		std::string passableFileName = entry.path().filename().string();
		if (entry.path().extension() == ".wwu")
		{
			passableFileName = passableFileName.substr(0, passableFileName.length() - 4);
		}

		if (entry.is_directory())
		{
			if (CheckNamingSettings(passableFileName, passableParentName, wwuSettingKey, "Folder", "Folder", GenerateGuid()))
			{
				IteratePhysicalFolder(entry.path().string(), wwuSettingKey, "Folder");
			}
		}
		else if (entry.path().extension() == ".wwu")
		{
			std::string id = GetWorkUnitIDFromXML(entry.path().string());
			if (id != "")
			{
				if (CheckNamingSettings(passableFileName, passableParentName, wwuSettingKey, "WorkUnit", "Folder", id))
				{
					IterateThroughWwu(id, passableFileName, wwuSettingKey, "WorkUnit");
				}
			}
		}
	}
}

std::string NamingConventionModule::GetWorkUnitIDFromXML(const std::string& wwuPath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(wwuPath.c_str());
	if (!result)
		return "";

	std::string wwuType = static_cast<std::string>(doc.child("WwiseDocument").first_child().name());

	if (static_cast<std::string>(doc.child("WwiseDocument").child(wwuType.c_str()).child("WorkUnit").attribute("Name").value()) != "Default Work Unit")
	{
		if (static_cast<std::string>(doc.child("WwiseDocument").child(wwuType.c_str()).child("WorkUnit").attribute("PersistMode").value()) == "Standalone")
		{
			return static_cast<std::string>(doc.child("WwiseDocument").child(wwuType.c_str()).child("WorkUnit").attribute("ID").value());
		}
	}

	return "";
}

void NamingConventionModule::IterateThroughWwu(const std::string id, const std::string parentName, const std::string& wwuSettingKey, const std::string& parentContainerKey)
{
	std::vector <std::string> optionList = { "id" , "name", "type"};
	AkJson results = wwizardClient->GetChildrenFromGuid(id, optionList);

	for (const auto& evt : results["return"].GetArray())
	{
		if (CheckNamingSettings(evt["name"].GetVariant().GetString(), parentName, wwuSettingKey, evt["type"].GetVariant().GetString(), parentContainerKey, evt["id"].GetVariant().GetString()))
		{
			IterateThroughWwu(evt["id"].GetVariant().GetString(), evt["name"].GetVariant().GetString(), wwuSettingKey, evt["type"].GetVariant().GetString());
		}
	}
}


bool NamingConventionModule::CheckNamingSettings(const std::string& currentFileName, const std::string& parentFileName, const std::string& wwuSettingKey, const std::string& containerKey, const std::string& parentContainerKey, const std::string& currentID)
{
	bool passingFlag = true;
	std::string currentSuffix = "";
	std::string checkedParentFileName = parentFileName;

	for (const auto& keyName : wwiseWorkFoldersToWwuType)
	{
		if (keyName.first == parentFileName)
		{
			checkedParentFileName = "";
			break;
		}
	}

	if (!activeNamingSetting->wwuSettings[wwuSettingKey].allowSpace)
	{
		passingFlag = !IsSpaceInPath(currentFileName);
		if (!passingFlag)
		{
			AddIssueToList(currentID, currentFileName, NamingIssue::SPACE);
		}
	}
	if (!activeNamingSetting->wwuSettings[wwuSettingKey].allowUpperCase && passingFlag)
	{
		passingFlag = !IsUppercaseInPath(currentFileName);
		if (!passingFlag)
		{
			AddIssueToList(currentID, currentFileName, NamingIssue::UPPERCASE);
		}
	}
	if (activeNamingSetting->wwuSettings[wwuSettingKey].applyPrefix && passingFlag)
	{
		passingFlag = IsPrefixRight(currentFileName, activeNamingSetting->wwuSettings[wwuSettingKey].prefixToApply);
		if (!passingFlag)
		{
			AddIssueToList(currentID, currentFileName, NamingIssue::PREFIX);
		}
	}
	if (passingFlag)
	{
		passingFlag = IsContainerWhiteListed(containerKey);

		if (passingFlag)
		{
			if (activeNamingSetting->containerSettings[containerKey].applyNumberSuffix || activeNamingSetting->containerSettings[containerKey].applyStringSuffix)
			{
				passingFlag = CheckSuffix(currentFileName, containerKey, currentSuffix);
				if (!passingFlag)
				{
					AddIssueToList(currentID, currentFileName, NamingIssue::Suffix);
				}
			}
		}
	}

	if (passingFlag)
	{
 		if (activeNamingSetting->wwuSettings[wwuSettingKey].applyPrefix)
		{
			passingFlag = IsParentHierarchyMatching(currentFileName, checkedParentFileName, activeNamingSetting->wwuSettings[wwuSettingKey].prefixToApply, currentSuffix, parentContainerKey, containerKey);
		}
		else
		{
			passingFlag = IsParentHierarchyMatching(currentFileName, checkedParentFileName, "", currentSuffix, parentContainerKey, containerKey);
		}

		if (!passingFlag)
		{
			AddIssueToList(currentID, currentFileName, NamingIssue::HIERARCHY);
		}
	}
	return passingFlag;
}

bool NamingConventionModule::IsParentHierarchyMatching(const std::string& fileName, const std::string& parentFileName, const std::string& prefix, const std::string& suffix, const std::string& parentContainerKey, const std::string& currentContainerKey)
{
	std::string parentSuffix = "";
	
	
	std::string cleanFileName = fileName;
	if (prefix.length() > 0)
	{
		if (prefix.length() >= fileName.length())
			return false;
		cleanFileName = fileName.substr(prefix.length() + 1, fileName.length() - prefix.length() - 1);
	}

	if (suffix.length() > 0)
	{
		if (suffix.length() >= cleanFileName.length())
			return false;

		cleanFileName = cleanFileName.substr(0, cleanFileName.length() - suffix.length() - 1);
	}

	if (cleanFileName == "")
	{
		return false;
	}

	if (fileName == parentFileName)
	{
		return false;
	}

	if (parentFileName == "")
	{
		for (auto it = cleanFileName.cbegin(); it != cleanFileName.cend(); ++it)
		{
			if (*it == '_')
			{
				return false;
			}
		}
		return true;
	}

	CheckSuffix(parentFileName, parentContainerKey, parentSuffix);

	std::string cleanParentName = parentFileName;
	if (parentSuffix.length() > 0)
	{
		cleanParentName = cleanParentName.substr(0, cleanParentName.length() - parentSuffix.length() - 1);
	}
	if (prefix.length() > 0)
	{
		cleanParentName = cleanParentName.substr(prefix.length() + 1, cleanParentName.length() - prefix.length() - 1);
	}

	if (cleanFileName == parentFileName && activeNamingSetting->containerSettings[currentContainerKey].allowUseContainerAsEnumeration && activeNamingSetting->containerSettings[currentContainerKey].applyNumberSuffix)
	{
		return true;
	}

	if (!IsOneUnderscorePerNewLayer(cleanFileName, cleanParentName))
	{
		return false;
	}

	auto foundLoc = cleanFileName.find(cleanParentName);
	if (foundLoc != std::string::npos)
	{
		std::string rest = cleanFileName.substr(cleanParentName.length() + 1, cleanFileName.length() - cleanParentName.length());
		if (rest.length() == 0)
		{
			return false;
		}		

		for (auto it = rest.cbegin(); it != rest.cend(); ++it)
		{
			if (*it == '_')
			{
				return false;
			}		
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool NamingConventionModule::IsSpaceInPath(const std::string& fileName)
{
	for (auto it = fileName.cbegin(); it != fileName.cend(); ++it) 
	{
		if (isspace(*it))
			return true;
	}
	return false;
}

bool NamingConventionModule::IsUppercaseInPath(const std::string& fileName)
{
	for (auto it = fileName.cbegin(); it != fileName.cend(); ++it)
	{
		if (isupper(*it))
			return true;
	}
	return false;
}

bool NamingConventionModule::IsPrefixRight(const std::string& fileName, const std::string& prefix)
{
	std::string fileNameFront = fileName.substr(0, prefix.length());
	if (fileNameFront == prefix)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool NamingConventionModule::IsContainerWhiteListed(const std::string& containerType)
{
	auto it = whitelistedContainers.find(containerType);
	if (it != whitelistedContainers.end())
		return true;
	else
		return false;
}

bool NamingConventionModule::CheckSuffix(const std::string& fileName, const std::string& containerType, std::string& outSuffix)
{
	ContainerSetting setting = activeNamingSetting->containerSettings[containerType];

	std::string suffixless = fileName;
	std::string stringSuffix = "";
	std::string numberSuffix = "";

	if (setting.applyNumberSuffix)
	{
		if (fileName.length() < std::to_string(setting.maxNumberAllowed).length())
			return false;

		numberSuffix = fileName.substr(fileName.length() - std::to_string(setting.maxNumberAllowed).length(), std::to_string(setting.maxNumberAllowed).length());

		for (const auto& digit : numberSuffix)
		{
			if (!isdigit(digit))
			{
				return false;
			}

		}

		if (std::stoi(numberSuffix) > setting.maxNumberAllowed)
		{
			return false;
		}

		if (numberSuffix.length() != std::to_string(setting.maxNumberAllowed).length())
		{
			return false;
		}

		suffixless = fileName.substr(0, fileName.length() - numberSuffix.length() - 1);
	}


	std::string wholeSuffix = "";
	if (setting.applyStringSuffix)
	{
		std::string stringSuffix = suffixless;

		for (const auto& suffix : setting.stringSuffixVector)
		{
			if (fileName.length() < suffix.length())
				return false;

			stringSuffix = suffixless.substr(suffixless.length() - suffix.length(), suffix.length());
			if (stringSuffix == suffix)
			{
				if (suffixless.at(suffixless.length() - suffix.length() -1) != '_')
				{
					return false;
				}
				else
				{
					wholeSuffix = stringSuffix;
				}
			}
		}
	}

	if (wholeSuffix == "" && setting.applyStringSuffix)
	{
		return false;
	}

	if (setting.applyNumberSuffix)
	{
		if (wholeSuffix != "")
		{
			wholeSuffix += '_' + numberSuffix;
		}
		else
		{
			wholeSuffix = numberSuffix;
		}
	}

	outSuffix = wholeSuffix;
	if (wholeSuffix == "")
		return false;

	return true;
}

bool NamingConventionModule::IsOneUnderscorePerNewLayer(const std::string& fileName, const std::string& parentName)
{
	if (fileName == parentName)
	{
		return false;
	}

	std::string rest = fileName;
	if (parentName != "")
	{
		rest = fileName.substr(parentName.length() + 1, fileName.length() - parentName.length() - 1);
	}

	for (const auto& cha : rest)
	{
		if (cha == '_')
		{
			return false;
		}
	}

	return true;
}

std::string NamingConventionModule::RemovePrefixSuffix(const std::string& fileName, const std::string& wwuSettingKey, const std::string& containerSettingKey)
{
	std::string tempName = fileName;

	if (activeNamingSetting->wwuSettings[wwuSettingKey].applyPrefix)
	{
		tempName = RemovePrefixFromName(tempName, wwuSettingKey);
	}
	if (activeNamingSetting->containerSettings[containerSettingKey].applyNumberSuffix || activeNamingSetting->containerSettings[containerSettingKey].applyStringSuffix)
	{
		tempName = RemoveSuffixFromName(tempName, containerSettingKey);
	}
	return tempName;
}

std::string NamingConventionModule::RemovePrefixFromName(const std::string& fileName, const std::string& wwuSettingKey)
{
	if (fileName == "")
	{
		std::string emptyParentResponse = "";
		return emptyParentResponse;
	}
	std::string prefix = activeNamingSetting->wwuSettings[wwuSettingKey].prefixToApply;

	std::string wPrefix = fileName.substr(prefix.length() + 1, fileName.length() - prefix.length() + 1);

	return wPrefix;
}

std::string NamingConventionModule::RemoveSuffixFromName(const std::string& fileName, const std::string& containerSettingKey)
{
	ContainerSetting setting = activeNamingSetting->containerSettings[containerSettingKey];
	std::string withoutSuffix = "";
	if (setting.applyNumberSuffix)
	{
		withoutSuffix = fileName.substr(0, fileName.length() - std::to_string(setting.maxNumberAllowed).length());
	}
	return fileName;
}

std::vector<std::string> NamingConventionModule::ConvertStringToVector(const std::string& inputSetting)
{
	std::string currentSuffix = "";
	std::vector<std::string> suffixVector;

	for (const auto& cha : inputSetting)
	{
		if (cha == ',')
		{
			suffixVector.emplace_back(currentSuffix);
			currentSuffix = "";
		}
		else
		{
			currentSuffix += cha;
		}
	}
	suffixVector.emplace_back(currentSuffix);
	return suffixVector;
}
