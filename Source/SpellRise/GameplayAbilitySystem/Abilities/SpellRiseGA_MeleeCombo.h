#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRiseGA_MeleeCombo.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAnimMontage;
class UAnimInstance;

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_MeleeCombo : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGA_MeleeCombo();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo")
	TObjectPtr<UAnimMontage> ComboMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo")
	TArray<FName> ComboSections;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo", meta=(ClampMin="0.1"))
	float PlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo")
	bool bStopMontageWhenAbilityEnds = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo|Events", meta=(Categories="Event"))
	FGameplayTag ComboWindowStartTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo|Events", meta=(Categories="Event"))
	FGameplayTag ComboWindowEndTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo|Events", meta=(Categories="Event"))
	FGameplayTag HitScanStartTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo|Events", meta=(Categories="Event"))
	FGameplayTag HitScanEndTag;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	int32 CurrentComboIndex = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	bool bComboWindowOpen = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	bool bQueuedNextInput = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	bool bConsumedThisWindow = false;

	// Evita que o clique inicial de ativação seja consumido como continuação de combo.
	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	bool bIgnoreFirstActivationPress = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	bool bHitScanActive = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Combo|State")
	float QueuedInputTimeSeconds = -1.0f;

	// Tempo de tolerância para aceitar input pouco antes da janela de combo abrir.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo|Input", meta=(ClampMin="0.00", ClampMax="0.50"))
	float InputBufferGraceSeconds = 0.16f;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ComboMontageTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboWindowStartTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboWindowEndTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitScanStartTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitScanEndTask = nullptr;

protected:
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

	virtual void NativeOnAbilityInputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void NativeOnAbilityInputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION()
	void OnComboMontageCompleted();

	UFUNCTION()
	void OnComboMontageBlendedIn();

	UFUNCTION()
	void OnComboMontageBlendOut();

	UFUNCTION()
	void OnComboMontageInterrupted();

	UFUNCTION()
	void OnComboMontageCancelled();

	UFUNCTION()
	void OnComboWindowStart(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnHitScanStart(FGameplayEventData Payload);

	UFUNCTION()
	void OnHitScanEnd(FGameplayEventData Payload);

protected:
	void InitializeComboSections();
	UAnimInstance* GetComboAnimInstance() const;
	void StartComboTasks();
	void StopComboTasks();
	void ResetRuntimeState();
	bool TryAdvanceComboSection();
	bool IsPayloadFromAvatar(const FGameplayEventData& Payload) const;
	void StopHitScanIfActive(const FGameplayEventData* Payload = nullptr);
	bool IsQueuedInputStillValid() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Combo", DisplayName="On Combo Window Opened")
	void K2_OnComboWindowOpened(int32 ComboIndex);

	UFUNCTION(BlueprintImplementableEvent, Category="Combo", DisplayName="On Combo Window Closed")
	void K2_OnComboWindowClosed(int32 ComboIndex);

	UFUNCTION(BlueprintImplementableEvent, Category="Combo", DisplayName="On Combo Advanced")
	void K2_OnComboAdvanced(int32 NewComboIndex, FName NewSection);

	UFUNCTION(BlueprintImplementableEvent, Category="Combo|HitScan", DisplayName="On HitScan Start")
	void K2_OnHitScanStart(const FGameplayEventData& Payload);

	UFUNCTION(BlueprintImplementableEvent, Category="Combo|HitScan", DisplayName="On HitScan End")
	void K2_OnHitScanEnd(const FGameplayEventData& Payload);
};
