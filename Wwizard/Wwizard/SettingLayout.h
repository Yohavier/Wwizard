#pragma once
#include "BaseLayout.h"
#include "SettingHandler.h"

class SettingLayout: public BaseLayout
{
public:
	SettingLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SettingHandler>& settingHandler);
	void RenderLayout() override;

private:
	std::unique_ptr<SettingHandler>& settingHandler;
};

