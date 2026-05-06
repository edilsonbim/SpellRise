// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRiseAbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_MeleeCombo.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseASCCombo, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseASCCues, Log, All);

namespace
{
	FGameplayTag GetPrimaryAbilityInputTag()
	{
		static const FGameplayTag PrimaryTag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Primary"), false);
		return PrimaryTag;
	}

	bool HasCooldownTag(const FGameplayTagContainer& Tags)
	{
		static const FGameplayTag CooldownRootTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown"), false);
		return CooldownRootTag.IsValid() && Tags.HasTag(CooldownRootTag);
	}

	bool GameplayEffectSpecHasCooldownTag(const FGameplayEffectSpec& Spec)
	{
		FGameplayTagContainer Tags;
		Spec.GetAllAssetTags(Tags);
		if (Spec.Def)
		{
			Tags.AppendTags(Spec.Def->GetGrantedTags());
		}
		return HasCooldownTag(Tags);
	}
}

USpellRiseAbilitySystemComponent::USpellRiseAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
	SelectedSpellSpecHandle = FGameplayAbilitySpecHandle();
}

void USpellRiseAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();


	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &USpellRiseAbilitySystemComponent::OnGameplayEffectAppliedToSelf);
	OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &USpellRiseAbilitySystemComponent::OnActiveGameplayEffectAddedToSelf);
	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &USpellRiseAbilitySystemComponent::OnGameplayEffectRemovedFromSelf);
}

void USpellRiseAbilitySystemComponent::NotifyAbilityCommit(UGameplayAbility* Ability)
{
	Super::NotifyAbilityCommit(Ability);
	BroadcastEquipmentAbilityStateChanged();
}

void USpellRiseAbilitySystemComponent::NotifyAbilityActivated(
	const FGameplayAbilitySpecHandle Handle,
	UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);
	BroadcastEquipmentAbilityStateChanged();
}

void USpellRiseAbilitySystemComponent::NotifyAbilityEnded(
	const FGameplayAbilitySpecHandle Handle,
	UGameplayAbility* Ability,
	const bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
	BroadcastEquipmentAbilityStateChanged();
}

void USpellRiseAbilitySystemComponent::BroadcastEquipmentAbilityStateChanged() const
{
	AActor* CurrentAvatarActor = GetAvatarActor();
	if (!CurrentAvatarActor)
	{
		return;
	}

	if (USpellRiseEquipmentManagerComponent* EquipmentManager = CurrentAvatarActor->FindComponentByClass<USpellRiseEquipmentManagerComponent>())
	{
		EquipmentManager->OnHUDEquipmentSlotsChanged.Broadcast();
	}
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
		return false;
	}

	const FName CurrentSection = SelectedAnimInstance->Montage_GetCurrentSection(ActiveMontage);
	const int32 CurrentSectionIndex = ActiveMontage->GetSectionIndex(CurrentSection);
	const int32 SectionCount = ActiveMontage->CompositeSections.Num();
	if (CurrentSectionIndex == INDEX_NONE || CurrentSectionIndex + 1 >= SectionCount)
	{
		return false;
	}

	const FName NextSection = ActiveMontage->CompositeSections[CurrentSectionIndex + 1].SectionName;
	if (NextSection.IsNone() || NextSection == CurrentSection)
	{
		return false;
	}

	SelectedAnimInstance->Montage_JumpToSection(NextSection, ActiveMontage);
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

	if (ShieldStatusTag.IsValid() && ShieldCueTag.IsValid() && Spec.Def && Spec.Def->GetGrantedTags().HasTagExact(ShieldStatusTag))
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = GetAvatarActor();
		CueParams.EffectCauser = GetAvatarActor();
		CueParams.SourceObject = Spec.Def;
		AddGameplayCue(ShieldCueTag, CueParams);
	}


	if (FillShieldCueTag.IsValid() && Spec.Def && GetNameSafe(Spec.Def).Contains(TEXT("GE_FillShield"), ESearchCase::IgnoreCase))
	{
		FGameplayCueParameters CueParams;
		CueParams.Instigator = GetAvatarActor();
		CueParams.EffectCauser = GetAvatarActor();
		CueParams.SourceObject = Spec.Def;
		AddGameplayCue(FillShieldCueTag, CueParams);
	}

	if (GameplayEffectSpecHasCooldownTag(Spec))
	{
		BroadcastEquipmentAbilityStateChanged();
	}
}

