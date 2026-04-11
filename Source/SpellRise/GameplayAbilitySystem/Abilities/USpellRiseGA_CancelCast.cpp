// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "USpellRiseGA_CancelCast.h"
#include "AbilitySystemComponent.h"

USpellRiseGA_CancelCast::USpellRiseGA_CancelCast()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	CastType = ESpellRiseAbilityCastType::Instant;
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


	const FGameplayTag CastingTag = FGameplayTag::RequestGameplayTag(TEXT("State.Casting"), false);

	if (CastingTag.IsValid())
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(CastingTag);

		ASC->CancelAbilities(&CancelTags, nullptr, this);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
