#include "SpellRiseAbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_MeleeCombo.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseASCCombo, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseASCCues, Log, All);

USpellRiseAbilitySystemComponent::USpellRiseAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void USpellRiseAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Catalyst logic is handled externally by UCatalystComponent.
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &USpellRiseAbilitySystemComponent::OnGameplayEffectAppliedToSelf);
	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &USpellRiseAbilitySystemComponent::OnGameplayEffectRemovedFromSelf);
}

int32 USpellRiseAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	const auto IsNativeComboActive = [this]() -> bool
	{
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.IsActive() && Cast<USpellRiseGA_MeleeCombo>(Spec.Ability))
			{
				return true;
			}
		}
		return false;
	};

	const bool bNativeComboActive = IsNativeComboActive();

	if (EventTag.IsValid())
	{
		const FGameplayTag ComboStartEvent = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Start"), false);
		const FGameplayTag ComboEndEvent = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.End"), false);
		const FGameplayTag ComboInputEvent = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Input"), false);
		const bool bIsComboEvent =
			EventTag.MatchesTagExact(ComboStartEvent) ||
			EventTag.MatchesTagExact(ComboEndEvent) ||
			EventTag.MatchesTagExact(ComboInputEvent);

		if (bIsComboEvent)
		{
			UE_LOG(
				LogSpellRiseASCCombo,
				Verbose,
				TEXT("[GAS][Event] ASC=%s Event=%s Instigator=%s Target=%s"),
				*GetNameSafe(this),
				*EventTag.ToString(),
				Payload ? *GetNameSafe(Payload->Instigator.Get()) : TEXT("None"),
				Payload ? *GetNameSafe(Payload->Target.Get()) : TEXT("None"));
		}

		if (!bNativeComboActive && EventTag.MatchesTagExact(ComboInputEvent))
		{
			bComboAdvanceRequested = true;
		}

		if (!bNativeComboActive && EventTag.MatchesTagExact(ComboStartEvent))
		{
			if (bComboAdvanceRequested)
			{
				const bool bAdvanced = TryAdvanceActiveComboMontage();
				UE_LOG(LogSpellRiseASCCombo, Verbose, TEXT("[GAS][ComboAdvance] Requested=1 Advanced=%d"), bAdvanced ? 1 : 0);
				if (bAdvanced)
				{
					bComboAdvanceRequested = false;
				}
			}
		}

		if (!bNativeComboActive && EventTag.MatchesTagExact(ComboEndEvent))
		{
			bComboAdvanceRequested = false;
		}
	}

	return Super::HandleGameplayEvent(EventTag, Payload);
}

