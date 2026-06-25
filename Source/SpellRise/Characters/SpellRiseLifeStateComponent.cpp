#include "SpellRise/Characters/SpellRiseLifeStateComponent.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseLifeState, Log, All);

USpellRiseLifeStateComponent::USpellRiseLifeStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USpellRiseLifeStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USpellRiseLifeStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellRiseLifeStateComponent, LifeState);
}

ASpellRiseCharacterBase* USpellRiseLifeStateComponent::GetCharacter() const
{
	return Cast<ASpellRiseCharacterBase>(GetOwner());
}

void USpellRiseLifeStateComponent::SetLifeState_Server(ESpellRiseLifeState NewState)
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	if (!Character || !Character->HasAuthority() || LifeState == NewState)
	{
		return;
	}

	const ESpellRiseLifeState OldState = LifeState;
	LifeState = NewState;
	OnRep_LifeState(OldState);
	Character->ForceNetUpdate();
	UE_LOG(LogSpellRiseLifeState, Log, TEXT("[LifeState][Transition] Character=%s Old=%d New=%d"),
		*GetNameSafe(Character), static_cast<int32>(OldState), static_cast<int32>(NewState));
}

void USpellRiseLifeStateComponent::OnRep_LifeState(ESpellRiseLifeState OldState)
{
	OnLifeStateChanged.Broadcast(LifeState, OldState);
	if (ASpellRiseCharacterBase* Character = GetCharacter())
	{
		Character->HandleLifeStatePresentation(LifeState, OldState);
	}
}

bool USpellRiseLifeStateComponent::HandleLethalHealth_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !Character->HasAuthority() || !ASC || IsDead())
	{
		return false;
	}

	if (!Character->GetPlayerState<ASpellRisePlayerState>())
	{
		FinalizeDeath_Server(TEXT("non_player_health_zero"), nullptr);
		return true;
	}

	if (Character->DownedCooldownTag.IsValid() && ASC->HasMatchingGameplayTag(Character->DownedCooldownTag))
	{
		FinalizeDeath_Server(TEXT("downed_cooldown_health_zero"), nullptr);
		return true;
	}

	EnterDowned_Server();
	return true;
}

void USpellRiseLifeStateComponent::EnterDowned_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !Character->HasAuthority() || !ASC || IsDead() || IsDowned())
	{
		return;
	}

	SetLifeState_Server(ESpellRiseLifeState::Downed);
	Character->bIsDowned = true;
	if (Character->DownedStateTag.IsValid())
	{
		ASC->AddLooseGameplayTag(Character->DownedStateTag);
	}
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), 1.f);
	ASC->CancelAllAbilities();
	Character->StopResourceRegen_Server();
	ActivateDownedAbility_Server();
	Character->SyncDeadStateFromASC(TEXT("LifeState.EnterDowned"));
	Character->RefreshRuntimeTickPolicy();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DownedExpirationTimerHandle);
		World->GetTimerManager().SetTimer(
			DownedExpirationTimerHandle,
			this,
			&USpellRiseLifeStateComponent::HandleDownedExpired_Server,
			FMath::Max(1.f, DownedDurationSeconds),
			false);
	}

	Character->MultiHandleDowned();
	Character->ClientUpdateDeathWidget(false);
}

void USpellRiseLifeStateComponent::HandleDownedExpired_Server()
{
	if (IsDowned())
	{
		if (DownedExpirationResult == ESpellRiseDownedExpirationResult::AutomaticRevive)
		{
			Revive_Server(nullptr);
		}
		else
		{
			FinalizeDeath_Server(TEXT("downed_timer_expired"), nullptr);
		}
	}
}

void USpellRiseLifeStateComponent::ClearDowned_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !Character->HasAuthority() || !ASC)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DownedExpirationTimerHandle);
	}
	if (Character->DownedStateTag.IsValid())
	{
		ASC->SetLooseGameplayTagCount(Character->DownedStateTag, 0);
	}
	Character->bIsDowned = false;
	CancelDownedAbility_Server();
}

