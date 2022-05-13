#pragma once
#include <string>
#include <vector>
#include "WwizardWwiseClient.h"
#include "QueryType.h"

struct BaseQueryStructure
{
	std::string name;
	std::string guid;
	std::string path;
	AkJson arg;
	QueryType structureType = QueryType::FOLDER;
	std::vector<BaseQueryStructure> subHierarchy;
	BaseQueryStructure() = default;

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType)
		: name(name)
		, guid(guid)
		, path(path)
		, structureType(queryType)
	{}

	BaseQueryStructure(std::string name, std::string guid, std::string path, QueryType queryType, AkJson arg)
		: name(name)
		, guid(guid)
		, path(path)
		, structureType(queryType)
		, arg(arg)
	{}
};