bool USpellRiseAbilitySystemComponent::TryAdvanceActiveComboMontage()
{
	AActor* LocalAvatarActor = GetAvatarActor();
	ACharacter* Character = Cast<ACharacter>(LocalAvatarActor);
	if (!Character)
	{
		UE_LOG(LogSpellRiseASCCombo, Verbose, TEXT("[GAS][ComboAdvance] Fail: Avatar is not Character. Avatar=%s"), *GetNameSafe(LocalAvatarActor));
		return false;
	}

	TArray<USkeletalMeshComponent*> CandidateMeshes;
	if (ASpellRiseCharacterBase* SpellRiseCharacter = Cast<ASpellRiseCharacterBase>(Character))
	{
		if (USkeletalMeshComponent* VisualMesh = SpellRiseCharacter->GetVisualMeshComponent())
		{
			CandidateMeshes.AddUnique(VisualMesh);
		}
		if (USkeletalMeshComponent* EquipMesh = SpellRiseCharacter->GetEquipmentAttachMeshComponent())
		{
			CandidateMeshes.AddUnique(EquipMesh);
		}
	}

	if (USkeletalMeshComponent* BaseMesh = Character->GetMesh())
	{
		CandidateMeshes.AddUnique(BaseMesh);
	}

	TArray<USkeletalMeshComponent*> AllMeshes;
	Character->GetComponents<USkeletalMeshComponent>(AllMeshes);
	for (USkeletalMeshComponent* MeshComp : AllMeshes)
	{
		CandidateMeshes.AddUnique(MeshComp);
	}

	UAnimInstance* SelectedAnimInstance = nullptr;
	UAnimMontage* ActiveMontage = nullptr;

	for (USkeletalMeshComponent* MeshComp : CandidateMeshes)
	{
		if (!MeshComp)
		{
			continue;
		}

		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		if (!AnimInstance)
		{
			continue;
		}

		UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();
		if (!Montage)
		{
			continue;
		}

		SelectedAnimInstance = AnimInstance;
		ActiveMontage = Montage;
		break;
	}

	if (!SelectedAnimInstance || !ActiveMontage)
	{
		UE_LOG(LogSpellRiseASCCombo, Verbose, TEXT("[GAS][ComboAdvance] Fail: No active montage found in candidate meshes. Character=%s"), *GetNameSafe(Character));
		return false;
	}

	const FName CurrentSection = SelectedAnimInstance->Montage_GetCurrentSection(ActiveMontage);
	const int32 CurrentSectionIndex = ActiveMontage->GetSectionIndex(CurrentSection);
	const int32 SectionCount = ActiveMontage->CompositeSections.Num();
	if (CurrentSectionIndex == INDEX_NONE || CurrentSectionIndex + 1 >= SectionCount)
	{
		UE_LOG(
			LogSpellRiseASCCombo,
			Verbose,
			TEXT("[GAS][ComboAdvance] Fail: Montage has no next section. Montage=%s CurrentSection=%s CurrentIdx=%d SectionCount=%d"),
			*GetNameSafe(ActiveMontage),
			*CurrentSection.ToString(),
			CurrentSectionIndex,
			SectionCount);
		return false;
	}

	const FName NextSection = ActiveMontage->CompositeSections[CurrentSectionIndex + 1].SectionName;
	if (NextSection.IsNone() || NextSection == CurrentSection)
	{
		UE_LOG(
			LogSpellRiseASCCombo,
			Verbose,
			TEXT("[GAS][ComboAdvance] Fail: Next section invalid. Montage=%s Current=%s Next=%s"),
			*GetNameSafe(ActiveMontage),
			*CurrentSection.ToString(),
			*NextSection.ToString());
		return false;
	}

	SelectedAnimInstance->Montage_JumpToSection(NextSection, ActiveMontage);
	UE_LOG(
		LogSpellRiseASCCombo,
		Verbose,
		TEXT("[GAS][ComboMontage] Forced section jump Montage=%s Current=%s Next=%s"),
		*GetNameSafe(ActiveMontage),
		*CurrentSection.ToString(),
		*NextSection.ToString());
	return true;
}

void USpellRiseAbilitySystemComponent::OnGameplayEffectAppliedToSelf(
	UAbilitySystemComponent* SourceASC,
	const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle ActiveHandle)
{
	const FGameplayTag ShieldStatusTag = FGameplayTag::RequestGameplayTag(TEXT("Status.Buff.Shield"), false);
	const FGameplayTag ShieldCueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldBubble"), false);
	const FGameplayTag FillShieldCueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldUp"), false);

	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	FString CueSummary;
	for (const FGameplayTag& Tag : AssetTags)
	{
		const FString TagString = Tag.ToString();
		if (TagString.StartsWith(TEXT("GameplayCue.")))
		{
			CueSummary += TagString + TEXT(" ");
		}
	}

	UE_LOG(
		LogSpellRiseASCCues,
		Warning,
		TEXT("[GAS][GE Applied] ASC=%s GE=%s Handle=%s DurationPolicy=%d Cues=%s SourceASC=%s"),
		*GetNameSafe(this),
		*GetNameSafe(Spec.Def),
		*ActiveHandle.ToString(),
		Spec.Def ? static_cast<int32>(Spec.Def->DurationPolicy) : -1,
		CueSummary.IsEmpty() ? TEXT("<none>") : *CueSummary,
		*GetNameSafe(SourceASC));

	if (ShieldStatusTag.IsValid() && ShieldCueTag.IsValid() && Spec.Def && Spec.Def->GetGrantedTags().HasTagExact(ShieldStatusTag))
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = GetAvatarActor();
		CueParams.EffectCauser = GetAvatarActor();
		CueParams.SourceObject = Spec.Def;
		AddGameplayCue(ShieldCueTag, CueParams);
		UE_LOG(LogSpellRiseASCCues, Warning, TEXT("[GAS][ShieldCue] Added from GE lifecycle. ASC=%s"), *GetNameSafe(this));
	}

	// FillShield visual should follow GE lifecycle to avoid blink from ability-level execute/remove races.
	if (FillShieldCueTag.IsValid() && Spec.Def && GetNameSafe(Spec.Def).Contains(TEXT("GE_FillShield"), ESearchCase::IgnoreCase))
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = GetAvatarActor();
		CueParams.EffectCauser = GetAvatarActor();
		CueParams.SourceObject = Spec.Def;
		AddGameplayCue(FillShieldCueTag, CueParams);
		UE_LOG(LogSpellRiseASCCues, Warning, TEXT("[GAS][FillShieldCue] Added from GE lifecycle. ASC=%s GE=%s"), *GetNameSafe(this), *GetNameSafe(Spec.Def));
	}
}

