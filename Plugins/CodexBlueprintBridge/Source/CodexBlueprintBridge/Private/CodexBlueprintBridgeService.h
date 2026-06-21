#pragma once

#include "CodexBlueprintBridgeBindingOps.h"
#include "CodexBlueprintBridgeBlueprintOps.h"
#include "CodexBlueprintBridgeGraphOps.h"
#include "CodexBlueprintBridgePolicy.h"
#include "CodexBlueprintBridgeTypes.h"
#include "CodexBlueprintBridgeWidgetOps.h"

class CODEXBLUEPRINTBRIDGE_API FCodexBlueprintBridgeService
{
public:
    FCodexBridgeResult Execute(const TSharedRef<FJsonObject>& Request);

private:
    FCodexBlueprintBridgePolicy Policy;
    FCodexBlueprintBridgeGraphOps GraphOps;
    FCodexBlueprintBridgeWidgetOps WidgetOps;
    FCodexBlueprintBridgeBindingOps BindingOps;
    FCodexBlueprintBridgeBlueprintOps BlueprintOps;
};
