#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Engine/World.h"

// ---------------------------------------------------------
// Gameplay Tags (LAZY INIT - evita static init no load do módulo)
// ---------------------------------------------------------
namespace SpellRiseGA_Tags
{
	// Avoid ensure if tag does not exist yet
	inline FGameplayTag GA_Active()
	{
		static FGameplayTag Tag =
			FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline FGameplayTag State_Dead()
	{
		static FGameplayTag Tag =
			FGameplayTag::RequestGameplayTag(FName("State.Dead"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline FGameplayTag State_Casting()
	{
		static FGameplayTag Tag =
			FGameplayTag::RequestGameplayTag(FName("State.Casting"), /*ErrorIfNotFound=*/false);
		return Tag;
	}
}

// ---------------------------------------------------------
// Helpers
// ---------------------------------------------------------
static bool SR_CanApplyDamage(AActor* Source, AActor* Target, bool bAllowSelfDamage, bool bAllowFriendlyFire)
{
	if (!Source || !Target) return false;

	// Self
	if (Source == Target)
	{
		return bAllowSelfDamage;
	}

	// Friendly fire: no seu caso, sempre true
	if (bAllowFriendlyFire)
	{
		return true;
	}

	// Se no futuro você tiver TeamID:
	// if (SR_AreAllies(Source, Target)) return false;

	return true;
}

USpellRiseGameplayAbility::USpellRiseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Safe tags
	if (SpellRiseGA_Tags::GA_Active().IsValid())
	{
		ActivationOwnedTags.AddTag(SpellRiseGA_Tags::GA_Active());
	}
	if (SpellRiseGA_Tags::State_Dead().IsValid())
	{
		ActivationBlockedTags.AddTag(SpellRiseGA_Tags::State_Dead());
	}

	// Default casting tag (may be invalid if tag not registered yet; that's OK)
	CastingStateTag = SpellRiseGA_Tags::State_Casting();

	// Design default
	bCancelIfReleasedEarly = false;

	ResetRuntimeState();
}

void USpellRiseGameplayAbility::SetAbilityLevel(int32 NewLevel)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		AbilitySpec->Level = NewLevel;
	}
}

UWorld* USpellRiseGameplayAbility::GetWorldSafe() const
{
	if (UWorld* World = GetWorld())
	{
		return World;
	}

	if (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
	{
		return CurrentActorInfo->AvatarActor->GetWorld();
	}

	return nullptr;
}

void USpellRiseGameplayAbility::ResetRuntimeState()
{
	bCastCompleted = false;
	bInputReleased = false;
	TimeHeld = 0.f;
	bHasFired = false;
}

void USpellRiseGameplayAbility::ClearCastTimer()
{
	if (UWorld* World = GetWorldSafe())
	{
		World->GetTimerManager().ClearTimer(CastTimerHandle);
	}
}

void USpellRiseGameplayAbility::ClearReleaseTask()
{
	if (WaitReleaseTask)
	{
		WaitReleaseTask->EndTask();
		WaitReleaseTask = nullptr;
	}
}

void USpellRiseGameplayAbility::AddCastingTag()
{
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid() && CastingStateTag.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->AddLooseGameplayTag(CastingStateTag);
	}
}

void USpellRiseGameplayAbility::RemoveCastingTag()
{
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid() && CastingStateTag.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(CastingStateTag);
	}
}

void USpellRiseGameplayAbility::StartCastingFlow()
{
	if (!IsActive() || bHasFired)
	{
		return;
	}

	// No cast => fire now
	if (!bUseCasting || CastTime <= 0.f)
	{
		FireInternal();
		return;
	}

	ResetRuntimeState();

	AddCastingTag();
	BP_OnCastStart();

	// Commit on start (optional)
	if (CommitPolicy == ESpellRiseCommitPolicy::CommitOnStart)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicate*/true, /*bWasCancelled*/true);
			return;
		}
	}

	// Wait for input release
	ClearReleaseTask();
	WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, /*bTestAlreadyReleased=*/true);
	if (WaitReleaseTask)
	{
		WaitReleaseTask->OnRelease.AddDynamic(this, &USpellRiseGameplayAbility::OnInputReleased);
		WaitReleaseTask->ReadyForActivation();
	}

	// Cast timer
	ClearCastTimer();
	if (UWorld* World = GetWorldSafe())
	{
		World->GetTimerManager().SetTimer(
			CastTimerHandle,
			this,
			&USpellRiseGameplayAbility::OnCastComplete,
			CastTime,
			/*bLoop=*/false
		);
	}
}

void USpellRiseGameplayAbility::FireNow()
{
	if (!IsActive() || bHasFired)
	{
		return;
	}
	FireInternal();
}

void USpellRiseGameplayAbility::ActivateSpellFlow()
{
	if (!IsActive() || bHasFired)
	{
		return;
	}

	if (bUseCasting && CastTime > 0.f)
	{
		StartCastingFlow();
	}
	else
	{
		FireNow();
	}
}

void USpellRiseGameplayAbility::OnInputReleased(float HeldTime)
{
	if (!IsActive() || bHasFired)
	{
		return;
	}

	bInputReleased = true;
	TimeHeld = HeldTime;

	// If cast completed, releasing fires immediately
	if (bCastCompleted)
	{
		FireInternal();
		return;
	}

	// Release early => cancel only if configured
	if (bCancelIfReleasedEarly)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateCancelAbility=*/true);
	}
	// else: keep casting; OnCastComplete will decide.
}

void USpellRiseGameplayAbility::OnCastComplete()
{
	if (!IsActive() || bHasFired)
	{
		return;
	}

	bCastCompleted = true;
	BP_OnCastCompleted();

	// Design:
	// - if already released => fire now
	// - if still holding => wait release
	if (bInputReleased)
	{
		FireInternal();
	}
}

void USpellRiseGameplayAbility::FireInternal()
{
	if (!IsActive() || bHasFired)
	{
		return;
	}

	if (!BP_CanFire())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateCancelAbility=*/true);
		return;
	}

	// Reentrancy guard first
	bHasFired = true;

	// Normalize state BEFORE BP fire
	ClearCastTimer();
	ClearReleaseTask();
	RemoveCastingTag();

	// Commit on fire (default)
	if (CommitPolicy == ESpellRiseCommitPolicy::CommitOnFire)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicate*/true, /*bWasCancelled*/true);
			return;
		}
	}

	BP_OnCastFired(GetChargeAlpha());

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicate*/true, /*bWasCancelled*/false);
}

float USpellRiseGameplayAbility::GetChargeAlpha() const
{
	if (!bUseCasting || CastTime <= 0.f)
	{
		return 1.f;
	}

	if (bCastCompleted)
	{
		return 1.f;
	}

	return FMath::Clamp(TimeHeld / CastTime, 0.f, 1.f);
}

void USpellRiseGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// Always cleanup
	ClearReleaseTask();
	ClearCastTimer();
	RemoveCastingTag();

	if (bWasCancelled && bUseCasting)
	{
		BP_OnCastCancelled();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USpellRiseGameplayAbility::HasPC() const
{
	const APawn* PawnObject = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!PawnObject)
	{
		return false;
	}

	AController* Controller = PawnObject->GetController();
	return (Controller && Controller->IsA<APlayerController>());
}
