#pragma once

#include "CodexBlueprintBridgeTypes.h"
#include "EdGraph/EdGraphPin.h"

class UBlueprint;

class FCodexBlueprintBridgeBlueprintOps
{
public:
    bool ListBlueprints(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool DescribeBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool AddVariable(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool SetVariableDefault(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool CompileBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool SaveBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;

private:
    static UBlueprint* LoadBlueprint(const FString& AssetPath, FCodexBridgeResult& Result);
    static bool BuildPinType(const FString& TypeName, FEdGraphPinType& OutPinType, FCodexBridgeResult& Result);
    static bool IsValidVariableName(const FString& Name);
    static FString GetAssetPath(const TSharedRef<FJsonObject>& Request);
};
