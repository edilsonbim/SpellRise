#include "SpellRiseGA_CastSpellBase.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"

#include "AbilitySystemComponent.h"

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

	// Default cast pause tag uses the generic spell cast pause event. You can override per spell.
	CastPauseEventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Spell.CastPause"), /*ErrorIfNotFound=*/false);
}

float USpellRiseGA_CastSpellBase::GetEffectiveCastTime() const
{
	return CastTime;
}

UAnimMontage* USpellRiseGA_CastSpellBase::GetEffectiveCastMontage() const
{
	return CastMontage;
}

FGameplayTag USpellRiseGA_CastSpellBase::GetEffectiveCastPauseEventTag() const
{
	return CastPauseEventTag;
}

UWorld* USpellRiseGA_CastSpellBase::GetWorldFromActorInfoSafe() const
{
	if (CurrentActorInfo)
	{
		if (const AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			return Avatar->GetWorld();
		}
	}

	return GetWorld();
}

void USpellRiseGA_CastSpellBase::AddCastingTag_Local()
{
	if (!CastingStateTag.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(CastingStateTag);
	}
}

void USpellRiseGA_CastSpellBase::RemoveCastingTag_Local()
{
	if (!CastingStateTag.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(CastingStateTag);
	}
}

float USpellRiseGA_CastSpellBase::GetChargeAlpha_Local() const
{
	const float EffectiveCastTime = FMath::Max(0.f, GetEffectiveCastTime());
	if (EffectiveCastTime <= 0.f)
	{
		return 1.f;
	}

	if (bCastCompleted)
	{
		return 1.f;
	}

	return FMath::Clamp(TimeHeld / EffectiveCastTime, 0.f, 1.f);
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

	// If a cast montage is specified (directly or via derived override), run the local cast flow.
	// Otherwise fall back to the base casting flow defined in USpellRiseGameplayAbility.
	if (GetEffectiveCastMontage())
	{
		StartCastFlow_Local();
	}
	else
	{
		StartCastingFlow();
	}
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

	// When using a cast montage, ensure tasks and timers are cleaned up properly before ending.
	if (GetEffectiveCastMontage())
	{
		ClearTasksAndTimer();

		// Resume any paused montage before delegating to parent.
		ResumeMontage();

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
	else
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void USpellRiseGA_CastSpellBase::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	// If we are playing a cast montage we need to resume playback when cancelling so that the montage
	// can complete its release section.
	if (GetEffectiveCastMontage())
	{
		ResumeMontage();
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

// --------------------------
// Custom cast flow helpers
// --------------------------

void USpellRiseGA_CastSpellBase::StartCastFlow_Local()
{
	// Guard: do not re-enter if already fired
	if (!IsActive() || bLocalHasFired)
	{
		return;
	}

	// Reset runtime state
	bPausedMontage    = false;
	bHoldPointReached = false;
	bLocalHasFired    = false;

	// Reset the base ability's casting state so blueprint/read-only values remain accurate
	bCastCompleted = false;
	bInputReleased = false;
	TimeHeld       = 0.f;

	// Apply casting tag (loose tag; avoids private helper in base class)
	AddCastingTag_Local();

	// Blueprint hook
	BP_OnCastStart();

	// Resolve montage/tag (derived may override)
	CastMontage = GetEffectiveCastMontage();
	CastPauseEventTag = GetEffectiveCastPauseEventTag();

	// Play montage if provided
	if (CastMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			CastMontage,
			/*Rate=*/1.0f,
			NAME_None,
			/*bStopWhenAbilityEnds=*/false);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &USpellRiseGA_CastSpellBase::OnMontageCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &USpellRiseGA_CastSpellBase::OnMontageInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &USpellRiseGA_CastSpellBase::OnMontageCancelled);
			MontageTask->ReadyForActivation();
		}
	}

	// Determine effective cast time (may be overridden by derived abilities).
	const float EffectiveCastTime = FMath::Max(0.f, GetEffectiveCastTime());

	// Wait for input release
	if (!WaitInputReleaseTask)
	{
		WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, /*bTestAlreadyReleased=*/true);
		if (WaitInputReleaseTask)
		{
			WaitInputReleaseTask->OnRelease.AddDynamic(this, &USpellRiseGA_CastSpellBase::OnInputReleased_Local);
			WaitInputReleaseTask->ReadyForActivation();
		}
	}

	// Listen for cast pause event if a tag is specified
	if (CastPauseEventTag.IsValid())
	{
		WaitCastPauseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			CastPauseEventTag,
			/*SourceFilter=*/nullptr,
			/*bOnlyTriggerOnce=*/false,
			/*bOnlyMatchExact=*/false);

		if (WaitCastPauseTask)
		{
			WaitCastPauseTask->EventReceived.AddDynamic(this, &USpellRiseGA_CastSpellBase::OnCastPauseEvent);
			WaitCastPauseTask->ReadyForActivation();
		}
	}

	// Set cast timer; if zero or negative we complete immediately
	if (EffectiveCastTime > 0.f)
	{
		if (UWorld* World = GetWorldFromActorInfoSafe())
		{
			World->GetTimerManager().SetTimer(
				CastTimerHandle,
				this,
				&USpellRiseGA_CastSpellBase::OnCastCompleted_Local,
				EffectiveCastTime,
				false);
		}
	}
	else
	{
		OnCastCompleted_Local();
	}
}

