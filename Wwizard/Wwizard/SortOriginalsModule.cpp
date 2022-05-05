#include "SortOriginalsModule.h"

SortOriginalsModule::SortOriginalsModule(const std::string& wwiseProjPath)
{
	std::string relativeWwiseProjectPath = wwiseProjPath;
	relativeWwiseProjectPath.erase(0, 1);
	relativeWwiseProjectPath.erase(relativeWwiseProjectPath.size() - 1);
	for (int i = static_cast<int>(relativeWwiseProjectPath.size()) - 1; i > 0; i--)
	{
		if (relativeWwiseProjectPath.at(i) == '\\')
			break;
		else
			relativeWwiseProjectPath.erase(i);
	}

	projectPath = relativeWwiseProjectPath;
	originalsPath = relativeWwiseProjectPath + "Originals\\SFX";
	actorMixerWwuPath = relativeWwiseProjectPath + "Actor-Mixer Hierarchy";
	interactiveMuisicWwuPath = relativeWwiseProjectPath + "Interactive Music Hierarchy";
}

void SortOriginalsModule::ClearPreviousSortData() 
{
	originalsDic.clear();
	fetchedWwuData.clear();
	musicDic.clear();
	sfxDic.clear();
}

void SortOriginalsModule::ScanOriginalsPath(const std::string path)
{
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (std::filesystem::is_directory(entry))
		{
			ScanOriginalsPath(entry.path().u8string());
		}
		else
		{
			if (originalsDic.find(entry.path().u8string()) == originalsDic.end())
			{
				if (entry.path().extension() == ".wav")
				{
					originalsDic.insert({ entry.path().u8string(), 0 });
				}		
			}
		}
	}
}
 
void SortOriginalsModule::FetchWwuDataInDirectory(const std::string& directory)
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

void SortOriginalsModule::FetchSingleWwuData(const std::string& path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return;

	pugi::xml_node data = doc.child("WwiseDocument").first_child().child("WorkUnit");
		
	bool isMusic = false;
	if (static_cast<std::string>(doc.child("WwiseDocument").first_child().name()) == "InteractiveMusic")
	{
		isMusic = true;
	}
	WwuLookUpData newWwuData = WwuLookUpData(data.attribute("Name").value(), data.attribute("ID").value(), data.attribute("PersistMode").value(), path, isMusic);
	fetchedWwuData.emplace_back(newWwuData);
}

void SortOriginalsModule::ScanWorkUnitOriginalsUse()
{
	for (const auto& data : fetchedWwuData)
	{
		if (data.wwuType == "Standalone")
		{
			ScanWorkUnitXMLByPath(data.path, data.isMusic);
		}
	}
}

void SortOriginalsModule::ScanWorkUnitXMLByPath(const std::string wwuPath, const bool& isMusic)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(wwuPath.c_str());
	if (!result)
		return;

	IterateXMLChildren(doc.child("WwiseDocument"), isMusic);
}

void SortOriginalsModule::IterateXMLChildren(const pugi::xml_node& parent, const bool& isMusic)
{
	for (pugi::xml_node children : parent)
	{
		if (static_cast<std::string>(children.name()) == "AudioFile")
		{
			auto nodeValue = children.text().as_string();
			int currentValue = originalsDic[originalsPath + "\\" + nodeValue] += 1;
			if (isMusic)
			{
				musicDic.emplace(originalsPath + "\\" + nodeValue, currentValue);
			}
			else
			{
				sfxDic.emplace(originalsPath + "\\" + nodeValue, currentValue);
			}
		}
		else if (static_cast<std::string>(children.name()) == "WorkUnit")
		{
			if (static_cast<std::string>(children.attribute("PersistMode").value()) == "Reference")
				ScanWorkUnitXMLByGuid(children.attribute("ID").as_string(), isMusic);
			IterateXMLChildren(children, isMusic);
		}
		else
		{
			IterateXMLChildren(children, isMusic);
		}
	}
}

void SortOriginalsModule::ScanWorkUnitXMLByGuid(const std::string& guid, const bool& isMusic)
{
	for (auto& data : fetchedWwuData)
	{
		if (data.guid == guid)
		{
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file(data.path.c_str());
			if (!result)
				return;

			IterateXMLChildren(doc.child("WwiseDocument"), isMusic);
		}
	}	
}


