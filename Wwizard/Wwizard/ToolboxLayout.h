#pragma once
#include "BaseLayout.h"
#include "ToolboxModule.h"

class ToolboxLayout : BaseLayout
{
public:
	ToolboxLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<ToolboxModule>& toolboxModule);
	void RenderLayout() override;

private:
	std::unique_ptr<ToolboxModule>& toolboxModule;
};

