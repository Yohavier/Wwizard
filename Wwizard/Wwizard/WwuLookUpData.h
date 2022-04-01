#pragma once
#include <string>

struct WwuLookUpData
{
	WwuLookUpData() = default;
	WwuLookUpData(std::string name, std::string guid, std::string wwuType, std::string path)
		:name(name)
		, guid(guid)
		, wwuType(wwuType)
		, path(path)
	{}

	std::string name;
	std::string guid;
	std::string wwuType;
	std::string path;
};