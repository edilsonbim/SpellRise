#pragma once

#include "CoreMinimal.h"
#include "SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"


// Required engine headers for types used in this ability
#include "TimerManager.h"
#include "Animation/AnimMontage.h"
#include "SpellRiseGA_CastSpellBase.generated.h"
// Forward declarations to avoid heavy includes in header
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitInputRelease;
class UAbilityTask_WaitGameplayEvent;
struct FTimerHandle;

/**
 * Base class for castable spells (hold to cast, release to fire).
 * Uses the casting flow implemented in USpellRiseGameplayAbility (StartCastingFlow / BP_OnCastFired).
 *
 * This class only adds an optional interrupt listener (GameplayEvent) to cancel casting.
 */
UCLASS(Blueprintable, Abstract)
class SPELLRISE_API USpellRiseGA_CastSpellBase : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGA_CastSpellBase();

protected:
	/**
	 * Extension points for derived abilities (e.g., staff/bow) to provide per-weapon data.
	 * Base implementations fall back to the GA's own editable properties.
	 */
	virtual float GetEffectiveCastTime() const;
	virtual UAnimMontage* GetEffectiveCastMontage() const;
	virtual FGameplayTag GetEffectiveCastPauseEventTag() const;

	// UGameplayAbility
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

	/** Override CancelAbility so we can clean up custom casting tasks and resume any paused montage
	 *  when the ability is cancelled (e.g. by an interrupt).  Without this override the montage
	 *  would remain paused and tasks would leak after cancellation. */
	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility) override;

	/** If received while casting, cancels the ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cast|Interrupt")
	FGameplayTag InterruptEventTag;

private:
	UFUNCTION()
	void HandleInterruptEventReceived(FGameplayEventData Payload);

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitInterruptTask = nullptr;

	/** Optional montage to play while casting.  If set, the GA will handle the cast sequence
	 *  internally (play montage, pause at the hold point, resume on fire) instead of relying solely
	 *  on Blueprint flow.  This allows for consistent hold-to-cast behaviour and ensures that
	 *  CastTime and input timing are respected.  The montage will NOT be stopped when the ability
	 *  ends, so the release section will continue to play out fully. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cast|Animation", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> CastMontage = nullptr;

	/** Gameplay tag used to pause the montage at the hold point.  If your cast montage contains an
	 *  AnimNotify_SpellCastPause notify, this tag should match the notify's event tag.  When the
	 *  notify triggers, the montage's play rate will be set to 0.f until the player releases the
	 *  input or the cast is cancelled. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cast|Animation", meta=(AllowPrivateAccess="true"))
	FGameplayTag CastPauseEventTag;

	// ---------- Runtime handles / tasks ----------
	FTimerHandle CastTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<class UAbilityTask_WaitInputRelease> WaitInputReleaseTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<class UAbilityTask_WaitGameplayEvent> WaitCastPauseTask = nullptr;

	// ---------- Runtime state ----------
	/** Flag used internally to prevent the custom casting flow from firing more than once.  We
	 *  cannot rely on the base ability's bHasFired because it is private. */
	bool bLocalHasFired = false;

	/** Whether the cast montage is currently paused at the hold point. */
	bool bPausedMontage = false;

	/** True once we reached the montage hold point (CastPauseEventTag). */
	bool bHoldPointReached = false;

	// ---------- Local helpers (avoid private helpers from USpellRiseGameplayAbility) ----------
	UWorld* GetWorldFromActorInfoSafe() const;
	void AddCastingTag_Local();
	void RemoveCastingTag_Local();
	float GetChargeAlpha_Local() const;

	/** Starts the internal cast flow when CastMontage is defined. */
	void StartCastFlow_Local();

	/** Clear timer and tasks associated with the custom cast flow. Also removes the loose casting tag. */
	void ClearTasksAndTimer();

	/** Pauses the montage at the hold point by setting its play rate to zero. */
	void PauseMontageAtHoldPoint();

	/** Resumes the montage playback if it was previously paused. */
	void ResumeMontage();

	/** Called when the cast timer elapses. Marks cast completed and fires if input already released. */
	void OnCastCompleted_Local();

	/** Called when the player releases the input. */
	UFUNCTION()
	void OnInputReleased_Local(float HeldTime);

	/** Called when the specified CastPauseEventTag is received (hold point). */
	UFUNCTION()
	void OnCastPauseEvent(FGameplayEventData Payload);

	/** Called when the montage finishes playing. Ends the ability normally. */
	UFUNCTION()
	void OnMontageCompleted();

	/** Called when the montage is interrupted. Ends the ability as cancelled. */
	UFUNCTION()
	void OnMontageInterrupted();

	/** Called when the montage is cancelled. Ends the ability as cancelled. */
	UFUNCTION()
	void OnMontageCancelled();

	/** Fires the spell. */
	void FireSpell();
};