void USpellRiseGA_CastSpellBase::ClearTasksAndTimer()
{
	// Clear cast timer
	if (UWorld* World = GetWorldFromActorInfoSafe())
	{
		World->GetTimerManager().ClearTimer(CastTimerHandle);
	}

	// End wait input release task
	if (WaitInputReleaseTask)
	{
		WaitInputReleaseTask->EndTask();
		WaitInputReleaseTask = nullptr;
	}

	// End wait cast pause task
	if (WaitCastPauseTask)
	{
		WaitCastPauseTask->EndTask();
		WaitCastPauseTask = nullptr;
	}

	// End montage task
	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}

	// Remove casting tag if present
	RemoveCastingTag_Local();
}

void USpellRiseGA_CastSpellBase::PauseMontageAtHoldPoint()
{
	if (bPausedMontage)
	{
		return;
	}

	if (!CastMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = (CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr);
	if (AnimInstance)
	{
		AnimInstance->Montage_SetPlayRate(CastMontage, 0.f);
		bPausedMontage = true;
	}
}

void USpellRiseGA_CastSpellBase::ResumeMontage()
{
	if (!bPausedMontage)
	{
		return;
	}

	if (!CastMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = (CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr);
	if (AnimInstance)
	{
		AnimInstance->Montage_SetPlayRate(CastMontage, 1.f);
	}

	bPausedMontage = false;
}

void USpellRiseGA_CastSpellBase::OnCastCompleted_Local()
{
	if (!IsActive() || bLocalHasFired || bCastCompleted)
	{
		return;
	}

	bCastCompleted = true;
	BP_OnCastCompleted();

	// Only pause at the hold point if we already reached it.
	if (bHoldPointReached)
	{
		PauseMontageAtHoldPoint();
	}

	// If player has already released input, fire immediately
	if (bInputReleased)
	{
		FireSpell();
	}
}

void USpellRiseGA_CastSpellBase::OnInputReleased_Local(float HeldTime)
{
	if (!IsActive() || bLocalHasFired)
	{
		return;
	}

	bInputReleased = true;
	TimeHeld = HeldTime;

	// If cast already completed, fire immediately
	if (bCastCompleted)
	{
		FireSpell();
		return;
	}

	// If configured to cancel when released early, cancel the ability
	if (bCancelIfReleasedEarly)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateCancelAbility=*/true);
	}
	// Otherwise keep casting until completion
}

void USpellRiseGA_CastSpellBase::OnCastPauseEvent(FGameplayEventData Payload)
{
	if (!IsActive() || bLocalHasFired)
	{
		return;
	}

	// Reached the montage "hold" point.
	// Pause montage, but do NOT complete the cast (cast completion is timer-driven).
	bHoldPointReached = true;
	PauseMontageAtHoldPoint();
}

void USpellRiseGA_CastSpellBase::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
}

void USpellRiseGA_CastSpellBase::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/true);
}

void USpellRiseGA_CastSpellBase::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/true);
}

void USpellRiseGA_CastSpellBase::FireSpell()
{
	if (!IsActive() || bLocalHasFired)
	{
		return;
	}

	bLocalHasFired = true;

	// Commit ability on fire if configured
	if (CommitPolicy == ESpellRiseCommitPolicy::CommitOnFire)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/true);
			return;
		}
	}

	// Resume montage for the release section
	ResumeMontage();

	// Call Blueprint fire hook
	BP_OnCastFired(GetChargeAlpha_Local());

	// If there is no montage task, end the ability immediately; otherwise wait for the montage to finish.
	if (!MontageTask)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
	}
}
