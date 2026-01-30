// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

/**
 * Input slots for ability bar / activation binding.
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Ability1 UMETA(DisplayName = "Ability1"),
	Ability2 UMETA(DisplayName = "Ability2"),
	Ability3 UMETA(DisplayName = "Ability3"),
	Ability4 UMETA(DisplayName = "Ability4"),
	Ability5 UMETA(DisplayName = "Ability5"),
	Ability6 UMETA(DisplayName = "Ability6"),
	Ability7 UMETA(DisplayName = "Ability7"),
	Ability8 UMETA(DisplayName = "Ability8")
};

UENUM(BlueprintType)
enum class ESpellRiseCommitPolicy : uint8
{
	CommitOnStart UMETA(DisplayName="Commit On Start"),
	CommitOnFire  UMETA(DisplayName="Commit On Fire"),
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility();

	// ===== UI / Input =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInAbilityBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation")
	bool AutoActivateWhenGranted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetAbilityLevel(int32 NewLevel);

	// =====================================================================
	// Casting (Opt-in)
	// =====================================================================

	/** If true, this ability can use the generic Hold-to-Cast flow. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bUseCasting = false;

	/** Cast duration in seconds (Hold until complete, then release to fire). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting", ClampMin="0.0"))
	float CastTime = 1.0f;

	/** If released before CastTime completes, cancel the ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	bool bCancelIfReleasedEarly = true;

	/** When to Commit (cost/cooldown). Typically CommitOnFire for spells. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	ESpellRiseCommitPolicy CommitPolicy = ESpellRiseCommitPolicy::CommitOnFire;

	/** Optional tag you can use for HUD/state while casting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	FGameplayTag CastingStateTag;

	/** Runtime: becomes true when CastTime completes. */
	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	bool bCastCompleted = false;

	/** Runtime: becomes true when input is released. */
	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	bool bInputReleased = false;

	/** Runtime: time the input was held (seconds). */
	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	float TimeHeld = 0.f;

	/**
	 * Start the generic casting flow (WaitInputRelease + Cast timer).
	 * Call this from your ability's ActivateAbility when bUseCasting=true.
	 */
	UFUNCTION(BlueprintCallable, Category="Casting")
	void StartCastingFlow();

	/**
	 * Fire instantly using the same flow (Commit + BP_OnCastFired).
	 * Useful for instant skills (or when you don't want a cast bar).
	 */
	UFUNCTION(BlueprintCallable, Category="Casting")
	void FireNow();

protected:
	// ===== Blueprint hooks =====

	/** Called right when casting starts (play montage, cues, start HUD). */
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastStart();

	/** Called when CastTime finishes (swap VFX/SFX to "ready"). */
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCompleted();

	/** Called if the ability is cancelled while casting. */
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCancelled();

	/**
	 * Called when the spell actually fires (spawn projectile, apply effects).
	 * ChargeAlpha is 0..1 based on TimeHeld/CastTime (future-proof for charge spells).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastFired(float ChargeAlpha);

	/** Optional validation before firing (targeting, resources, etc). */
	UFUNCTION(BlueprintNativeEvent, Category="Casting|BP")
	bool BP_CanFire() const;
	virtual bool BP_CanFire_Implementation() const { return true; }

	// UGameplayAbility
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	FTimerHandle CastTimerHandle;

	UFUNCTION()
	void OnInputReleased(float HeldTime);

	void OnCastComplete();
	void FireInternal();

	float GetChargeAlpha() const;
	void ClearCastTimer();

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	bool HasPC() const;
};
