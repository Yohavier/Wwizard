#pragma once
#include <string>

struct ContainerSettings
{
	ContainerSettings() = default;
	ContainerSettings(bool allowNumberSuffix, bool allowStringSuffix, int suffixLayers, int maxNumberAllowed, std::string stringSuffixes)
		: allowNumberSuffix(allowNumberSuffix)
		, allowStringSuffix(allowStringSuffix)
		, suffixLayers(suffixLayers)
		, maxNumberAllowed(maxNumberAllowed)
		, stringSuffixes(stringSuffixes)
	{
	}

	bool allowNumberSuffix = false;
	bool allowStringSuffix = false;

	int suffixLayers = 0;
	int maxNumberAllowed = 0;

	std::string stringSuffixes = "";

	bool IsStringInSuffixList(std::string layer)
	{
		std::string newSuffix;
		for (auto& c : stringSuffixes)
		{
			if (c != ' ')
			{
				if (c == ',')
				{
					if (newSuffix == layer)
					{
						return true;
					}
					newSuffix = "";
				}
				else
				{
					newSuffix += c;
				}
			}
		}
		if (newSuffix != "")
		{
			if (newSuffix == layer)
			{
				return true;
			}
		}

		return false;
	}

	bool IsNumberInRange(std::string number)
	{

		if (std::to_string(maxNumberAllowed).size() == number.size())
		{
			int numLayer = std::stoi(number);
			if (numLayer <= maxNumberAllowed)
			{
				return true;
			}
		}
		return false;
	}

	bool IsSuffixCountInRange(int layerCount)
	{
		if (layerCount <= maxNumberAllowed)
		{
			return true;
		}
		return false;
	}

	bool IsNumber(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}
};