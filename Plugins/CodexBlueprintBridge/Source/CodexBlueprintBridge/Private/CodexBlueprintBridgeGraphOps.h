#pragma once

#include "CodexBlueprintBridgeTypes.h"

class UBlueprint;
class UEdGraph;
class UEdGraphNode;
class UEdGraphPin;

class FCodexBlueprintBridgeGraphOps
{
public:
    bool DescribeGraphs(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool DescribeGraphNodes(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool AddNode(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool RemoveNode(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool ConnectPins(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool DisconnectPins(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;

private:
    static UBlueprint* LoadBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result);
    static UEdGraph* FindGraph(UBlueprint* Blueprint, const FString& GraphName, FCodexBridgeResult& Result);
    static UEdGraphNode* FindNode(UEdGraph* Graph, const FString& NodeId, FCodexBridgeResult& Result, const TCHAR* Field);
    static UEdGraphPin* FindPin(
        UEdGraphNode* Node,
        const FString& PinName,
        const FString& Direction,
        FCodexBridgeResult& Result,
        const TCHAR* Field);
    static FString GetNodeId(const UEdGraphNode* Node);
    static FString PinDirectionToString(const UEdGraphPin* Pin);
    static TSharedRef<FJsonObject> DescribePin(const UEdGraphPin* Pin);
    static TSharedRef<FJsonObject> DescribeNode(const UEdGraphNode* Node);
    static void MarkModified(UBlueprint* Blueprint);
};
