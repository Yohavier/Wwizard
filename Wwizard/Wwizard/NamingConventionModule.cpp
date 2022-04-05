#include "NamingConventionModule.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "rapidjson/RapidJsonUtils.h"

/*
* TODO
* suffix
*/

NamingConventionModule::NamingConventionModule()
{
	LoadNamingConvention();
}

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

	LoadNamingConvention();
}

NamingConventionModule::~NamingConventionModule()
{
	SaveNamingConvention();
}

void NamingConventionModule::ScanWorkUnitData(std::string directory)
{
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (std::filesystem::is_directory(entry))
		{
			ScanWorkUnitData(entry.path().u8string());
		}
		else
		{
			if (entry.path().extension() == ".wwu")
			{
				FetchWwuData(entry.path().u8string());
			}
		}
	}
}

void NamingConventionModule::FetchWwuData(std::string path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return;

	pugi::xml_node data = doc.child("WwiseDocument").first_child().child("WorkUnit");
	WwuLookUpData newWwuData = WwuLookUpData(data.attribute("Name").value(), data.attribute("ID").value(), data.attribute("PersistMode").value(), path);
	wwuData.emplace_back(newWwuData);
}

void NamingConventionModule::CheckNamingConvention()
{
	namingIssueResults.clear();
	ScanWorkUnitData(projectPath);
	IterateFolder(projectPath, "");
}

void NamingConventionModule::ApplyPrefix(std::string& namePath, std::string fullFolderName, const WwuSettings& newPrefix)
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

void NamingConventionModule::IterateFolder(std::string path, std::string namePath)
{
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (std::filesystem::is_directory(entry))
		{
			if (entry.path().filename().u8string() != ".cache" && entry.path().filename().u8string() != "GeneratedSoundBanks" && entry.path().filename().u8string() != "Originals" && entry.path().filename().u8string() != "Presets")
			{
				std::string newPath = namePath + levelSeparator + entry.path().filename().u8string();
				IterateFolder(entry.path().u8string(), newPath);
			}
		}
		else if (entry.path().extension() == ".wwu")
		{
			ScanWorkUnitXMLByPath(entry.path().u8string(), namePath);
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
				ModularResolve(doc.child("WwiseDocument").child(wwuType.c_str()), namePath, wwuType);
			}
		}
	}
}

void NamingConventionModule::ModularResolve(pugi::xml_node wwuNode, std::string namePath, std::string wwuType)
{
	for (auto& node : wwuNode)
	{
		if (static_cast<std::string>(node.name()) == "WorkUnit")
		{
			if (static_cast<std::string>(node.attribute("PersistMode").value()) == "Reference")
			{
				for (auto& data : wwuData)
				{
					if (data.guid == static_cast<std::string>(node.attribute("ID").value()))
					{
						pugi::xml_document doc;
						pugi::xml_parse_result result = doc.load_file(data.path.c_str());
						if (!result)
							return;

						ModularResolve(doc.child("WwiseDocument"), namePath, wwuType);
					}
				}
			}
			else
			{
				std::string newNamePath = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()), node.name());
				if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str() && namingIssueResults.find(newNamePath) == namingIssueResults.end())
				{
					namingIssueResults.emplace(static_cast<std::string>(node.attribute("Name").value()), newNamePath);
				}
				CheckNameForSpace(static_cast<std::string>(node.attribute("Name").value()), wwuSettings[wwuType].allowSpace);
				ModularResolve(node, newNamePath, wwuType);
			}
		}
		else if (whitelistedContainers.find(static_cast<std::string>(node.name())) != whitelistedContainers.end())
		{
			std::string newNamePath = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()), static_cast<std::string>(node.name()));
			if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str() && namingIssueResults.find(newNamePath) == namingIssueResults.end())
			{
				namingIssueResults.emplace(static_cast<std::string>(node.attribute("Name").value()), newNamePath);
			}

			CheckNameForSpace(static_cast<std::string>(node.attribute("Name").value()), wwuSettings[wwuType].allowSpace);
			ModularResolve(node, newNamePath, wwuType);
		}
		else
		{
			ModularResolve(node, namePath, wwuType);
		}
	}
}

std::string NamingConventionModule::AddLastNamePathLayer(const std::string& currentNamePath, std::string newNodeName, std::string containerName)
{
	if (currentNamePath == "")
	{
		CheckForMultipleSeparatorsPerLayer(newNodeName, newNodeName, containerName);
		return newNodeName;
	}
	else
	{
		if (newNodeName.find(levelSeparator) < newNodeName.size())
		{
			newNodeName.erase(0, currentNamePath.size() + 1);
		}
		std::string newName = currentNamePath + levelSeparator + newNodeName;
		CheckForMultipleSeparatorsPerLayer(newNodeName, newName, containerName);

		return newName;
	}
}

void NamingConventionModule::SaveNamingConvention()
{
	rapidjson::Document d;
	d.SetObject();

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

void NamingConventionModule::LoadNamingConvention()
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

		for(const auto& wwu : whitelistedWwuTypes)
		{
			wwuSettings.emplace(d["WwuSettings"][wwu.c_str()]["name"].GetString(), WwuSettings(d["WwuSettings"][wwu.c_str()]["prefixToApply"].GetString(),
																										 d["WwuSettings"][wwu.c_str()]["applyPrefix"].GetBool(), 
																										 d["WwuSettings"][wwu.c_str()]["apply"].GetBool(), 
																										 d["WwuSettings"][wwu.c_str()]["allowSpace"].GetBool()));
		}
	}

	for (const auto& container : whitelistedContainers)
	{
		containerSettings.emplace(container.c_str(), ContainerSettings());
	}
}

void NamingConventionModule::CheckNameForSpace(std::string currentName, bool allowSpace)
{
	if (!allowSpace)
	{
		size_t loc = currentName.find(" ");
		if (loc < currentName.size())
		{
			namingIssueResults.emplace(currentName, "Doesnt allow Space");
		}
	}
}

void NamingConventionModule::CheckForMultipleSeparatorsPerLayer(std::string newNameLayer, std::string currentName, std::string containerName)
{
	if (newNameLayer.find("_") < newNameLayer.size())
	{
		if (!IsCorrectSuffix(currentName, newNameLayer.substr(newNameLayer.find("_") + 1), containerName))
		{
			namingIssueResults.emplace(currentName, "Multiple Separators or wrong suffix");
		}
	}
}

//split into functions 
//determine when it calls false and when true
bool NamingConventionModule::IsCorrectSuffix(std::string currentName, std::string newNameLayer, std::string containerName)
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

			if (container->second.IsSuffixCountInRange(separatorLocations.size()))
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

					if (IsNumber(layer) && !numLayerAppeared && container->second.allowNumberSuffix)
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

bool NamingConventionModule::IsNumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}