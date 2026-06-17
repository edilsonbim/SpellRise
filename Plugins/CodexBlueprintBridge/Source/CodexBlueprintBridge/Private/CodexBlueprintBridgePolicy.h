#pragma once

#include "CodexBlueprintBridgeTypes.h"

class FCodexBlueprintBridgePolicy
{
public:
    bool ValidateBaseRequest(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool ValidateAssetPath(const FString& AssetPath, FCodexBridgeResult& Result) const;
    bool AllowsReplicatedVariable(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
};
