#pragma once
#include <string>

struct WwuSettings
{
	WwuSettings() = default;
	WwuSettings(std::string prefixToApply, bool applyPrefix, bool applyNamingConventionCheck, bool allowSpace)
		: prefixToApply(prefixToApply)
		, applyPrefix(applyPrefix)
		, applyNamingConventionCheck(applyNamingConventionCheck)
		, allowSpace(allowSpace)
	{}

	std::string prefixToApply = "";

	bool applyPrefix = false;
	bool applyNamingConventionCheck = false;
	bool allowSpace = false;
	bool allowUpperCase = false;
};