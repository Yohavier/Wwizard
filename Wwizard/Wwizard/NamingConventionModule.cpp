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
* Exclude Default Work Units
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

void NamingConventionModule::ApplyPrefix(std::string& namePath, std::string fullFolderName, const WwuSpaceSettings& newPrefix)
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

	if (static_cast<std::string>(doc.child("WwiseDocument").child(wwuType.c_str()).child("WorkUnit").attribute("PersistMode").value()) == "Standalone")
	{
		if (wwuSpaceSettings[wwuType].applyNamingConventionCheck)
		{
			if (whitelistedWwuTypes.find(wwuType) != whitelistedWwuTypes.end())
			{
				ApplyPrefix(namePath, stringToReplace[wwuType], wwuSpaceSettings[wwuType]);
			}
			ModularResolve(doc.child("WwiseDocument").child(wwuType.c_str()), namePath, wwuType);
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
				std::string newNamePath = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()));
				if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str() && namingIssueResults.find(newNamePath) == namingIssueResults.end())
				{
					namingIssueResults.emplace(static_cast<std::string>(node.attribute("Name").value()), newNamePath);
				}
				CheckNameForSpace(static_cast<std::string>(node.attribute("Name").value()), wwuSpaceSettings[wwuType].allowSpace);
				ModularResolve(node, newNamePath, wwuType);
			}
		}
		else if (whitelistedContainers.find(static_cast<std::string>(node.name())) != whitelistedContainers.end())
		{
			std::string newNamePath = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()));
			if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str() && namingIssueResults.find(newNamePath) == namingIssueResults.end())
			{
				namingIssueResults.emplace(static_cast<std::string>(node.attribute("Name").value()), newNamePath);
			}

			CheckNameForSpace(static_cast<std::string>(node.attribute("Name").value()), wwuSpaceSettings[wwuType].allowSpace);
			ModularResolve(node, newNamePath, wwuType);
		}
		else
		{
			ModularResolve(node, namePath, wwuType);
		}
	}
}

std::string NamingConventionModule::AddLastNamePathLayer(const std::string& currentNamePath, std::string newNodeName)
{
	if (currentNamePath == "")
	{
		CheckForMultipleSeparatorsPerLayer(newNodeName, newNodeName);
		return newNodeName;
	}
	else
	{
		if (newNodeName.find(levelSeparator) < newNodeName.size())
		{
			newNodeName.erase(0, currentNamePath.size() + 1);
		}
		std::string newName = currentNamePath + levelSeparator + newNodeName;
		CheckForMultipleSeparatorsPerLayer(newNodeName, newName);

		return newName;
	}
}

void NamingConventionModule::SaveNamingConvention()
{
	rapidjson::Document d;
	d.SetObject();

	rapidjson::Value wwuSettings(rapidjson::kObjectType);

	for (const auto& wwu : wwuSpaceSettings)
	{
		rapidjson::Value rapidJsonWwuSetting(rapidjson::kObjectType);
		
		rapidjson::Value name;
		name = rapidjson::StringRef(wwu.first.c_str());
		rapidJsonWwuSetting.AddMember("name", name, d.GetAllocator());

		rapidjson::Value apply;
		apply.SetBool(wwu.second.applyNamingConventionCheck);
		rapidJsonWwuSetting.AddMember("apply", apply, d.GetAllocator());

		rapidjson::Value applyPrefix;
		applyPrefix.SetBool(wwu.second.applyPrefix);
		rapidJsonWwuSetting.AddMember("applyPrefix", applyPrefix, d.GetAllocator());

		rapidjson::Value prefixToApply;
		prefixToApply = rapidjson::StringRef(wwu.second.prefixToApply.c_str());
		rapidJsonWwuSetting.AddMember("prefixToApply", prefixToApply, d.GetAllocator());

		rapidjson::Value allowSpace;
		allowSpace.SetBool(wwu.second.allowSpace);
		rapidJsonWwuSetting.AddMember("allowSpace", allowSpace, d.GetAllocator());

		wwuSettings.AddMember(rapidjson::StringRef(wwu.first.c_str()), rapidJsonWwuSetting, d.GetAllocator());
	}
	d.AddMember("WwuSettings",wwuSettings, d.GetAllocator());

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
			wwuSpaceSettings.emplace(d["WwuSettings"][wwu.c_str()]["name"].GetString(), WwuSpaceSettings(d["WwuSettings"][wwu.c_str()]["prefixToApply"].GetString(), d["WwuSettings"][wwu.c_str()]["applyPrefix"].GetBool(), d["WwuSettings"][wwu.c_str()]["apply"].GetBool(), d["WwuSettings"][wwu.c_str()]["allowSpace"].GetBool()));
		}
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

void NamingConventionModule::CheckForMultipleSeparatorsPerLayer(std::string newNameLayer, std::string currentName)
{
	if (newNameLayer.find("_") < newNameLayer.size())
	{
		namingIssueResults.emplace(currentName, "Doesnt allow Muiltiple Separators");
	}
}