void USpellRiseAbilitySystemComponent::OnGameplayEffectRemovedFromSelf(const FActiveGameplayEffect& ActiveEffect)
{
	const UGameplayEffect* Def = ActiveEffect.Spec.Def;
	const FGameplayTag ShieldStatusTag = FGameplayTag::RequestGameplayTag(TEXT("Status.Buff.Shield"), false);
	const FGameplayTag ShieldCueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldBubble"), false);
	const FGameplayTag FillShieldCueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldUp"), false);
	FGameplayTagContainer AssetTags;
	if (Def)
	{
		AssetTags.AppendTags(Def->GetGrantedTags());
	}

	FString CueSummary;
	for (const FGameplayTag& Tag : AssetTags)
	{
		const FString TagString = Tag.ToString();
		if (TagString.StartsWith(TEXT("GameplayCue.")))
		{
			CueSummary += TagString + TEXT(" ");
		}
	}

	UE_LOG(
		LogSpellRiseASCCues,
		Warning,
		TEXT("[GAS][GE Removed] ASC=%s GE=%s Cues=%s"),
		*GetNameSafe(this),
		*GetNameSafe(Def),
		CueSummary.IsEmpty() ? TEXT("<none>") : *CueSummary);

	if (ShieldStatusTag.IsValid() && ShieldCueTag.IsValid() && AssetTags.HasTagExact(ShieldStatusTag))
	{
		RemoveGameplayCue(ShieldCueTag);
		UE_LOG(LogSpellRiseASCCues, Warning, TEXT("[GAS][ShieldCue] Removed from GE lifecycle. ASC=%s"), *GetNameSafe(this));
	}

	if (FillShieldCueTag.IsValid() && Def && GetNameSafe(Def).Contains(TEXT("GE_FillShield"), ESearchCase::IgnoreCase))
	{
		RemoveGameplayCue(FillShieldCueTag);
		UE_LOG(LogSpellRiseASCCues, Warning, TEXT("[GAS][FillShieldCue] Removed from GE lifecycle. ASC=%s GE=%s"), *GetNameSafe(this), *GetNameSafe(Def));
	}
}

void USpellRiseAbilitySystemComponent::SR_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> MatchingHandles;
	GetAbilitySpecsFromInputTag(InputTag, MatchingHandles);

	for (const FGameplayAbilitySpecHandle& Handle : MatchingHandles)
	{
		InputPressedSpecHandles.AddUnique(Handle);
		InputHeldSpecHandles.AddUnique(Handle);
	}
}

void USpellRiseAbilitySystemComponent::SR_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> MatchingHandles;
	GetAbilitySpecsFromInputTag(InputTag, MatchingHandles);

	for (const FGameplayAbilitySpecHandle& Handle : MatchingHandles)
	{
		InputReleasedSpecHandles.AddUnique(Handle);
		InputHeldSpecHandles.Remove(Handle);
	}
}

void USpellRiseAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	InputPressedSpecHandles.AddUnique(Spec.Handle);
	InputHeldSpecHandles.AddUnique(Spec.Handle);
}

void USpellRiseAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	InputReleasedSpecHandles.AddUnique(Spec.Handle);
	InputHeldSpecHandles.Remove(Spec.Handle);
}

void USpellRiseAbilitySystemComponent::SR_ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

bool USpellRiseAbilitySystemComponent::AbilitySpecMatchesInputTag(const FGameplayAbilitySpec& Spec, const FGameplayTag& InputTag) const
{
	if (!InputTag.IsValid() || !Spec.Ability)
	{
		return false;
	}

	// Fonte principal: tags dinâmicas da spec
	if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
	{
		return true;
	}

	// Fallback para assets antigos
	if (const USpellRiseGameplayAbility* SpellRiseAbility = Cast<USpellRiseGameplayAbility>(Spec.Ability))
	{
		return SpellRiseAbility->AbilityInputTag.IsValid()
			&& SpellRiseAbility->AbilityInputTag.MatchesTagExact(InputTag);
	}

	return false;
}

