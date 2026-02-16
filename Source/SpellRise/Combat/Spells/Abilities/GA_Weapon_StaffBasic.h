// ============================================================================
// GA_Weapon_StaffBasic.h
// Path: Source/SpellRise/Combat/Spells/Abilities/GA_Weapon_StaffBasic.h
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h"
#include "GA_Weapon_StaffBasic.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitInputRelease;

class UDA_StaffWeaponData;
class USpellRiseWeaponComponent;

UCLASS()
class SPELLRISE_API UGA_Weapon_StaffBasic : public USpellRiseGA_CastSpellBase
{
	GENERATED_BODY()

public:
	UGA_Weapon_StaffBasic();

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

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility) override;

private:
	// Tasks (nomes exclusivos; não podem colidir com a base)
	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_PlayMontageAndWait> StaffMontageTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitInputRelease> StaffWaitInputReleaseTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> StaffWaitCastPauseTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitGameplayEvent> StaffWaitInterruptEventTask = nullptr;

	FTimerHandle StaffCastTimerHandle;

	// Runtime state
	bool bStaffPausedMontage = false;
	bool bStaffHasFired = false;
	bool bStaffCastCompleted = false;
	bool bStaffHoldPointReached = false;
	bool bStaffInputReleased = false;
	float StaffTimeHeld = 0.f;

	// Helpers
	const UDA_StaffWeaponData* GetWeaponData() const;
	USpellRiseWeaponComponent* GetWeaponComponent() const;

	void StartCastFlow_Local();
	void ClearTasksAndTimer();

	void PauseMontageAtHoldPoint();
	void ResumeMontage();

	void OnCastCompleted_Local();

	// Callbacks (nomes exclusivos)
	UFUNCTION()
	void Staff_OnInputReleased_Local(float HeldTime);

	UFUNCTION()
	void Staff_OnCastPauseEvent(FGameplayEventData Payload);

	UFUNCTION()
	void Staff_OnInterruptEvent(FGameplayEventData Payload);

	UFUNCTION()
	void Staff_OnMontageCompleted();

	UFUNCTION()
	void Staff_OnMontageInterrupted();

	UFUNCTION()
	void Staff_OnMontageCancelled();

	void FireSpell();
	void SpawnProjectile_Server();
};
