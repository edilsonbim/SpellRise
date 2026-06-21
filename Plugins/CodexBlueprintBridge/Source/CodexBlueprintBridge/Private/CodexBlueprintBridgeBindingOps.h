#pragma once

#include "CodexBlueprintBridgeTypes.h"

class UEdGraph;
class UWidgetBlueprint;

class FCodexBlueprintBridgeBindingOps
{
public:
    bool DescribeBindings(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool BindWidgetEvent(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool UnbindWidgetEvent(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool GetOrCreateFunctionGraph(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool SetOnMouseButtonDownReply(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;

private:
    static UWidgetBlueprint* LoadWidgetBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result);
    static UEdGraph* FindFunctionGraph(const UWidgetBlueprint* Blueprint, FName FunctionName);
    static bool ReadRequiredString(
        const TSharedRef<FJsonObject>& Request,
        const TCHAR* Field,
        FString& OutValue,
        FCodexBridgeResult& Result);
};
