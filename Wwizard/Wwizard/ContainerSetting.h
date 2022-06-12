#pragma once
#include <string>
#include <vector>

struct ContainerSettings
{
	ContainerSettings() = default;
	ContainerSettings(bool allowNumberSuffix, bool allowStringSuffix, int maxNumberAllowed)
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