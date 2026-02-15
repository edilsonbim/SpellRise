#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"

#include "GA_Weapon_MeleeLight.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAnimInstance;
class UAbilitySystemComponent;
class USpellRiseMeleeComponent;

UCLASS()
class SPELLRISE_API UGA_Weapon_MeleeLight : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Weapon_MeleeLight();

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
	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim|Legacy")
	TObjectPtr<UAnimMontage> AttackMontage_Legacy = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.1"))
	float MontagePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Debug")
	bool bDebug = false;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.25", ClampMax="3.0"))
	float AttackSpeedMin = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.25", ClampMax="3.0"))
	float AttackSpeedMax = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Anim", meta=(ClampMin="0.0"))
	float StopBlendOutTime = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category="Melee|Net")
	uint8 AttackId = 0;

private:
	int32 ComboIndex = 0;
	bool bAcceptingComboInput = false;
	bool bComboInputQueued = false;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitComboBeginTask = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitComboEndTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedAttackMontage = nullptr;

	TWeakObjectPtr<USpellRiseMeleeComponent> CachedMeleeComponent;

	UPROPERTY(Transient)
	TArray<FName> CachedComboSections;

	UPROPERTY(Transient)
	int32 CachedMaxComboHits = 3;

	UPROPERTY(Transient)
	FGameplayTag CachedComboWindowBeginTag;

	UPROPERTY(Transient)
	FGameplayTag CachedComboWindowEndTag;

	// ✅ Dedicated-server safe: server não depende de anim notify para saber quando a seção terminou
	FTimerHandle ServerSectionEndTimer;

	// Helpers
	
	AActor* GetAvatar() const;
	APawn* GetAvatarPawn() const;
	UAnimInstance* GetAvatarAnimInstance() const;
	UAbilitySystemComponent* GetASC() const;

	bool IsServer() const;
	bool IsLocallyControlled() const;
	float GetAimYaw() const;

	void CacheWeaponData();
	void InitializeMeleeComponent();

	float GetAttackSpeedMultiplierFromASC() const;
	void ApplyCurrentMontagePlayRate(float NewRate) const;

	void StartComboAt(int32 NewIndex);
	int32 ExtractComboIndexFromPayload(const FGameplayEventData& Payload) const;

	void FireLocalStartAttackForWindow(int32 WindowIdx);

	void StopAttackMontage();
	void StopAllTasks();
	void StartWaitingForComboWindowBegin();
	void StartWaitingForComboWindowEnd();
	void FinishCombo(bool bCancelled);

	// ✅ Anti auto-chain (runtime)
	void ClearAutoChainLinks() const;
	FName GetCurrentMontageSectionName() const;
	bool TryAdvanceCombo_Server(const TCHAR* Reason);
	
	// ✅ Server authority over section endings
	void ScheduleServerSectionEndTimer();
	UFUNCTION()
	void OnServerSectionEnd();

	UFUNCTION()
	void OnComboWindowBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
	
	// Helpers
	int32 GetCurrentComboIndexFromMontage() const;
	void SetNextSection_LocalOrASC(const FName& From, const FName& To);
	bool IsMontagePlaying() const;

};
