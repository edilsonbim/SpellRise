#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

USpellRiseGameplayAbility::USpellRiseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = true;
}

void USpellRiseGameplayAbility::SetAbilityLevel(int32 NewLevel)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		AbilitySpec->Level = NewLevel;
	}
}

USpellRiseAbilitySystemComponent* USpellRiseGameplayAbility::GetSpellRiseAbilitySystemComponentFromActorInfo() const
{
	return Cast<USpellRiseAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

ASpellRiseCharacterBase* USpellRiseGameplayAbility::GetSpellRiseCharacterFromActorInfo() const
{
	return Cast<ASpellRiseCharacterBase>(GetAvatarActorFromActorInfo());
}

AController* USpellRiseGameplayAbility::GetControllerFromActorInfo() const
{
	if (!CurrentActorInfo)
	{
		return nullptr;
	}

	if (APlayerController* PC = CurrentActorInfo->PlayerController.Get())
	{
		return PC;
	}

	if (APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo()))
	{
		return AvatarPawn->GetController();
	}

	return nullptr;
}

void USpellRiseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	const bool bShouldAutoActivate = bActivateAbilityOnGranted || AutoActivateWhenGranted || ActivationPolicy == ESpellRiseAbilityActivationPolicy::OnSpawn;
	if (!bShouldAutoActivate || !ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !ActorInfo->AvatarActor.IsValid())
	{
		return;
	}

	if (Spec.IsActive())
	{
		return;
	}

	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
}

bool USpellRiseGameplayAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (!bCanActivate && OptionalRelevantTags)
	{
		const_cast<USpellRiseGameplayAbility*>(this)->NativeOnAbilityFailedToActivate(*OptionalRelevantTags);
	}

	return bCanActivate;
}

void USpellRiseGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bIsAbilityInputPressed = false;
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		bIsAbilityInputPressed = Spec->InputPressed;
	}
}

void USpellRiseGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	bIsAbilityInputPressed = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USpellRiseGameplayAbility::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	bIsAbilityInputPressed = true;
	NativeOnAbilityInputPressed(Handle, ActorInfo, ActivationInfo);
	K2_OnAbilityInputPressed();
}

void USpellRiseGameplayAbility::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	bIsAbilityInputPressed = false;
	NativeOnAbilityInputReleased(Handle, ActorInfo, ActivationInfo);
	K2_OnAbilityInputReleased();
}

void USpellRiseGameplayAbility::NativeOnAbilityInputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void USpellRiseGameplayAbility::NativeOnAbilityInputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void USpellRiseGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailureTags)
{
	K2_OnAbilityFailedToActivate(FailureTags);
}
