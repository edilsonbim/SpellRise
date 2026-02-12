#pragma once

#include "CoreMinimal.h"
#include "SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGA_CastSpellBase.generated.h"

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

	/** If received while casting, cancels the ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cast|Interrupt")
	FGameplayTag InterruptEventTag;

private:
	UFUNCTION()
	void HandleInterruptEventReceived(FGameplayEventData Payload);

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* WaitInterruptTask = nullptr;
};
