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
#include <regex>
#include "WwuLookUpData.h"

class SortOriginalsModule
{
public:
	void LoadModule(std::string wwiseProjPath);

	void CreateUnusedOriginalsList();
	void FinalizeDeleteUnusedOriginals(bool wantDelete);
	void SortOriginals();

	const int&& GetOriginalsCount();
	const std::string& GetOriginalPath();
	const std::vector<std::string> GetUnusedOriginals();

private:
	void ScanOriginalsPath(std::string path);
	void ScanWorkUnitXMLByPath(std::string wwuPath);
	void ScanWorkUnitXMLByGuid(std::string guid);
	void IterateXMLChildren(pugi::xml_node parent);

	void PreFetchAllWwuData(std::string directory);
	void FetchSingleWwuData(std::string wwuPath);
	void ScanWorkUnitOriginalsUse();

	void CreateFolderStructureFromWorkUnitPath(const std::string wwuFolderPath);
	void CreateFolderStructureFomWwu(pugi::xml_node& parent, std::string currentOriginalsPath);

	bool DeleteEmptyFolders(std::string directory);

	void ClearPreviousSortData();
	void Scan();

public:
	bool workUnitFlag;
	bool physicalFolderFlag;
	bool virtualFolderFlag;
	bool actorMixerFlag;
	bool randomContainerFlag;
	bool sequenceContainerFlag;
	bool switchContainerFlag;
	bool blendContainerFlag;
	bool soundSFXFlag;
	bool voiceSoundFlag;

	bool musicSwitchContainerFlag;
	bool musicPlaylistContainerFlag;
	bool musicSegmentFlag;	
	bool musicTrackFlag;

private:
	std::string originalsPath;
	std::string actorMixerWwuPath;
	std::string interactiveMuisicWwuPath;
	std::string projectPath;
	std::vector<std::string> unusedOriginalsPaths;

	std::map<std::string, int> originalsDic;

	std::string container[6] = { "WorkUnit", "Folder", "BlendContainer", "RandomSequenceContainer", "ActorMixer", "SwitchContainer" };

	std::vector<WwuLookUpData> prefetchedWwuData;
};

