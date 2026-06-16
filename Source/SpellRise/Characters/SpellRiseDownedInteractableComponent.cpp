#include "SpellRise/Characters/SpellRiseDownedInteractableComponent.h"

#include "InteractionComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseDownedInteraction, Log, All);

USpellRiseDownedInteractableComponent::USpellRiseDownedInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InteractionDistance = 320.f;
	InteractionTime = 5.f;
	InteractableNameText = FText::FromString(TEXT("Player"));
	InteractableActionText = FText::FromString(TEXT("Reviver"));
}

void USpellRiseDownedInteractableComponent::OnRegister()
{
	Super::OnRegister();

	InteractableNameText = FText::FromString(TEXT("Player"));
	if (InteractionMode == ESpellRiseDownedInteractionMode::Finish)
	{
		InteractionTime = 3.f;
		InteractableActionText = FText::FromString(TEXT("Finalizar"));
		return;
	}

	InteractionTime = 5.f;
	InteractableActionText = FText::FromString(TEXT("Reviver"));
}

bool USpellRiseDownedInteractableComponent::CanInteract_Implementation(
	APawn* Interactor,
	UNarrativeInteractionComponent* InteractionComp,
	FText& OutErrorText)
{
	ASpellRiseCharacterBase* DownedCharacter = Cast<ASpellRiseCharacterBase>(GetOwner());
	ASpellRiseCharacterBase* InteractorCharacter = Cast<ASpellRiseCharacterBase>(Interactor);
	if (!DownedCharacter || !InteractorCharacter)
	{
		OutErrorText = FText::FromString(TEXT("Invalid target."));
		UE_LOG(LogSpellRiseDownedInteraction, Verbose, TEXT("[DownedInteract][Reject] Reason=invalid_target Owner=%s Interactor=%s"), *GetNameSafe(GetOwner()), *GetNameSafe(Interactor));
		return false;
	}

	if (DownedCharacter == InteractorCharacter)
	{
		OutErrorText = FText::FromString(TEXT("Invalid target."));
		UE_LOG(LogSpellRiseDownedInteraction, Verbose, TEXT("[DownedInteract][Reject] Reason=self_interact Character=%s"), *GetNameSafe(DownedCharacter));
		return false;
	}

	if (!DownedCharacter->IsDowned() || DownedCharacter->IsDead())
	{
		OutErrorText = FText::FromString(TEXT("Target is not downed."));
		UE_LOG(
			LogSpellRiseDownedInteraction,
			Verbose,
			TEXT("[DownedInteract][Reject] Reason=target_not_downed Target=%s Downed=%d Dead=%d"),
			*GetNameSafe(DownedCharacter),
			DownedCharacter->IsDowned() ? 1 : 0,
			DownedCharacter->IsDead() ? 1 : 0);
		return false;
	}

	if (InteractorCharacter->IsDead() || InteractorCharacter->IsDowned())
	{
		OutErrorText = FText::FromString(TEXT("You cannot interact right now."));
		UE_LOG(
			LogSpellRiseDownedInteraction,
			Verbose,
			TEXT("[DownedInteract][Reject] Reason=interactor_unavailable Interactor=%s Downed=%d Dead=%d"),
			*GetNameSafe(InteractorCharacter),
			InteractorCharacter->IsDowned() ? 1 : 0,
			InteractorCharacter->IsDead() ? 1 : 0);
		return false;
	}

	UE_LOG(LogSpellRiseDownedInteraction, Verbose, TEXT("[DownedInteract][CanInteract] Target=%s Interactor=%s Mode=%d"), *GetNameSafe(DownedCharacter), *GetNameSafe(InteractorCharacter), static_cast<int32>(InteractionMode));
	return Super::CanInteract_Implementation(Interactor, InteractionComp, OutErrorText);
}

void USpellRiseDownedInteractableComponent::OnInteract_Implementation(
	APawn* Interactor,
	UNarrativeInteractionComponent* InteractionComp)
{
	ASpellRiseCharacterBase* DownedCharacter = Cast<ASpellRiseCharacterBase>(GetOwner());
	ASpellRiseCharacterBase* InteractorCharacter = Cast<ASpellRiseCharacterBase>(Interactor);
	if (!DownedCharacter || !InteractorCharacter || !DownedCharacter->HasAuthority())
	{
		UE_LOG(LogSpellRiseDownedInteraction, Warning, TEXT("[DownedInteract][InteractRejected] Reason=invalid_authority Target=%s Interactor=%s Authority=%d"), *GetNameSafe(DownedCharacter), *GetNameSafe(InteractorCharacter), DownedCharacter && DownedCharacter->HasAuthority() ? 1 : 0);
		return;
	}

	if (InteractionMode == ESpellRiseDownedInteractionMode::Finish)
	{
		DownedCharacter->FinishDownedByInteractor_Server(InteractorCharacter);
		return;
	}

	const bool bRevived = DownedCharacter->ReviveFromDowned_Server(InteractorCharacter);
	UE_LOG(LogSpellRiseDownedInteraction, Log, TEXT("[DownedInteract][Revive] Target=%s Interactor=%s Success=%d"), *GetNameSafe(DownedCharacter), *GetNameSafe(InteractorCharacter), bRevived ? 1 : 0);
}
