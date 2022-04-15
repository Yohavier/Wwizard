#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <map>
#include <set>
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
	SortOriginalsModule() = delete;
	SortOriginalsModule(const std::string& wwiseProjPath);

	void CollectUnusedOriginals();
	void DeleteUnusedOriginals(const bool wantDelete);
	void SortOriginals();

	const int& GetOriginalsCount();
	const std::string& GetOriginalPath();
	const std::set<std::string>& GetUnusedOriginals();

private:
	void ScanOriginalsPath(const std::string path);
	void ScanWorkUnitXMLByPath(const std::string wwuPath);
	void ScanWorkUnitXMLByGuid(const std::string& guid);
	void IterateXMLChildren(const pugi::xml_node& parent);

	void PreFetchAllWwuData(const std::string& directory);
	void FetchSingleWwuData(const std::string& wwuPath);
	void ScanWorkUnitOriginalsUse();

	void CreateFolderStructureFromWorkUnitPath(const std::string& wwuFolderPath);
	void CreateFolderStructureFomWwu(const pugi::xml_node& parent, const std::string& currentOriginalsPath);

	bool DeleteEmptyFolders(const std::string& directory);

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

	std::map<std::string, int> originalsDic;

	std::vector<WwuLookUpData> prefetchedWwuData;
	std::set<std::string> unusedOriginalsPaths;

	const std::string container[6] = { "WorkUnit", "Folder", "BlendContainer", "RandomSequenceContainer", "ActorMixer", "SwitchContainer" };
};

