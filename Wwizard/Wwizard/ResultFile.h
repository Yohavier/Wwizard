#pragma once
#include <string>
#include "NamingIssues.h"

struct BaseResultFile
{
	BaseResultFile() = delete;
	BaseResultFile(std::string guid, std::string name)
		: guid(guid)
		, name(name)
	{}

	std::string guid;
	std::string name;
};

struct NamingResultFile : public BaseResultFile
{
	NamingResultFile() = delete;
	NamingResultFile(std::string guid, std::string currentName, NamingIssue issue)
		: BaseResultFile(guid, currentName)
		, issue(issue)
	{}
	NamingIssue issue;
};

struct QueryResultFile : public BaseResultFile
{
	QueryResultFile() = delete;
	QueryResultFile(std::string name, std::string guid, std::string path, std::string type, int color)
		: BaseResultFile(guid, name)
		, type(type)
		, color(color)
		, path(path)
	{}

	std::string type;
	std::string path;
	int color;
};

struct ColorResultFile : public BaseResultFile
{
public:
	ColorResultFile() = delete;
	ColorResultFile(std::string objectID, std::string parentID, int colorSettingMode, int applyableColorID, std::string objectPath, std::string name)
		: parentID(parentID)
		, colorSettingMode(colorSettingMode)
		, applyableColorID(applyableColorID)
		, objectPath(objectPath)
		, BaseResultFile(objectID, name)
	{}

	std::string parentID;
	int colorSettingMode = 0;
	int applyableColorID = 0;
	std::string objectPath;
};