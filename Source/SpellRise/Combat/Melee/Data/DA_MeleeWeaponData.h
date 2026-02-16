#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "Engine/DataAsset.h"

#include "DA_MeleeWeaponData.generated.h"

class UAnimMontage;
class UGameplayAbility;
class UGameplayEffect;
class UParticleSystem;
class USoundBase;
class AActor;

// ============================================================================
// TRACE CONFIG
// ============================================================================
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

// ============================================================================
// DAMAGE CONFIG
// ============================================================================
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

// ============================================================================
// COMBO CONFIG
// ============================================================================
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo|Events")
	FGameplayTag ComboWindowBeginTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combo|Events")
	FGameplayTag ComboWindowEndTag;
};

// ============================================================================
// FEEDBACK CONFIG
// ============================================================================
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

// ============================================================================
// DATA ASSET
// ============================================================================
UCLASS(BlueprintType, Const)
class SPELLRISE_API UDA_MeleeWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDA_MeleeWeaponData();

	// Identity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Identity")
	FName WeaponId;

	// Melee Config
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	FMeleeTraceConfig TraceConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	FMeleeDamageConfig DamageConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	FMeleeComboConfig ComboConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	FMeleeFeedbackConfig FeedbackConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Melee")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;

	// Visual
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Visual")
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Visual")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Visual")
	FTransform AttachOffset;

	// Abilities
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantAbilities;

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetFinalBaseDamage() const
	{
		return DamageConfig.BaseDamage * DamageConfig.DamageScalar;
	}

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMaxHitDistanceResolved() const
	{
		if (DamageConfig.MaxHitDistance > 0.f)
			return DamageConfig.MaxHitDistance;

		return FMath::Max(100.f, TraceConfig.MinStepDistance * 10.f);
	}
};
