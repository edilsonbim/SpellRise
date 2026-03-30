#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRiseCastReleaseGameplayAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class SPELLRISE_API USpellRiseCastReleaseGameplayAbility : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseCastReleaseGameplayAbility();

protected:
	// =========================
	// Config
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(ClampMin="0.0"))
	float CastTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bCommitAbilityOnCastStart = false;

	// Se true, a ability termina assim que o fire request for despachado.
	// Se false, o BP controla o EndAbility manualmente.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bEndAbilityOnFireRequest = false;

	// Se true, o client local recebe um evento separado para feedback/prediction.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bDispatchPredictedLocalFireEvent = true;

protected:
	// =========================
	// Runtime
	// =========================

	UPROPERTY(BlueprintReadOnly, Category="Casting")
	bool bCastStarted = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting")
	bool bCastCompleted = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting")
	bool bReleaseRequested = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting")
	bool bReleasedBeforeCastComplete = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting")
	bool bFireRequestDispatched = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting")
	float ReleaseHeldTime = 0.f;

	FTimerHandle CastTimerHandle;
	double CastStartTimeSeconds = 0.0;

protected:
	// =========================
	// GAS
	// =========================

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

	virtual void NativeOnAbilityInputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	// =========================
	// Internal Flow
	// =========================

	void BeginCast();
	void FinishCast();
	void DispatchFireRequest();
	void ClearCastTimer();
	void ResetCastState();

	float ResolveElapsedCastTime() const;

	UFUNCTION()
	void HandleCastTimerFinished();

public:
	// =========================
	// Helpers
	// =========================

	UFUNCTION(BlueprintPure, Category="Casting")
	bool IsCastStarted() const
	{
		return bCastStarted;
	}

	UFUNCTION(BlueprintPure, Category="Casting")
	bool IsCastCompleted() const
	{
		return bCastCompleted;
	}

	UFUNCTION(BlueprintPure, Category="Casting")
	bool HasReleaseBeenRequested() const
	{
		return bReleaseRequested;
	}

	UFUNCTION(BlueprintPure, Category="Casting")
	bool WasReleasedBeforeCompletion() const
	{
		return bReleasedBeforeCastComplete;
	}

	UFUNCTION(BlueprintPure, Category="Casting")
	bool HasFireRequestBeenDispatched() const
	{
		return bFireRequestDispatched;
	}

	UFUNCTION(BlueprintPure, Category="Casting")
	float GetCastElapsedTime() const
	{
		return ResolveElapsedCastTime();
	}

	UFUNCTION(BlueprintPure, Category="Casting")
	float GetCastRemainingTime() const;

	UFUNCTION(BlueprintPure, Category="Casting")
	float GetCastAlpha() const;

protected:
	// =========================
	// Blueprint Events
	// =========================

	UFUNCTION(BlueprintImplementableEvent, Category="Casting", DisplayName="On Cast Started")
	void K2_OnCastStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting", DisplayName="On Cast Completed")
	void K2_OnCastCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting", DisplayName="On Cast Cancelled")
	void K2_OnCastCancelled();

	// SERVER:
	// use este evento para WaitTargetData, spawn real e dano real.
	UFUNCTION(BlueprintImplementableEvent, Category="Casting", DisplayName="On Cast Fire Requested")
	void K2_OnCastFireRequested(bool bInReleasedBeforeCastComplete, float InReleaseHeldTime);

	// CLIENT LOCAL:
	// use este evento só para feedback local/predição visual.
	UFUNCTION(BlueprintImplementableEvent, Category="Casting", DisplayName="On Cast Fire Predicted Local")
	void K2_OnCastFirePredictedLocal(bool bInReleasedBeforeCastComplete, float InReleaseHeldTime);
};