void USpellRiseAbilitySystemComponent::OnActiveGameplayEffectAddedToSelf(
	UAbilitySystemComponent* SourceASC,
	const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle ActiveHandle)
{
	if (GameplayEffectSpecHasCooldownTag(Spec))
	{
		BroadcastEquipmentAbilityStateChanged();
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
		ActiveEffect.Spec.GetAllAssetTags(AssetTags);
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

	if (ShieldStatusTag.IsValid() && ShieldCueTag.IsValid() && AssetTags.HasTagExact(ShieldStatusTag))
	{
		RemoveGameplayCue(ShieldCueTag);
	}

	if (FillShieldCueTag.IsValid() && Def && GetNameSafe(Def).Contains(TEXT("GE_FillShield"), ESearchCase::IgnoreCase))
	{
		RemoveGameplayCue(FillShieldCueTag);
	}

	if (HasCooldownTag(AssetTags))
	{
		BroadcastEquipmentAbilityStateChanged();
	}
}

void USpellRiseAbilitySystemComponent::SR_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	if (InputTag.MatchesTagExact(GetPrimaryAbilityInputTag()))
	{
		if (FGameplayAbilitySpec* SelectedSpec = FindAbilitySpecFromHandle(SelectedSpellSpecHandle))
		{
			InputPressedSpecHandles.AddUnique(SelectedSpec->Handle);
			InputHeldSpecHandles.AddUnique(SelectedSpec->Handle);
			return;
		}
	}

	TArray<FGameplayAbilitySpecHandle> MatchingHandles;
	GetAbilitySpecsFromInputTag(InputTag, MatchingHandles);

	for (const FGameplayAbilitySpecHandle& Handle : MatchingHandles)
	{
		const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		const USpellRiseGameplayAbility* SpellAbility = Spec ? Cast<USpellRiseGameplayAbility>(Spec->Ability) : nullptr;
		if (SpellAbility && !SpellAbility->FiresFromOwnInputTag())
		{
			SelectedSpellSpecHandle = Handle;
			continue;
		}

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

	if (InputTag.MatchesTagExact(GetPrimaryAbilityInputTag()))
	{
		if (FGameplayAbilitySpec* SelectedSpec = FindAbilitySpecFromHandle(SelectedSpellSpecHandle))
		{
			if (!InputHeldSpecHandles.Contains(SelectedSpec->Handle))
			{
				return;
			}

			InputReleasedSpecHandles.AddUnique(SelectedSpec->Handle);
			InputHeldSpecHandles.Remove(SelectedSpec->Handle);
			return;
		}
	}

	TArray<FGameplayAbilitySpecHandle> MatchingHandles;
	GetAbilitySpecsFromInputTag(InputTag, MatchingHandles);

	for (const FGameplayAbilitySpecHandle& Handle : MatchingHandles)
	{
		const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!InputHeldSpecHandles.Contains(Handle))
		{
			continue;
		}

		// Spells com bFireOnAbilityInput=false sao armadas pelo slot mas disparadas pelo Primary;
		// soltar a tecla do slot nao deve sintetizar InputReleased (evita cancelar cast/channel ao soltar SkillN).
		if (const USpellRiseGameplayAbility* SpellAbility = Spec ? Cast<USpellRiseGameplayAbility>(Spec->Ability) : nullptr)
		{
			if (SpellAbility && !SpellAbility->FiresFromOwnInputTag())
			{
				continue;
			}
		}

		InputReleasedSpecHandles.AddUnique(Handle);
		InputHeldSpecHandles.Remove(Handle);
	}
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


	if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
	{
		return true;
	}


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


	if (!IsOwnerActorAuthoritative() && Spec.Ability)
	{
		if (Spec.Ability->GetNetExecutionPolicy() != EGameplayAbilityNetExecutionPolicy::LocalOnly)
		{
			ServerSetInputPressed(Spec.Handle);
		}
	}

	if (Spec.IsActive())
	{
		Super::AbilitySpecInputPressed(Spec);
	}
}

