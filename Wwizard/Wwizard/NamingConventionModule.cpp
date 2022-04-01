#include "NamingConventionModule.h"
#include <iostream>

/*
* TODO
* allow space in name
* multiple "_" are not handled correctly yet
* suffix
* save and load naming convention as json
*/

NamingConventionModule::NamingConventionModule()
{
	for (auto& tab : whitelistedWwuTypes)
	{
		wwuSpaceSettings.emplace(tab, WwuSpaceSettings());
	}
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
			ModularResolve(doc.child("WwiseDocument").child(wwuType.c_str()), namePath);
		}
	}
}

void NamingConventionModule::ModularResolve(pugi::xml_node wwuNode, std::string namePath)
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

						ModularResolve(doc.child("WwiseDocument"), namePath);
					}
				}
			}
			else
			{
				std::string newNamePath = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()));
				if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str())
				{
					namingIssueResults.emplace(static_cast<std::string>(node.attribute("Name").value()), newNamePath);
				}
				ModularResolve(node, newNamePath);
			}
		}
		else if (whitelistedContainers.find(static_cast<std::string>(node.name())) != whitelistedContainers.end())
		{
			std::string newNamePath = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()));
			if (static_cast<std::string>(node.attribute("Name").value()) != newNamePath.c_str())
			{
				namingIssueResults.emplace(static_cast<std::string>(node.attribute("Name").value()), newNamePath);
			}
			ModularResolve(node, newNamePath);
		}
		else
		{
			ModularResolve(node, namePath);
		}
	}
}

std::string NamingConventionModule::AddLastNamePathLayer(const std::string& currentNamePath, std::string newNodeName)
{
	size_t lastLayerStart = newNodeName.rfind(levelSeparator);
	if (lastLayerStart < newNodeName.size())
	{
		newNodeName.erase(0, lastLayerStart + 1);
	}
	if (currentNamePath == "")
	{
		return newNodeName;
	}
	else
	{
		return currentNamePath + levelSeparator + newNodeName;
	}
}