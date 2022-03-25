#include "SortOriginalsModule.h"

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

	originalsPath = wwiseProjPath + "Originals\\SFX";
	actorMixerWwuPath = wwiseProjPath + "Actor-Mixer Hierarchy";
	interactiveMuisicWwuPath = wwiseProjPath + "Interactive Music Hierarchy";

	ScanOriginalsPath(originalsPath);
	ScanWorkUnitData(actorMixerWwuPath);
	ScanWorkUnitOriginalsUse();
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
	auto data = doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit");
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
	//opens wwu file succesfully
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(wwuPath.c_str());
	if (!result)
		return;

	IterateXMLChildren(doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit"));
}

void SortOriginalsModule::IterateXMLChildren(pugi::xml_node parent)
{
	for (pugi::xml_node children : parent)
	{
		if (static_cast<std::string>(children.name()) == "Sound")
		{
			std::cout << children.attribute("Name").value() << "\n";
			IterateXMLChildren(children);
		}
		else if (static_cast<std::string>(children.name()) == "AudioFile")
		{
			originalsDic[originalsPath + "\\" + children.text().as_string()] += 1;
		}
		else if (static_cast<std::string>(children.name()) == "WorkUnit")
		{
			if (static_cast<std::string>(children.attribute("PersistMode").value()) == "Reference")
				ScanWorkUnitXMLByGuid(children.attribute("ID").as_string());
		}
		else
		{
			if (std::find(std::begin(container), std::end(container), static_cast<std::string>(children.name())) != std::end(container))
			{
				std::cout << children.attribute("Name").value() << "\n";
			}

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

			IterateXMLChildren(doc.child("WwiseDocument").child("AudioObjects").child("WorkUnit"));
		}
	}	
}


void SortOriginalsModule::DeleteUnusedOriginals()
{
	std::vector<std::string> deletedPaths;
	for (auto& originalWav : originalsDic)
	{
		if (originalWav.second == 0)
		{
			std::filesystem::remove(std::filesystem::path(originalWav.first));
			deletedPaths.push_back(originalWav.first);
		}
	}
	for (auto& dPath : deletedPaths)
	{
		originalsDic.erase(dPath);
	}
}

