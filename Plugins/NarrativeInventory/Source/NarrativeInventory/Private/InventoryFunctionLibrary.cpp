// Copyright Narrative Tools 2025. 

#include "InventoryFunctionLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "InventoryComponent.h"
#include "NarrativeItem.h"
#include "Algo/Sort.h"

DEFINE_LOG_CATEGORY_STATIC(LogNarrativeInventoryFunctionLibrary, Log, All);

namespace
{
	static UNarrativeInventoryComponent* ResolveBestInventoryComponent(AActor* Target)
	{
		if (!Target)
		{
			return nullptr;
		}

		TArray<UNarrativeInventoryComponent*> InventoryComponents;
		Target->GetComponents<UNarrativeInventoryComponent>(InventoryComponents);

		if (InventoryComponents.Num() <= 0)
		{
			return nullptr;
		}

		if (InventoryComponents.Num() == 1)
		{
			return InventoryComponents[0];
		}

		// Deterministic order to avoid client/server choosing different components.
		Algo::Sort(InventoryComponents, [](const UNarrativeInventoryComponent* A, const UNarrativeInventoryComponent* B)
		{
			return GetNameSafe(A) < GetNameSafe(B);
		});

		auto ScoreInventory = [](const UNarrativeInventoryComponent* Inventory) -> int32
		{
			if (!Inventory)
			{
				return -1000;
			}

			const FString Name = Inventory->GetName();
			int32 Score = 0;

			if (Name.Equals(TEXT("NarrativeInventoryComponent"), ESearchCase::IgnoreCase))
			{
				Score += 100;
			}
			else if (Name.Equals(TEXT("InventoryComponent"), ESearchCase::IgnoreCase))
			{
				Score += 90;
			}
			else if (Name.Contains(TEXT("inventory"), ESearchCase::IgnoreCase))
			{
				Score += 40;
			}

			if (Inventory->GetIsReplicated())
			{
				Score += 20;
			}

			Score += FMath::Clamp(Inventory->GetCapacity(), 0, 1000) / 20;
			return Score;
		};

		UNarrativeInventoryComponent* BestInventory = InventoryComponents[0];
		int32 BestScore = ScoreInventory(BestInventory);
		for (int32 Index = 1; Index < InventoryComponents.Num(); ++Index)
		{
			UNarrativeInventoryComponent* Candidate = InventoryComponents[Index];
			const int32 CandidateScore = ScoreInventory(Candidate);
			if (CandidateScore > BestScore)
			{
				BestInventory = Candidate;
				BestScore = CandidateScore;
			}
		}

		TArray<FString> ComponentNames;
		ComponentNames.Reserve(InventoryComponents.Num());
		for (const UNarrativeInventoryComponent* Inventory : InventoryComponents)
		{
			ComponentNames.Add(GetNameSafe(Inventory));
		}

		UE_LOG(
			LogNarrativeInventoryFunctionLibrary,
			Warning,
			TEXT("[NarrativeInventory] Multiple inventory components found on %s. Components=[%s] Selected=%s"),
			*GetNameSafe(Target),
			*FString::Join(ComponentNames, TEXT(", ")),
			*GetNameSafe(BestInventory));

		return BestInventory;
	}
}

class UNarrativeInventoryComponent* UInventoryFunctionLibrary::GetInventoryComponentFromTarget(AActor* Target)
{
	if (!Target)
	{
		return nullptr;
	}

	if (UNarrativeInventoryComponent* InventoryComp = ResolveBestInventoryComponent(Target))
	{
		return InventoryComp;
	}

	//Try player state, then pawn, then controller
	if (const APawn* OwningPawn = Cast<APawn>(Target))
	{
		if (const APlayerState* PlayerState = OwningPawn->GetPlayerState<APlayerState>())
		{
			if (UNarrativeInventoryComponent* InventoryComp = ResolveBestInventoryComponent(const_cast<APlayerState*>(PlayerState)))
			{
				return InventoryComp;
			}
		}

		if (const AController* OwningController = OwningPawn->GetController())
		{
			if (UNarrativeInventoryComponent* InventoryComp = ResolveBestInventoryComponent(const_cast<AController*>(OwningController)))
			{
				return InventoryComp;
			}
		}
	}
	else if (const APlayerController* OwningController = Cast<APlayerController>(Target))
	{
		if (OwningController->GetPawn())
		{
			if (const APlayerState* PlayerState = OwningController->GetPlayerState<APlayerState>())
			{
				if (UNarrativeInventoryComponent* InventoryComp = ResolveBestInventoryComponent(const_cast<APlayerState*>(PlayerState)))
				{
					return InventoryComp;
				}
			}

			return ResolveBestInventoryComponent(OwningController->GetPawn());
		}
	}

	return nullptr;
}


TArray<class UNarrativeItem*> UInventoryFunctionLibrary::SortItemArrayAlphabetical(TArray<class UNarrativeItem*> InItems, const bool bReverse)
{

	TArray<class UNarrativeItem*> RetItems = InItems;

	//Sort the replies by their Y position in the graph
	if (bReverse)
	{
		RetItems.Sort([](const UNarrativeItem& ItemA, const UNarrativeItem& ItemB) {
			return ItemA.DisplayName.ToString() < ItemB.DisplayName.ToString();
			});
	}
	else
	{
		RetItems.Sort([](const UNarrativeItem& ItemA, const UNarrativeItem& ItemB) {
			return ItemA.DisplayName.ToString() > ItemB.DisplayName.ToString();
			});
	}

	return RetItems;
}

TArray<class UNarrativeItem*> UInventoryFunctionLibrary::SortItemArrayWeight(TArray<class UNarrativeItem*> InItems, const bool bReverse)
{
	TArray<class UNarrativeItem*> RetItems = InItems;

	//Sort the replies by their Y position in the graph
	if (bReverse)
	{
		RetItems.Sort([](const UNarrativeItem& ItemA, const UNarrativeItem& ItemB) {
			return ItemA.GetStackWeight() > ItemB.GetStackWeight();
			});
	}
	else
	{
		RetItems.Sort([](const UNarrativeItem& ItemA, const UNarrativeItem& ItemB) {
			return ItemA.GetStackWeight() < ItemB.GetStackWeight();
			});
	}

	
	return RetItems;
}
