#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManagerTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellRiseInventoryBlueprintLibrary.generated.h"

class USpellRiseItemDefinition;
class USpellRiseInventoryComponent;
class USpellRiseInventoryViewModelComponent;

UCLASS()
class SPELLRISE_API USpellRiseInventoryBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory", meta=(DisplayName="Resolve SpellRise Item Definition"))
	static USpellRiseItemDefinition* ResolveItemDefinition(FPrimaryAssetId DefinitionId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory", meta=(DisplayName="Get SpellRise Inventory Component from Target"))
	static USpellRiseInventoryComponent* GetInventoryComponentFromTarget(UObject* Target);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory", meta=(DisplayName="Get SpellRise Inventory ViewModel from Target"))
	static USpellRiseInventoryViewModelComponent* GetInventoryViewModelFromTarget(UObject* Target);
};