void USpellRiseAbilitySystemComponent::GetAbilitySpecsFromInputTag(
	const FGameplayTag& InputTag,
	TArray<FGameplayAbilitySpecHandle>& OutSpecHandles) const
{
	OutSpecHandles.Reset();

	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilitySpecMatchesInputTag(Spec, InputTag))
		{
			OutSpecHandles.AddUnique(Spec.Handle);
		}
	}
}

void USpellRiseAbilitySystemComponent::MarkSpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Spec.InputPressed = true;

	if (Spec.IsActive())
	{
		Super::AbilitySpecInputPressed(Spec);
	}
}

void USpellRiseAbilitySystemComponent::MarkSpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Spec.InputPressed = false;

	if (Spec.IsActive())
	{
		Super::AbilitySpecInputReleased(Spec);
	}
}

bool USpellRiseAbilitySystemComponent::SR_TryActivateAbilityByInputTag(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		return false;
	}

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilitySpecMatchesInputTag(Spec, InputTag))
		{
			Spec.InputPressed = true;

			const bool bAllowRemoteActivation = true;
			return TryActivateAbility(Spec.Handle, bAllowRemoteActivation);
		}
	}

	return false;
}

USpellRiseGameplayAbility* USpellRiseAbilitySystemComponent::SR_GetSpellRiseAbilityForInputTag(FGameplayTag InputTag) const
{
	if (!InputTag.IsValid())
	{
		return nullptr;
	}

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilitySpecMatchesInputTag(Spec, InputTag))
		{
			return Cast<USpellRiseGameplayAbility>(Spec.Ability);
		}
	}

	return nullptr;
}

