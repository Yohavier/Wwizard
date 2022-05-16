#pragma once
#include <string>

class BaseModule
{
public:
	BaseModule(){}
	virtual void OnConnectionStatusChange(const bool newConnectionStatus) = 0;
	virtual void OnSettingsChange(const std::string projectPath, const std::string sdkPath) {}
};

