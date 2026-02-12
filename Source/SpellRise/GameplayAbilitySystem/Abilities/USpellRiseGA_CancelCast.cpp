#include "USpellRiseGA_CancelCast.h"
#include "AbilitySystemComponent.h"

USpellRiseGA_CancelCast::USpellRiseGA_CancelCast()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Cancel cast precisa ser instantâneo e não entrar no seu fluxo de cast
	bUseCasting = false; // assumindo que existe no USpellRiseGameplayAbility (pelo seu sistema)
}

void USpellRiseGA_CancelCast::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Não use CommitAbility aqui: cancelar cast não deve falhar por custo/cooldown
	const FGameplayTag CastingTag = FGameplayTag::RequestGameplayTag(TEXT("State.Casting"), /*ErrorIfNotFound=*/false);

	if (CastingTag.IsValid())
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(CastingTag);

		ASC->CancelAbilities(&CancelTags, nullptr, this);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
