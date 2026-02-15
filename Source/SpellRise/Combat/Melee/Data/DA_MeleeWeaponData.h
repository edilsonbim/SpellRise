#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

#include "DA_MeleeWeaponData.generated.h"

class UAnimMontage;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FMeleeTraceConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace", meta=(ClampMin="1.0"))
	float TraceRadius = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace", meta=(ClampMin="1", ClampMax="12"))
	int32 MaxTraceSteps = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace", meta=(ClampMin="1.0"))
	float MinStepDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace")
	FName SocketBase = "Melee_Base";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace")
	FName SocketTip = "Melee_Tip";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace|Baked")
	bool bEnableBakedTracing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Trace|Baked", meta=(ClampMin="5", ClampMax="30"))
	int32 BakedTracePoints = 15;
};

USTRUCT(BlueprintType)
struct FMeleeDamageConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage", meta=(ClampMin="0.0"))
	float BaseDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage", meta=(ClampMin="0.0"))
	float DamageScalar = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	TArray<float> ComboScaling = { 1.00f, 1.10f, 1.25f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage|Cooldown", meta=(ClampMin="0.0"))
	float TargetCooldown = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage|Cooldown")
	bool bResetCooldownPerCombo = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage|Tags")
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage|Tags")
	FGameplayTagContainer HitReactTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage", meta=(ClampMin="0.0"))
	float MaxHitDistance = 0.0f;
};

USTRUCT(BlueprintType)
struct FMeleeComboConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo")
	TArray<FName> ComboSections = { "Hit1", "Hit2", "Hit3" };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo", meta=(ClampMin="1", ClampMax="5"))
	int32 MaxComboHits = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo")
	float StopBlendOutTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo")
	FGameplayTag ComboWindowBeginTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo")
	FGameplayTag ComboWindowEndTag;
};

USTRUCT(BlueprintType)
struct FMeleeFeedbackConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback|Visual")
	TObjectPtr<UParticleSystem> HitEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback|Visual")
	TObjectPtr<UParticleSystem> BloodEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback|Audio")
	TObjectPtr<USoundBase> WhooshSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback|Audio")
	TObjectPtr<USoundBase> HitSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Feedback|Audio")
	TObjectPtr<USoundBase> BlockSound = nullptr;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API UDA_MeleeWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDA_MeleeWeaponData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	FName WeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Config")
	FMeleeTraceConfig TraceConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Config")
	FMeleeDamageConfig DamageConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Config")
	FMeleeComboConfig ComboConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Config")
	FMeleeFeedbackConfig FeedbackConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FTransform AttachOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantAbilities;

	// ✅ Per-weapon damage GE override (used by MeleeComponent)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintPure, Category="Weapon")
	FORCEINLINE float GetFinalBaseDamage() const
	{
		return DamageConfig.BaseDamage * DamageConfig.DamageScalar;
	}

	UFUNCTION(BlueprintPure, Category="Weapon")
	FORCEINLINE float GetMaxHitDistanceResolved() const
	{
		if (DamageConfig.MaxHitDistance > 0.f) return DamageConfig.MaxHitDistance;
		return FMath::Max(100.f, TraceConfig.MinStepDistance * 10.f);
	}
};
