// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseGameplayAbilityRuntime, Log, All);

USpellRiseGameplayAbility::USpellRiseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = false;
	CastDurationMagnitudeTag = FGameplayTag::RequestGameplayTag(TEXT("Data.CastDuration"), false);
}

void USpellRiseGameplayAbility::SetAbilityLevel(int32 NewLevel)
{
	if (!HasServerAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	if (!AbilitySpec)
	{
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
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return false;
	}

	if (!HasServerAuthority() && !IsLocallyControlledAbility())
	{
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

bool USpellRiseGameplayAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USpellRiseGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ResetSpellRuntimeState();

	if (const FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		bIsAbilityInputPressed = AbilitySpec->InputPressed;
	}

	switch (CastType)
	{
	case ESpellRiseAbilityCastType::Instant:
		if (TryCommitSpellAbility())
		{
			ExecuteSpellFromCurrentMode();
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		break;

	case ESpellRiseAbilityCastType::Cast:
		StartCastFlow();
		break;

	case ESpellRiseAbilityCastType::Channel:
		StartChannelFlow();
		break;

	default:
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		break;
	}
}

void USpellRiseGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	RemoveCastingEffect();
	RemoveCastingBarEffect();
	NotifyHUDCastStopped();

	if (bIsChanneling)
	{
		StopChannelFlow(bWasCancelled);
	}

	if (!bKeepSelectedAfterAbilityEnds && ActorInfo && ActorInfo->IsLocallyControlled())
	{
		if (USpellRiseAbilitySystemComponent* SpellRiseASC = Cast<USpellRiseAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
		{
			if (SpellRiseASC->SR_IsSelectedSpellAbilityHandle(Handle))
			{
				if (ASpellRiseCharacterBase* SpellRiseCharacter = Cast<ASpellRiseCharacterBase>(ActorInfo->AvatarActor.Get()))
				{
					SpellRiseCharacter->ClearSelectedAbility();
				}
				else
				{
					SpellRiseASC->SR_ClearSelectedSpellAbility();
				}
			}
		}
	}

	ResetSpellRuntimeState();

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
}

void USpellRiseGameplayAbility::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	bIsAbilityInputPressed = false;
	NativeOnAbilityInputReleased(Handle, ActorInfo, ActivationInfo);
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
	if (!IsActive())
	{
		return;
	}

	if (CastType == ESpellRiseAbilityCastType::Cast && !bHasExecutedSpell)
	{
		if (bAwaitingReleaseAfterCastComplete)
		{
			FinishCastFlow();
			return;
		}

		if (bIsCasting && CastCompletionPolicy == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		return;
	}

	if (CastType == ESpellRiseAbilityCastType::Channel && bEndChannelOnInputRelease)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

bool USpellRiseGameplayAbility::TryCommitSpellAbility()
{
	if (!bCommitAbilityOnActivate)
	{
		return true;
	}

	const bool bCommitted = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	if (!bCommitted)
	{
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Verbose,
			TEXT("CommitAbility failed for '%s' (owner=%s)"),
			*GetName(),
			CurrentActorInfo && CurrentActorInfo->OwnerActor.IsValid() ? *CurrentActorInfo->OwnerActor->GetName() : TEXT("None"));
	}

	return bCommitted;
}

void USpellRiseGameplayAbility::ApplyCastingEffect()
{
	if (!CastingGameplayEffectClass)
	{
		return;
	}

	if (!CurrentActorInfo)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CastingGameplayEffectClass, GetAbilityLevel());
		if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
		{
			return;
		}

		CastingGameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void USpellRiseGameplayAbility::RemoveCastingEffect()
{
	if (!CastingGameplayEffectHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveActiveGameplayEffect(CastingGameplayEffectHandle);
	}

	CastingGameplayEffectHandle = FActiveGameplayEffectHandle();
}

void USpellRiseGameplayAbility::ApplyCastingBarEffect()
{
	if (!CastingBarGameplayEffectClass || CastTime <= 0.0f)
	{
		return;
	}

	if (!CurrentActorInfo)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CastingBarGameplayEffectClass, GetAbilityLevel());
		if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
		{
			return;
		}

		if (CastDurationMagnitudeTag.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(CastDurationMagnitudeTag, CastTime);
		}

		CastingBarGameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void USpellRiseGameplayAbility::RemoveCastingBarEffect()
{
	if (!CastingBarGameplayEffectHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveActiveGameplayEffect(CastingBarGameplayEffectHandle);
	}

	CastingBarGameplayEffectHandle = FActiveGameplayEffectHandle();
}

void USpellRiseGameplayAbility::NotifyHUDCastStarted(float Duration) const
{
	if (!IsLocallyControlledAbility())
	{
		return;
	}

	if (Duration <= 0.0f)
	{
		return;
	}

	if (AController* Controller = GetControllerFromActorInfo())
	{
		if (ASpellRisePlayerController* SRPC = Cast<ASpellRisePlayerController>(Controller))
		{
			SRPC->BP_StartCastBar(Duration);
		}
	}
}

void USpellRiseGameplayAbility::NotifyHUDCastStopped() const
{
	if (!IsLocallyControlledAbility())
	{
		return;
	}

	if (AController* Controller = GetControllerFromActorInfo())
	{
		if (ASpellRisePlayerController* SRPC = Cast<ASpellRisePlayerController>(Controller))
		{
			SRPC->BP_StopCastBar();
		}
	}
}

void USpellRiseGameplayAbility::StartCastFlow()
{
	if (!TryCommitSpellAbility())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	bIsCasting = true;
	CastElapsedTime = 0.0f;
	CastStartTimeSeconds = 0.0;
	bAwaitingReleaseAfterCastComplete = false;
	ApplyCastingEffect();
	ApplyCastingBarEffect();
	NotifyHUDCastStarted(CastTime);

	if (const UWorld* World = GetWorld())
	{
		CastStartTimeSeconds = World->GetTimeSeconds();
	}

	NativeOnCastStarted();

	const bool bWaitReleaseAfterCast =
		(CastCompletionPolicy == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete);

	if (CastTime <= 0.0f)
	{
		bIsCasting = false;
		if (bWaitReleaseAfterCast)
		{
			bAwaitingReleaseAfterCastComplete = true;
			return;
		}

		FinishCastFlow();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CastTimerHandle,
			this,
			&USpellRiseGameplayAbility::HandleCastFinished,
			CastTime,
			false);
	}
}

void USpellRiseGameplayAbility::FinishCastFlow()
{
	if (!IsActive() || bHasExecutedSpell)
	{
		return;
	}

	bAwaitingReleaseAfterCastComplete = false;
	CastElapsedTime = ResolveElapsedCastTime();

	if (bIsCasting)
	{
		bIsCasting = false;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CastTimerHandle);
		}
	}

	ExecuteSpellFromCurrentMode();
}

