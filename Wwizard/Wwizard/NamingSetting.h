#pragma once
#include <string>
#include <vector>
#include <map>

struct WwuSetting
{
	WwuSetting() = default;
	WwuSetting(std::string prefixToApply, bool applyPrefix, bool allowSpace, bool allowUppercase)
		: prefixToApply(prefixToApply)
		, applyPrefix(applyPrefix)
		, allowSpace(allowSpace)
		, allowUpperCase(allowUppercase)
	{}

	std::string prefixToApply = "";

	bool applyPrefix = false;
	bool allowSpace = false;
	bool allowUpperCase = false;
};

struct ContainerSetting
{
	ContainerSetting() = default;
	ContainerSetting(bool allowNumberSuffix, bool allowStringSuffix, int maxNumberAllowed)
		: applyNumberSuffix(allowNumberSuffix)
		, applyStringSuffix(allowStringSuffix)
		, maxNumberAllowed(maxNumberAllowed)
	{
	}

	bool applyNumberSuffix = false;
	bool applyStringSuffix = false;

	int maxNumberAllowed = 0;

	std::vector<std::string> stringSuffixVector;

	void AddNewSuffix(std::string& newSuffix)
	{
		stringSuffixVector.emplace_back(newSuffix);
	}

	void RemoveSuffix(const std::string& removeSuffix)
	{
		for (auto it = stringSuffixVector.begin(); it != stringSuffixVector.end(); ++it)
		{
			if (*it == removeSuffix)
			{
				stringSuffixVector.erase(it);
				return;
			}
		}
	}
};

struct NamingSetting
{
	NamingSetting() = default;

	std::map<std::string, WwuSetting> wwuSettings;
	std::map<std::string, ContainerSetting> containerSettings;
};