#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexBlueprintBridgeCommandlet.generated.h"

UCLASS()
class UCodexBlueprintBridgeCommandlet final : public UCommandlet
{
    GENERATED_BODY()

public:
    UCodexBlueprintBridgeCommandlet();

    virtual int32 Main(const FString& Params) override;
};
