#pragma once

class BaseModule
{
public:
	BaseModule(){}
	virtual void OnConnectionStatusChange(const bool newConnectionStatus) = 0;
};

