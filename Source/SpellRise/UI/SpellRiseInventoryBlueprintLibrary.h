#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManagerTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellRiseInventoryBlueprintLibrary.generated.h"

class USpellRiseItemDefinition;

UCLASS()
class SPELLRISE_API USpellRiseInventoryBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory", meta=(DisplayName="Resolve SpellRise Item Definition"))
	static USpellRiseItemDefinition* ResolveItemDefinition(FPrimaryAssetId DefinitionId);
};
