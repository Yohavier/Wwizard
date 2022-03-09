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

	void SaveSettings(std::string wwiseProjectPath, std::string sdkPath, std::string waapiIP, int waapiPort);
	void LoadSettings();

	std::string* GetWwisProjectPathRef() { return &wwiseProjectPath; }
	std::string* GetSDKPath() { return &sdkPath; }
	std::string* GetWaapiIP() { return &waapiIP; }
	int* GetWaaapiPort() { return &waapiPort; }

private:
	void WriteToJson();

public:
	std::string wwiseProjectPath;
	std::string sdkPath;
	std::string waapiIP;
	int waapiPort;
};

