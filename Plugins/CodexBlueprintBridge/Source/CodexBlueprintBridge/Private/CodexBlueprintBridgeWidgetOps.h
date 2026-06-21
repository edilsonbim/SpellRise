#pragma once

#include "CodexBlueprintBridgeTypes.h"

class UWidget;
class UWidgetBlueprint;

class FCodexBlueprintBridgeWidgetOps
{
public:
    bool DescribeWidgetTree(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool AddWidget(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool RemoveWidget(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool RenameWidget(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool SetWidgetProperty(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;

private:
    static UWidgetBlueprint* LoadWidgetBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result);
    static UWidget* FindWidget(UWidgetBlueprint* Blueprint, const FString& Name);
    static bool IsValidWidgetName(const FString& Name);
    static void MarkModified(UWidgetBlueprint* Blueprint, bool bStructural);
};
