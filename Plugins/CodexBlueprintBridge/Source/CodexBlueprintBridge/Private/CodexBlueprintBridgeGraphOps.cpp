#include "CodexBlueprintBridgeGraphOps.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphSchema_K2_Actions.h"
#include "Engine/Blueprint.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CustomEvent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "UObject/Class.h"

namespace
{
bool ReadRequiredString(
    const TSharedRef<FJsonObject>& Request,
    const TCHAR* Field,
    FString& OutValue,
    FCodexBridgeResult& Result)
{
    if (!Request->TryGetStringField(Field, OutValue) || OutValue.IsEmpty())
    {
        Result.Errors.Add({
            TEXT("MissingField"),
            FString::Printf(TEXT("Request must include a non-empty '%s' string."), Field),
            Field});
        return false;
    }

    return true;
}

bool IsValidIdentifier(const FString& Value)
{
    if (Value.IsEmpty() || (!FChar::IsAlpha(Value[0]) && Value[0] != TEXT('_')))
    {
        return false;
    }

    for (const TCHAR Character : Value)
    {
        if (!FChar::IsAlnum(Character) && Character != TEXT('_'))
        {
            return false;
        }
    }

    return true;
}

void GatherBlueprintGraphs(UBlueprint* Blueprint, TArray<UEdGraph*>& OutGraphs)
{
    if (Blueprint)
    {
        Blueprint->GetAllGraphs(OutGraphs);
    }
}
}

bool FCodexBlueprintBridgeGraphOps::DescribeGraphs(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    TArray<UEdGraph*> Graphs;
    GatherBlueprintGraphs(Blueprint, Graphs);

    TArray<TSharedPtr<FJsonValue>> GraphValues;
    for (const UEdGraph* Graph : Graphs)
    {
        if (!Graph)
        {
            continue;
        }

        const TSharedRef<FJsonObject> GraphObject = MakeShared<FJsonObject>();
        GraphObject->SetStringField(TEXT("name"), Graph->GetName());
        GraphObject->SetStringField(TEXT("schema"), Graph->GetSchema() ? Graph->GetSchema()->GetClass()->GetPathName() : TEXT(""));
        GraphObject->SetNumberField(TEXT("nodeCount"), Graph->Nodes.Num());
        GraphObject->SetBoolField(TEXT("editable"), Graph->bEditable);
        GraphValues.Add(MakeShared<FJsonValueObject>(GraphObject));
    }

    FString AssetPath;
    Request->TryGetStringField(TEXT("asset"), AssetPath);
    Result.Data->SetStringField(TEXT("asset"), AssetPath);
    Result.Data->SetArrayField(TEXT("graphs"), GraphValues);
    return true;
}

bool FCodexBlueprintBridgeGraphOps::DescribeGraphNodes(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString GraphName;
    if (!ReadRequiredString(Request, TEXT("graph"), GraphName, Result))
    {
        return false;
    }

    UEdGraph* Graph = FindGraph(Blueprint, GraphName, Result);
    if (!Graph)
    {
        return false;
    }

    TArray<TSharedPtr<FJsonValue>> NodeValues;
    for (const UEdGraphNode* Node : Graph->Nodes)
    {
        if (Node)
        {
            NodeValues.Add(MakeShared<FJsonValueObject>(DescribeNode(Node)));
        }
    }

    Result.Data->SetStringField(TEXT("graph"), Graph->GetName());
    Result.Data->SetArrayField(TEXT("nodes"), NodeValues);
    return true;
}

