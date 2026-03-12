#include "SpellRiseAbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h"

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
	AActor* Avatar = GetAvatarActor();
	APawn* AvatarPawn = Cast<APawn>(Avatar);

	if (!AvatarPawn || !AvatarPawn->IsLocallyControlled())
	{
		SR_ClearAbilityInput();
		return;
	}

	if (bGamePaused)
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
				if (!IsOwnerActorAuthoritative() && Spec->Ability->bReplicateInputDirectly)
				{
					ServerSetInputPressed(Spec->Handle);
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
				if (!IsOwnerActorAuthoritative() && Spec->Ability->bReplicateInputDirectly)
				{
					ServerSetInputReleased(Spec->Handle);
				}
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