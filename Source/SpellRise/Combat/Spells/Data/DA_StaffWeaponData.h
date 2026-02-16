// ============================================================================
// DA_StaffWeaponData.h
// Path: Source/SpellRise/Combat/Spells/Data/DA_StaffWeaponData.h
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ✅ UPrimaryDataAsset lives here
#include "Engine/DataAsset.h"

#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"

#include "DA_StaffWeaponData.generated.h"

class UAnimMontage;
class UGameplayAbility;
class UGameplayEffect;
class AActor;

// ============================================================================
// SPELL FEEDBACK CONFIG
// ============================================================================
// ✅ IMPORTANT: name must match everywhere (UHT, generated files, your code)
USTRUCT(BlueprintType)
struct FStaffSpellFeedbackConfig
{
	GENERATED_BODY()

	// Optional local muzzle fx (client cosmetic)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	TObjectPtr<UParticleSystem> MuzzleEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	TObjectPtr<USoundBase> FireSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	TObjectPtr<USoundBase> CastStartSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	TObjectPtr<USoundBase> CancelSound = nullptr;

	// ✅ GameplayCue tag for impact VFX/SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback", meta=(Categories="GameplayCue"))
	FGameplayTag ImpactCueTag;
};

// ============================================================================
// SPELL CONFIG
// ============================================================================
USTRUCT(BlueprintType)
struct FStaffSpellConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Animation")
	TObjectPtr<UAnimMontage> CastMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Animation")
	FGameplayTag CastPauseEventTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Animation")
	FGameplayTag CancelEventTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Projectile")
	FName SpellMuzzleSocket = "Spell_Muzzle";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Damage")
	TSubclassOf<UGameplayEffect> ProjectileDamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Damage", meta=(ClampMin="0.0"))
	float BaseDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Damage", meta=(ClampMin="0.0"))
	float DamageScalar = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Projectile", meta=(ClampMin="0.0"))
	float MaxRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Cast", meta=(ClampMin="0.0"))
	float CastTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Tags")
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spell|Tags")
	FGameplayTagContainer HitReactTags;
};

// ============================================================================
// DATA ASSET
// ============================================================================
UCLASS(BlueprintType, Const)
class SPELLRISE_API UDA_StaffWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDA_StaffWeaponData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Spell")
	FStaffSpellConfig SpellConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Feedback")
	FStaffSpellFeedbackConfig FeedbackConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Visual")
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Visual")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Visual")
	FTransform AttachOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantAbilities;

	UFUNCTION(BlueprintPure, Category="Weapon|Spell")
	FORCEINLINE float GetFinalSpellBaseDamage() const
	{
		return SpellConfig.BaseDamage * SpellConfig.DamageScalar;
	}
};