bool FCodexBlueprintBridgeGraphOps::AddNode(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString GraphName;
    FString NodeType;
    if (!ReadRequiredString(Request, TEXT("graph"), GraphName, Result)
        || !ReadRequiredString(Request, TEXT("nodeType"), NodeType, Result))
    {
        return false;
    }

    UEdGraph* Graph = FindGraph(Blueprint, GraphName, Result);
    if (!Graph)
    {
        return false;
    }

    if (!Graph->bEditable || !Graph->GetSchema() || !Graph->GetSchema()->IsA<UEdGraphSchema_K2>())
    {
        Result.Errors.Add({TEXT("GraphNotEditable"), TEXT("Target must be an editable K2 graph."), TEXT("graph")});
        return false;
    }

    double PositionX = 0.0;
    double PositionY = 0.0;
    Request->TryGetNumberField(TEXT("x"), PositionX);
    Request->TryGetNumberField(TEXT("y"), PositionY);

    UEdGraphNode* NewNode = nullptr;
    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "AddGraphNode", "Add Blueprint Graph Node"));
    Blueprint->Modify();
    Graph->Modify();

    if (NodeType == TEXT("K2Node_CustomEvent"))
    {
        FString EventName;
        if (!ReadRequiredString(Request, TEXT("eventName"), EventName, Result))
        {
            return false;
        }
        if (!IsValidIdentifier(EventName))
        {
            Result.Errors.Add({TEXT("InvalidEventName"), TEXT("eventName must be a valid identifier."), TEXT("eventName")});
            return false;
        }
        if (FBlueprintEditorUtils::FindCustomEventNode(Blueprint, FName(*EventName)))
        {
            Result.Errors.Add({TEXT("EventAlreadyExists"), TEXT("A custom event with this name already exists."), TEXT("eventName")});
            return false;
        }

        NewNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_CustomEvent>(
            Graph,
            FVector2D(PositionX, PositionY),
            EK2NewNodeFlags::None,
            [&EventName](UK2Node_CustomEvent* EventNode)
            {
                EventNode->CustomFunctionName = FName(*EventName);
            });
    }
    else if (NodeType == TEXT("K2Node_CallFunction"))
    {
        FString FunctionPath;
        if (!ReadRequiredString(Request, TEXT("function"), FunctionPath, Result))
        {
            return false;
        }

        UFunction* Function = FindObject<UFunction>(nullptr, *FunctionPath);
        if (!Function)
        {
            Function = LoadObject<UFunction>(nullptr, *FunctionPath);
        }
        if (!Function || Function->HasAnyFunctionFlags(FUNC_Delegate))
        {
            Result.Errors.Add({
                TEXT("FunctionNotFound"),
                TEXT("function must be a valid non-delegate UFunction object path."),
                TEXT("function")});
            return false;
        }
        if (!Function->HasAnyFunctionFlags(FUNC_BlueprintCallable | FUNC_BlueprintPure)
            || Function->HasAnyFunctionFlags(FUNC_EditorOnly))
        {
            Result.Errors.Add({
                TEXT("FunctionNotAllowed"),
                TEXT("Only non-editor BlueprintCallable or BlueprintPure functions are allowed."),
                TEXT("function")});
            return false;
        }

        NewNode = FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_CallFunction>(
            Graph,
            FVector2D(PositionX, PositionY),
            EK2NewNodeFlags::None,
            [Function](UK2Node_CallFunction* CallNode)
            {
                CallNode->SetFromFunction(Function);
            });
    }
    else
    {
        Result.Errors.Add({
            TEXT("NodeTypeNotAllowed"),
            TEXT("nodeType whitelist only allows K2Node_CustomEvent and K2Node_CallFunction."),
            TEXT("nodeType")});
        return false;
    }

    if (!NewNode)
    {
        Result.Errors.Add({
            TEXT("NodeCreationFailed"),
            TEXT("K2 node creation failed."),
            TEXT("nodeType")
        });
        return false;
    }

    NewNode->NodePosX = FMath::Clamp(FMath::RoundToInt(PositionX), -1000000, 1000000);
    NewNode->NodePosY = FMath::Clamp(FMath::RoundToInt(PositionY), -1000000, 1000000);

    MarkModified(Blueprint);
    Result.Data->SetStringField(TEXT("graph"), Graph->GetName());
    Result.Data->SetObjectField(TEXT("node"), DescribeNode(NewNode));
    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved."));
    return true;
}

bool FCodexBlueprintBridgeGraphOps::RemoveNode(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString GraphName;
    FString NodeId;
    if (!ReadRequiredString(Request, TEXT("graph"), GraphName, Result)
        || !ReadRequiredString(Request, TEXT("nodeId"), NodeId, Result))
    {
        return false;
    }

    UEdGraph* Graph = FindGraph(Blueprint, GraphName, Result);
    UEdGraphNode* Node = Graph ? FindNode(Graph, NodeId, Result, TEXT("nodeId")) : nullptr;
    if (!Node)
    {
        return false;
    }
    if (!Node->CanUserDeleteNode())
    {
        Result.Errors.Add({TEXT("NodeNotDeletable"), TEXT("The target node cannot be deleted by users."), TEXT("nodeId")});
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "RemoveGraphNode", "Remove Blueprint Graph Node"));
    Blueprint->Modify();
    Graph->Modify();
    Node->Modify();
    const FString RemovedId = GetNodeId(Node);
    Node->DestroyNode();
    MarkModified(Blueprint);

    Result.Data->SetStringField(TEXT("graph"), Graph->GetName());
    Result.Data->SetStringField(TEXT("removedNodeId"), RemovedId);
    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved."));
    return true;
}

