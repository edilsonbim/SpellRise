#pragma once

#include "CodexBlueprintBridgeBlueprintOps.h"
#include "CodexBlueprintBridgePolicy.h"
#include "CodexBlueprintBridgeTypes.h"

class FCodexBlueprintBridgeService
{
public:
    FCodexBridgeResult Execute(const TSharedRef<FJsonObject>& Request);

private:
    FCodexBlueprintBridgePolicy Policy;
    FCodexBlueprintBridgeBlueprintOps BlueprintOps;
};
