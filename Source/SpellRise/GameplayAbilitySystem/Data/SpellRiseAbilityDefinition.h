#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityHotbarComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityGrantTypes.h"
#include "SpellRiseAbilityDefinition.generated.h"

class UGameplayAbility;
class UTexture2D;

UENUM(BlueprintType)
enum class ESpellRiseAbilityDefinitionSlotGroup : uint8
{
	None UMETA(DisplayName="None"),
	Weapon UMETA(DisplayName="Weapon"),
	Common UMETA(DisplayName="Common")
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseAbilityDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability")
	FName DefinitionType = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability")
	TObjectPtr<UTexture2D> ActiveIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability", meta=(MultiLine="true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability", meta=(ClampMin="1", UIMin="1"))
	int32 MaxLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Hotbar")
	ESpellRiseAbilityDefinitionSlotGroup SlotGroup = ESpellRiseAbilityDefinitionSlotGroup::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Requirements", meta=(Categories="Weapon"))
	FGameplayTagContainer RequiredWeaponTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|GAS")
	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|GAS")
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|GAS")
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToRemove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Progression", meta=(Categories="Progression.Weapon"))
	FGameplayTag WeaponProgressionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Progression", meta=(Categories="Progression.School"))
	FGameplayTag SchoolProgressionTag;
};
