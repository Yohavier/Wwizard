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


NamingConventionModule::NamingConventionModule(const std::string& wwiseProjPath, std::unique_ptr<WwizardWwiseClient>& wwizardClient)
	:wwizardClient(wwizardClient)
{
	std::string relativeWwiseProjPath = wwiseProjPath;
	relativeWwiseProjPath.erase(0, 1);
	relativeWwiseProjPath.erase(relativeWwiseProjPath.size() - 1);
	for (int i = static_cast<int>(relativeWwiseProjPath.size()) - 1; i > 0; i--)
	{
		if (relativeWwiseProjPath.at(i) == '\\')
			break;
		else
			relativeWwiseProjPath.erase(i);
	}
	projectPath = relativeWwiseProjPath;

	LoadNamingConventionSettings();
}

NamingConventionModule::~NamingConventionModule()
{
	SaveNamingConventionSettings();
}

void NamingConventionModule::BeginNamingConventionProcess()
{
	ClearOldData();

	FetchWwuDataInDirectory(projectPath);

	StartCheckingNamingConvention(projectPath, "");

	currentNamingConventionThread = nullptr;
}

void NamingConventionModule::FetchWwuDataInDirectory(const std::string& directory)
{
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (std::filesystem::is_directory(entry))
		{ 
			FetchWwuDataInDirectory(entry.path().u8string());
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

void NamingConventionModule::FetchSingleWwuData(const std::string& path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return;

	pugi::xml_node data = doc.child("WwiseDocument").first_child().child("WorkUnit");
	WwuLookUpData newWwuData = WwuLookUpData(data.attribute("Name").value(), data.attribute("ID").value(), data.attribute("PersistMode").value(), path, false);
	prefetchedWwuData.emplace_back(newWwuData);
}

void NamingConventionModule::ClearOldData()
{
	namingIssueResults.clear();
}

void NamingConventionModule::AddIssueToList(const std::string& guid, const std::string& name, const Issue& issue)
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

const std::string& NamingConventionModule::GetErrorMessageFromIssue(const Issue& issue)
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

void NamingConventionModule::OnConnectionStatusChange(const bool newConnectionStatus)
{
	namingIssueResults.clear();
	prefetchedWwuData.clear();

	if (newConnectionStatus)
	{
		FetchWwuDataInDirectory(projectPath);
	}
}

void NamingConventionModule::StartCheckingNamingConvention(const std::string& folderPath, std::string constructedNamePath)
{
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (std::filesystem::is_directory(entry))
		{
			if (entry.path().filename().u8string() != ".cache" && entry.path().filename().u8string() != "GeneratedSoundBanks" && entry.path().filename().u8string() != "Originals" && entry.path().filename().u8string() != "Presets")
			{
				std::vector<std::string> optionList = { "id", "name", "type", "path" };
				std::string relativeFolderPath = "\\" + entry.path().u8string().substr(projectPath.size(), entry.path().u8string().size() - projectPath.size());
				AkJson obj = wwizardClient->GetObjectFromPath(relativeFolderPath, optionList);
				
				std::string lastAddedNameLayer = AddLastNamePathLayer(constructedNamePath, entry.path().filename().u8string(), "Folder");

				if (wwiseWorkFoldersToWwuType.find(relativeFolderPath.substr(1)) != wwiseWorkFoldersToWwuType.end())
				{
					if (whitelistedWwuTypes.find(wwiseWorkFoldersToWwuType[relativeFolderPath.substr(1)]) != whitelistedWwuTypes.end())
					{
						ApplyPrefix(constructedNamePath, wwuSettings[wwiseWorkFoldersToWwuType[relativeFolderPath.substr(1)]]);
					}
				}
				else
				{
					if (!RunChecks(obj["return"].GetArray()[0]["name"].GetVariant().GetString(), obj["return"].GetArray()[0]["id"].GetVariant().GetString(), "folder", lastAddedNameLayer, constructedNamePath, "Folder"))
					{
						break;
					}
				}

				StartCheckingNamingConvention(entry.path().u8string(), constructedNamePath);
			}
		}
		else if (entry.path().extension() == ".wwu")
		{
			ScanWorkUnitXMLByPath(entry.path().u8string(), constructedNamePath);
		}
	}
}

void NamingConventionModule::ApplyPrefix(std::string& constructedNamePath, const WwuSettings& currentWwuSettings)
{
	if (currentWwuSettings.applyPrefix)
	{
		constructedNamePath = currentWwuSettings.prefixToApply;
	}
	else
	{
		constructedNamePath = "";
	}
}

void NamingConventionModule::ScanWorkUnitXMLByPath(const std::string& wwuPath, std::string& namePath)
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
				IterateThroughWwu(doc.child("WwiseDocument").child(wwuType.c_str()), namePath, wwuType);
			}
		}
	}
}

