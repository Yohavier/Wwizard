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

NamingConventionModule::NamingConventionModule(std::string wwiseProjPath)
{
	wwiseProjPath.erase(0, 1);
	wwiseProjPath.erase(wwiseProjPath.size() - 1);
	for (int i = static_cast<int>(wwiseProjPath.size()) - 1; i > 0; i--)
	{
		if (wwiseProjPath.at(i) == '\\')
			break;
		else
			wwiseProjPath.erase(i);
	}
	projectPath = wwiseProjPath;

	LoadNamingConventionSettings();
}

NamingConventionModule::~NamingConventionModule()
{
	SaveNamingConventionSettings();
}

bool NamingConventionModule::CheckNamingConvention()
{
	ClearOldData();

	PreFetchAllWwuData(projectPath);

	StartCheckingNamingConvention(projectPath, "");

	return DetermineResult();
}

//Prefetch
void NamingConventionModule::PreFetchAllWwuData(std::string directory)
{
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (std::filesystem::is_directory(entry))
		{
			PreFetchAllWwuData(entry.path().u8string());
		}
		else
		{
			if (entry.path().extension() == ".wwu")
			{
				FetchSingleWwuData(entry.path().u8string());
			}
		}
	}
}

void NamingConventionModule::FetchSingleWwuData(std::string path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return;

	pugi::xml_node data = doc.child("WwiseDocument").first_child().child("WorkUnit");
	WwuLookUpData newWwuData = WwuLookUpData(data.attribute("Name").value(), data.attribute("ID").value(), data.attribute("PersistMode").value(), path);
	prefetchedWwuData.emplace_back(newWwuData);
}


//Helper
void NamingConventionModule::ClearOldData()
{
	namingIssueResults.clear();
}

bool NamingConventionModule::DetermineResult()
{
	return namingIssueResults.empty();
}

void NamingConventionModule::AddIssueToList(std::string guid, std::string name, Issue issue)
{
	namingIssueResults.emplace(guid, NamingResultFile(guid, name, issue));
}


//Getter
const std::string& NamingConventionModule::GetErrorMessageFromIssue(Issue issue)
{
	return issueMessages[issue];
}

const std::vector<NamingResultFile> NamingConventionModule::GetIssueList()
{
	std::vector<NamingResultFile> issueList;
	for (const auto& issue : namingIssueResults)
	{
		issueList.push_back(issue.second);
	}
	return issueList;
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


//Scan Naming Convention
void NamingConventionModule::StartCheckingNamingConvention(std::string path, std::string namePath)
{
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (std::filesystem::is_directory(entry))
		{
			if (entry.path().filename().u8string() != ".cache" && entry.path().filename().u8string() != "GeneratedSoundBanks" && entry.path().filename().u8string() != "Originals" && entry.path().filename().u8string() != "Presets")
			{
				std::string newPath = namePath + levelSeparator + entry.path().filename().u8string();
				StartCheckingNamingConvention(entry.path().u8string(), newPath);
			}
		}
		else if (entry.path().extension() == ".wwu")
		{
			ScanWorkUnitXMLByPath(entry.path().u8string(), namePath);
		}
	}
}

void NamingConventionModule::ApplyPrefix(std::string& namePath, std::string& fullFolderName, const WwuSettings& newPrefix)
{
	size_t stringLoc = namePath.find(fullFolderName);
	if (stringLoc < namePath.size())
	{
		namePath.erase(stringLoc, stringLoc + fullFolderName.size() + 1);
		if (newPrefix.applyPrefix)
		{
			namePath = newPrefix.prefixToApply + namePath;
		}
	}
}

