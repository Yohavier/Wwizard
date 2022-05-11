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
#include "ImNodesEz.h"

struct Connection
{
    /// `id` that was passed to BeginNode() of input node.
    void* InputNode = nullptr;
    /// Descriptor of input slot.
    const char* InputSlot = nullptr;
    /// `id` that was passed to BeginNode() of output node.
    void* OutputNode = nullptr;
    /// Descriptor of output slot.
    const char* OutputSlot = nullptr;

    bool operator==(const Connection& other) const
    {
        return InputNode == other.InputNode &&
            InputSlot == other.InputSlot &&
            OutputNode == other.OutputNode &&
            OutputSlot == other.OutputSlot;
    }

    bool operator!=(const Connection& other) const
    {
        return !operator ==(other);
    }
};

enum NodeSlotTypes
{
    NodeSlotPosition = 1,   // ID can not be 0
    NodeSlotRotation,
    QueryResults,
};

/// A structure holding node state.
struct MyNode
{
    /// Title which will be displayed at the center-top of the node.
    const char* Title = nullptr;
    /// Flag indicating that node is selected by the user.
    bool Selected = false;
    /// Node position on the canvas.
    ImVec2 Pos{};
    /// List of node connections.
    std::vector<Connection> Connections{};
    /// A list of input slots current node has.
    std::vector<ImNodes::Ez::SlotInfo> InputSlots{};
    /// A list of output slots current node has.
    std::vector<ImNodes::Ez::SlotInfo> OutputSlots{};

    std::string nodeContent = "";

    MyNode() = default;

    MyNode(const char* title,
        const std::vector<ImNodes::Ez::SlotInfo>&& input_slots,
        const std::vector<ImNodes::Ez::SlotInfo>&& output_slots)
    {
        Title = title;
        InputSlots = input_slots;
        OutputSlots = output_slots;
        nodeContent = title;
    }

    /// Deletes connection from this node.
    void DeleteConnection(const Connection& connection)
    {
        for (auto it = Connections.begin(); it != Connections.end(); ++it)
        {
            if (connection == *it)
            {
                Connections.erase(it);
                break;
            }
        }
    }
};

struct MyQueryNode : public MyNode
{
    MyQueryNode(const char* title,
        const std::vector<ImNodes::Ez::SlotInfo>&& input_slots,
        const std::vector<ImNodes::Ez::SlotInfo>&& output_slots,
        std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient)
        :wwizardWwiseClient(wwizardWwiseClient)
    {
        MyNode::Title = title;
        MyNode::InputSlots = input_slots;
        MyNode::OutputSlots = output_slots;
    }

    void SetQueryDetails(std::string name, std::string guid)
    {
        queryName = name;
        queryGuid = guid;
        MyNode::nodeContent = name;
    }

    void RerunQuery()
    {
        queryResults.clear();
        AkJson results = wwizardWwiseClient->RunQueryFromGuuid(queryGuid);
        for (const auto& result : results["return"].GetArray())
        {

        }
    }

    std::string queryName = "";
    std::string queryGuid = "";
    std::vector<QueryResultFile> queryResults;
    std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient;
    std::map<std::string, QueryResultFile> queryResults;
};


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
	void ShowDetails();
	void ShowQueryNodeEditor();

public:
    
    std::map<std::string, std::function<MyNode* ()>> available_nodes{
    {"Query", [this]() -> MyNode* {  return new MyQueryNode("Query", {
    }, 
    {
        {"Results", QueryResults}                                      // Output slots
    }, this->wwizardWwiseClient); }},

    {"And", []() -> MyNode* { return new MyNode("And", {
        {"Results 1", QueryResults},
        {"Results 2", QueryResults}// Input slots
    }, {
        {"Results", QueryResults}// Output slots
    }); }},

    {"Or", []() -> MyNode* { return new MyNode("Or", {
        {"Results 1", QueryResults},
        {"Results 2", QueryResults}  // Input slots
    }, {
        {"Results", QueryResults}// Output slots
    }); }},

    };
    std::vector<MyNode*> nodes;

private:
	bool useQueryNodeEditor = true;
	const std::unique_ptr<QueryEditorModule>& queryEditorModule;
};