void NamingConventionModule::IterateThroughWwu(const pugi::xml_node& wwuNode, std::string namePath, const std::string& wwuType)
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
				std::string lastAddedLayer = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()), static_cast<std::string>(node.name()));

				if (RunChecks(static_cast<std::string>(node.attribute("Name").value()), static_cast<std::string>(node.attribute("ID").value()), wwuType, lastAddedLayer, namePath, static_cast<std::string>(node.name())))
				{
					IterateThroughWwu(node, namePath, wwuType);
				}	
			}
		}
		else if (whitelistedContainers.find(static_cast<std::string>(node.name())) != whitelistedContainers.end())
		{
			std::string lastAddedLayer = AddLastNamePathLayer(namePath, static_cast<std::string>(node.attribute("Name").value()), static_cast<std::string>(node.name()));
			
			if(RunChecks(static_cast<std::string>(node.attribute("Name").value()), static_cast<std::string>(node.attribute("ID").value()), wwuType, lastAddedLayer, namePath, static_cast<std::string>(node.name())))
			{
				IterateThroughWwu(node, namePath, wwuType);
			}
		}
		else
		{
			IterateThroughWwu(node, namePath, wwuType);
		}
	}
}

std::string NamingConventionModule::AddLastNamePathLayer(std::string& currentNamePath, const std::string& newName, const std::string& containerName)
{
	std::string newNodeName = newName;
	if (currentNamePath == "")
	{
		currentNamePath = newNodeName;
		return newNodeName;
	}
	else
	{
		if (newNodeName.find(levelSeparator) != std::string::npos)
		{
			newNodeName.erase(0, currentNamePath.size() + 1);
		}
		currentNamePath += levelSeparator + newNodeName;

		return newNodeName;
	}
}

bool NamingConventionModule::RunChecks(const std::string& nodeName, const std::string& nodeID, const std::string& wwuType, const std::string & lastAddedLayer, const std::string& reconstructedNamePath, const std::string& containerName)
{
	if (CheckRightPrefix(nodeName, nodeID, wwuType)
		&& CheckNameForSpace(nodeName, nodeID, wwuSettings[wwuType].allowSpace)
		&& CheckUppercaseRule(nodeName, nodeID, wwuSettings[wwuType].allowUpperCase)
		&& CheckForMultipleSeparatorsPerLayer(nodeName, nodeID, lastAddedLayer, containerName)
		&& CheckHierarchy(nodeName,reconstructedNamePath, nodeID))
	{
		return true;
	}
	return false;
}

bool NamingConventionModule::CheckNameForSpace(const std::string& nodeName, const std::string& nodeID, bool& allowSpace)
{
	std::string currentName = nodeName;
	if (!allowSpace)
	{
		size_t loc = currentName.find(" ");
		if (loc < currentName.size())
		{
			AddIssueToList(nodeID, nodeName, Issue::SPACE);
			return false;
		}
	}
	return true;
}