void USpellRiseGameplayAbility::StartChannelFlow()
{
	if (!TryCommitSpellAbility())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	bIsChanneling = true;
	NativeOnChannelStarted();
	NativeOnSpellExecuted();

	if (ChannelInterval <= 0.0f)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ChannelTimerHandle,
			this,
			&USpellRiseGameplayAbility::HandleChannelTick,
			ChannelInterval,
			true);
	}
}

void USpellRiseGameplayAbility::TickChannelFlow()
{
	if (!IsActive() || !bIsChanneling)
	{
		return;
	}

	if (bCommitAbilityEveryChannelTick && !CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	NativeOnSpellExecuted();
}

void USpellRiseGameplayAbility::StopChannelFlow(bool bWasCancelled)
{
	if (!bIsChanneling)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ChannelTimerHandle);
	}

	bIsChanneling = false;
	NativeOnChannelStopped(bWasCancelled);
}

void USpellRiseGameplayAbility::ExecuteSpellFromCurrentMode()
{
	if (!IsActive() || bHasExecutedSpell)
	{
		return;
	}

	bHasExecutedSpell = true;
	NativeOnSpellExecuted();

	if (bEndAbilityAfterExecution && CastType != ESpellRiseAbilityCastType::Channel)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USpellRiseGameplayAbility::ResetSpellRuntimeState()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CastTimerHandle);
		World->GetTimerManager().ClearTimer(ChannelTimerHandle);
	}

	bIsAbilityInputPressed = false;
	bIsCasting = false;
	bIsChanneling = false;
	bHasExecutedSpell = false;
	CastElapsedTime = 0.0f;
	bAwaitingReleaseAfterCastComplete = false;
	CastingGameplayEffectHandle = FActiveGameplayEffectHandle();
	CastingBarGameplayEffectHandle = FActiveGameplayEffectHandle();
	CastStartTimeSeconds = 0.0;
}

float USpellRiseGameplayAbility::ResolveElapsedCastTime() const
{
	if (CastElapsedTime > 0.0f)
	{
		return CastElapsedTime;
	}

	const UWorld* World = GetWorld();
	if (!World || CastStartTimeSeconds <= 0.0)
	{
		return 0.0f;
	}

	return FMath::Max(0.0f, static_cast<float>(World->GetTimeSeconds() - CastStartTimeSeconds));
}

void USpellRiseGameplayAbility::HandleCastFinished()
{
	CastElapsedTime = ResolveElapsedCastTime();
	bIsCasting = false;

	if (CastCompletionPolicy == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete)
	{
		bAwaitingReleaseAfterCastComplete = true;
		return;
	}

	FinishCastFlow();
}

void USpellRiseGameplayAbility::HandleChannelTick()
{
	TickChannelFlow();
}

void USpellRiseGameplayAbility::NativeOnSpellExecuted()
{
	K2_OnSpellExecuted();
}

void USpellRiseGameplayAbility::NativeOnCastStarted()
{
	K2_OnCastStarted();
}

void USpellRiseGameplayAbility::NativeOnChannelStarted()
{
	K2_OnChannelStarted();
}

void USpellRiseGameplayAbility::NativeOnChannelStopped(bool bWasCancelled)
{
	K2_OnChannelStopped(bWasCancelled);
}