void USpellRiseAbilitySystemComponent::MarkSpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Spec.InputPressed = false;


	if (!IsOwnerActorAuthoritative() && Spec.Ability)
	{
		if (Spec.Ability->GetNetExecutionPolicy() != EGameplayAbilityNetExecutionPolicy::LocalOnly)
		{
			ServerSetInputReleased(Spec.Handle);
		}
	}

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

	if (InputTag.MatchesTagExact(GetPrimaryAbilityInputTag()))
	{
		if (FGameplayAbilitySpec* SelectedSpec = FindAbilitySpecFromHandle(SelectedSpellSpecHandle))
		{
			SelectedSpec->InputPressed = true;
			return TryActivateAbility(SelectedSpec->Handle, true);
		}
	}

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilitySpecMatchesInputTag(Spec, InputTag))
		{
			if (const USpellRiseGameplayAbility* SpellAbility = Cast<USpellRiseGameplayAbility>(Spec.Ability))
			{
				if (!SpellAbility->FiresFromOwnInputTag())
				{
					SelectedSpellSpecHandle = Spec.Handle;
					return true;
				}
			}

			Spec.InputPressed = true;

			const bool bAllowRemoteActivation = true;
			const bool bActivated = TryActivateAbility(Spec.Handle, bAllowRemoteActivation);
			if (!bActivated)
			{
				FGameplayTagContainer FailureTags;
				const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
				if (!Spec.Ability->CanActivateAbility(Spec.Handle, ActorInfo, nullptr, nullptr, &FailureTags))
				{
				}
			}
			return bActivated;
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

	if (InputTag.MatchesTagExact(GetPrimaryAbilityInputTag()))
	{
		if (USpellRiseGameplayAbility* SelectedAbility = SR_GetSelectedSpellAbility())
		{
			return SelectedAbility;
		}
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

USpellRiseGameplayAbility* USpellRiseAbilitySystemComponent::SR_GetSelectedSpellAbility() const
{
	const FGameplayAbilitySpec* SelectedSpec = FindAbilitySpecFromHandle(SelectedSpellSpecHandle);
	return SelectedSpec ? Cast<USpellRiseGameplayAbility>(SelectedSpec->Ability) : nullptr;
}

void USpellRiseAbilitySystemComponent::SR_ClearSelectedSpellAbility()
{
	SelectedSpellSpecHandle = FGameplayAbilitySpecHandle();
}

void USpellRiseAbilitySystemComponent::SR_SetSelectedSpellAbilityByInputTag(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		SR_ClearSelectedSpellAbility();
		return;
	}

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!AbilitySpecMatchesInputTag(Spec, InputTag))
		{
			continue;
		}

		if (const USpellRiseGameplayAbility* SpellAbility = Cast<USpellRiseGameplayAbility>(Spec.Ability))
		{
			if (!SpellAbility->FiresFromOwnInputTag())
			{
				SelectedSpellSpecHandle = Spec.Handle;
				return;
			}
		}
	}

	SR_ClearSelectedSpellAbility();
}

bool USpellRiseAbilitySystemComponent::SR_IsSelectedSpellAbilityHandle(FGameplayAbilitySpecHandle AbilityHandle) const
{
	return AbilityHandle.IsValid()
		&& SelectedSpellSpecHandle.IsValid()
		&& AbilityHandle == SelectedSpellSpecHandle;
}

void USpellRiseAbilitySystemComponent::SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{

	(void)DeltaTime;
	(void)bGamePaused;

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

	const auto LogActivationFailure = [&](const FGameplayAbilitySpec* Spec)
	{
		if (!Spec || !Spec->Ability)
		{
			return;
		}

		FGameplayTagContainer FailureTags;
		const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
		const bool bCanActivate = Spec->Ability->CanActivateAbility(
			Spec->Handle,
			ActorInfo,
			nullptr,
			nullptr,
			&FailureTags);
		if (bCanActivate)
		{
			return;
		}

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

				const FPredictionKey PredictionKey = GetSpecPredictionKey(Spec);
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					Spec->Handle,
					PredictionKey);
			}
			else
			{
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}


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

				continue;
			}
		}
	}


	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec || !Spec->Ability)
		{
			continue;
		}

		Spec->InputPressed = true;

		const bool bAllowRemoteActivation = true;
		const bool bActivated = TryActivateAbility(Handle, bAllowRemoteActivation);
		if (!bActivated)
		{
			LogActivationFailure(Spec);
		}
	}


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