bool NamingConventionModule::CheckForMultipleSeparatorsPerLayer(const std::string& nodeName, const std::string& nodeID, const std::string& newNameLayer, const std::string& containerName)
{
	std::string currentName = nodeName;
	if (newNameLayer.find("_") != std::string::npos)
	{
		if (!IsCorrectSuffix(currentName, newNameLayer.substr(newNameLayer.find("_") + 1), containerName))
		{
			AddIssueToList(nodeID, nodeName, Issue::SEPARATOR);
			return false;
		}
	}
	return true;
}

bool NamingConventionModule::IsCorrectSuffix(const std::string& currentName, const std::string& newNameLayer, const std::string& containerName)
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

bool NamingConventionModule::CheckUppercaseRule(const std::string& nodeName, const std::string& nodeID, const bool& allowUpperCase)
{
	if (allowUpperCase)
		return true;

	std::string currentName = nodeName;
	
	if (std::any_of(currentName.begin(), currentName.end(), isupper))
	{
		AddIssueToList(nodeID, nodeName, Issue::UPPERCASE);
		return false;
	}

	return true;
}

bool NamingConventionModule::CheckRightPrefix(const std::string& nodeName, const std::string& nodeID, const std::string& wwuType)
{
	if (!wwuSettings[wwuType].applyPrefix)
	{
		return true;
	}

	std::string rightPrefix = wwuSettings[wwuType].prefixToApply;
	std::string name = nodeName;

	auto separatorPlace = name.find(levelSeparator);
	if (separatorPlace != std::string::npos)
	{
		if (name.substr(0, separatorPlace) != rightPrefix)
		{
			AddIssueToList(nodeID, nodeName, Issue::PREFIX);
			return false;
		}
	}
	else if (name != rightPrefix)
	{
		AddIssueToList(nodeID, nodeName, Issue::PREFIX);
		return false;
	}
	return true;
}

bool NamingConventionModule::CheckHierarchy(const std::string& currentName, const std::string& constructedName, const std::string& nameID)
{
	if (currentName != constructedName)
	{
		AddIssueToList(nameID, currentName, Issue::HIERARCHY);
		return false;
	}
	return true;
}

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
			if (d.HasMember("WwuSettings"))
			{
				if (d["WwuSettings"].HasMember(wwu.c_str()))
				{
					if (d["WwuSettings"][wwu.c_str()].HasMember("name") && d["WwuSettings"][wwu.c_str()].HasMember("prefixToApply") && d["WwuSettings"][wwu.c_str()].HasMember("applyPrefix") && d["WwuSettings"][wwu.c_str()].HasMember("apply") && d["WwuSettings"][wwu.c_str()].HasMember("allowSpace"))
					{
						wwuSettings.emplace(d["WwuSettings"][wwu.c_str()]["name"].GetString(), WwuSettings(d["WwuSettings"][wwu.c_str()]["prefixToApply"].GetString(),
							d["WwuSettings"][wwu.c_str()]["applyPrefix"].GetBool(),
							d["WwuSettings"][wwu.c_str()]["apply"].GetBool(),
							d["WwuSettings"][wwu.c_str()]["allowSpace"].GetBool()));
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
					if (d["ContainerSettings"][container.c_str()].HasMember("allowNumberSuffix") && d["ContainerSettings"][container.c_str()].HasMember("allowStringSuffix") && d["ContainerSettings"][container.c_str()].HasMember("suffixLayers") && d["ContainerSettings"][container.c_str()].HasMember("maxNumberAllowed") && d["ContainerSettings"][container.c_str()].HasMember("stringSuffixes"))
					{
						containerSettings.emplace(container.c_str(), ContainerSettings(d["ContainerSettings"][container.c_str()]["allowNumberSuffix"].GetBool(),
							d["ContainerSettings"][container.c_str()]["allowStringSuffix"].GetBool(),
							d["ContainerSettings"][container.c_str()]["suffixLayers"].GetInt(),
							d["ContainerSettings"][container.c_str()]["maxNumberAllowed"].GetInt(),
							d["ContainerSettings"][container.c_str()]["stringSuffixes"].GetString()));
					}
				}
			}
		}
	}
}