#pragma once
#include "SettingHandler.h"

void SettingHandler::SaveSettings(const std::string wwiseProjectPath, const std::string sdkPath, const std::string waapiIP, const int waapiPort)
{
	this->wwiseProjectPath = wwiseProjectPath;
	this->sdkPath = sdkPath;
	this->waapiIP = waapiIP;
	this->waapiPort = waapiPort;

    WriteToJson();
    settingChangeFlag = true;
}

void SettingHandler::LoadSettings()
{
    auto path = static_cast<std::string>(SOLUTION_DIR) + "/SavedData/Settings.json";
    FILE* fp = fopen(path.c_str(), "rb");
    if (fp != 0)
    {
        char* readBuffer = new char[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document settingDoc;
        settingDoc.ParseStream(is);
        fclose(fp);

        if (settingDoc.HasMember("wwiseProjectPath") && settingDoc.HasMember("sdkPath") && settingDoc.HasMember("waapiIP") && settingDoc.HasMember("waapiPort"))
        {
            this->wwiseProjectPath = settingDoc["wwiseProjectPath"].GetString();
            this->sdkPath = settingDoc["sdkPath"].GetString();
            this->waapiIP = settingDoc["waapiIP"].GetString();
            this->waapiPort = settingDoc["waapiPort"].GetInt();
        }
    }   
}

void SettingHandler::WriteToJson()
{
    rapidjson::Document d;
    d.SetObject();

    rapidjson::Value wwiseProjectPathValue;
    wwiseProjectPathValue.SetString(rapidjson::StringRef(wwiseProjectPath.c_str()));
    d.AddMember("wwiseProjectPath", wwiseProjectPathValue, d.GetAllocator());

    rapidjson::Value sdkPathValue;
    sdkPathValue.SetString(rapidjson::StringRef(sdkPath.c_str()));
    d.AddMember("sdkPath", sdkPathValue, d.GetAllocator());

    rapidjson::Value waapiIPValue;
    waapiIPValue.SetString(rapidjson::StringRef(waapiIP.c_str()));
    d.AddMember("waapiIP", waapiIPValue, d.GetAllocator());

    rapidjson::Value waapiPortValue;
    waapiPortValue.SetInt(waapiPort);
    d.AddMember("waapiPort", waapiPortValue, d.GetAllocator());

    auto path = static_cast<std::string>(SOLUTION_DIR) + "/SavedData/Settings.json";
    FILE* fp = fopen(path.c_str(), "wb");
    if (fp != 0)
    {
        char* writeBuffer = new char[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        d.Accept(writer);

        fclose(fp);
    }
}

const bool SettingHandler::IsProjectPathValid()
{
    std::string path = wwiseProjectPath;
    path.erase(0, 1);
    path.erase(path.size() - 1);
    return std::filesystem::exists(path);
}