#include "SpellRise/UI/SpellRiseInventoryBlueprintLibrary.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"
#include "SpellRise/Inventory/SpellRiseItemDefinitionResolver.h"
#include "SpellRise/Inventory/SpellRiseItemDefinition.h"
#include "SpellRise/UI/SpellRiseInventoryViewModelComponent.h"

namespace SpellRiseInventoryBlueprintLibrary
{
	template <typename ComponentType>
	ComponentType* FindComponentFromTarget(UObject* Target, const int32 Depth = 0)
	{
		if (!Target || Depth > 4)
		{
			return nullptr;
		}

		if (ComponentType* Component = Cast<ComponentType>(Target))
		{
			return Component;
		}

		if (UActorComponent* ActorComponent = Cast<UActorComponent>(Target))
		{
			if (AActor* Owner = ActorComponent->GetOwner())
			{
				return FindComponentFromTarget<ComponentType>(Owner, Depth + 1);
			}
		}

		if (AActor* Actor = Cast<AActor>(Target))
		{
			if (ComponentType* Component = Actor->FindComponentByClass<ComponentType>())
			{
				return Component;
			}

			if (AController* Controller = Cast<AController>(Actor))
			{
				if (APlayerState* PlayerState = Controller->PlayerState)
				{
					if (ComponentType* Component = PlayerState->FindComponentByClass<ComponentType>())
					{
						return Component;
					}
				}
				if (APawn* Pawn = Controller->GetPawn())
				{
					if (ComponentType* Component = Pawn->FindComponentByClass<ComponentType>())
					{
						return Component;
					}
					if (APlayerState* PlayerState = Pawn->GetPlayerState())
					{
						if (ComponentType* Component = PlayerState->FindComponentByClass<ComponentType>())
						{
							return Component;
						}
					}
				}
			}

			if (APawn* Pawn = Cast<APawn>(Actor))
			{
				if (APlayerState* PlayerState = Pawn->GetPlayerState())
				{
					if (ComponentType* Component = PlayerState->FindComponentByClass<ComponentType>())
					{
						return Component;
					}
				}
				if (AController* Controller = Pawn->GetController())
				{
					if (ComponentType* Component = Controller->FindComponentByClass<ComponentType>())
					{
						return Component;
					}
					if (APlayerState* PlayerState = Controller->PlayerState)
					{
						if (ComponentType* Component = PlayerState->FindComponentByClass<ComponentType>())
						{
							return Component;
						}
					}
				}
			}

			if (AActor* Owner = Actor->GetOwner())
			{
				return FindComponentFromTarget<ComponentType>(Owner, Depth + 1);
			}
		}

		return nullptr;
	}
}

USpellRiseItemDefinition* USpellRiseInventoryBlueprintLibrary::ResolveItemDefinition(const FPrimaryAssetId DefinitionId)
{
	if (!DefinitionId.IsValid())
	{
		return nullptr;
	}

	return SpellRiseItemDefinitionResolver::ResolveItemDefinition(DefinitionId);
}

USpellRiseInventoryComponent* USpellRiseInventoryBlueprintLibrary::GetInventoryComponentFromTarget(UObject* Target)
{
	return SpellRiseInventoryBlueprintLibrary::FindComponentFromTarget<USpellRiseInventoryComponent>(Target);
}

USpellRiseInventoryViewModelComponent* USpellRiseInventoryBlueprintLibrary::GetInventoryViewModelFromTarget(UObject* Target)
{
	return SpellRiseInventoryBlueprintLibrary::FindComponentFromTarget<USpellRiseInventoryViewModelComponent>(Target);
}
