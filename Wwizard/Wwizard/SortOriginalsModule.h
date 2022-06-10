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
#include <thread>
#include "BaseModule.h"

class SortOriginalsModule: public BaseModule
{
public:
	SortOriginalsModule() = delete;
	SortOriginalsModule(const std::string& wwiseProjPath);

	void CollectUnusedOriginals();
	void DeleteUnusedOriginals();
	void ClearCollectedOriginalsList();

	const int GetOriginalsCount();
	const std::string& GetOriginalPath();
	const std::set<std::string>& GetUnusedOriginals();
	const int GetMusicCount();
	const int GetSFXCount();

	void StartSortOriginalsThread();
	void BeginScanProcess();

	void SetProjectPath(std::string newProjectPath);
	void OnConnectionStatusChange(const bool newConnectionStatus) override;
	void OnSettingsChange(const std::string projectPath, const std::string sdkPath) override;

private:
	void ScanOriginalsPath(const std::string path);
	void ScanWorkUnitXMLByPath(const std::string wwuPath, const bool& isMusic);
	void ScanWorkUnitXMLByGuid(const std::string& guid, const bool& isMusic);
	void IterateXMLChildren(const pugi::xml_node& parent, const bool& isMusic);

	void FetchWwuDataInDirectory(const std::string& directory);
	void FetchSingleWwuData(const std::string& wwuPath);
	void ScanWorkUnitOriginalsUse();

	void CreateFolderStructureFromWorkUnitPath(const std::string& wwuFolderPath);
	void CreateFolderStructureFomWwu(const pugi::xml_node& parent, const std::string& currentOriginalsPath);

	bool DeleteEmptyFolders(const std::string& directory);

	void ClearPreviousSortData();

	void SortOriginals();

public:
	bool workUnitFlag = false;
	bool physicalFolderFlag = false;
	bool virtualFolderFlag = false;
	bool actorMixerFlag = false;
	bool randomContainerFlag = false;
	bool sequenceContainerFlag = false;
	bool switchContainerFlag = false;
	bool blendContainerFlag = false;
	bool soundSFXFlag = false;
	bool voiceSoundFlag = false;

	bool musicSwitchContainerFlag = false;
	bool musicPlaylistContainerFlag = false;
	bool musicSegmentFlag = false;
	bool musicTrackFlag = false;

private:
	std::string originalsPath;
	std::string actorMixerWwuPath;
	std::string interactiveMuisicWwuPath;
	std::string projectPath;

	std::map<std::string, int> originalsDic;
	std::map<std::string, int> musicDic;
	std::map<std::string, int> sfxDic;

	std::vector<WwuLookUpData> fetchedWwuData;
	std::set<std::string> unusedOriginalsPaths;

	std::thread* currentSortingThread = nullptr;
	std::thread* currentScanThread = nullptr;
};

