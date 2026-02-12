#include "SpellRiseGA_CastSpellBase.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USpellRiseGA_CastSpellBase::USpellRiseGA_CastSpellBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// This class is specifically for castable spells.
	bUseCasting = true;

	// Defaults (you can override per-spell in BP)
	CastingStateTag   = FGameplayTag::RequestGameplayTag(TEXT("State.Casting"));
	InterruptEventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Interrupt.Cast"));

	// Helpful for input driven abilities in networked play (optional, but fine).
	bReplicateInputDirectly = true;

	// Keep tag ownership consistent with the casting system.
	if (CastingStateTag.IsValid())
	{
		ActivationOwnedTags.AddTag(CastingStateTag);
	}
}

void USpellRiseGA_CastSpellBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Listen for interrupt event (optional).
	if (InterruptEventTag.IsValid())
	{
		WaitInterruptTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			InterruptEventTag,
			nullptr,
			false,
			false);

		if (WaitInterruptTask)
		{
			WaitInterruptTask->EventReceived.AddDynamic(this, &USpellRiseGA_CastSpellBase::HandleInterruptEventReceived);
			WaitInterruptTask->ReadyForActivation();
		}
	}

	// Start hold-to-cast flow (implemented in USpellRiseGameplayAbility).
	StartCastingFlow();
}

void USpellRiseGA_CastSpellBase::HandleInterruptEventReceived(FGameplayEventData Payload)
{
	// Cancel the cast immediately.
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void USpellRiseGA_CastSpellBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// End tasks
	if (WaitInterruptTask)
	{
		WaitInterruptTask->EndTask();
		WaitInterruptTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
