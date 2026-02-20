#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Engine/World.h"

namespace SpellRiseGA_Tags
{
	inline FGameplayTag GA_Active()
	{
		static FGameplayTag Tag =
			FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active"), false);
		return Tag;
	}

	inline FGameplayTag State_Dead()
	{
		static FGameplayTag Tag =
			FGameplayTag::RequestGameplayTag(FName("State.Dead"), false);
		return Tag;
	}

	inline FGameplayTag State_Casting()
	{
		static FGameplayTag Tag =
			FGameplayTag::RequestGameplayTag(FName("State.Casting"), false);
		return Tag;
	}
}

static bool SR_CanApplyDamage(AActor* Source, AActor* Target, bool bAllowSelfDamage, bool bAllowFriendlyFire)
{
	if (!Source || !Target) return false;

	if (Source == Target)
	{
		return bAllowSelfDamage;
	}

	if (bAllowFriendlyFire)
	{
		return true;
	}

	return true;
}

USpellRiseGameplayAbility::USpellRiseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	if (SpellRiseGA_Tags::GA_Active().IsValid())
	{
		ActivationOwnedTags.AddTag(SpellRiseGA_Tags::GA_Active());
	}
	if (SpellRiseGA_Tags::State_Dead().IsValid())
	{
		ActivationBlockedTags.AddTag(SpellRiseGA_Tags::State_Dead());
	}

	CastingStateTag = SpellRiseGA_Tags::State_Casting();

	bCancelIfReleasedEarly = false;

	bReplicateInputDirectly = true;

	ResetRuntimeState();
}

void USpellRiseGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bChannelAbility)
	{
		AddCastingTag();
	}
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

	if (!bUseCasting || CastTime <= 0.f)
	{
		FireInternal();
		return;
	}

	ResetRuntimeState();

	AddCastingTag();
	BP_OnCastStart();

	if (CommitPolicy == ESpellRiseCommitPolicy::CommitOnStart)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
	}

	ClearReleaseTask();
	WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (WaitReleaseTask)
	{
		WaitReleaseTask->OnRelease.AddDynamic(this, &USpellRiseGameplayAbility::OnInputReleased);
		WaitReleaseTask->ReadyForActivation();
	}

	ClearCastTimer();
	if (UWorld* World = GetWorldSafe())
	{
		World->GetTimerManager().SetTimer(
			CastTimerHandle,
			this,
			&USpellRiseGameplayAbility::OnCastComplete,
			CastTime,
			false
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

	if (bCastCompleted)
	{
		FireInternal();
		return;
	}

	if (bCancelIfReleasedEarly)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void USpellRiseGameplayAbility::OnCastComplete()
{
	if (!IsActive() || bHasFired)
	{
		return;
	}

	bCastCompleted = true;
	BP_OnCastCompleted();

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
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	bHasFired = true;

	ClearCastTimer();
	ClearReleaseTask();
	RemoveCastingTag();

	if (CommitPolicy == ESpellRiseCommitPolicy::CommitOnFire)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
	}

	BP_OnCastFired(GetChargeAlpha());

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USpellRiseGameplayAbility::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (bChannelAbility && IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
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
