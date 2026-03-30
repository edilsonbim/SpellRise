#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseGameplayAbilityRuntime, Log, All);

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
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Warning,
			TEXT("[GAS][AbilityLevel] Ignored non-authority SetAbilityLevel ability=%s newLevel=%d"),
			*GetNameSafe(this),
			NewLevel);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Warning,
			TEXT("[GAS][AbilityLevel] Missing ASC while setting level ability=%s newLevel=%d"),
			*GetNameSafe(this),
			NewLevel);
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	if (!AbilitySpec)
	{
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Warning,
			TEXT("[GAS][AbilityLevel] Missing current spec ability=%s newLevel=%d"),
			*GetNameSafe(this),
			NewLevel);
		return;
	}

	AbilitySpec->Level = NewLevel;
	ASC->MarkAbilitySpecDirty(*AbilitySpec);
}

bool USpellRiseGameplayAbility::SR_TriggerGameplayCueReliable(
	FGameplayTag CueTag,
	ESpellRiseCueTriggerMode TriggerMode)
{
	if (!CueTag.IsValid())
	{
		UE_LOG(LogSpellRiseGameplayAbilityRuntime, Warning, TEXT("[GAS][Cue] Invalid cue tag ability=%s"), *GetNameSafe(this));
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		UE_LOG(LogSpellRiseGameplayAbilityRuntime, Warning, TEXT("[GAS][Cue] ASC null in ability=%s cue=%s"), *GetNameSafe(this), *CueTag.ToString());
		return false;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogSpellRiseGameplayAbilityRuntime, Warning, TEXT("[GAS][Cue] Avatar null in ability=%s cue=%s"), *GetNameSafe(this), *CueTag.ToString());
		return false;
	}

	if (!HasServerAuthority() && !IsLocallyControlledAbility())
	{
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Warning,
			TEXT("[GAS][Cue] Rejected cue on non-authority/non-local ability=%s cue=%s"),
			*GetNameSafe(this),
			*CueTag.ToString());
		return false;
	}

	FGameplayCueParameters CueParameters;
	CueParameters.Instigator = AvatarActor;
	CueParameters.EffectCauser = AvatarActor;
	CueParameters.SourceObject = AvatarActor;
	CueParameters.AbilityLevel = GetAbilityLevel();

	switch (TriggerMode)
	{
	case ESpellRiseCueTriggerMode::Execute:
		ASC->ExecuteGameplayCue(CueTag, CueParameters);
		break;
	case ESpellRiseCueTriggerMode::Add:
		ASC->AddGameplayCue(CueTag, CueParameters);
		break;
	case ESpellRiseCueTriggerMode::Remove:
		ASC->RemoveGameplayCue(CueTag);
		break;
	default:
		return false;
	}

	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Warning,
		TEXT("[GAS][Cue] ability=%s mode=%d cue=%s role=%d local=%d"),
		*GetNameSafe(this),
		static_cast<int32>(TriggerMode),
		*CueTag.ToString(),
		CurrentActorInfo ? static_cast<int32>(CurrentActorInfo->AvatarActor.IsValid() ? CurrentActorInfo->AvatarActor->GetLocalRole() : ROLE_None) : -1,
		IsLocallyControlledAbility() ? 1 : 0);

	return true;
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

	if (!bCanActivate)
	{
		const FGameplayTagContainer FailureTags = OptionalRelevantTags ? *OptionalRelevantTags : FGameplayTagContainer();
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Warning,
			TEXT("[GAS][AbilityActivateDenied] ability=%s authority=%d local=%d netExec=%d activationPolicy=%d activationGroup=%d failureTags=%s"),
			*GetNameSafe(this),
			ActorInfo && ActorInfo->IsNetAuthority() ? 1 : 0,
			ActorInfo && ActorInfo->IsLocallyControlled() ? 1 : 0,
			static_cast<int32>(NetExecutionPolicy),
			static_cast<int32>(ActivationPolicy),
			static_cast<int32>(ActivationGroup),
			*FailureTags.ToString());

		if (ShouldBroadcastAbilityFailure(ActorInfo))
		{
			const_cast<USpellRiseGameplayAbility*>(this)->NativeOnAbilityFailedToActivate(FailureTags);
		}
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

	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[GAS][AbilityActivate] ability=%s authority=%d local=%d netExec=%d inputPressed=%d"),
		*GetNameSafe(this),
		ActorInfo && ActorInfo->IsNetAuthority() ? 1 : 0,
		ActorInfo && ActorInfo->IsLocallyControlled() ? 1 : 0,
		static_cast<int32>(NetExecutionPolicy),
		bIsAbilityInputPressed ? 1 : 0);

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

	if (ShouldBroadcastAbilityInputEvents(ActorInfo))
	{
		K2_OnAbilityInputPressed();
	}
}

void USpellRiseGameplayAbility::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	bIsAbilityInputPressed = false;

	NativeOnAbilityInputReleased(Handle, ActorInfo, ActivationInfo);

	if (ShouldBroadcastAbilityInputEvents(ActorInfo))
	{
		K2_OnAbilityInputReleased();
	}
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
		const bool bActivated = ASC->TryActivateAbility(Spec.Handle);
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Verbose,
			TEXT("[GAS][AutoActivateAttempt] ability=%s path=%s result=%d authority=%d local=%d netExec=%d activationPolicy=%d"),
			*GetNameSafe(this),
			bClientShouldActivate ? TEXT("client_predicted") : TEXT("server_authoritative"),
			bActivated ? 1 : 0,
			ActorInfo->IsNetAuthority() ? 1 : 0,
			ActorInfo->IsLocallyControlled() ? 1 : 0,
			static_cast<int32>(NetExecutionPolicy),
			static_cast<int32>(ActivationPolicy));

		if (!bActivated)
		{
			UE_LOG(
				LogSpellRiseGameplayAbilityRuntime,
				Warning,
				TEXT("[GAS][AutoActivateDenied] ability=%s path=%s authority=%d local=%d netExec=%d activationPolicy=%d"),
				*GetNameSafe(this),
				bClientShouldActivate ? TEXT("client_predicted") : TEXT("server_authoritative"),
				ActorInfo->IsNetAuthority() ? 1 : 0,
				ActorInfo->IsLocallyControlled() ? 1 : 0,
				static_cast<int32>(NetExecutionPolicy),
				static_cast<int32>(ActivationPolicy));
		}
	}
}

bool USpellRiseGameplayAbility::ShouldBroadcastAbilityInputEvents(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo && (ActorInfo->IsLocallyControlled() || ActorInfo->IsNetAuthority());
}

bool USpellRiseGameplayAbility::ShouldBroadcastAbilityFailure(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo && ActorInfo->IsLocallyControlled();
}
