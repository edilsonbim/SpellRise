#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRiseWeaponDefinition.generated.h"

class AActor;
class UGameplayEffect;
class USpellRiseWeaponDefinition;

UENUM(BlueprintType)
enum class ESpellRiseWeaponHandPolicy : uint8
{
	OneHanded UMETA(DisplayName="One Handed"),
	TwoHanded UMETA(DisplayName="Two Handed"),
	OffHand UMETA(DisplayName="Off Hand")
};

UENUM(BlueprintType)
enum class ESpellRiseWeaponLoadoutSlot : uint8
{
	WeaponSlot1 UMETA(DisplayName="Weapon Slot 1"),
	WeaponSlot2 UMETA(DisplayName="Weapon Slot 2"),
	OffHand UMETA(DisplayName="Off Hand")
};

USTRUCT(BlueprintType)
struct FSpellRiseWeaponLoadoutSlotView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	ESpellRiseWeaponLoadoutSlot Slot = ESpellRiseWeaponLoadoutSlot::WeaponSlot1;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<class UEquippableItem> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<AActor> WeaponActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<USpellRiseWeaponDefinition> WeaponDefinition = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	FGameplayTag WeaponProgressionTag;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	ESpellRiseWeaponHandPolicy HandPolicy = ESpellRiseWeaponHandPolicy::OneHanded;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	bool bIsSuppressed = false;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseWeaponDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon")
	FGameplayTag WeaponTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon|Progression", meta=(Categories="Progression.Weapon"))
	FGameplayTag WeaponProgressionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon")
	ESpellRiseWeaponHandPolicy HandPolicy = ESpellRiseWeaponHandPolicy::OneHanded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon", meta=(AllowedClasses="/Script/Engine.Actor", DisplayName="Weapon Actor Class"))
	TSoftClassPtr<AActor> WeaponActorClassRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon|Attach")
	FName EquippedSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon|Attach")
	FName StowedSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon|GAS")
	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon|GAS", meta=(AllowedClasses="/Script/GameplayAbilities.GameplayEffect", DisplayName="Granted Effects While Equipped"))
	TArray<TSoftClassPtr<UGameplayEffect>> GrantedEffectClassesWhileEquipped;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon|GAS", meta=(ForceInlineRow))
	TMap<FGameplayTag, float> SetByCallerMagnitudes;
};
