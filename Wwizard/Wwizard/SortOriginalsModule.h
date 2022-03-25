#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <map>
#include <fstream>
#include <vector>
#include <stdio.h>
#include "pugixml-1.12/src/pugixml.hpp"
#include <algorithm>

struct WwuLookUpData
{
	WwuLookUpData() = default;
	WwuLookUpData(std::string name, std::string guid, std::string wwuType, std::string path)
		:name(name)
		, guid(guid)
		, wwuType(wwuType)
		, path(path)
	{}

	std::string name;
	std::string guid;
	std::string wwuType;
	std::string path;
};

class SortOriginalsModule
{
public:
	int&& GetOriginalsCount() { return static_cast<int>(originalsDic.size()); }
	std::string& GetOriginalPath() { return originalsPath; }
	void ScanOriginalsPath(std::string path);
	void LoadModule(std::string wwiseProjPath);

	void DeleteUnusedOriginals();

private:
	void ScanWorkUnitXMLByPath(std::string wwuPath);
	void ScanWorkUnitXMLByGuid(std::string guid);
	void IterateXMLChildren(pugi::xml_node parent);

	void ScanWorkUnitData(std::string directory);
	void FetchWwuData(std::string wwuPath);
	void ScanWorkUnitOriginalsUse();

public:
	bool workUnit;
	bool physicalFolder;
	bool virtualFolder;
	bool actorMixer;
	bool randomContainer;
	bool sequenceContainer;
	bool switchContainer;
	bool blendContainer;
	bool soundSFX;
	bool voiceSound;

	bool musicSwitchContainer;
	bool musicPlaylistContainer;
	bool musicSegment;	
	bool musicTrack;

private:
	std::string originalsPath;
	std::string actorMixerWwuPath;
	std::string interactiveMuisicWwuPath;

	std::map<std::string, int> originalsDic;

	std::string container[6] = { "WorkUnit", "Folder", "BlendContainer", "RandomSequenceContainer", "ActorMixer", "SwitchContainer" };

	std::vector<WwuLookUpData> wwuData;

};

