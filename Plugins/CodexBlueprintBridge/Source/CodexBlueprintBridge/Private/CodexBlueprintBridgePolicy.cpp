#include "CodexBlueprintBridgePolicy.h"

bool FCodexBlueprintBridgePolicy::ValidateBaseRequest(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    int32 Version = 0;
    if (!Request->TryGetNumberField(TEXT("version"), Version) || Version != 1)
    {
        Result.Errors.Add({TEXT("UnsupportedVersion"), TEXT("Only request version 1 is supported."), TEXT("version")});
        return false;
    }

    FString Operation;
    if (!Request->TryGetStringField(TEXT("operation"), Operation) || Operation.IsEmpty())
    {
        Result.Errors.Add({TEXT("MissingOperation"), TEXT("Request must include operation."), TEXT("operation")});
        return false;
    }

    Result.Operation = Operation;
    return true;
}

bool FCodexBlueprintBridgePolicy::ValidateAssetPath(const FString& AssetPath, FCodexBridgeResult& Result) const
{
    if (AssetPath.IsEmpty())
    {
        Result.Errors.Add({TEXT("MissingAsset"), TEXT("Request must include asset."), TEXT("asset")});
        return false;
    }

    if (!AssetPath.StartsWith(TEXT("/Game/")) || AssetPath.Contains(TEXT("..")) || AssetPath.Contains(TEXT("\\")))
    {
        Result.Errors.Add({TEXT("InvalidAssetPath"), TEXT("Asset path must be a package path under /Game."), TEXT("asset")});
        return false;
    }

    return true;
}

bool FCodexBlueprintBridgePolicy::AllowsReplicatedVariable(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    bool bReplicated = false;
    Request->TryGetBoolField(TEXT("replicated"), bReplicated);
    if (!bReplicated)
    {
        return true;
    }

    Result.Errors.Add({
        TEXT("ReplicatedVariableBlocked"),
        TEXT("Adding replicated Blueprint variables is blocked in the MVP until replication flags, lifetime conditions, OnRep policy and network review output are implemented."),
        TEXT("replicated")
    });
    return false;
}
