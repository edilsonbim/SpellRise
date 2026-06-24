// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Equipment/SpellRiseWeaponComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseGameplayAbilityRuntime, Log, All);

namespace SpellRiseAbilityProgression
{
	static FGameplayTag ResolveWeaponProgressionTagFromWeaponTag(const FGameplayTag& WeaponTag)
	{
		if (!WeaponTag.IsValid())
		{
			return FGameplayTag();
		}

		const FString WeaponTagName = WeaponTag.ToString();
		if (WeaponTagName.EndsWith(TEXT(".TwoHandSword"), ESearchCase::IgnoreCase) ||
			WeaponTagName.EndsWith(TEXT(".Katana"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.TwoHandSword"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".TwoHandHammer"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.TwoHandHammer"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".TwoHandAxe"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.TwoHandAxe"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".Sword"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.Sword"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".Hammer"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.Hammer"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".Axe"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.Axe"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".Bow"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.Bow"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".Shield"), ESearchCase::IgnoreCase) || WeaponTagName == TEXT("Weapon.Shield"))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.Shield"), false);
		}
		if (WeaponTagName.EndsWith(TEXT(".Staff"), ESearchCase::IgnoreCase))
		{
			return FGameplayTag::RequestGameplayTag(TEXT("Progression.Weapon.Staff"), false);
		}

		return FGameplayTag();
	}

	static bool EquippedWeaponTagsContainProgressionTag(
		const FGameplayTagContainer& EquippedWeaponTags,
		const FGameplayTag& RequiredProgressionTag)
	{
		if (!RequiredProgressionTag.IsValid())
		{
			return true;
		}

		for (const FGameplayTag& EquippedWeaponTag : EquippedWeaponTags)
		{
			const FGameplayTag ResolvedProgressionTag = ResolveWeaponProgressionTagFromWeaponTag(EquippedWeaponTag);
			if (ResolvedProgressionTag.IsValid() && ResolvedProgressionTag == RequiredProgressionTag)
			{
				return true;
			}
		}

		return false;
	}
}

USpellRiseGameplayAbility::USpellRiseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = false;
	CastDurationMagnitudeTag = FGameplayTag::RequestGameplayTag(TEXT("Data.CastDuration"), false);
	const FGameplayTag DeadStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false);
	if (DeadStateTag.IsValid())
	{
		ActivationBlockedTags.AddTag(DeadStateTag);
	}
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

bool USpellRiseGameplayAbility::HasPC() const
{
	return Cast<APlayerController>(GetControllerFromActorInfo()) != nullptr;
}

bool USpellRiseGameplayAbility::AreWeaponRequirementsMet() const
{
	return AreWeaponRequirementsMetForActorInfo(CurrentActorInfo);
}

bool USpellRiseGameplayAbility::AreWeaponRequirementsMetForActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!WeaponProgressionTag.IsValid())
	{
		return true;
	}

	const FGameplayAbilityActorInfo* EffectiveActorInfo = ActorInfo;
	AActor* AvatarActor = EffectiveActorInfo ? EffectiveActorInfo->AvatarActor.Get() : GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return false;
	}

	const USpellRiseWeaponComponent* WeaponComponent = AvatarActor->FindComponentByClass<USpellRiseWeaponComponent>();
	FGameplayTagContainer EquippedWeaponTags;
	if (WeaponComponent)
	{
		WeaponComponent->GetActiveWeaponTags(EquippedWeaponTags);
	}

	return SpellRiseAbilityProgression::EquippedWeaponTagsContainProgressionTag(
		EquippedWeaponTags,
		WeaponProgressionTag);
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
	const FGameplayTag DownedStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Downed"), false);
	if (!bAllowWhileDowned
		&& ActorInfo
		&& ActorInfo->AbilitySystemComponent.IsValid()
		&& DownedStateTag.IsValid()
		&& ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(DownedStateTag))
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return AreWeaponRequirementsMetForActorInfo(ActorInfo);
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
		ExecuteSpellFromCurrentMode();
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

void USpellRiseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!AutoActivateWhenGranted || Spec.IsActive() || !ActorInfo)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	const EGameplayAbilityNetExecutionPolicy::Type ExecutionPolicy = GetNetExecutionPolicy();
	const bool bCanActivateFromAuthority =
		ActorInfo->IsNetAuthority() &&
		(ExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly ||
		 ExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated ||
		 ExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted);

	const bool bCanActivateLocally =
		ActorInfo->IsLocallyControlled() &&
		ExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly;

	if (!bCanActivateFromAuthority && !bCanActivateLocally)
	{
		return;
	}

	ASC->TryActivateAbility(Spec.Handle);
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
	bHasReceivedInputReleaseSinceCastStart = true;
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

		if (bIsCasting && GetEffectiveCastCompletionPolicy() == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete)
		{
			UE_LOG(LogSpellRiseGameplayAbilityRuntime, Verbose, TEXT("[CAST] Release before cast complete marked ability=%s"), *GetNameSafe(this));
			return;
		}

		return;
	}

	if (CastType == ESpellRiseAbilityCastType::Channel && bEndChannelOnInputRelease)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

float USpellRiseGameplayAbility::GetCastRemainingTime() const
{
	if (CastType != ESpellRiseAbilityCastType::Cast)
	{
		return 0.0f;
	}

	if (CastTime <= 0.0f)
	{
		return 0.0f;
	}

	if (bIsCasting)
	{
		return FMath::Max(0.0f, CastTime - ResolveElapsedCastTime());
	}

	return FMath::Max(0.0f, CastRemainingTime);
}

float USpellRiseGameplayAbility::GetCastProgressAlpha() const
{
	if (CastType != ESpellRiseAbilityCastType::Cast || CastTime <= 0.0f)
	{
		return 1.0f;
	}

	const float RemainingTime = GetCastRemainingTime();
	return FMath::Clamp((CastTime - RemainingTime) / CastTime, 0.0f, 1.0f);
}

float USpellRiseGameplayAbility::GetSuggestedCastMontagePlayRate(float MontageLength) const
{
	if (MontageLength <= 0.0f)
	{
		return 1.0f;
	}

	if (CastType != ESpellRiseAbilityCastType::Cast)
	{
		return 1.0f;
	}

	const float EffectiveCastWindow = bIsCasting
		? FMath::Max(KINDA_SMALL_NUMBER, GetCastRemainingTime())
		: FMath::Max(KINDA_SMALL_NUMBER, CastTime);

	return FMath::Max(0.01f, MontageLength / EffectiveCastWindow);
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
	if (!bShowCastBar)
	{
		return;
	}

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
	if (!bShowCastBar)
	{
		return;
	}

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
	if (!bShowCastBar)
	{
		return;
	}

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
	bIsCasting = true;
	CastElapsedTime = 0.0f;
	CastRemainingTime = FMath::Max(0.0f, CastTime);
	CastStartTimeSeconds = 0.0;
	bAwaitingReleaseAfterCastComplete = false;
	bHasReceivedInputReleaseSinceCastStart = false;
	ApplyCastingEffect();
	ApplyCastingBarEffect();
	NotifyHUDCastStarted(CastTime);

	if (const UWorld* World = GetWorld())
	{
		CastStartTimeSeconds = World->GetTimeSeconds();
	}

	NativeOnCastStarted();

	const bool bWaitReleaseAfterCast =
		(GetEffectiveCastCompletionPolicy() == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete);

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
	bHasReceivedInputReleaseSinceCastStart = false;
	CastElapsedTime = ResolveElapsedCastTime();
	CastRemainingTime = FMath::Max(0.0f, CastTime - CastElapsedTime);

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
	CastRemainingTime = 0.0f;
	bAwaitingReleaseAfterCastComplete = false;
	CastingGameplayEffectHandle = FActiveGameplayEffectHandle();
	CastingBarGameplayEffectHandle = FActiveGameplayEffectHandle();
	CastStartTimeSeconds = 0.0;
}

ESpellRiseCastCompletionPolicy USpellRiseGameplayAbility::GetEffectiveCastCompletionPolicy() const
{
	if (!HasPC())
	{
		return ESpellRiseCastCompletionPolicy::AutoFireOnCastComplete;
	}

	return CastCompletionPolicy;
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
	CastRemainingTime = FMath::Max(0.0f, CastTime - CastElapsedTime);
	bIsCasting = false;

	if (GetEffectiveCastCompletionPolicy() == ESpellRiseCastCompletionPolicy::WaitReleaseAfterCastComplete)
	{
		if (bHasReceivedInputReleaseSinceCastStart)
		{
			FinishCastFlow();
			return;
		}

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
