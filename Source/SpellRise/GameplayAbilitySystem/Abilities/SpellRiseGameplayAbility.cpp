#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

USpellRiseGameplayAbility::USpellRiseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Importante:
	// A base NÃO deve replicar input diretamente quando o projeto usa pipeline custom por Input Tag no ASC.
	// Isso reduz risco de fluxo duplicado/confuso entre ASC custom, InputPressed/InputReleased e prediction.
	bReplicateInputDirectly = false;
}

void USpellRiseGameplayAbility::SetAbilityLevel(int32 NewLevel)
{
	if (!HasServerAuthority())
	{
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	if (!AbilitySpec)
	{
		return;
	}

	AbilitySpec->Level = NewLevel;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->MarkAbilitySpecDirty(*AbilitySpec);
	}
}

bool USpellRiseGameplayAbility::HasServerAuthority() const
{
	return CurrentActorInfo && CurrentActorInfo->IsNetAuthority();
}

bool USpellRiseGameplayAbility::IsLocallyControlledAbility() const
{
	return CurrentActorInfo && CurrentActorInfo->IsLocallyControlled();
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

	if (AController* Controller = CurrentActorInfo->PlayerController.Get())
	{
		return Controller;
	}

	if (APawn* AvatarPawn = Cast<APawn>(CurrentActorInfo->AvatarActor.Get()))
	{
		if (AController* Controller = AvatarPawn->GetController())
		{
			return Controller;
		}
	}

	AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
	while (TestActor)
	{
		if (AController* Controller = Cast<AController>(TestActor))
		{
			return Controller;
		}

		if (APawn* Pawn = Cast<APawn>(TestActor))
		{
			if (AController* Controller = Pawn->GetController())
			{
				return Controller;
			}
		}

		TestActor = TestActor->GetOwner();
	}

	return nullptr;
}

void USpellRiseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateAbilityOnGrantOrSpawn(ActorInfo, Spec);
}

void USpellRiseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	TryActivateAbilityOnGrantOrSpawn(ActorInfo, Spec);
}

bool USpellRiseGameplayAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(
		Handle,
		ActorInfo,
		SourceTags,
		TargetTags,
		OptionalRelevantTags);

	if (!bCanActivate && ShouldBroadcastAbilityFailure(ActorInfo))
	{
		const FGameplayTagContainer FailureTags = OptionalRelevantTags ? *OptionalRelevantTags : FGameplayTagContainer();
		const_cast<USpellRiseGameplayAbility*>(this)->NativeOnAbilityFailedToActivate(FailureTags);
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

	if (const FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		bIsAbilityInputPressed = AbilitySpec->InputPressed;
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

void USpellRiseGameplayAbility::NativeOnAbilityInputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void USpellRiseGameplayAbility::NativeOnAbilityInputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void USpellRiseGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailureTags)
{
	K2_OnAbilityFailedToActivate(FailureTags);
}

bool USpellRiseGameplayAbility::ShouldAutoActivateOnGrantOrSpawn() const
{
	return bActivateAbilityOnGranted
		|| AutoActivateWhenGranted
		|| ActivationPolicy == ESpellRiseAbilityActivationPolicy::OnSpawn;
}

void USpellRiseGameplayAbility::TryActivateAbilityOnGrantOrSpawn(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec) const
{
	if (!ShouldAutoActivateOnGrantOrSpawn())
	{
		return;
	}

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !ActorInfo->AvatarActor.IsValid())
	{
		return;
	}

	if (Spec.IsActive())
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

	if (!ASC || !AvatarActor)
	{
		return;
	}

	// Evita ativar em avatar inválido/transitório.
	if (AvatarActor->GetTearOff() || AvatarActor->GetLifeSpan() > 0.0f)
	{
		return;
	}

	const bool bIsLocalExecution =
		(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) ||
		(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);

	const bool bIsServerExecution =
		(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) ||
		(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

	const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
	const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

	if (bClientShouldActivate || bServerShouldActivate)
	{
		ASC->TryActivateAbility(Spec.Handle);
	}
}

bool USpellRiseGameplayAbility::ShouldBroadcastAbilityFailure(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo && ActorInfo->IsLocallyControlled();
}