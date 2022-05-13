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