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
#include "helper.h"

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
        const std::vector<ImNodes::Ez::SlotInfo>&& output_slots,
        const std::string nodeGuid)
        :nodeGuid(nodeGuid)
        ,Title(title)
        ,InputSlots(input_slots)
        ,OutputSlots(output_slots)
    {
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

    std::string nodeGuid;
};

struct MyQueryNode : public MyNode
{
    MyQueryNode(const char* title,
        const std::vector<ImNodes::Ez::SlotInfo>&& input_slots,
        const std::vector<ImNodes::Ez::SlotInfo>&& output_slots,
        std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient,
        const std::string nodeGuid)
        :wwizardWwiseClient(wwizardWwiseClient)
    {
        MyNode::Title = title;
        MyNode::InputSlots = input_slots;
        MyNode::OutputSlots = output_slots;
        MyNode::nodeGuid = nodeGuid;
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
        if (results.HasKey("return"))
        {
            for (const auto& result : results["return"].GetArray())
            {
                queryResults.emplace(result["id"].GetVariant().GetString(), QueryResultFile(result["name"].GetVariant().GetString(), result["id"].GetVariant().GetString(), result["path"].GetVariant().GetString(), result["type"].GetVariant().GetString(), 0));
            }
        }
    }

    std::string queryName = "";
    std::string queryGuid = "";
    std::unique_ptr<WwizardWwiseClient>& wwizardWwiseClient;
    std::map<std::string, QueryResultFile> queryResults;
};

struct MyOutputNode : public MyNode
{

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

    void RecalculateNodeGraph();
    std::map<std::string, QueryResultFile> CalculateNextNode(MyNode* node);

public:
    std::map<std::string, std::function<MyNode* ()>> available_nodes{
    {"Query", [this]() -> MyNode* {  return new MyQueryNode("Query", {
    }, 
    {
        {"Results", QueryResults}                                      
    }, this->wwizardWwiseClient, GenerateGuid()); }},

    {"And", []() -> MyNode* { return new MyNode("And", {
        {"Results 1", QueryResults}
    }, {
        {"Results", QueryResults}// Output slots
    }, GenerateGuid()); }},

    {"Or", []() -> MyNode* { return new MyNode("Or", {
        {"Results 1", QueryResults}
    }, {
        {"Results", QueryResults}// Output slots
    }, GenerateGuid()); }},
    
    {"Output", []() -> MyNode* { return new MyNode("Output", {{"Results", QueryResults}}, {}, GenerateGuid()); }},

    };
    std::map<std::string, MyNode*> nodes;

private:
	bool useQueryNodeEditor = true;
	const std::unique_ptr<QueryEditorModule>& queryEditorModule;
    MyNode* outputNode;
};