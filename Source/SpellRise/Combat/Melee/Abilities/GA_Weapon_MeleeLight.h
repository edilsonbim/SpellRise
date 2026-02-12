#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"

#include "GA_Weapon_MeleeLight.generated.h"

class UAnimMontage;
class UGameplayEffect;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAnimInstance;
class UAbilitySystemComponent;
class UDA_WeaponDefinition;

/**
 * Light melee combo ability (3-hit default).
 *
 * Multiplayer goals:
 * - LocalPredicted for responsiveness (client plays montage immediately).
 * - Server authoritative for damage + combo progression (anti-cheat / hardcore).
 * - Simulated proxies get montage via ASC montage replication.
 * - Input is replicated directly so the server can advance combos reliably.
 */
UCLASS()
class SPELLRISE_API UGA_Weapon_MeleeLight : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Weapon_MeleeLight();

	// Input buffering for combo.
	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	// AttackSpeedMultiplier (DerivedStats) -> Montage PlayRate
	float GetAttackSpeedMultiplierFromASC() const;
	void ApplyCurrentMontagePlayRate(float NewRate) const;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Damage")
	TSubclassOf<UGameplayEffect> DamageGE;

	// Legacy fallback (pre-DA)
	UPROPERTY(EditDefaultsOnly, Category="Melee|Damage|Legacy", meta=(ClampMin="0.0"))
	TArray<float> BaseDamagePerHit;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Damage|Legacy", meta=(ClampMin="0.0"))
	float BaseDamage = 25.f;

	// Default montage if DA doesn't provide one.
	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.1"))
	float MontagePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Combo")
	TArray<FName> ComboSections;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Combo", meta=(ClampMin="1", ClampMax="3"))
	int32 MaxComboHits = 3;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Combo")
	FGameplayTag ComboWindowBeginTag;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Combo")
	FGameplayTag ComboWindowEndTag;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Debug")
	bool bDebug = false;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	// AAA limits to prevent absurd speed via buffs/lag/exploits
	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.25", ClampMax="3.0"))
	float AttackSpeedMin = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.25", ClampMax="3.0"))
	float AttackSpeedMax = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.0"))
	float StopBlendOutTime = 0.08f;

private:
	// --------------------
	// Runtime state
	// --------------------
	int32 ComboIndex = 0;
	bool bAcceptingComboInput = false;
	bool bComboInputQueued = false;
	bool bDidHitThisWindow = false;

	// When true: server decides damage + combo; client only predicts montage flow.
	static constexpr bool bServerAuthoritativeComboFlow = true;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitComboBeginTask = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitComboEndTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UDA_WeaponDefinition> CachedWeaponDA = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedAttackMontage = nullptr;

private:
	// --------------------
	// Helpers
	// --------------------
	AActor* GetAvatar() const;
	UAnimInstance* GetAvatarAnimInstance() const;
	UAbilitySystemComponent* GetASC() const;

	bool IsServer() const;
	bool IsLocallyControlled() const;

	void CacheWeaponDataAndMontage();

	float GetBaseDamageFromDAOrLegacy(int32 HitIndex) const;
	float GetDamageScalingFromDA(int32 HitIndex) const;

	void StopAttackMontage();
	void StopAllTasks();

	// Combo flow
	void StartComboAt(int32 NewIndex);
	void TryApplyHitOnceForCurrentWindow();

	void StartWaitingForComboWindowBegin();
	void StartWaitingForComboWindowEnd();

	bool IsValidComboEventPayload(const FGameplayEventData& Payload) const;

	// Events
	UFUNCTION()
	void OnComboWindowBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	void FinishCombo(bool bCancelled);
};
