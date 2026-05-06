#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpellRiseGameplayEffectTagLibrary.generated.h"

class UGameplayEffect;

UCLASS()
class SPELLRISE_API USpellRiseGameplayEffectTagLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="SpellRise|GAS|GameplayEffect")
	static bool GameplayEffectHasGrantedTag(const UGameplayEffect* GameplayEffect, FGameplayTag Tag, bool bExactMatch = false);

	UFUNCTION(BlueprintPure, Category="SpellRise|GAS|GameplayEffect")
	static bool GameplayEffectClassHasGrantedTag(TSubclassOf<UGameplayEffect> GameplayEffectClass, FGameplayTag Tag, bool bExactMatch = false);

	UFUNCTION(BlueprintPure, Category="SpellRise|GAS|GameplayEffect")
	static FGameplayTagContainer GetGameplayEffectGrantedTags(const UGameplayEffect* GameplayEffect);

	UFUNCTION(BlueprintPure, Category="SpellRise|GAS|GameplayEffect")
	static const UGameplayEffect* GetGameplayEffectFromActiveHandleSafe(FActiveGameplayEffectHandle ActiveHandle, bool& bIsActive);
};