bool FCodexBlueprintBridgeGraphOps::ConnectPins(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString GraphName;
    FString FromNodeId;
    FString FromPinName;
    FString ToNodeId;
    FString ToPinName;
    if (!ReadRequiredString(Request, TEXT("graph"), GraphName, Result)
        || !ReadRequiredString(Request, TEXT("fromNodeId"), FromNodeId, Result)
        || !ReadRequiredString(Request, TEXT("fromPin"), FromPinName, Result)
        || !ReadRequiredString(Request, TEXT("toNodeId"), ToNodeId, Result)
        || !ReadRequiredString(Request, TEXT("toPin"), ToPinName, Result))
    {
        return false;
    }

    UEdGraph* Graph = FindGraph(Blueprint, GraphName, Result);
    UEdGraphNode* FromNode = Graph ? FindNode(Graph, FromNodeId, Result, TEXT("fromNodeId")) : nullptr;
    UEdGraphNode* ToNode = Graph ? FindNode(Graph, ToNodeId, Result, TEXT("toNodeId")) : nullptr;
    if (!FromNode || !ToNode)
    {
        return false;
    }

    UEdGraphPin* FromPin = FindPin(FromNode, FromPinName, TEXT("output"), Result, TEXT("fromPin"));
    UEdGraphPin* ToPin = FindPin(ToNode, ToPinName, TEXT("input"), Result, TEXT("toPin"));
    const UEdGraphSchema* Schema = Graph->GetSchema();
    if (!FromPin || !ToPin || !Schema)
    {
        return false;
    }
    if (FromPin->LinkedTo.Contains(ToPin))
    {
        Result.Errors.Add({TEXT("PinsAlreadyConnected"), TEXT("The requested pins are already connected."), TEXT("fromPin")});
        return false;
    }
    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "ConnectGraphPins", "Connect Blueprint Graph Pins"));
    Blueprint->Modify();
    Graph->Modify();
    FromPin->Modify();
    ToPin->Modify();
    if (!Schema->TryCreateConnection(FromPin, ToPin))
    {
        Result.Errors.Add({TEXT("ConnectionRejected"), TEXT("The graph schema rejected this pin connection."), TEXT("toPin")});
        return false;
    }

    MarkModified(Blueprint);
    Result.Data->SetStringField(TEXT("graph"), Graph->GetName());
    Result.Data->SetStringField(TEXT("fromNodeId"), GetNodeId(FromNode));
    Result.Data->SetStringField(TEXT("fromPin"), FromPin->PinName.ToString());
    Result.Data->SetStringField(TEXT("toNodeId"), GetNodeId(ToNode));
    Result.Data->SetStringField(TEXT("toPin"), ToPin->PinName.ToString());
    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved."));
    return true;
}

