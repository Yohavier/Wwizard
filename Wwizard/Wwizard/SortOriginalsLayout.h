#pragma once
#include "BaseLayout.h"
#include "SortOriginalsModule.h"

class SortOriginalsLayout : public BaseLayout
{
public:
	SortOriginalsLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<SortOriginalsModule>& sortOriginalsModule);
	void RenderLayout() override;

private:
	std::unique_ptr<SortOriginalsModule>& sortOriginalsModule;
};