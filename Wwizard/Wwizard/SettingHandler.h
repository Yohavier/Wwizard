#pragma once
#include <string>	
#include "rapidjson/document.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <filesystem>

class SettingHandler
{
public:
	SettingHandler()
	{
		LoadSettings();
	}

	~SettingHandler()
	{
		WriteToJson();
	}

	void SaveSettings(const std::string wwiseProjectPath, const std::string sdkPath, const std::string waapiIP, const int waapiPort);
	void LoadSettings();

	std::string& GetWwisProjectPathRef() { return wwiseProjectPath; }
	std::string& GetSDKPath() { return sdkPath; }
	std::string& GetWaapiIP() { return waapiIP; }
	int& GetWaaapiPort() { return waapiPort; }
	const bool IsProjectPathValid();

private:
	void WriteToJson();

public:
	std::string wwiseProjectPath;
	std::string sdkPath;
	std::string waapiIP;
	int waapiPort;
};