void NamingConventionModule::ScanWorkUnitXMLByPath(std::string wwuPath, std::string namePath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(wwuPath.c_str());
	if (!result)
		return;

	std::string wwuType = static_cast<std::string>(doc.child("WwiseDocument").first_child().name());

	if (static_cast<std::string>(doc.child("WwiseDocument").child(wwuType.c_str()).child("WorkUnit").attribute("Name").value()) != "Default Work Unit")
	{
		if (static_cast<std::string>(doc.child("WwiseDocument").child(wwuType.c_str()).child("WorkUnit").attribute("PersistMode").value()) == "Standalone")
		{
			if (wwuSettings[wwuType].applyNamingConventionCheck)
			{
				if (whitelistedWwuTypes.find(wwuType) != whitelistedWwuTypes.end())
				{
					ApplyPrefix(namePath, stringToReplace[wwuType], wwuSettings[wwuType]);
				}
				IterateThroughWwu(doc.child("WwiseDocument").child(wwuType.c_str()), namePath, wwuType);
			}
		}
	}
}

void NamingConventionModule::IterateThroughWwu(pugi::xml_node wwuNode, std::string namePath, std::string wwuType)
{
	for (auto& node : wwuNode)
	{
		if (static_cast<std::string>(node.name()) == "WorkUnit")
		{
			if (static_cast<std::string>(node.attribute("PersistMode").value()) == "Reference")
			{
				for (auto& data : prefetchedWwuData)
				{
					if (data.guid == static_cast<std::string>(node.attribute("ID").value()))
					{
						pugi::xml_document doc;
						pugi::xml_parse_result result = doc.load_file(data.path.c_str());
						if (!result)
							return;

						IterateThroughWwu(doc.child("WwiseDocument"), namePath, wwuType);
					}
				}
			}
			else
			{
				std::string newNamePath = AddLastNamePathLayer(namePath, node, node.name());
				if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str() && namingIssueResults.find(newNamePath) == namingIssueResults.end())
				{
					AddIssueToList(static_cast<std::string>(node.attribute("ID").value()), static_cast<std::string>(node.attribute("Name").value()), Issue::HIERARCHY);
				}
				CheckNameForSpace(node, wwuSettings[wwuType].allowSpace);
				CheckUppercaseRule(node, wwuSettings[wwuType].allowUpperCase);
				IterateThroughWwu(node, newNamePath, wwuType);
			}
		}
		else if (whitelistedContainers.find(static_cast<std::string>(node.name())) != whitelistedContainers.end())
		{
			std::string newNamePath = AddLastNamePathLayer(namePath, node, static_cast<std::string>(node.name()));
			if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str() && namingIssueResults.find(newNamePath) == namingIssueResults.end())
			{
				AddIssueToList(static_cast<std::string>(node.attribute("ID").value()), static_cast<std::string>(node.attribute("Name").value()), Issue::HIERARCHY);
			}

			CheckNameForSpace(node, wwuSettings[wwuType].allowSpace);
			CheckUppercaseRule(node, wwuSettings[wwuType].allowUpperCase);
			IterateThroughWwu(node, newNamePath, wwuType);
		}
		else
		{
			IterateThroughWwu(node, namePath, wwuType);
		}
	}
}

std::string NamingConventionModule::AddLastNamePathLayer(const std::string& currentNamePath, pugi::xml_node& newNode, std::string containerName)
{
	std::string newNodeName = static_cast<std::string>(newNode.attribute("Name").value());
	if (currentNamePath == "")
	{
		CheckForMultipleSeparatorsPerLayer(newNodeName, newNode, containerName);
		return newNodeName;
	}
	else
	{
		if (newNodeName.find(levelSeparator) < newNodeName.size())
		{
			newNodeName.erase(0, currentNamePath.size() + 1);
		}
		std::string newName = currentNamePath + levelSeparator + newNodeName;
		CheckForMultipleSeparatorsPerLayer(newNodeName, newNode, containerName);

		return newName;
	}
}

