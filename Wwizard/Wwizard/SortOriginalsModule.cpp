#include "SortOriginalsModule.h"
#include <regex>
void SortOriginalsModule::LoadModule(std::string wwiseProjPath)
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
	originalsPath = wwiseProjPath + "Originals\\SFX";
	actorMixerWwuPath = wwiseProjPath + "Actor-Mixer Hierarchy";
	interactiveMuisicWwuPath = wwiseProjPath + "Interactive Music Hierarchy";

	Scan();
}

void SortOriginalsModule::ClearPreviousSortData() 
{
	originalsDic.clear();
	wwuData.clear();
}
//Originals 
void SortOriginalsModule::ScanOriginalsPath(std::string path)
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

//Wwu 
void SortOriginalsModule::ScanWorkUnitData(std::string directory)
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

void SortOriginalsModule::FetchWwuData(std::string path)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return;

	pugi::xml_node data = doc.child("WwiseDocument").first_child().child("WorkUnit");
		
	WwuLookUpData newWwuData = WwuLookUpData(data.attribute("Name").value(), data.attribute("ID").value(), data.attribute("PersistMode").value(), path);
	wwuData.emplace_back(newWwuData);
}

void SortOriginalsModule::ScanWorkUnitOriginalsUse()
{
	for (const auto& data : wwuData)
	{
		if (data.wwuType == "Standalone")
		{
			ScanWorkUnitXMLByPath(data.path);
		}
	}
}

void SortOriginalsModule::ScanWorkUnitXMLByPath(std::string wwuPath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(wwuPath.c_str());
	if (!result)
		return;

	IterateXMLChildren(doc.child("WwiseDocument"));
}

void SortOriginalsModule::IterateXMLChildren(pugi::xml_node parent)
{
	for (pugi::xml_node children : parent)
	{
		if (static_cast<std::string>(children.name()) == "AudioFile")
		{
			auto nodeValue = children.text().as_string();
			originalsDic[originalsPath + "\\" + nodeValue] += 1;
		}
		else if (static_cast<std::string>(children.name()) == "WorkUnit")
		{
			if (static_cast<std::string>(children.attribute("PersistMode").value()) == "Reference")
				ScanWorkUnitXMLByGuid(children.attribute("ID").as_string());
			IterateXMLChildren(children);
		}
		else
		{
			IterateXMLChildren(children);
		}
	}
}

void SortOriginalsModule::ScanWorkUnitXMLByGuid(std::string guid)
{
	for (auto& data : wwuData)
	{
		if (data.guid == guid)
		{
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file(data.path.c_str());
			if (!result)
				return;

			IterateXMLChildren(doc.child("WwiseDocument"));
		}
	}	
}


void SortOriginalsModule::CreateUnusedOriginalsList()
{
	unusedOriginalsPaths.clear();
	for (auto& originalWav : originalsDic)
	{
		if (originalWav.second == 0)
		{
			unusedOriginalsPaths.push_back(originalWav.first);
		}
	}

}

void SortOriginalsModule::FinalizeDeleteUnusedOriginals(bool wantDelete)
{
	if (wantDelete)
	{
		for (auto& dPath : unusedOriginalsPaths)
		{
			std::filesystem::remove(std::filesystem::path(dPath));
			originalsDic.erase(dPath);
		}
	}
	unusedOriginalsPaths.clear();
}

void SortOriginalsModule::SortOriginals()
{
	Scan();
	std::filesystem::create_directory(originalsPath + "\\Multiuse");

	CreateFolderStructureFromWorkUnitPath(actorMixerWwuPath);
	CreateFolderStructureFromWorkUnitPath(interactiveMuisicWwuPath);
	DeleteEmptyFolders(originalsPath);
}


void SortOriginalsModule::CreateFolderStructureFromWorkUnitPath(const std::string wwuFolderPath)
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

void SortOriginalsModule::CreateFolderStructureFomWwu(pugi::xml_node& parent, std::string currentOriginalsPath)
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
				for (const auto& data : wwuData)
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

bool SortOriginalsModule::DeleteEmptyFolders(std::string directory)
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
		std::filesystem::remove_all(directory);
	}
	return deleteFlag;
}

void SortOriginalsModule::Scan()
{
	ClearPreviousSortData();
	ScanOriginalsPath(originalsPath);
	ScanWorkUnitData(actorMixerWwuPath);
	ScanWorkUnitData(interactiveMuisicWwuPath);
	ScanWorkUnitOriginalsUse();
}