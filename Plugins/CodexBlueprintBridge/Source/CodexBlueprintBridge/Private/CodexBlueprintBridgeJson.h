#pragma once

#include "CodexBlueprintBridgeTypes.h"

class FCodexBlueprintBridgeJson
{
public:
    static bool LoadJsonFile(const FString& Path, TSharedPtr<FJsonObject>& OutJson, FString& OutError);
    static bool SaveResult(const FString& Path, const FCodexBridgeResult& Result, FString* OutError = nullptr);
};
