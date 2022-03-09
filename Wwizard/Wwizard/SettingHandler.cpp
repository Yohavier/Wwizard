#include "SettingHandler.h"

void SettingHandler::SaveSettings(std::string wwiseProjectPath, std::string sdkPath, std::string waapiIP, int waapiPort)
{
	this->wwiseProjectPath = wwiseProjectPath;
	this->sdkPath = sdkPath;
	this->waapiIP = waapiIP;
	this->waapiPort = waapiPort;
}

void SettingHandler::LoadSettings()
{
    FILE* fp = fopen("../SavedData/Settings.json", "rb");
    char* readBuffer = new char[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document settingDoc;
    settingDoc.ParseStream(is);
    fclose(fp);

    assert(settingDoc["wwiseProjectPath"].IsString());
    assert(settingDoc["sdkPath"].IsString());
    assert(settingDoc["waapiIP"].IsString());
    assert(settingDoc["waapiPort"].IsInt());

    this->wwiseProjectPath = settingDoc["wwiseProjectPath"].GetString();
    this->sdkPath = settingDoc["sdkPath"].GetString();
    this->waapiIP = settingDoc["waapiIP"].GetString();
    this->waapiPort = settingDoc["waapiPort"].GetInt();
}

SettingHandler::~SettingHandler()
{
    WriteToJson();
}

void SettingHandler::WriteToJson()
{
    rapidjson::Document d;
    d.SetObject();

    rapidjson::Value wwiseProjectPathValue;
    wwiseProjectPathValue.SetString(rapidjson::StringRef(wwiseProjectPath.c_str()));
    d.AddMember("wwiseProjectPath", wwiseProjectPathValue, d.GetAllocator());

    rapidjson::Value sdkPathValue;
    sdkPathValue.SetString("asd");
    d.AddMember("sdkPath", sdkPathValue, d.GetAllocator());

    rapidjson::Value waapiIPValue;
    waapiIPValue.SetString(rapidjson::StringRef(waapiIP.c_str()));
    d.AddMember("waapiIP", waapiIPValue, d.GetAllocator());

    rapidjson::Value waapiPortValue;
    waapiPortValue.SetInt(waapiPort);
    d.AddMember("waapiPort", waapiPortValue, d.GetAllocator());

    FILE* fp = fopen("../SavedData/Settings.json", "wb");
    char* writeBuffer = new char[65536];
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    d.Accept(writer);

    fclose(fp);
}
