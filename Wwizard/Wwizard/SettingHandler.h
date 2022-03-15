#pragma once
#include <string>	
#include "rapidjson/document.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

class SettingHandler
{
public:
	SettingHandler()
	{
		LoadSettings();
	}
	~SettingHandler();

	void SaveSettings(const std::string wwiseProjectPath, const std::string sdkPath, const std::string waapiIP, const int waapiPort);
	void LoadSettings();

	const std::string& GetWwisProjectPathRef() { return wwiseProjectPath; }
	const std::string& GetSDKPath() { return sdkPath; }
	const std::string& GetWaapiIP() { return waapiIP; }
	const int& GetWaaapiPort() { return waapiPort; }

private:
	void WriteToJson();

public:
	std::string wwiseProjectPath;
	std::string sdkPath;
	std::string waapiIP;
	int waapiPort;
};