bool FCodexBlueprintBridgeGraphOps::DisconnectPins(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString GraphName;
    FString FromNodeId;
    FString FromPinName;
    FString ToNodeId;
    FString ToPinName;
    if (!ReadRequiredString(Request, TEXT("graph"), GraphName, Result)
        || !ReadRequiredString(Request, TEXT("fromNodeId"), FromNodeId, Result)
        || !ReadRequiredString(Request, TEXT("fromPin"), FromPinName, Result)
        || !ReadRequiredString(Request, TEXT("toNodeId"), ToNodeId, Result)
        || !ReadRequiredString(Request, TEXT("toPin"), ToPinName, Result))
    {
        return false;
    }

    UEdGraph* Graph = FindGraph(Blueprint, GraphName, Result);
    UEdGraphNode* FromNode = Graph ? FindNode(Graph, FromNodeId, Result, TEXT("fromNodeId")) : nullptr;
    UEdGraphNode* ToNode = Graph ? FindNode(Graph, ToNodeId, Result, TEXT("toNodeId")) : nullptr;
    if (!FromNode || !ToNode)
    {
        return false;
    }

    UEdGraphPin* FromPin = FindPin(FromNode, FromPinName, TEXT("output"), Result, TEXT("fromPin"));
    UEdGraphPin* ToPin = FindPin(ToNode, ToPinName, TEXT("input"), Result, TEXT("toPin"));
    const UEdGraphSchema* Schema = Graph->GetSchema();
    if (!FromPin || !ToPin || !Schema)
    {
        return false;
    }
    if (!FromPin->LinkedTo.Contains(ToPin))
    {
        Result.Errors.Add({TEXT("PinsNotConnected"), TEXT("The requested pins are not connected."), TEXT("fromPin")});
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "DisconnectGraphPins", "Disconnect Blueprint Graph Pins"));
    Blueprint->Modify();
    Graph->Modify();
    FromPin->Modify();
    ToPin->Modify();
    Schema->BreakSinglePinLink(FromPin, ToPin);
    MarkModified(Blueprint);

    Result.Data->SetStringField(TEXT("graph"), Graph->GetName());
    Result.Data->SetStringField(TEXT("fromNodeId"), GetNodeId(FromNode));
    Result.Data->SetStringField(TEXT("fromPin"), FromPin->PinName.ToString());
    Result.Data->SetStringField(TEXT("toNodeId"), GetNodeId(ToNode));
    Result.Data->SetStringField(TEXT("toPin"), ToPin->PinName.ToString());
    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved."));
    return true;
}

UBlueprint* FCodexBlueprintBridgeGraphOps::LoadBlueprint(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result)
{
    FString AssetPath;
    if (!ReadRequiredString(Request, TEXT("asset"), AssetPath, Result))
    {
        return nullptr;
    }
    if (!AssetPath.StartsWith(TEXT("/Game/")) || AssetPath.Contains(TEXT(".")))
    {
        Result.Errors.Add({
            TEXT("InvalidAssetPath"),
            TEXT("asset must be a long package path under /Game without an object suffix."),
            TEXT("asset")});
        return nullptr;
    }

    UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *AssetPath);
    if (!Blueprint)
    {
        Result.Errors.Add({TEXT("AssetNotFound"), TEXT("Blueprint asset was not found."), TEXT("asset")});
    }
    return Blueprint;
}

UEdGraph* FCodexBlueprintBridgeGraphOps::FindGraph(
    UBlueprint* Blueprint,
    const FString& GraphName,
    FCodexBridgeResult& Result)
{
    TArray<UEdGraph*> Graphs;
    GatherBlueprintGraphs(Blueprint, Graphs);

    UEdGraph* Match = nullptr;
    for (UEdGraph* Graph : Graphs)
    {
        if (Graph && Graph->GetName().Equals(GraphName, ESearchCase::CaseSensitive))
        {
            if (Match)
            {
                Result.Errors.Add({
                    TEXT("AmbiguousGraph"),
                    TEXT("Multiple graphs have the requested name; operation refused."),
                    TEXT("graph")});
                return nullptr;
            }
            Match = Graph;
        }
    }

    if (!Match)
    {
        Result.Errors.Add({TEXT("GraphNotFound"), TEXT("Graph was not found in the Blueprint."), TEXT("graph")});
    }
    return Match;
}

UEdGraphNode* FCodexBlueprintBridgeGraphOps::FindNode(
    UEdGraph* Graph,
    const FString& NodeId,
    FCodexBridgeResult& Result,
    const TCHAR* Field)
{
    FGuid Guid;
    if (!FGuid::Parse(NodeId, Guid))
    {
        Result.Errors.Add({TEXT("InvalidNodeId"), TEXT("Node id must be a valid GUID."), Field});
        return nullptr;
    }

    for (UEdGraphNode* Node : Graph->Nodes)
    {
        if (Node && Node->NodeGuid == Guid)
        {
            return Node;
        }
    }

    Result.Errors.Add({TEXT("NodeNotFound"), TEXT("Node was not found in the requested graph."), Field});
    return nullptr;
}

