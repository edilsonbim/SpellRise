#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

class UAbilityTask_WaitInputRelease;

/**
 * Input slots for ability bar / activation binding.
 *
 * AAA rule:
 * - NEVER change existing numeric values (Blueprints serialize the underlying byte).
 * - Only append new entries at the end, OR explicitly assign numeric values to freeze them forever.
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	// ---- Frozen legacy slots (do not change) ----
	None     = 0 UMETA(DisplayName="None"),
	Ability1 = 1 UMETA(DisplayName="Ability1"),
	Ability2 = 2 UMETA(DisplayName="Ability2"),
	Ability3 = 3 UMETA(DisplayName="Ability3"),
	Ability4 = 4 UMETA(DisplayName="Ability4"),
	Ability5 = 5 UMETA(DisplayName="Ability5"),
	Ability6 = 6 UMETA(DisplayName="Ability6"),
	Ability7 = 7 UMETA(DisplayName="Ability7"),
	Ability8 = 8 UMETA(DisplayName="Ability8"),

	// ---- Appended (safe) ----
	PrimaryAttack   = 9  UMETA(DisplayName="PrimaryAttack"),
	SecondaryAttack = 10 UMETA(DisplayName="SecondaryAttack"),
	Cancel          = 11 UMETA(DisplayName="Cancel"),
};

UENUM(BlueprintType)
enum class ESpellRiseCommitPolicy : uint8
{
	CommitOnStart = 0 UMETA(DisplayName="Commit On Start"),
	CommitOnFire  = 1 UMETA(DisplayName="Commit On Fire"),
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility();

	// ==========================
	// UI / Input
	// ==========================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	bool ShouldShowInAbilityBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool AutoActivateWhenGranted = false;

	/** Slot used in GiveAbility: becomes InputID in FGameplayAbilitySpec. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UFUNCTION(BlueprintCallable, Category="Ability")
	void SetAbilityLevel(int32 NewLevel);

	// ==========================
	// Casting (opt-in)
	// ==========================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bUseCasting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting", ClampMin="0.0"))
	float CastTime = 1.0f;

	/**
	 * Design:
	 * - FALSE: release before completion -> keep casting -> fire when complete
	 * - TRUE : release before completion -> cancel immediately
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	bool bCancelIfReleasedEarly = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	ESpellRiseCommitPolicy CommitPolicy = ESpellRiseCommitPolicy::CommitOnFire;

	/** Tag applied during casting. Default: State.Casting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	FGameplayTag CastingStateTag;

	// ==========================
	// Runtime state (exposed for UI/debug)
	// ==========================
	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	bool bCastCompleted = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	bool bInputReleased = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	float TimeHeld = 0.f;

	/** Starts hold-to-cast flow. */
	UFUNCTION(BlueprintCallable, Category="Casting")
	void StartCastingFlow();

	/** Fires immediately (ignores casting). */
	UFUNCTION(BlueprintCallable, Category="Casting")
	void FireNow();

	/** Convenience: if bUseCasting, cast; otherwise fire. */
	UFUNCTION(BlueprintCallable, Category="Casting")
	void ActivateSpellFlow();

protected:
	// BP hooks
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastStart();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCancelled();

	/** Fires the spell. ChargeAlpha is 1.0 when completed. */
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastFired(float ChargeAlpha);

	UFUNCTION(BlueprintNativeEvent, Category="Casting|BP")
	bool BP_CanFire() const;
	virtual bool BP_CanFire_Implementation() const { return true; }

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	/** Debug helper */
	UFUNCTION(BlueprintPure, Category="Input")
	int32 GetInputID_Int() const { return static_cast<int32>(AbilityInputID); }

private:
	// Guard against reentrancy / double fire
	bool bHasFired = false;

	FTimerHandle CastTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitInputRelease> WaitReleaseTask = nullptr;

	UWorld* GetWorldSafe() const;

	void ResetRuntimeState();
	void ClearCastTimer();
	void ClearReleaseTask();

	void AddCastingTag();
	void RemoveCastingTag();

	UFUNCTION()
	void OnInputReleased(float HeldTime);

	void OnCastComplete();
	void FireInternal();
	float GetChargeAlpha() const;

	UFUNCTION(BlueprintCallable, Category="Helpers")
	bool HasPC() const;
};