void USpellRiseAbilitySystemComponent::SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	// SpellRise does not support gameplay pause; keep processing input even if a caller
	// passes a paused state from legacy call sites.
	(void)bGamePaused;

	const FGameplayTag ComboAbilityTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.MeleeAttack.Combo"), false);
	const auto GetSpecPredictionKey = [](const FGameplayAbilitySpec* Spec) -> FPredictionKey
	{
		if (!Spec)
		{
			return FPredictionKey();
		}

		for (UGameplayAbility* Instance : Spec->GetAbilityInstances())
		{
			if (Instance && Instance->IsActive())
			{
				return Instance->GetCurrentActivationInfoRef().GetActivationPredictionKey();
			}
		}

		return FPredictionKey();
	};

	const auto IsComboSpec = [&](const FGameplayAbilitySpec* Spec) -> bool
	{
		if (!Spec || !Spec->Ability || !ComboAbilityTag.IsValid())
		{
			return false;
		}

		return Spec->Ability->AbilityTags.HasTagExact(ComboAbilityTag);
	};
	const auto IsNativeComboSpec = [&](const FGameplayAbilitySpec* Spec) -> bool
	{
		return Spec && Cast<USpellRiseGA_MeleeCombo>(Spec->Ability) != nullptr;
	};

	AActor* Avatar = GetAvatarActor();
	APawn* AvatarPawn = Cast<APawn>(Avatar);

	if (!AvatarPawn || !AvatarPawn->IsLocallyControlled())
	{
		SR_ClearAbilityInput();
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reserve(InputPressedSpecHandles.Num());

	TSet<FGameplayAbilitySpecHandle> PressedThisFrame;
	PressedThisFrame.Reserve(InputPressedSpecHandles.Num());

	// 1) Pressed
	{
		const TArray<FGameplayAbilitySpecHandle> PressedSnapshot = InputPressedSpecHandles;

		for (const FGameplayAbilitySpecHandle& Handle : PressedSnapshot)
		{
			PressedThisFrame.Add(Handle);

			FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
			if (!Spec || !Spec->Ability)
			{
				continue;
			}

			MarkSpecInputPressed(*Spec);

			if (Spec->IsActive())
			{
				if (!IsOwnerActorAuthoritative() && (Spec->Ability->bReplicateInputDirectly || IsComboSpec(Spec)))
				{
					ServerSetInputPressed(Spec->Handle);
				}

				// Ensure active abilities that rely on WaitInputPress receive replicated generic input events.
				const FPredictionKey PredictionKey = GetSpecPredictionKey(Spec);
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					Spec->Handle,
					PredictionKey);

				if (IsComboSpec(Spec) && !IsNativeComboSpec(Spec))
				{
					static const FGameplayTag ComboStartTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Start"), false);
					static const FGameplayTag ComboInputTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Input"), false);
					if (AActor* LocalAvatarActor = GetAvatarActor())
					{
						FGameplayEventData StartData;
						StartData.EventTag = ComboStartTag;
						StartData.Instigator = LocalAvatarActor;
						StartData.Target = LocalAvatarActor;
						UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(LocalAvatarActor, ComboStartTag, StartData);

						FGameplayEventData InputData;
						InputData.EventTag = ComboInputTag;
						InputData.Instigator = LocalAvatarActor;
						InputData.Target = LocalAvatarActor;
						UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(LocalAvatarActor, ComboInputTag, InputData);

						UE_LOG(LogSpellRiseASCCombo, Verbose, TEXT("[GAS][ComboInput] Active combo press forwarded. Ability=%s Handle=%s"), *GetNameSafe(Spec->Ability), *Spec->Handle.ToString());
					}
				}
			}
			else
			{
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}

	// 2) Held
	{
		const TArray<FGameplayAbilitySpecHandle> HeldSnapshot = InputHeldSpecHandles;

		for (const FGameplayAbilitySpecHandle& Handle : HeldSnapshot)
		{
			if (PressedThisFrame.Contains(Handle))
			{
				continue;
			}

			FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
			if (!Spec || !Spec->Ability)
			{
				continue;
			}

			Spec->InputPressed = true;

			if (Spec->IsActive())
			{
				// Native combo handles buffered presses explicitly; do not spam InputPressed every held tick.
				if (IsNativeComboSpec(Spec))
				{
					continue;
				}

				Super::AbilitySpecInputPressed(*Spec);
			}
		}
	}

	// 3) Activate
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec || !Spec->Ability)
		{
			continue;
		}

		Spec->InputPressed = true;

		const bool bAllowRemoteActivation = true;
		TryActivateAbility(Handle, bAllowRemoteActivation);
	}

	// 4) Released
	{
		const TArray<FGameplayAbilitySpecHandle> ReleasedSnapshot = InputReleasedSpecHandles;

		for (const FGameplayAbilitySpecHandle& Handle : ReleasedSnapshot)
		{
			FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
			if (!Spec || !Spec->Ability)
			{
				continue;
			}

			MarkSpecInputReleased(*Spec);

			if (Spec->IsActive())
			{
				if (!IsOwnerActorAuthoritative() && (Spec->Ability->bReplicateInputDirectly || IsComboSpec(Spec)))
				{
					ServerSetInputReleased(Spec->Handle);
				}

				// Mirror pressed behavior for abilities waiting on input release tasks.
				const FPredictionKey PredictionKey = GetSpecPredictionKey(Spec);
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputReleased,
					Spec->Handle,
					PredictionKey);
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void USpellRiseAbilitySystemComponent::SetTagRelationshipMapping(USpellRiseAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void USpellRiseAbilitySystemComponent::GetAdditionalActivationTagRequirements(
	const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer& OutActivationRequired,
	FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(
			AbilityTags,
			&OutActivationRequired,
			&OutActivationBlocked);
	}
}

void USpellRiseAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(
	const FGameplayTagContainer& AbilityTags,
	UGameplayAbility* RequestingAbility,
	bool bEnableBlockTags,
	const FGameplayTagContainer& BlockTags,
	bool bExecuteCancelTags,
	const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(
			AbilityTags,
			&ModifiedBlockTags,
			&ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(
		AbilityTags,
		RequestingAbility,
		bEnableBlockTags,
		ModifiedBlockTags,
		bExecuteCancelTags,
		ModifiedCancelTags);
}

void USpellRiseAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	AActor* Avatar = GetAvatarActor();
	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Avatar);
	if (!Character)
	{
		return;
	}

	bool bAbilitiesChanged = (LastActivatableAbilities.Num() != ActivatableAbilities.Items.Num());

	if (!bAbilitiesChanged)
	{
		for (int32 i = 0; i < ActivatableAbilities.Items.Num(); ++i)
		{
			if (LastActivatableAbilities.IsValidIndex(i) &&
				LastActivatableAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
			{
				bAbilitiesChanged = true;
				break;
			}
		}
	}

	if (bAbilitiesChanged)
	{
		Character->SendAbilitiesChangedEvent();
		LastActivatableAbilities = ActivatableAbilities.Items;
	}
}