void USpellRiseLifeStateComponent::FinalizeDeath_Server(FName Reason, ASpellRiseCharacterBase* Finalizer)
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !Character->HasAuthority() || !ASC || IsDead())
	{
		return;
	}

	ClearDowned_Server();
	ClearReviveRecovery_Server();
	ClearDownedCooldown_Server();
	SetLifeState_Server(ESpellRiseLifeState::Dead);
	Character->bIsDead = true;

	if (Character->GE_Death)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(Character);
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Character->GE_Death, 1.f, Context);
		if (Spec.IsValid() && Spec.Data.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
	if (Character->DeadStateTag.IsValid() && !ASC->HasMatchingGameplayTag(Character->DeadStateTag))
	{
		ASC->AddLooseGameplayTag(Character->DeadStateTag);
	}

	ASC->CancelAllAbilities();
	Character->StopResourceRegen_Server();
	Character->SyncDeadStateFromASC(TEXT("LifeState.FinalizeDeath"));
	Character->RefreshRuntimeTickPolicy();
	Character->ClientUpdateDeathWidget(true);
	Character->ProcessFullLootDrop_Server(Character->GetActorLocation());
	Character->ScheduleCorpseDespawn_Server();
	Character->ScheduleRespawn_Server();
	Character->SendFinalDeathNotification(Finalizer, Reason);
}

bool USpellRiseLifeStateComponent::ValidateInteractor_Server(const ASpellRiseCharacterBase* Interactor, FString* OutReason) const
{
	const ASpellRiseCharacterBase* Character = GetCharacter();
	if (!Character || !Character->HasAuthority() || !Interactor || Interactor == Character
		|| Interactor->IsDead() || Interactor->IsDowned() || !IsDowned())
	{
		if (OutReason) { *OutReason = TEXT("invalid_state_or_actor"); }
		return false;
	}

	if (FVector::DistSquared(Character->GetActorLocation(), Interactor->GetActorLocation())
		> FMath::Square(FMath::Max(50.f, InteractionMaxDistance)))
	{
		if (OutReason) { *OutReason = TEXT("distance_exceeded"); }
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LifeStateInteractionLOS), false, Interactor);
	Params.AddIgnoredActor(Interactor);
	const bool bBlocked = Character->GetWorld()
		&& Character->GetWorld()->LineTraceSingleByChannel(
			Hit,
			Interactor->GetPawnViewLocation(),
			Character->GetActorLocation(),
			ECC_Visibility,
			Params)
		&& Hit.GetActor() != Character;
	if (bBlocked)
	{
		if (OutReason) { *OutReason = TEXT("line_of_sight_blocked"); }
		return false;
	}
	return true;
}

bool USpellRiseLifeStateComponent::Revive_Server(ASpellRiseCharacterBase* Reviver)
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !Character->HasAuthority() || !ASC || !IsDowned()
		|| (Reviver && !ValidateInteractor_Server(Reviver)))
	{
		return false;
	}

	ClearDowned_Server();
	Character->ClearPendingDeathTimers_Server();
	ASC->CancelAllAbilities();

	const float Wisdom = ASC->GetNumericAttribute(UCombatAttributeSet::GetWisdomAttribute());
	const float Alpha = FMath::Clamp((Wisdom - 100.f) / 40.f, 0.f, 1.f);
	const float HealthPct = FMath::Lerp(ReviveHealthPercentAt100Wisdom, ReviveHealthPercentAt140Wisdom, Alpha);
	const float ManaPct = FMath::Lerp(ReviveManaPercentAt100Wisdom, ReviveManaPercentAt140Wisdom, Alpha);
	const float StaminaPct = FMath::Lerp(ReviveStaminaPercentAt100Wisdom, ReviveStaminaPercentAt140Wisdom, Alpha);
	const float RecoverySeconds = FMath::Lerp(ReviveRecoverySecondsAt100Wisdom, ReviveRecoverySecondsAt140Wisdom, Alpha);

	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(),
		FMath::Max(1.f, ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute()) * HealthPct));
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(),
		ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute()) * ManaPct);
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(),
		ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute()) * StaminaPct);

	SetLifeState_Server(ESpellRiseLifeState::ReviveRecovery);
	if (Character->ReviveRecoveryTag.IsValid())
	{
		ASC->AddLooseGameplayTag(Character->ReviveRecoveryTag);
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ReviveRecoveryTimerHandle,
			this,
			&USpellRiseLifeStateComponent::ClearReviveRecovery_Server,
			FMath::Max(0.01f, RecoverySeconds),
			false);
	}
	StartDownedCooldown_Server();

	Character->bIsDead = false;
	Character->RefreshRuntimeTickPolicy();
	Character->ApplyRegenStartupEffects();
	Character->StartResourceRegen_Server();
	Character->MultiHandleRevivedFromDowned();
	HideDownedActions_Local();
	Character->SendReviveNotification(Reviver);
	return true;
}

