#pragma once
#include "BaseLayout.h"
#include "NamingConventionModule.h"

class NamingConventionLayout : public BaseLayout
{
public:
	NamingConventionLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<NamingConventionModule>& namingConventionModule);
	void RenderLayout() override;

private:
	std::unique_ptr<NamingConventionModule>& namingConventionModule;
};

