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
	ContainerSetting(bool allowNumberSuffix, bool allowStringSuffix, int maxNumberAllowed, bool allowUseContainerAsEnumeration)
		: applyNumberSuffix(allowNumberSuffix)
		, applyStringSuffix(allowStringSuffix)
		, maxNumberAllowed(maxNumberAllowed)
		, allowUseContainerAsEnumeration(allowUseContainerAsEnumeration)
	{
	}

	bool applyNumberSuffix = false;
	bool applyStringSuffix = false;

	int maxNumberAllowed = 0;
	bool allowUseContainerAsEnumeration = false;
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

class NamingSetting
{
public:
	NamingSetting() = default;

	std::map<std::string, WwuSetting> wwuSettings;
	std::map<std::string, ContainerSetting> containerSettings;

	void Reset()
	{
		for (auto& wwu : wwuSettings)
		{
			wwu.second.allowSpace = false;
			wwu.second.allowUpperCase = false;
			wwu.second.applyPrefix = false;
			wwu.second.prefixToApply = "";
		}

		for (auto& container : containerSettings)
		{
			container.second.applyNumberSuffix = false;
			container.second.applyStringSuffix = false;
			container.second.maxNumberAllowed = 0;
			container.second.stringSuffixVector.clear();
		}
	}
};