UEdGraphPin* FCodexBlueprintBridgeGraphOps::FindPin(
    UEdGraphNode* Node,
    const FString& PinName,
    const FString& Direction,
    FCodexBridgeResult& Result,
    const TCHAR* Field)
{
    const EEdGraphPinDirection ExpectedDirection =
        Direction == TEXT("output") ? EGPD_Output : EGPD_Input;

    UEdGraphPin* Match = nullptr;
    for (UEdGraphPin* Pin : Node->Pins)
    {
        if (Pin
            && Pin->PinName.ToString().Equals(PinName, ESearchCase::CaseSensitive)
            && Pin->Direction == ExpectedDirection)
        {
            if (Match)
            {
                Result.Errors.Add({
                    TEXT("AmbiguousPin"),
                    TEXT("Multiple pins match the requested name and direction."),
                    Field});
                return nullptr;
            }
            Match = Pin;
        }
    }

    if (!Match)
    {
        Result.Errors.Add({
            TEXT("PinNotFound"),
            FString::Printf(TEXT("No %s pin with the requested name was found."), *Direction),
            Field});
    }
    return Match;
}

FString FCodexBlueprintBridgeGraphOps::GetNodeId(const UEdGraphNode* Node)
{
    return Node ? Node->NodeGuid.ToString(EGuidFormats::DigitsWithHyphens) : TEXT("");
}

FString FCodexBlueprintBridgeGraphOps::PinDirectionToString(const UEdGraphPin* Pin)
{
    return Pin && Pin->Direction == EGPD_Output ? TEXT("output") : TEXT("input");
}

TSharedRef<FJsonObject> FCodexBlueprintBridgeGraphOps::DescribePin(const UEdGraphPin* Pin)
{
    const TSharedRef<FJsonObject> PinObject = MakeShared<FJsonObject>();
    PinObject->SetStringField(TEXT("name"), Pin ? Pin->PinName.ToString() : TEXT(""));
    PinObject->SetStringField(TEXT("direction"), PinDirectionToString(Pin));
    PinObject->SetStringField(TEXT("category"), Pin ? Pin->PinType.PinCategory.ToString() : TEXT(""));
    PinObject->SetStringField(TEXT("subCategory"), Pin ? Pin->PinType.PinSubCategory.ToString() : TEXT(""));
    PinObject->SetStringField(
        TEXT("subCategoryObject"),
        Pin && Pin->PinType.PinSubCategoryObject.Get()
            ? Pin->PinType.PinSubCategoryObject.Get()->GetPathName()
            : TEXT(""));
    PinObject->SetStringField(TEXT("defaultValue"), Pin ? Pin->DefaultValue : TEXT(""));

    TArray<TSharedPtr<FJsonValue>> Links;
    if (Pin)
    {
        for (const UEdGraphPin* LinkedPin : Pin->LinkedTo)
        {
            if (!LinkedPin || !LinkedPin->GetOwningNode())
            {
                continue;
            }

            const TSharedRef<FJsonObject> LinkObject = MakeShared<FJsonObject>();
            LinkObject->SetStringField(TEXT("nodeId"), GetNodeId(LinkedPin->GetOwningNode()));
            LinkObject->SetStringField(TEXT("pin"), LinkedPin->PinName.ToString());
            Links.Add(MakeShared<FJsonValueObject>(LinkObject));
        }
    }
    PinObject->SetArrayField(TEXT("links"), Links);
    return PinObject;
}

TSharedRef<FJsonObject> FCodexBlueprintBridgeGraphOps::DescribeNode(const UEdGraphNode* Node)
{
    const TSharedRef<FJsonObject> NodeObject = MakeShared<FJsonObject>();
    NodeObject->SetStringField(TEXT("id"), GetNodeId(Node));
    NodeObject->SetStringField(TEXT("class"), Node ? Node->GetClass()->GetPathName() : TEXT(""));
    NodeObject->SetStringField(TEXT("title"), Node ? Node->GetNodeTitle(ENodeTitleType::ListView).ToString() : TEXT(""));
    NodeObject->SetNumberField(TEXT("x"), Node ? Node->NodePosX : 0);
    NodeObject->SetNumberField(TEXT("y"), Node ? Node->NodePosY : 0);
    NodeObject->SetBoolField(TEXT("canDelete"), Node && Node->CanUserDeleteNode());

    TArray<TSharedPtr<FJsonValue>> Pins;
    if (Node)
    {
        for (const UEdGraphPin* Pin : Node->Pins)
        {
            if (Pin)
            {
                Pins.Add(MakeShared<FJsonValueObject>(DescribePin(Pin)));
            }
        }
    }
    NodeObject->SetArrayField(TEXT("pins"), Pins);
    return NodeObject;
}

void FCodexBlueprintBridgeGraphOps::MarkModified(UBlueprint* Blueprint)
{
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}
