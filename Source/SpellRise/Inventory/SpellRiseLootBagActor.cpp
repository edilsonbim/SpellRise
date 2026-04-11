// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Inventory/SpellRiseLootBagActor.h"

#include "Containers/StringConv.h"
#include "Net/UnrealNetwork.h"

#include "InteractableComponent.h"
#include "InventoryComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseLootBagActor, Log, All);

ASpellRiseLootBagActor::ASpellRiseLootBagActor()
{
	bReplicates = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(5.0f);
	SetMinNetUpdateFrequency(2.0f);
}

void ASpellRiseLootBagActor::BeginPlay()
{
	Super::BeginPlay();

	CachedInventoryComponent = FindComponentByClass<UNarrativeInventoryComponent>();
	ResolveLootInteractable();
	ApplyDeadPlayerNameToInteractable();
}

void ASpellRiseLootBagActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpellRiseLootBagActor, DeadPlayerDisplayName);
}

void ASpellRiseLootBagActor::SetDeadPlayerDisplayName(const FString& NewDeadPlayerDisplayName)
{
	if (!HasAuthority())
	{
		return;
	}

	const FString SanitizedDisplayName = SanitizeDisplayNameForNet(NewDeadPlayerDisplayName);
	if (DeadPlayerDisplayName.Equals(SanitizedDisplayName, ESearchCase::CaseSensitive))
	{
		return;
	}

	DeadPlayerDisplayName = SanitizedDisplayName;
	OnRep_DeadPlayerDisplayName();
	ForceNetUpdate();
}

void ASpellRiseLootBagActor::OnRep_DeadPlayerDisplayName()
{
	ApplyDeadPlayerNameToInteractable();
}

void ASpellRiseLootBagActor::ApplyDeadPlayerNameToInteractable()
{
	UNarrativeInteractableComponent* LootInteractable = ResolveLootInteractable();
	if (!LootInteractable)
	{
		return;
	}

	if (DeadPlayerDisplayName.IsEmpty())
	{
		return;
	}

	LootInteractable->SetInteractableNameText(FText::FromString(DeadPlayerDisplayName));
}

FString ASpellRiseLootBagActor::SanitizeDisplayNameForNet(const FString& InDisplayName) const
{
	FString Sanitized = InDisplayName.TrimStartAndEnd();
	Sanitized.ReplaceInline(TEXT("\n"), TEXT(" "));
	Sanitized.ReplaceInline(TEXT("\r"), TEXT(" "));
	Sanitized.ReplaceInline(TEXT("\t"), TEXT(" "));

	if (Sanitized.IsEmpty())
	{
		Sanitized = TEXT("Unknown");
	}

	const int32 ClampedMaxBytes = FMath::Clamp(DeadPlayerNameMaxUtf8Bytes, 16, 256);
	if (FTCHARToUTF8(*Sanitized).Length() <= ClampedMaxBytes)
	{
		return Sanitized;
	}

	FString Truncated;
	Truncated.Reserve(Sanitized.Len());
	for (int32 CharIndex = 0; CharIndex < Sanitized.Len(); ++CharIndex)
	{
		Truncated.AppendChar(Sanitized[CharIndex]);
		if (FTCHARToUTF8(*Truncated).Length() > ClampedMaxBytes)
		{
			Truncated.LeftInline(FMath::Max(0, Truncated.Len() - 1), EAllowShrinking::No);
			break;
		}
	}

	if (Truncated.IsEmpty())
	{
		Truncated = TEXT("Unknown");
	}

	return Truncated;
}

UNarrativeInteractableComponent* ASpellRiseLootBagActor::ResolveLootInteractable()
{
	if (CachedLootInteractable)
	{
		return CachedLootInteractable;
	}

	TArray<UNarrativeInteractableComponent*> InteractableComponents;
	GetComponents<UNarrativeInteractableComponent>(InteractableComponents);

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