bool USpellRiseLifeStateComponent::Gank_Server(ASpellRiseCharacterBase* Finisher)
{
	if (!ValidateInteractor_Server(Finisher))
	{
		return false;
	}
	FinalizeDeath_Server(TEXT("finished_by_player"), Finisher);
	return true;
}

void USpellRiseLifeStateComponent::AcceptDeath_Server()
{
	if (IsDowned())
	{
		FinalizeDeath_Server(TEXT("accepted_by_player"), nullptr);
	}
}

void USpellRiseLifeStateComponent::StartDownedCooldown_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !ASC || DownedCooldownSeconds <= 0.f)
	{
		return;
	}
	ClearDownedCooldown_Server();
	ASC->AddLooseGameplayTag(Character->DownedCooldownTag);
	GetWorld()->GetTimerManager().SetTimer(
		DownedCooldownTimerHandle,
		this,
		&USpellRiseLifeStateComponent::ClearDownedCooldown_Server,
		DownedCooldownSeconds,
		false);
}

void USpellRiseLifeStateComponent::ClearDownedCooldown_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !ASC) { return; }
	if (UWorld* World = GetWorld()) { World->GetTimerManager().ClearTimer(DownedCooldownTimerHandle); }
	if (Character->DownedCooldownTag.IsValid())
	{
		ASC->SetLooseGameplayTagCount(Character->DownedCooldownTag, 0);
	}
}

void USpellRiseLifeStateComponent::ClearReviveRecovery_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !ASC) { return; }
	if (UWorld* World = GetWorld()) { World->GetTimerManager().ClearTimer(ReviveRecoveryTimerHandle); }
	if (Character->ReviveRecoveryTag.IsValid())
	{
		ASC->SetLooseGameplayTagCount(Character->ReviveRecoveryTag, 0);
	}
	if (!IsDead())
	{
		SetLifeState_Server(ESpellRiseLifeState::Alive);
	}
}

void USpellRiseLifeStateComponent::ActivateDownedAbility_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (!Character || !ASC || !DownedAbilityClass) { return; }
	if (!DownedAbilityHandle.IsValid())
	{
		DownedAbilityHandle = ASC->GiveAbility(
			FGameplayAbilitySpec(DownedAbilityClass, 1, INDEX_NONE, Character->GetPlayerState()));
	}
	ASC->TryActivateAbility(DownedAbilityHandle);
}

void USpellRiseLifeStateComponent::CancelDownedAbility_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	USpellRiseAbilitySystemComponent* ASC = Character ? Character->GetSpellRiseASC() : nullptr;
	if (ASC && DownedAbilityHandle.IsValid())
	{
		ASC->CancelAbilityHandle(DownedAbilityHandle);
	}
}

void USpellRiseLifeStateComponent::ResetForSpawn_Server()
{
	ASpellRiseCharacterBase* Character = GetCharacter();
	if (!Character || !Character->HasAuthority()) { return; }
	Character->ClearPendingDeathTimers_Server();
	ClearDowned_Server();
	ClearReviveRecovery_Server();
	ClearDownedCooldown_Server();
	SetLifeState_Server(ESpellRiseLifeState::Alive);
	Character->bIsDead = false;
	Character->bIsDowned = false;
}

void USpellRiseLifeStateComponent::ShowDownedActions_Local(ASpellRiseCharacterBase* TargetCharacter)
{
	OnShowDownedActions.Broadcast(TargetCharacter);
}

void USpellRiseLifeStateComponent::HideDownedActions_Local()
{
	OnHideDownedActions.Broadcast();
}
