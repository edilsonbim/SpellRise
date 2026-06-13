#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SpellRiseAbilityGrantTypes.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FSpellRiseGrantedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SpellRise|GAS|Grant", meta=(AllowedClasses="/Script/GameplayAbilities.GameplayAbility", DisplayName="Ability"))
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SpellRise|GAS|Grant", meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SpellRise|GAS|Grant")
	bool bAutoActivateIfNoInputTag = false;
};
