#include "CodexBlueprintBridgeService.h"

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
        Result.Errors.Add({TEXT("UnknownOperation"), FString::Printf(TEXT("Unknown operation: %s"), *Operation), TEXT("operation")});
    }

    return Result;
}
