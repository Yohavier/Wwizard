#pragma once
#include "BaseLayout.h"
#include "WwiseColors.h"

class HomeLayout: public BaseLayout
{
public:
	HomeLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient);
	void RenderLayout() override;
};

