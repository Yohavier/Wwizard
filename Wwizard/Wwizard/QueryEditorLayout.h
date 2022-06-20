#pragma once
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#   define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "BaseLayout.h"
#include "QueryEditorModule.h"

#include <vector>
#include <map>
#include <string>
#include <imgui.h>
#include <imgui_internal.h>
#include "MyNode.h"
#include "helper.h"
#include "WaqlIntelliSense.h"

class QueryEditorLayout : public BaseLayout
{
public:
	QueryEditorLayout(std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient, std::unique_ptr<QueryEditorModule>& queryEditorModule);
	void RenderLayout() override;

private:
	void ShowActiveQueries();
	void ShowQueryResults();
	void ShowWaapiQueries();
	void ShowWaqlQueries();
	void ShowWwiseQueries(const BaseQueryStructure& queryObject);
	void ShowQueryCreator();
	void DetailsWidget();
	void ShowQueryNodeEditor();

    void SettingWidget();
    void AvailableQueriesWidget();
    void ActiveQueriesWidget();
    void ResultWidget();

    void RecalculateNodeGraph();
    void SetUseQueryNodeEditor(bool newState);
    void UpdateSelectedNode(MyNode* newSelectedNode);

    static int HandleArgInput(ImGuiInputTextCallbackData* data);

public:
    std::map<std::string, std::function<MyNode* ()>> available_nodes
    {
        {"Query", [this]() -> MyNode* {  return new MyQueryNode("Query", {}, {{"Results", QueryResults}  }, this->wwizardWwiseClient, GenerateGuid()); }},

        {"And", []() -> MyNode* { return new MyNode("And", {{"Results 1", QueryResults}}, {{"Results", QueryResults}}, GenerateGuid()); }},

        {"Or", []() -> MyNode* { return new MyNode("Or", {{"Results 1", QueryResults}}, {{"Results", QueryResults}}, GenerateGuid()); }},

        {"Output", []() -> MyNode* { return new MyNode("Output", {{"Results", QueryResults}}, {}, GenerateGuid()); }},
    };

    std::map<std::string, MyNode*> nodes;

private:
	bool useQueryNodeEditor = true;
	const std::unique_ptr<QueryEditorModule>& queryEditorModule;
    WaqlIntelliSense* intelliSense;

    MyNode* outputNode;
    MyNode* currentSelectedNode = nullptr;

    const char* items[2] = { "Standard", "Nodegraph" };
    const char* current_item = items[0];
};