void SortOriginalsModule::CollectUnusedOriginals()
{
	BeginScanProcess();
	unusedOriginalsPaths.clear();
	for (auto& originalWav : originalsDic)
	{
		if (originalWav.second == 0)
		{
			unusedOriginalsPaths.insert(originalWav.first);
		}
	}

}

void SortOriginalsModule::DeleteUnusedOriginals()
{
	for (auto& dPath : unusedOriginalsPaths)
	{
		std::filesystem::remove(std::filesystem::path(dPath));
		originalsDic.erase(dPath);
	}
	ClearCollectedOriginalsList();
}

void SortOriginalsModule::ClearCollectedOriginalsList()
{
	unusedOriginalsPaths.clear();
}

void SortOriginalsModule::SortOriginals()
{
	BeginScanProcess();
	if (GetOriginalsCount() > 0)
	{
		std::filesystem::create_directory(originalsPath + "\\Multiuse");

		CreateFolderStructureFromWorkUnitPath(actorMixerWwuPath);
		CreateFolderStructureFromWorkUnitPath(interactiveMuisicWwuPath);
	}
	DeleteEmptyFolders(originalsPath);
	currentSortingThread = nullptr;
}


void SortOriginalsModule::CreateFolderStructureFromWorkUnitPath(const std::string& wwuFolderPath)
{
	std::string modWwuPath = wwuFolderPath;
	if (physicalFolderFlag)
	{
		auto extension = modWwuPath.find(projectPath);
		if (extension != std::string::npos)
			modWwuPath.erase(extension, projectPath.length());
		modWwuPath = "\\" + modWwuPath;
		std::filesystem::create_directory(originalsPath + modWwuPath);
	}
	else
	{
		modWwuPath = "";
	}


	for (const auto& entry : std::filesystem::directory_iterator(wwuFolderPath))
	{
		if (std::filesystem::is_directory(entry))
		{
			CreateFolderStructureFromWorkUnitPath(entry.path().u8string());
		}
		else
		{
			if (entry.path().extension() == ".wwu")
			{
				pugi::xml_document doc;
				pugi::xml_parse_result result = doc.load_file(entry.path().u8string().c_str());
				if (!result)
					return;
				
				if (static_cast<std::string>(doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit").attribute("PersistMode").value()) == "Standalone" || static_cast<std::string>(doc.child("WwiseDocument").child("InteractiveMusic").child("WorkUnit").attribute("PersistMode").value()) == "Standalone")
				{
					auto root = doc.child("WwiseDocument");
					CreateFolderStructureFomWwu(root, originalsPath + modWwuPath);
				}
				doc.save_file(entry.path().u8string().c_str());
			}
		}
	}
}

void SortOriginalsModule::CreateFolderStructureFomWwu(const pugi::xml_node& parent, const std::string& currentOriginalsPath)
{
	for (pugi::xml_node children : parent)
	{
		if (static_cast<std::string>(children.name()) == "WorkUnit")
		{
			if (static_cast<std::string>(children.attribute("PersistMode").value()) == "Standalone")
			{	
				std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
				if (workUnitFlag)
				{
					std::filesystem::create_directory(newPath);
					CreateFolderStructureFomWwu(children, newPath);
				}
				else
				{
					CreateFolderStructureFomWwu(children, currentOriginalsPath);
				}
				
			}
			else if (static_cast<std::string>(children.attribute("PersistMode").value()) == "Reference")
			{
				for (const auto& data : fetchedWwuData)
				{
					if (data.guid == static_cast<std::string>(children.attribute("ID").value()))
					{
						pugi::xml_document doc;
						pugi::xml_parse_result result = doc.load_file(data.path.c_str());
						if (result)
						{
							std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
							
							if (workUnitFlag)
							{
								std::filesystem::create_directory(newPath);
								pugi::xml_node root;
								if(doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit").attribute("Name").as_string() != "")
									root = doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit");
								else
									root = doc.child("WwiseDocument").child("InteractiveMusic").child("WorkUnit");
								CreateFolderStructureFomWwu(root, newPath);
							}
							else
							{
								pugi::xml_node root;
								if (doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit").attribute("Name").as_string() != "")
									root = doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit");
								else
									root = doc.child("WwiseDocument").child("InteractiveMusic").child("WorkUnit");
								
								CreateFolderStructureFomWwu(root, currentOriginalsPath);
							}
						}
						doc.save_file(data.path.c_str());
					}
				}
			}
		}
		else if (static_cast<std::string>(children.name()) == "Folder")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (virtualFolderFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "SwitchContainer")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (switchContainerFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "BlendContainer")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (blendContainerFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "RandomSequenceContainer")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (randomContainerFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "ActorMixer")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (actorMixerFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "Sound")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (soundSFXFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "MusicPlaylistContainer")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (musicPlaylistContainerFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "MusicSwitchContainer")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (musicSwitchContainerFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "MusicSegment")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (musicSegmentFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "MusicTrack")
		{
			std::string newPath = currentOriginalsPath + "\\" + children.attribute("Name").as_string();
			if (musicTrackFlag)
			{
				std::filesystem::create_directory(newPath);
				CreateFolderStructureFomWwu(children, newPath);
			}
			else
			{
				CreateFolderStructureFomWwu(children, currentOriginalsPath);
			}
		}
		else if (static_cast<std::string>(children.name()) == "AudioFile")
		{
			std::string oldPath = originalsPath + "\\" + children.text().as_string();
			std::string newPath;

			if (originalsDic[oldPath] == 1)
			{
				newPath = currentOriginalsPath + "\\" + std::filesystem::path(children.text().as_string()).filename().u8string();

				auto modWwuPath = newPath;
				auto extension = modWwuPath.find(originalsPath);
				if (extension != std::string::npos)
					modWwuPath.erase(extension, originalsPath.length() + 1);
				modWwuPath = modWwuPath;
				children.text().set(modWwuPath.c_str());
			}
			else 
			{
				newPath = originalsPath + "\\" + "Multiuse" + "\\" + std::filesystem::path(children.text().as_string()).filename().u8string();
				std::string xmlPath = "Multiuse\\" + std::filesystem::path(children.text().as_string()).filename().u8string();
				children.text().set(xmlPath.c_str());	
			}

			if (std::filesystem::exists(oldPath))
			{
				std::filesystem::rename(std::filesystem::path(oldPath), std::filesystem::path(newPath));
			}
		}
		else
		{
			CreateFolderStructureFomWwu(children, currentOriginalsPath);
		}
	}
}

bool SortOriginalsModule::DeleteEmptyFolders(const std::string& directory)
{
	bool deleteFlag = true;
	for (auto& subfolder : std::filesystem::directory_iterator(directory))
	{
		if (std::filesystem::is_directory(subfolder))
		{
			if (!DeleteEmptyFolders(subfolder.path().u8string()))
			{
				deleteFlag = false;
			}
		}
		else if (subfolder.path().extension() == ".wav")
		{
			deleteFlag = false;
		}
	}

	if (deleteFlag)
	{
		if (directory != originalsPath)
		{
			std::filesystem::remove_all(directory);
		}
	}
	return deleteFlag;
}

void SortOriginalsModule::BeginScanProcess()
{
	ClearPreviousSortData();
	ScanOriginalsPath(originalsPath);
	FetchWwuDataInDirectory(actorMixerWwuPath);
	FetchWwuDataInDirectory(interactiveMuisicWwuPath);
	ScanWorkUnitOriginalsUse();
}

void SortOriginalsModule::OnConnectionStatusChange(const bool newConnectionStatus)
{
	originalsDic.clear();
	musicDic.clear();
	sfxDic.clear();
	unusedOriginalsPaths.clear();
	fetchedWwuData.clear();

	if (newConnectionStatus)
	{
		FetchWwuDataInDirectory(actorMixerWwuPath);
		FetchWwuDataInDirectory(interactiveMuisicWwuPath);
	}
}

void SortOriginalsModule::StartSortOriginalsThread()
{
	if (currentSortingThread != nullptr)
	{
		return;
	}

	std::cout << "Start Sorting Thread" << std::endl;
	std::thread sortThread(&SortOriginalsModule::SortOriginals, this);
	currentSortingThread = &sortThread;
	sortThread.detach();
}


const int& SortOriginalsModule::GetOriginalsCount() 
{ 
	return static_cast<int>(originalsDic.size()); 
}

const std::string& SortOriginalsModule::GetOriginalPath() 
{ 
	return originalsPath; 
}

const std::set<std::string>& SortOriginalsModule::GetUnusedOriginals()
{
	return unusedOriginalsPaths;
}

const int& SortOriginalsModule::GetMusicCount()
{
	return static_cast<int>(musicDic.size());
}

const int& SortOriginalsModule::GetSFXCount()
{
	return static_cast<int>(sfxDic.size());
}

