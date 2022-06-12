#pragma once
#include <string>

struct WwuSettings
{
	WwuSettings() = default;
	WwuSettings(std::string prefixToApply, bool applyPrefix, bool allowSpace, bool allowUppercase)
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