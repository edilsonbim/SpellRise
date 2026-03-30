#include "SpellRise/Inventory/SpellRiseLootBagNameComponent.h"

#include "Net/UnrealNetwork.h"

#include "InteractableComponent.h"

USpellRiseLootBagNameComponent::USpellRiseLootBagNameComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void USpellRiseLootBagNameComponent::BeginPlay()
{
	Super::BeginPlay();
	ApplyDisplayNameToInteractable();
}

void USpellRiseLootBagNameComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellRiseLootBagNameComponent, DeadPlayerDisplayName);
}

void USpellRiseLootBagNameComponent::SetDeadPlayerDisplayName_Server(const FString& InName)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (DeadPlayerDisplayName.Equals(InName, ESearchCase::CaseSensitive))
	{
		return;
	}

	DeadPlayerDisplayName = InName;
	OnRep_DeadPlayerDisplayName();
	GetOwner()->ForceNetUpdate();
}

void USpellRiseLootBagNameComponent::OnRep_DeadPlayerDisplayName()
{
	ApplyDisplayNameToInteractable();
}

void USpellRiseLootBagNameComponent::ApplyDisplayNameToInteractable()
{
	if (DeadPlayerDisplayName.IsEmpty())
	{
		return;
	}

	if (UNarrativeInteractableComponent* Interactable = ResolveLootInteractable())
	{
		Interactable->SetInteractableNameText(FText::FromString(DeadPlayerDisplayName));
	}
}

UNarrativeInteractableComponent* USpellRiseLootBagNameComponent::ResolveLootInteractable()
{
	if (CachedLootInteractable)
	{
		return CachedLootInteractable;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	TArray<UNarrativeInteractableComponent*> InteractableComponents;
	OwnerActor->GetComponents<UNarrativeInteractableComponent>(InteractableComponents);

	for (UNarrativeInteractableComponent* Interactable : InteractableComponents)
	{
		if (Interactable && Interactable->GetFName() == TEXT("Interactable_Loot"))
		{
			CachedLootInteractable = Interactable;
			return CachedLootInteractable;
		}
	}

	for (UNarrativeInteractableComponent* Interactable : InteractableComponents)
	{
		if (Interactable)
		{
			CachedLootInteractable = Interactable;
			return CachedLootInteractable;
		}
	}

	return nullptr;
}
