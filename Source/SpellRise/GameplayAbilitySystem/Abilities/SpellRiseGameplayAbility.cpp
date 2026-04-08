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
		float CooldownRemaining = 0.0f;
		float CooldownDuration = 0.0f;
		bool bSpecIsActive = false;

		if (ActorInfo)
		{
			GetCooldownTimeRemainingAndDuration(Handle, ActorInfo, CooldownRemaining, CooldownDuration);

			if (const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
			{
				if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle))
				{
					bSpecIsActive = Spec->IsActive();
				}
			}
		}

		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Warning,
			TEXT("[GAS][AbilityActivateDenied] ability=%s authority=%d local=%d netExec=%d castType=%d specActive=%d cooldownRemaining=%.2f cooldownDuration=%.2f failureTags=%s"),
			*GetNameSafe(this),
			ActorInfo && ActorInfo->IsNetAuthority() ? 1 : 0,
			ActorInfo && ActorInfo->IsLocallyControlled() ? 1 : 0,
			static_cast<int32>(NetExecutionPolicy),
			static_cast<int32>(CastType),
			bSpecIsActive ? 1 : 0,
			CooldownRemaining,
			CooldownDuration,
			*FailureTags.ToString());
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
	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[GAS][EndAbility] ability=%s wasCancelled=%d castType=%d inputPressed=%d casting=%d channeling=%d"),
		*GetNameSafe(this),
		bWasCancelled ? 1 : 0,
		static_cast<int32>(CastType),
		bIsAbilityInputPressed ? 1 : 0,
		bIsCasting ? 1 : 0,
		bIsChanneling ? 1 : 0);

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
	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[GAS][InputReleased] ability=%s castType=%d isActive=%d hasExecuted=%d isCasting=%d awaitingRelease=%d policy=%d"),
		*GetNameSafe(this),
		static_cast<int32>(CastType),
		IsActive() ? 1 : 0,
		bHasExecutedSpell ? 1 : 0,
		bIsCasting ? 1 : 0,
		bAwaitingReleaseAfterCastComplete ? 1 : 0,
		static_cast<int32>(CastCompletionPolicy));

	if (!IsActive())
	{
		return;
	}

	if (CastType == ESpellRiseAbilityCastType::Cast && !bHasExecutedSpell)
	{
		if (CastCompletionPolicy == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete && bAwaitingReleaseAfterCastComplete)
		{
			bAwaitingReleaseAfterCastComplete = false;
			FinishCastFlow();
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
			Warning,
			TEXT("[GAS][AbilityCommitDenied] ability=%s castType=%d"),
			*GetNameSafe(this),
			static_cast<int32>(CastType));
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
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Verbose,
			TEXT("[CAST] Applied casting GE=%s handle=%s ability=%s"),
			*CastingGameplayEffectClass->GetName(),
			*CastingGameplayEffectHandle.ToString(),
			*GetNameSafe(this));
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
	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[CAST] Removed casting GE handle=%s ability=%s"),
		*CastingGameplayEffectHandle.ToString(),
		*GetNameSafe(this));
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
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Verbose,
			TEXT("[CAST] Applied casting bar GE=%s handle=%s duration=%.2f ability=%s"),
			*CastingBarGameplayEffectClass->GetName(),
			*CastingBarGameplayEffectHandle.ToString(),
			CastTime,
			*GetNameSafe(this));
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
	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[CAST] Removed casting bar GE handle=%s ability=%s"),
		*CastingBarGameplayEffectHandle.ToString(),
		*GetNameSafe(this));
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

	UE_LOG(LogSpellRiseGameplayAbilityRuntime, Verbose, TEXT("[HUD][Cast] Starting cast duration=%.2f ability=%s"), Duration, *GetNameSafe(this));

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

	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[CAST] StartCastFlow begin ability=%s CastTime=%.2f"),
		*GetNameSafe(this),
		CastTime);

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

	if (CastTime <= 0.0f)
	{
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

	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[CAST] FinishCastFlow ability=%s elapsed=%.2f"),
		*GetNameSafe(this),
		CastElapsedTime);

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

	UE_LOG(
		LogSpellRiseGameplayAbilityRuntime,
		Verbose,
		TEXT("[CAST] ExecuteSpellFromCurrentMode ability=%s castType=%d"),
		*GetNameSafe(this),
		static_cast<int32>(CastType));

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

	if (CastCompletionPolicy == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete && bIsAbilityInputPressed)
	{
		bAwaitingReleaseAfterCastComplete = true;
		UE_LOG(
			LogSpellRiseGameplayAbilityRuntime,
			Verbose,
			TEXT("[CAST] Cast finished, waiting input release ability=%s elapsed=%.2f"),
			*GetNameSafe(this),
			CastElapsedTime);
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
