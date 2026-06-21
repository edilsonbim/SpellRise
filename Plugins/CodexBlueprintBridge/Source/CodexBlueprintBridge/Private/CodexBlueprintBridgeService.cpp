#include "CodexBlueprintBridgeService.h"

namespace
{
bool CompleteDryRun(
    const FCodexBridgeRequestPolicy& RequestPolicy,
    const ECodexBridgeMutationKind MutationKind,
    FCodexBridgeResult& Result)
{
    if (!RequestPolicy.bDryRun)
    {
        return false;
    }

    Result.bSuccess = true;
    Result.Data->SetBoolField(TEXT("dryRun"), true);
    Result.Data->SetStringField(
        TEXT("mutationKind"),
        MutationKind == ECodexBridgeMutationKind::Graph
            ? TEXT("Graph")
            : MutationKind == ECodexBridgeMutationKind::Widget
                ? TEXT("Widget")
                : TEXT("Binding"));
    Result.Data->SetNumberField(TEXT("mutationCount"), RequestPolicy.MutationCount);
    Result.Data->SetNumberField(TEXT("payloadBytes"), RequestPolicy.PayloadBytes);
    return true;
}
}

FCodexBridgeResult FCodexBlueprintBridgeService::Execute(const TSharedRef<FJsonObject>& Request)
{
    FCodexBridgeResult Result;
    Result.Data = MakeShared<FJsonObject>();

    if (!Policy.ValidateBaseRequest(Request, Result))
    {
        return Result;
    }

    const FString Operation = Result.Operation;

    if (Operation == TEXT("ListBlueprints"))
    {
        Result.bSuccess = BlueprintOps.ListBlueprints(Request, Result);
        return Result;
    }

    FString AssetPath;
    Request->TryGetStringField(TEXT("asset"), AssetPath);
    if (!Policy.ValidateAssetPath(AssetPath, Result))
    {
        return Result;
    }

    if (Operation == TEXT("DescribeBlueprint"))
    {
        Result.bSuccess = BlueprintOps.DescribeBlueprint(Request, Result);
    }
    else if (Operation == TEXT("DescribeGraphs"))
    {
        Result.bSuccess = GraphOps.DescribeGraphs(Request, Result);
    }
    else if (Operation == TEXT("DescribeGraphNodes"))
    {
        Result.bSuccess = GraphOps.DescribeGraphNodes(Request, Result);
    }
    else if (Operation == TEXT("DescribeWidgetTree"))
    {
        Result.bSuccess = WidgetOps.DescribeWidgetTree(Request, Result);
    }
    else if (Operation == TEXT("DescribeBindings"))
    {
        Result.bSuccess = BindingOps.DescribeBindings(Request, Result);
    }
    else if (Operation == TEXT("AddVariable"))
    {
        if (Policy.AllowsReplicatedVariable(Request, Result))
        {
            Result.bSuccess = BlueprintOps.AddVariable(Request, Result);
        }
    }
    else if (Operation == TEXT("SetVariableDefault"))
    {
        Result.bSuccess = BlueprintOps.SetVariableDefault(Request, Result);
    }
    else if (Operation == TEXT("CompileBlueprint"))
    {
        Result.bSuccess = BlueprintOps.CompileBlueprint(Request, Result);
    }
    else if (Operation == TEXT("SaveBlueprint"))
    {
        Result.bSuccess = BlueprintOps.SaveBlueprint(Request, Result);
    }
    else
    {
        ECodexBridgeMutationKind MutationKind = ECodexBridgeMutationKind::None;

        if (Operation == TEXT("AddNode")
            || Operation == TEXT("RemoveNode")
            || Operation == TEXT("ConnectPins")
            || Operation == TEXT("DisconnectPins"))
        {
            MutationKind = ECodexBridgeMutationKind::Graph;
        }
        else if (Operation == TEXT("AddWidget")
            || Operation == TEXT("RemoveWidget")
            || Operation == TEXT("RenameWidget")
            || Operation == TEXT("SetWidgetProperty"))
        {
            MutationKind = ECodexBridgeMutationKind::Widget;
        }
        else if (Operation == TEXT("BindWidgetEvent")
            || Operation == TEXT("UnbindWidgetEvent")
            || Operation == TEXT("GetOrCreateFunctionGraph")
            || Operation == TEXT("SetOnMouseButtonDownReply"))
        {
            MutationKind = ECodexBridgeMutationKind::Binding;
        }

        if (MutationKind == ECodexBridgeMutationKind::None)
        {
            Result.Errors.Add({
                TEXT("UnknownOperation"),
                FString::Printf(TEXT("Unknown operation: %s"), *Operation),
                TEXT("operation")
            });
            return Result;
        }

        FCodexBridgeRequestPolicy RequestPolicy;
        if (!Policy.ValidateMutationRequest(Request, MutationKind, Result, &RequestPolicy)
            || CompleteDryRun(RequestPolicy, MutationKind, Result))
        {
            return Result;
        }

        if (Operation == TEXT("AddNode"))
        {
            Result.bSuccess = GraphOps.AddNode(Request, Result);
        }
        else if (Operation == TEXT("RemoveNode"))
        {
            Result.bSuccess = GraphOps.RemoveNode(Request, Result);
        }
        else if (Operation == TEXT("ConnectPins"))
        {
            Result.bSuccess = GraphOps.ConnectPins(Request, Result);
        }
        else if (Operation == TEXT("DisconnectPins"))
        {
            Result.bSuccess = GraphOps.DisconnectPins(Request, Result);
        }
        else if (Operation == TEXT("AddWidget"))
        {
            Result.bSuccess = WidgetOps.AddWidget(Request, Result);
        }
        else if (Operation == TEXT("RemoveWidget"))
        {
            Result.bSuccess = WidgetOps.RemoveWidget(Request, Result);
        }
        else if (Operation == TEXT("RenameWidget"))
        {
            Result.bSuccess = WidgetOps.RenameWidget(Request, Result);
        }
        else if (Operation == TEXT("SetWidgetProperty"))
        {
            Result.bSuccess = WidgetOps.SetWidgetProperty(Request, Result);
        }
        else if (Operation == TEXT("BindWidgetEvent"))
        {
            Result.bSuccess = BindingOps.BindWidgetEvent(Request, Result);
        }
        else if (Operation == TEXT("UnbindWidgetEvent"))
        {
            Result.bSuccess = BindingOps.UnbindWidgetEvent(Request, Result);
        }
        else if (Operation == TEXT("GetOrCreateFunctionGraph"))
        {
            Result.bSuccess = BindingOps.GetOrCreateFunctionGraph(Request, Result);
        }
        else if (Operation == TEXT("SetOnMouseButtonDownReply"))
        {
            Result.bSuccess = BindingOps.SetOnMouseButtonDownReply(Request, Result);
        }
    }

    return Result;
}
