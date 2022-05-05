#pragma once
#include "BaseLayout.h"
#include "ColorCodingModule.h"

class ColorCodingLayout : public BaseLayout
{
public:
	ColorCodingLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<ColorCodingModule>& colorCodingModule);
	void RenderLayout() override;

private:
	std::unique_ptr<ColorCodingModule>& colorCodingModule;
};