void NamingConventionModule::CheckNameForSpace(pugi::xml_node& currentNode, bool& allowSpace)
{
	std::string currentName = static_cast<std::string>(currentNode.attribute("Name").value());
	if (!allowSpace)
	{
		size_t loc = currentName.find(" ");
		if (loc < currentName.size())
		{
			AddIssueToList(static_cast<std::string>(currentNode.attribute("ID").value()), static_cast<std::string>(currentNode.attribute("Name").value()), Issue::SPACE);
		}
	}
}

void NamingConventionModule::CheckForMultipleSeparatorsPerLayer(std::string newNameLayer, pugi::xml_node& currentNode, std::string containerName)
{
	std::string currentName = static_cast<std::string>(currentNode.attribute("Name").value());
	if (newNameLayer.find("_") < newNameLayer.size())
	{
		if (!IsCorrectSuffix(currentName, newNameLayer.substr(newNameLayer.find("_") + 1), containerName))
		{
			AddIssueToList(static_cast<std::string>(currentNode.attribute("ID").value()), static_cast<std::string>(currentNode.attribute("Name").value()), Issue::SEPARATOR);
		}
	}
}

bool NamingConventionModule::IsCorrectSuffix(std::string& currentName, std::string newNameLayer, std::string& containerName)
{
	auto container = containerSettings.find(containerName);
	if (container != containerSettings.end())
	{
		if (container->second.allowNumberSuffix || container->second.allowStringSuffix)
		{
			std::vector<int> separatorLocations;
			bool numLayerAppeared = false;
			for (int i = 0; i< newNameLayer.size(); i++)
			{
				if (newNameLayer.at(i) == '_')
					separatorLocations.push_back(i);
			}

			if (container->second.IsSuffixCountInRange(static_cast<int>(separatorLocations.size())))
			{
				int readIndex = 0;
				for (int sep = 0; sep < separatorLocations.size() + 1; sep++)
				{
					std::string layer;
					if (sep >= separatorLocations.size())
					{
						layer = newNameLayer.substr(readIndex);
					}
					else
					{
						layer = newNameLayer.substr(readIndex, separatorLocations[sep] - readIndex);
						readIndex = separatorLocations[sep] + 1;
					}

					if (container->second.IsNumber(layer) && !numLayerAppeared && container->second.allowNumberSuffix)
					{
						numLayerAppeared = true;
						if (!container->second.IsNumberInRange(layer))
						{
							return false;
						}
					}
					else if (container->second.IsStringInSuffixList(layer) && container->second.allowStringSuffix)
					{
						//passed test	
					}
					else
					{
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool NamingConventionModule::CheckUppercaseRule(pugi::xml_node& currentNode, bool& allowUpperCase)
{
	if (allowUpperCase)
		return true;

	std::string currentName = static_cast<std::string>(currentNode.attribute("Name").value());
	
	if (std::any_of(currentName.begin(), currentName.end(), isupper))
	{
		AddIssueToList(static_cast<std::string>(currentNode.attribute("ID").value()), static_cast<std::string>(currentNode.attribute("Name").value()), Issue::UPPERCASE);
		return false;
	}

	return true;
}

// Save and Loading
void NamingConventionModule::SaveNamingConventionSettings()
{
	rapidjson::Document d;
	d.SetObject();

	//wwu settings
	rapidjson::Value rapidjsonWwuSettings(rapidjson::kObjectType);
	for (const auto& wwu : wwuSettings)
	{
		rapidjson::Value settings(rapidjson::kObjectType);

		rapidjson::Value name;
		name = rapidjson::StringRef(wwu.first.c_str());
		settings.AddMember("name", name, d.GetAllocator());

		rapidjson::Value apply;
		apply.SetBool(wwu.second.applyNamingConventionCheck);
		settings.AddMember("apply", apply, d.GetAllocator());

		rapidjson::Value applyPrefix;
		applyPrefix.SetBool(wwu.second.applyPrefix);
		settings.AddMember("applyPrefix", applyPrefix, d.GetAllocator());

		rapidjson::Value prefixToApply;
		prefixToApply = rapidjson::StringRef(wwu.second.prefixToApply.c_str());
		settings.AddMember("prefixToApply", prefixToApply, d.GetAllocator());

		rapidjson::Value allowSpace;
		allowSpace.SetBool(wwu.second.allowSpace);
		settings.AddMember("allowSpace", allowSpace, d.GetAllocator());

		rapidjsonWwuSettings.AddMember(rapidjson::StringRef(wwu.first.c_str()), settings, d.GetAllocator());
	}
	d.AddMember("WwuSettings", rapidjsonWwuSettings, d.GetAllocator());

	//container settings
	rapidjson::Value rapidjsonContainerSettings(rapidjson::kObjectType);
	for (const auto& container : containerSettings)
	{
		rapidjson::Value cSettings(rapidjson::kObjectType);

		rapidjson::Value name;
		name = rapidjson::StringRef(container.first.c_str());
		cSettings.AddMember("name", name, d.GetAllocator());

		rapidjson::Value allowNumberSuffix;
		allowNumberSuffix.SetBool(container.second.allowNumberSuffix);
		cSettings.AddMember("allowNumberSuffix", allowNumberSuffix, d.GetAllocator());

		rapidjson::Value allowStringSuffix;
		allowStringSuffix.SetBool(container.second.allowStringSuffix);
		cSettings.AddMember("allowStringSuffix", allowStringSuffix, d.GetAllocator());

		rapidjson::Value suffixLayers;
		suffixLayers.SetInt(container.second.suffixLayers);
		cSettings.AddMember("suffixLayers", suffixLayers, d.GetAllocator());

		rapidjson::Value maxNumberAllowed;
		maxNumberAllowed.SetInt(container.second.maxNumberAllowed);
		cSettings.AddMember("maxNumberAllowed", maxNumberAllowed, d.GetAllocator());

		rapidjson::Value stringSuffixes;
		stringSuffixes = rapidjson::StringRef(container.second.stringSuffixes.c_str());
		cSettings.AddMember("stringSuffixes", stringSuffixes, d.GetAllocator());

		rapidjsonContainerSettings.AddMember(rapidjson::StringRef(container.first.c_str()), cSettings, d.GetAllocator());
	}
	d.AddMember("ContainerSettings", rapidjsonContainerSettings, d.GetAllocator());

	auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/NamingConventionSettings.json";
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

void NamingConventionModule::LoadNamingConventionSettings()
{
	auto path = static_cast<std::string>(SOLUTION_DIR) + "SavedData/NamingConventionSettings.json";
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp != 0)
	{
		char* readBuffer = new char[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

		rapidjson::Document d;
		d.ParseStream(is);
		fclose(fp);

		for (const auto& wwu : whitelistedWwuTypes)
		{
			wwuSettings.emplace(d["WwuSettings"][wwu.c_str()]["name"].GetString(), WwuSettings(d["WwuSettings"][wwu.c_str()]["prefixToApply"].GetString(),
																							   d["WwuSettings"][wwu.c_str()]["applyPrefix"].GetBool(),
																							   d["WwuSettings"][wwu.c_str()]["apply"].GetBool(),
																							   d["WwuSettings"][wwu.c_str()]["allowSpace"].GetBool()));
		}
		for (const auto& container : whitelistedContainers)
		{
			containerSettings.emplace(container.c_str(), ContainerSettings(d["ContainerSettings"][container.c_str()]["allowNumberSuffix"].GetBool(),
																		   d["ContainerSettings"][container.c_str()]["allowStringSuffix"].GetBool(),
																		   d["ContainerSettings"][container.c_str()]["suffixLayers"].GetInt(),
																		   d["ContainerSettings"][container.c_str()]["maxNumberAllowed"].GetInt(),
																		   d["ContainerSettings"][container.c_str()]["stringSuffixes"].GetString()));
		}
	}
}

