#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseCastReleaseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"

USpellRiseCastReleaseGameplayAbility::USpellRiseCastReleaseGameplayAbility()
{
	static const FGameplayTag CastingTag = FGameplayTag::RequestGameplayTag(TEXT("State.Casting"), false);
	if (CastingTag.IsValid())
	{
		ActivationOwnedTags.AddTag(CastingTag);
	}
}

void USpellRiseCastReleaseGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ResetCastState();
	BeginCast();
}

void USpellRiseCastReleaseGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ClearCastTimer();

	if (bWasCancelled && !bFireRequestDispatched)
	{
		K2_OnCastCancelled();
	}

	ResetCastState();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USpellRiseCastReleaseGameplayAbility::NativeOnAbilityInputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::NativeOnAbilityInputReleased(Handle, ActorInfo, ActivationInfo);

	if (!IsActive() || bFireRequestDispatched)
	{
		return;
	}

	bReleaseRequested = true;
	ReleaseHeldTime = ResolveElapsedCastTime();

	// Release chegou antes do fim do cast:
	// apenas marca intenção e espera o timer terminar.
	if (!bCastCompleted)
	{
		bReleasedBeforeCastComplete = true;
		return;
	}

	// Release chegou depois que o cast já terminou:
	// dispara imediatamente.
	DispatchFireRequest();
}

void USpellRiseCastReleaseGameplayAbility::BeginCast()
{
	if (!IsActive())
	{
		return;
	}

	// Commit primeiro, para não tocar FX/montage se a ability falhar.
	if (bCommitAbilityOnCastStart)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
	}

	bCastStarted = true;
	CastStartTimeSeconds = 0.0;

	if (const UWorld* World = GetWorld())
	{
		CastStartTimeSeconds = World->GetTimeSeconds();
	}

	K2_OnCastStarted();

	if (CastTime <= 0.f)
	{
		FinishCast();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CastTimerHandle,
			this,
			&USpellRiseCastReleaseGameplayAbility::HandleCastTimerFinished,
			CastTime,
			false);
	}
}

void USpellRiseCastReleaseGameplayAbility::FinishCast()
{
	if (!IsActive() || bCastCompleted)
	{
		return;
	}

	bCastCompleted = true;
	K2_OnCastCompleted();

	// Só dispara automaticamente se o input já tiver sido solto.
	if (bReleaseRequested)
	{
		DispatchFireRequest();
	}
}

void USpellRiseCastReleaseGameplayAbility::DispatchFireRequest()
{
	if (!IsActive() || bFireRequestDispatched)
	{
		return;
	}

	bFireRequestDispatched = true;

	// =========================
	// CLIENT LOCAL (prediction/cosmetic)
	// =========================
	if (bDispatchPredictedLocalFireEvent && IsLocallyControlledAbility() && !HasServerAuthority())
	{
		K2_OnCastFirePredictedLocal(bReleasedBeforeCastComplete, ReleaseHeldTime);
	}

	// =========================
	// SERVER (real gameplay)
	// =========================
	if (HasServerAuthority())
	{
		K2_OnCastFireRequested(bReleasedBeforeCastComplete, ReleaseHeldTime);
	}

	if (bEndAbilityOnFireRequest)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USpellRiseCastReleaseGameplayAbility::ClearCastTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CastTimerHandle);
	}
}

void USpellRiseCastReleaseGameplayAbility::ResetCastState()
{
	ClearCastTimer();

	bCastStarted = false;
	bCastCompleted = false;
	bReleaseRequested = false;
	bReleasedBeforeCastComplete = false;
	bFireRequestDispatched = false;
	ReleaseHeldTime = 0.f;
	CastStartTimeSeconds = 0.0;
}

float USpellRiseCastReleaseGameplayAbility::ResolveElapsedCastTime() const
{
	const UWorld* World = GetWorld();
	if (!World || !bCastStarted)
	{
		return 0.f;
	}

	if (CastStartTimeSeconds <= 0.0)
	{
		return 0.f;
	}

	return FMath::Max(0.f, static_cast<float>(World->GetTimeSeconds() - CastStartTimeSeconds));
}

void USpellRiseCastReleaseGameplayAbility::HandleCastTimerFinished()
{
	FinishCast();
}

float USpellRiseCastReleaseGameplayAbility::GetCastRemainingTime() const
{
	if (!bCastStarted || bCastCompleted)
	{
		return 0.f;
	}

	return FMath::Max(0.f, CastTime - ResolveElapsedCastTime());
}

float USpellRiseCastReleaseGameplayAbility::GetCastAlpha() const
{
	if (CastTime <= 0.f)
	{
		return 1.f;
	}

	if (bCastCompleted)
	{
		return 1.f;
	}

	return FMath::Clamp(ResolveElapsedCastTime() / CastTime, 0.f, 1.f);
}