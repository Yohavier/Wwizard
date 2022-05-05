#pragma once
#include "WwizardWwiseClient.h"
#include "imgui_internal.h"
#include "imgui.h"
#include "imgui_stdlib.h"

class BaseLayout
{
public:
	BaseLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient)
		: wwizardWwiseClient(wwizardWwiseClient)
	{}

	virtual void RenderLayout() = 0;

public:
	std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient;
};