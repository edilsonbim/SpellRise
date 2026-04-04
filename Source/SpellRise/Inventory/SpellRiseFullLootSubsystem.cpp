#include "SpellRise/Inventory/SpellRiseFullLootSubsystem.h"

#include "Components/PrimitiveComponent.h"
#include "Containers/StringConv.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/UnrealType.h"

#include "InteractableComponent.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "NarrativeItem.h"
#include "NavigationMarkerComponent.h"
#include "SpellRise/Inventory/SpellRiseLootBagActor.h"
#include "SpellRise/Inventory/SpellRiseLootBagNameComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseFullLoot, Log, All);

namespace
{
	constexpr TCHAR DefaultNarrativeBagClassPath[] = TEXT("/Game/UI/InventorySystem/BP_Bag.BP_Bag_C");
	constexpr TCHAR LegacyBagClassPath[] = TEXT("/Game/UI/InventorySystem/BP_Storage_Bag.BP_Storage_Bag_C");

	struct FPendingNarrativeTransfer
	{
		TWeakObjectPtr<UNarrativeInventoryComponent> SourceInventory;
		TWeakObjectPtr<UNarrativeItem> SourceItem;
		TSubclassOf<UNarrativeItem> ItemClass = nullptr;
		int32 Quantity = 0;
	};

	static bool IsAuthorityWorld(const UWorld* World)
	{
		return World
			&& (World->GetNetMode() == NM_ListenServer
				|| World->GetNetMode() == NM_DedicatedServer
				|| World->GetNetMode() == NM_Standalone);
	}

	static FVector ResolveGroundSpawnLocation(
		UWorld* World,
		const AActor* ActorToIgnore,
		const FVector& TraceOriginLocation,
		const FVector& FallbackLocation)
	{
		if (!World)
		{
			return FallbackLocation;
		}

		const FVector TraceStart = TraceOriginLocation + FVector(0.f, 0.f, 80.f);
		const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, 3000.f);

		FHitResult Hit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FullLootGroundTrace), false, ActorToIgnore);
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams)
			|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams)
			|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams))
		{
			return Hit.ImpactPoint + FVector(0.f, 0.f, 8.f);
		}

		return FallbackLocation;
	}

	static void StabilizeLootBagActor(AActor* LootBagActor)
	{
		if (!LootBagActor)
		{
			return;
		}

		TArray<UPrimitiveComponent*> PrimitiveComponents;
		LootBagActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (UPrimitiveComponent* Primitive : PrimitiveComponents)
		{
			if (!Primitive)
			{
				continue;
			}

			if (Primitive->IsSimulatingPhysics())
			{
				Primitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
				Primitive->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
				Primitive->SetSimulatePhysics(false);
			}
		}

		TArray<UMovementComponent*> MovementComponents;
		LootBagActor->GetComponents<UMovementComponent>(MovementComponents);
		for (UMovementComponent* MovementComp : MovementComponents)
		{
			if (!MovementComp)
			{
				continue;
			}

			MovementComp->StopMovementImmediately();
			MovementComp->Deactivate();
		}
	}

	static void SnapLootBagToGround(UWorld* World, AActor* LootBagActor)
	{
		if (!World || !LootBagActor)
		{
			return;
		}

		FVector Origin = FVector::ZeroVector;
		FVector Extent = FVector::ZeroVector;
		LootBagActor->GetActorBounds(true, Origin, Extent);

		const FVector TraceStart = Origin + FVector(0.f, 0.f, FMath::Max(120.f, Extent.Z + 120.f));
		const FVector TraceEnd = Origin - FVector(0.f, 0.f, 5000.f);
		FHitResult Hit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FullLootSnapGroundTrace), false, LootBagActor);

		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams)
			|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams)
			|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams))
		{
			const FVector SnappedLocation(
				LootBagActor->GetActorLocation().X,
				LootBagActor->GetActorLocation().Y,
				Hit.ImpactPoint.Z + FMath::Max(2.f, Extent.Z + 2.f));
			LootBagActor->SetActorLocation(SnappedLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}

	static void ConfigureLootMarkerForVictimOnly(AActor* LootBagActor)
	{
		if (!LootBagActor || !LootBagActor->HasAuthority())
		{
			return;
		}

		TArray<UNavigationMarkerComponent*> MarkerComponents;
		LootBagActor->GetComponents<UNavigationMarkerComponent>(MarkerComponents);

		if (MarkerComponents.Num() <= 0)
		{
			UNavigationMarkerComponent* NewMarker = NewObject<UNavigationMarkerComponent>(LootBagActor, TEXT("SpellRiseLootVictimMarker"));
			if (NewMarker)
			{
				LootBagActor->AddInstanceComponent(NewMarker);
				NewMarker->RegisterComponent();
				MarkerComponents.Add(NewMarker);
			}
		}

		for (UNavigationMarkerComponent* Marker : MarkerComponents)
		{
			if (!Marker)
			{
				continue;
			}

			Marker->bOnlyVisibleToOwningPlayer = true;
			FGameplayTagContainer LootMarkerDomain;
			LootMarkerDomain.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Navigator.NavigatorTypes.Minimap"), false));
			LootMarkerDomain.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Navigator.NavigatorTypes.WorldMap"), false));
			Marker->SetDomain(LootMarkerDomain);

			Marker->DefaultMarkerSettings.bOverride_IconTint = true;
			Marker->DefaultMarkerSettings.IconTint = FLinearColor(1.f, 0.f, 0.f, 1.f);
			Marker->MinimapOverrideSettings.bOverride_IconTint = true;
			Marker->MinimapOverrideSettings.IconTint = FLinearColor(1.f, 0.f, 0.f, 1.f);
			Marker->WorldMapOverrideSettings.bOverride_IconTint = true;
			Marker->WorldMapOverrideSettings.IconTint = FLinearColor(1.f, 0.f, 0.f, 1.f);
			Marker->RefreshMarker();
		}
	}

	static TSubclassOf<AActor> ResolveLootBagClass(TSubclassOf<AActor> OverrideClass)
	{
		if (OverrideClass)
		{
			const bool bLooksLikeOfficialBag =
				OverrideClass->GetName().Contains(TEXT("BP_Bag"), ESearchCase::CaseSensitive)
				|| OverrideClass->IsChildOf(ASpellRiseLootBagActor::StaticClass());

			if (bLooksLikeOfficialBag)
			{
				return OverrideClass;
			}

			UE_LOG(
				LogSpellRiseFullLoot,
				Warning,
				TEXT("[FullLoot] FullLootBagClass override ignorado (%s). Usando BP_Bag oficial."),
				*GetPathNameSafe(*OverrideClass));
		}

		static TSoftClassPtr<AActor> DefaultBagClass{FSoftObjectPath(DefaultNarrativeBagClassPath)};
		if (UClass* LoadedDefaultClass = DefaultBagClass.LoadSynchronous())
		{
			return LoadedDefaultClass;
		}

		static TSoftClassPtr<AActor> LegacyBagClass{FSoftObjectPath(LegacyBagClassPath)};
		return LegacyBagClass.LoadSynchronous();
	}

	static APlayerState* ResolvePlayerStateFromDeadActor(const AActor* DeadActor)
	{
		if (!DeadActor)
		{
			return nullptr;
		}

		if (const APawn* DeadPawn = Cast<APawn>(DeadActor))
		{
			return DeadPawn->GetPlayerState();
		}

		if (const APlayerState* DeadPlayerState = Cast<APlayerState>(DeadActor))
		{
			return const_cast<APlayerState*>(DeadPlayerState);
		}

		return nullptr;
	}

	static FString ResolveDeadPlayerName(const AActor* DeadActor)
	{
		if (!DeadActor)
		{
			return FString();
		}

		if (const APlayerState* PlayerState = ResolvePlayerStateFromDeadActor(DeadActor))
		{
			const FString PlayerName = PlayerState->GetPlayerName();
			if (!PlayerName.IsEmpty())
			{
				return PlayerName;
			}
		}

		return DeadActor->GetName();
	}

	static FString SanitizeDeadPlayerNameForNet(const FString& InName, const int32 MaxUtf8Bytes)
	{
		FString Sanitized = InName;
		Sanitized = Sanitized.TrimStartAndEnd();
		Sanitized.ReplaceInline(TEXT("\n"), TEXT(" "));
		Sanitized.ReplaceInline(TEXT("\r"), TEXT(" "));
		Sanitized.ReplaceInline(TEXT("\t"), TEXT(" "));

		if (Sanitized.IsEmpty())
		{
			Sanitized = TEXT("Unknown");
		}

		const int32 ClampedMaxBytes = FMath::Clamp(MaxUtf8Bytes, 16, 256);
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

	static UNarrativeInteractableComponent* FindLootInteractableComponent(AActor* LootBagActor)
	{
		if (!LootBagActor)
		{
			return nullptr;
		}

		TArray<UNarrativeInteractableComponent*> InteractableComponents;
		LootBagActor->GetComponents<UNarrativeInteractableComponent>(InteractableComponents);

		for (UNarrativeInteractableComponent* InteractableComp : InteractableComponents)
		{
			if (InteractableComp && InteractableComp->GetFName() == TEXT("Interactable_Loot"))
			{
				return InteractableComp;
			}
		}

		for (UNarrativeInteractableComponent* InteractableComp : InteractableComponents)
		{
			if (InteractableComp)
			{
				return InteractableComp;
			}
		}

		return nullptr;
	}

	static bool RollbackAddedQuantity(
		UNarrativeInventoryComponent* BagInventory,
		TSubclassOf<UNarrativeItem> ItemClass,
		const int32 QuantityToRollback)
	{
		if (!BagInventory || !ItemClass || QuantityToRollback <= 0)
		{
			return true;
		}

		int32 LeftToRollback = QuantityToRollback;
		TArray<UNarrativeItem*> BagStacks = BagInventory->FindItemsOfClass(ItemClass, false);
		for (UNarrativeItem* Stack : BagStacks)
		{
			if (!Stack || LeftToRollback <= 0)
			{
				continue;
			}

			const int32 RemovedNow = BagInventory->ConsumeItem(Stack, LeftToRollback);
			if (RemovedNow > 0)
			{
				LeftToRollback -= RemovedNow;
			}
		}

		return LeftToRollback <= 0;
	}

	static bool IsInventoryEmpty(const UNarrativeInventoryComponent* Inventory)
	{
		if (!Inventory)
		{
			return true;
		}

		for (const UNarrativeItem* Item : Inventory->GetItems())
		{
			if (Item && Item->GetQuantity() > 0)
			{
				return false;
			}
		}

		return true;
	}

	static int32 CountTotalItemQuantity(const UNarrativeInventoryComponent* Inventory)
	{
		if (!Inventory)
		{
			return 0;
		}

		int32 TotalQuantity = 0;
		for (const UNarrativeItem* Item : Inventory->GetItems())
		{
			if (Item)
			{
				TotalQuantity += FMath::Max(0, Item->GetQuantity());
			}
		}

		return TotalQuantity;
	}

	static void SetDeadPlayerNamePropertyIfPresent(UObject* TargetObject, const FString& DeadPlayerDisplayName, int32& InOutSetCount)
	{
		if (!TargetObject)
		{
			return;
		}

		if (FStrProperty* StringProperty = FindFProperty<FStrProperty>(TargetObject->GetClass(), TEXT("DeadPlayerDisplayName")))
		{
			StringProperty->SetPropertyValue_InContainer(TargetObject, DeadPlayerDisplayName);
			++InOutSetCount;
		}

		if (FStrProperty* StringProperty = FindFProperty<FStrProperty>(TargetObject->GetClass(), TEXT("DeadPlayerName")))
		{
			StringProperty->SetPropertyValue_InContainer(TargetObject, DeadPlayerDisplayName);
			++InOutSetCount;
		}
	}

	static void SetLootBagDeadPlayerName(AActor* LootBagActor, const FString& DeadPlayerDisplayName)
	{
		if (!LootBagActor || DeadPlayerDisplayName.IsEmpty())
		{
			return;
		}

		int32 SetCount = 0;

		if (ASpellRiseLootBagActor* SpellRiseBag = Cast<ASpellRiseLootBagActor>(LootBagActor))
		{
			SpellRiseBag->SetDeadPlayerDisplayName(DeadPlayerDisplayName);
			++SetCount;
		}

		if (LootBagActor->HasAuthority())
		{
			USpellRiseLootBagNameComponent* LootBagNameComponent = LootBagActor->FindComponentByClass<USpellRiseLootBagNameComponent>();
			if (!LootBagNameComponent)
			{
				LootBagNameComponent = NewObject<USpellRiseLootBagNameComponent>(LootBagActor, TEXT("SpellRiseLootBagNameComponent"));
				if (LootBagNameComponent)
				{
					LootBagActor->AddInstanceComponent(LootBagNameComponent);
					LootBagNameComponent->RegisterComponent();
				}
			}

			if (LootBagNameComponent)
			{
				LootBagNameComponent->SetDeadPlayerDisplayName_Server(DeadPlayerDisplayName);
				++SetCount;
			}
		}

		SetDeadPlayerNamePropertyIfPresent(LootBagActor, DeadPlayerDisplayName, SetCount);

		TArray<UActorComponent*> Components;
		LootBagActor->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			SetDeadPlayerNamePropertyIfPresent(Component, DeadPlayerDisplayName, SetCount);
		}

		if (UNarrativeInteractableComponent* LootInteractable = FindLootInteractableComponent(LootBagActor))
		{
			LootInteractable->SetInteractableNameText(FText::FromString(DeadPlayerDisplayName));
			++SetCount;
		}

		UE_LOG(
			LogSpellRiseFullLoot,
			Log,
			TEXT("[FullLoot] Nome da bag atualizado. Bag=%s Nome='%s' SetCount=%d"),
			*GetNameSafe(LootBagActor),
			*DeadPlayerDisplayName,
			SetCount);
	}

	static UNarrativeInventoryComponent* GetLootSourceReflective(const UNarrativeInventoryComponent* Inventory)
	{
		if (!Inventory)
		{
			return nullptr;
		}

		static const FObjectProperty* LootSourceProperty = FindFProperty<FObjectProperty>(
			UNarrativeInventoryComponent::StaticClass(),
			TEXT("LootSource"));
		if (!LootSourceProperty)
		{
			return nullptr;
		}

		return Cast<UNarrativeInventoryComponent>(
			LootSourceProperty->GetObjectPropertyValue_InContainer(Inventory));
	}

	static int32 ClearLootSourceReferencesToBag(UWorld* World, UNarrativeInventoryComponent* BagInventory)
	{
		if (!World || !BagInventory)
		{
			return 0;
		}

		int32 ClearedCount = 0;
		TArray<UNarrativeInventoryComponent*> AllInventories;
		for (TObjectIterator<UNarrativeInventoryComponent> It; It; ++It)
		{
			UNarrativeInventoryComponent* Inventory = *It;
			if (!Inventory || Inventory == BagInventory || Inventory->GetWorld() != World)
			{
				continue;
			}

			if (!Inventory->GetOwner() || !Inventory->GetOwner()->HasAuthority())
			{
				continue;
			}

			if (GetLootSourceReflective(Inventory) == BagInventory)
			{
				Inventory->SetLootSource(nullptr);
				++ClearedCount;
			}
		}

		return ClearedCount;
	}

	static int32 ClearLootSourceReferencesToSources(
		UWorld* World,
		const TSet<TWeakObjectPtr<UNarrativeInventoryComponent>>& SourcesToClear)
	{
		if (!World || SourcesToClear.Num() <= 0)
		{
			return 0;
		}

		int32 ClearedCount = 0;
		for (TObjectIterator<UNarrativeInventoryComponent> It; It; ++It)
		{
			UNarrativeInventoryComponent* Inventory = *It;
			if (!Inventory || Inventory->GetWorld() != World)
			{
				continue;
			}

			if (!Inventory->GetOwner() || !Inventory->GetOwner()->HasAuthority())
			{
				continue;
			}

			UNarrativeInventoryComponent* CurrentLootSource = GetLootSourceReflective(Inventory);
			if (!CurrentLootSource)
			{
				continue;
			}

			if (SourcesToClear.Contains(CurrentLootSource))
			{
				Inventory->SetLootSource(nullptr);
				++ClearedCount;
			}
		}

		return ClearedCount;
	}

	static bool IsPlayerOwnedInventory(const UNarrativeInventoryComponent* Inventory)
	{
		if (!Inventory)
		{
			return false;
		}

		const AActor* Owner = Inventory->GetOwner();
		if (Owner && Owner->IsA(ASpellRiseLootBagActor::StaticClass()))
		{
			return false;
		}

		if (Owner && (Owner->IsA<APawn>() || Owner->IsA<APlayerState>() || Owner->IsA<APlayerController>()))
		{
			return true;
		}

		return Inventory->GetOwningController() != nullptr || Inventory->GetOwningPawn() != nullptr;
	}

	static int32 ClearInvalidPlayerLootSources(UWorld* World)
	{
		if (!World)
		{
			return 0;
		}

		int32 ClearedCount = 0;
		for (TObjectIterator<UNarrativeInventoryComponent> It; It; ++It)
		{
			UNarrativeInventoryComponent* Inventory = *It;
			if (!Inventory || Inventory->GetWorld() != World || !IsPlayerOwnedInventory(Inventory))
			{
				continue;
			}

			if (!Inventory->GetOwner() || !Inventory->GetOwner()->HasAuthority())
			{
				continue;
			}

			UNarrativeInventoryComponent* CurrentLootSource = GetLootSourceReflective(Inventory);
			if (!CurrentLootSource)
			{
				continue;
			}

			const bool bInvalidSource =
				!IsValid(CurrentLootSource)
				|| !IsValid(CurrentLootSource->GetOwner())
				|| CurrentLootSource->GetOwner()->IsActorBeingDestroyed();

			const bool bBlockedSource = IsPlayerOwnedInventory(CurrentLootSource);
			if (bInvalidSource || bBlockedSource)
			{
				Inventory->SetLootSource(nullptr);
				++ClearedCount;
			}
		}

		return ClearedCount;
	}
}

void USpellRiseFullLootSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TrackedLootBags.Reset();
}

void USpellRiseFullLootSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LootBagMonitorTimerHandle);
	}

	TrackedLootBags.Reset();
	Super::Deinitialize();
}

void USpellRiseFullLootSubsystem::HandleCharacterDeath(AActor* DeadActor, TSubclassOf<AActor> LootBagClassOverride)
{
	UWorld* World = GetWorld();
	if (!World || !DeadActor || !DeadActor->HasAuthority() || !IsAuthorityWorld(World))
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Abortado contexto invalido. World=%d Char=%s Auth=%d NetMode=%d"),
			World ? 1 : 0,
			*GetNameSafe(DeadActor),
			DeadActor && DeadActor->HasAuthority() ? 1 : 0,
			World ? static_cast<int32>(World->GetNetMode()) : -1);
		return;
	}

	const FVector DeathLocation = DeadActor->GetActorLocation();
	if (LootBagSpawnDelaySeconds > KINDA_SMALL_NUMBER)
	{
		const float DelaySeconds = LootBagSpawnDelaySeconds;
		TWeakObjectPtr<USpellRiseFullLootSubsystem> WeakSubsystem(this);
		TWeakObjectPtr<AActor> WeakDeadActor(DeadActor);
		TWeakObjectPtr<APlayerState> WeakDeadPlayerState(ResolvePlayerStateFromDeadActor(DeadActor));

		FTimerDelegate SpawnDelayDelegate = FTimerDelegate::CreateLambda(
			[WeakSubsystem, WeakDeadActor, WeakDeadPlayerState, LootBagClassOverride, DeathLocation]()
			{
				USpellRiseFullLootSubsystem* Subsystem = WeakSubsystem.Get();
				AActor* ResolvedDeadActor = WeakDeadActor.Get();
				APlayerState* ResolvedDeadPlayerState = WeakDeadPlayerState.Get();
				if (!Subsystem)
				{
					return;
				}

				// Fallback importante: o Pawn pode ter sido destruido antes do delay da bag.
				// Nesse caso, ainda processamos o loot usando PlayerState (owner autoritativo do inventario/GAS).
				if (!ResolvedDeadActor)
				{
					ResolvedDeadActor = ResolvedDeadPlayerState;
				}

				if (!ResolvedDeadActor)
				{
					return;
				}

				Subsystem->ProcessCharacterDeathNow(ResolvedDeadActor, LootBagClassOverride, DeathLocation);
			});

		FTimerHandle DelayHandle;
		World->GetTimerManager().SetTimer(DelayHandle, SpawnDelayDelegate, DelaySeconds, false);

		UE_LOG(
			LogSpellRiseFullLoot,
			Log,
			TEXT("[FullLoot] Spawn da bag agendado para %.2fs apos a morte. Char=%s"),
			DelaySeconds,
			*GetNameSafe(DeadActor));
		return;
	}

	ProcessCharacterDeathNow(DeadActor, LootBagClassOverride, DeathLocation);
}

void USpellRiseFullLootSubsystem::ProcessCharacterDeathNow(
	AActor* DeadActor,
	TSubclassOf<AActor> LootBagClassOverride,
	const FVector& DeathLocation)
{
	UWorld* World = GetWorld();
	if (!World || !DeadActor || !DeadActor->HasAuthority() || !IsAuthorityWorld(World))
	{
		return;
	}

	const int32 ClearedInvalidPlayerLootSources = ClearInvalidPlayerLootSources(World);
	if (ClearedInvalidPlayerLootSources > 0)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Limpeza global de LootSource invalido/player-owned aplicada. Cleared=%d"),
			ClearedInvalidPlayerLootSources);
	}

	TArray<UNarrativeInventoryComponent*> SourceInventories;
	GatherEligibleInventoryComponents(DeadActor, false, SourceInventories);
	GatherEligibleInventoryComponents(ResolvePlayerStateFromDeadActor(DeadActor), true, SourceInventories);

	UE_LOG(
		LogSpellRiseFullLoot,
		Log,
		TEXT("[FullLoot] Morte detectada. Char=%s InventariosElegiveis=%d"),
		*GetNameSafe(DeadActor),
		SourceInventories.Num());

	if (SourceInventories.Num() <= 0)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Nenhum inventario Narrative elegivel para %s"),
			*GetNameSafe(DeadActor));
		return;
	}

	TSet<TWeakObjectPtr<UNarrativeInventoryComponent>> VictimSourceSet;
	for (UNarrativeInventoryComponent* SourceInventory : SourceInventories)
	{
		if (SourceInventory)
		{
			VictimSourceSet.Add(SourceInventory);
		}
	}

	const int32 ClearedVictimSourceLooters = ClearLootSourceReferencesToSources(World, VictimSourceSet);
	if (ClearedVictimSourceLooters > 0)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Limpeza preventiva de LootSource da vitima aplicada. Char=%s InventariosVitima=%d LootersReset=%d"),
			*GetNameSafe(DeadActor),
			VictimSourceSet.Num(),
			ClearedVictimSourceLooters);
	}

	TArray<FPendingNarrativeTransfer> PendingTransfers;
	int32 CandidateItemStacks = 0;
	int32 CandidateQuantity = 0;
	int32 RejectedNonDroppable = 0;
	int32 RejectedInvalid = 0;

	for (UNarrativeInventoryComponent* SourceInventory : SourceInventories)
	{
		if (!SourceInventory)
		{
			continue;
		}

		for (UNarrativeItem* Item : SourceInventory->GetItems())
		{
			if (!Item || !Item->GetClass())
			{
				++RejectedInvalid;
				continue;
			}

			const int32 Quantity = Item->GetQuantity();
			if (Quantity <= 0)
			{
				++RejectedInvalid;
				continue;
			}

			if (!Item->CanBeRemoved())
			{
				++RejectedNonDroppable;
				UE_LOG(
					LogSpellRiseFullLoot,
					Log,
					TEXT("[FullLoot][Policy] Item bloqueado por CanBeRemoved=false. Inventory=%s Item=%s Qty=%d"),
					*GetNameSafe(SourceInventory),
					*GetNameSafe(Item),
					Quantity);
				continue;
			}

			FPendingNarrativeTransfer Transfer;
			Transfer.SourceInventory = SourceInventory;
			Transfer.SourceItem = Item;
			Transfer.ItemClass = Item->GetClass();
			Transfer.Quantity = Quantity;
			PendingTransfers.Add(MoveTemp(Transfer));

			++CandidateItemStacks;
			CandidateQuantity += Quantity;
		}
	}

	UE_LOG(
		LogSpellRiseFullLoot,
		Warning,
		TEXT("[FullLoot] Candidatos=%d stacks / %d quantidade. RejeitadosNoDrop=%d RejeitadosInvalidos=%d"),
		CandidateItemStacks,
		CandidateQuantity,
		RejectedNonDroppable,
		RejectedInvalid);

	if (PendingTransfers.Num() <= 0)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Nenhum item elegivel para dropar. Char=%s"),
			*GetNameSafe(DeadActor));
		return;
	}

	const TSubclassOf<AActor> LootBagClass = ResolveLootBagClass(LootBagClassOverride);
	if (!LootBagClass)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Error,
			TEXT("[FullLoot] Classe da bag invalida. Char=%s"),
			*GetNameSafe(DeadActor));
		return;
	}

	FTransform SpawnTransform = DeadActor->GetActorTransform();
	SpawnTransform.SetScale3D(FVector::OneVector);
	SpawnTransform.SetLocation(ResolveGroundSpawnLocation(World, DeadActor, DeathLocation, DeathLocation));

	// Narrative rejects LootSource that is player-owned (Pawn/PlayerState/Controller).
	// Keep bag owner null so bag inventory remains loot-eligible for player inventories.
	AActor* LootBagOwner = nullptr;

	AActor* LootBagActor = World->SpawnActorDeferred<AActor>(
		LootBagClass,
		SpawnTransform,
		LootBagOwner,
		Cast<APawn>(DeadActor),
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!LootBagActor)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Error,
			TEXT("[FullLoot] Falha ao spawnar bag para %s"),
			*GetNameSafe(DeadActor));
		return;
	}

	LootBagActor->SetReplicates(true);
	LootBagActor->SetReplicateMovement(true);
	LootBagActor->SetNetUpdateFrequency(FMath::Max(1.0f, LootBagNetUpdateFrequency));
	LootBagActor->SetMinNetUpdateFrequency(FMath::Max(1.0f, LootBagNetUpdateFrequency * 0.4f));
	LootBagActor->FinishSpawning(SpawnTransform);
	StabilizeLootBagActor(LootBagActor);
	SnapLootBagToGround(World, LootBagActor);
	ConfigureLootMarkerForVictimOnly(LootBagActor);

	const FString DeadPlayerDisplayName = SanitizeDeadPlayerNameForNet(
		ResolveDeadPlayerName(DeadActor),
		DeadPlayerNameMaxUtf8Bytes);
	SetLootBagDeadPlayerName(LootBagActor, DeadPlayerDisplayName);

	UNarrativeInventoryComponent* LootBagInventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(LootBagActor);
	if (!LootBagInventory)
	{
		LootBagInventory = LootBagActor->FindComponentByClass<UNarrativeInventoryComponent>();
	}

	if (!LootBagInventory)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Error,
			TEXT("[FullLoot] Bag sem UNarrativeInventoryComponent. Bag=%s"),
			*GetNameSafe(LootBagActor));
		LootBagActor->Destroy();
		return;
	}

	int32 AttemptedTransfers = 0;
	int32 MovedTransfers = 0;
	int32 MovedQuantity = 0;
	int32 RejectedByCapacity = 0;
	int32 RollbackFailures = 0;

	for (const FPendingNarrativeTransfer& Transfer : PendingTransfers)
	{
		UNarrativeInventoryComponent* SourceInventory = Transfer.SourceInventory.Get();
		UNarrativeItem* SourceItem = Transfer.SourceItem.Get();

		if (!SourceInventory || !SourceItem || !Transfer.ItemClass)
		{
			continue;
		}

		if (!SourceItem->CanBeRemoved())
		{
			UE_LOG(
				LogSpellRiseFullLoot,
				Warning,
				TEXT("[FullLoot][Policy] Item ficou nao removivel durante transfer. Src=%s Item=%s"),
				*GetNameSafe(SourceInventory),
				*GetNameSafe(SourceItem));
			continue;
		}

		const int32 CurrentQuantity = SourceItem->GetQuantity();
		if (CurrentQuantity <= 0)
		{
			continue;
		}

		const int32 RequestedQuantity = FMath::Clamp(Transfer.Quantity, 1, CurrentQuantity);
		++AttemptedTransfers;

		const FItemAddResult AddResult = LootBagInventory->TryAddItemFromClass(Transfer.ItemClass, RequestedQuantity, false);
		if (AddResult.AmountGiven <= 0)
		{
			++RejectedByCapacity;
			UE_LOG(
				LogSpellRiseFullLoot,
				Warning,
				TEXT("[FullLoot] Bag sem espaco para item. Src=%s ItemClass=%s Req=%d Error='%s'"),
				*GetNameSafe(SourceInventory),
				*GetNameSafe(*Transfer.ItemClass),
				RequestedQuantity,
				*AddResult.ErrorText.ToString());
			continue;
		}

		int32 RemovedFromSource = SourceInventory->ConsumeItem(SourceItem, AddResult.AmountGiven);
		if (RemovedFromSource < 0)
		{
			RemovedFromSource = 0;
		}
		RemovedFromSource = FMath::Min(RemovedFromSource, AddResult.AmountGiven);

		const int32 ExcessInBag = AddResult.AmountGiven - RemovedFromSource;
		if (ExcessInBag > 0)
		{
			const bool bRollbackOk = RollbackAddedQuantity(LootBagInventory, Transfer.ItemClass, ExcessInBag);
			if (!bRollbackOk)
			{
				++RollbackFailures;
				UE_LOG(
					LogSpellRiseFullLoot,
					Error,
					TEXT("[FullLoot] Rollback parcial falhou. Bag=%s ItemClass=%s Excess=%d"),
					*GetNameSafe(LootBagActor),
					*GetNameSafe(*Transfer.ItemClass),
					ExcessInBag);
			}
		}

		if (RemovedFromSource > 0)
		{
			++MovedTransfers;
			MovedQuantity += RemovedFromSource;
		}
	}

	if (MovedQuantity <= 0)
	{
		const int32 ClearedInvalidAfterAbort = ClearInvalidPlayerLootSources(World);
		if (ClearedInvalidAfterAbort > 0)
		{
			UE_LOG(
				LogSpellRiseFullLoot,
				Warning,
				TEXT("[FullLoot] Limpeza pos-aborto aplicada. Cleared=%d"),
				ClearedInvalidAfterAbort);
		}

		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Nenhum item foi movido. Destroy bag=%s"),
			*GetNameSafe(LootBagActor));
		LootBagActor->Destroy();
		return;
	}

	RegisterTrackedLootBag(LootBagActor, LootBagInventory);

	const int32 ClearedInvalidAfterTransfer = ClearInvalidPlayerLootSources(World);
	if (ClearedInvalidAfterTransfer > 0)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] Limpeza pos-transferencia aplicada. Cleared=%d"),
			ClearedInvalidAfterTransfer);
	}

	UE_LOG(
		LogSpellRiseFullLoot,
		Warning,
		TEXT("[FullLoot] Sucesso. Bag=%s Char=%s Tentativas=%d Movidas=%d QuantidadeMovida=%d RejCapacidade=%d RollbackFalhas=%d"),
		*GetNameSafe(LootBagActor),
		*GetNameSafe(DeadActor),
		AttemptedTransfers,
		MovedTransfers,
		MovedQuantity,
		RejectedByCapacity,
		RollbackFailures);
}

void USpellRiseFullLootSubsystem::GatherEligibleInventoryComponents(
	AActor* OwnerActor,
	const bool bOwnerIsPlayerState,
	TArray<UNarrativeInventoryComponent*>& OutInventoryComponents) const
{
	if (!OwnerActor)
	{
		return;
	}

	if (UNarrativeInventoryComponent* PrimaryInventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(OwnerActor))
	{
		FString PrimaryRejectReason;
		if (IsInventoryComponentEligibleForDeathLoot(PrimaryInventory, bOwnerIsPlayerState, PrimaryRejectReason))
		{
			OutInventoryComponents.AddUnique(PrimaryInventory);
		}
	}

	TArray<UNarrativeInventoryComponent*> OwnerInventories;
	OwnerActor->GetComponents<UNarrativeInventoryComponent>(OwnerInventories);

	int32 AddedForOwner = 0;
	for (UNarrativeInventoryComponent* InventoryComponent : OwnerInventories)
	{
		if (!InventoryComponent)
		{
			continue;
		}

		FString RejectReason;
		if (!IsInventoryComponentEligibleForDeathLoot(InventoryComponent, bOwnerIsPlayerState, RejectReason))
		{
			UE_LOG(
				LogSpellRiseFullLoot,
				Warning,
				TEXT("[FullLoot][Policy] Inventario rejeitado. Owner=%s IsPlayerState=%d Comp=%s Nome=%s Razao=%s"),
				*GetNameSafe(OwnerActor),
				bOwnerIsPlayerState ? 1 : 0,
				*GetNameSafe(InventoryComponent),
				*InventoryComponent->GetFName().ToString(),
				*RejectReason);
			continue;
		}

		OutInventoryComponents.AddUnique(InventoryComponent);
		++AddedForOwner;

		if (AddedForOwner >= FMath::Max(1, MaxCollectedInventoryComponentsPerOwner))
		{
			UE_LOG(
				LogSpellRiseFullLoot,
				Warning,
				TEXT("[FullLoot][Policy] Limite por owner atingido (%d). Owner=%s"),
				MaxCollectedInventoryComponentsPerOwner,
				*GetNameSafe(OwnerActor));
			break;
		}
	}
}

bool USpellRiseFullLootSubsystem::IsInventoryComponentEligibleForDeathLoot(
	const UNarrativeInventoryComponent* InventoryComponent,
	const bool bOwnerIsPlayerState,
	FString& OutRejectReason) const
{
	OutRejectReason.Reset();

	if (!InventoryComponent)
	{
		OutRejectReason = TEXT("NullInventory");
		return false;
	}

	const FName ComponentName = InventoryComponent->GetFName();
	if (DeniedDeathLootInventoryComponentNames.Contains(ComponentName))
	{
		OutRejectReason = TEXT("DeniedByName");
		return false;
	}

	if (!bUseStrictDeathLootInventoryPolicy)
	{
		return true;
	}

	const TArray<FName>& AllowedNames = bOwnerIsPlayerState
		? AllowedPlayerStateInventoryComponentNames
		: AllowedCharacterInventoryComponentNames;

	if (AllowedNames.Num() <= 0)
	{
		OutRejectReason = TEXT("AllowedListEmpty");
		return false;
	}

	if (!AllowedNames.Contains(ComponentName))
	{
		const FString ComponentNameLower = ComponentName.ToString().ToLower();
		const bool bLooksLikeNarrativePlayerInventory =
			ComponentNameLower.Contains(TEXT("narrativeinventory"))
			|| ComponentNameLower.Contains(TEXT("inventory"))
			|| ComponentNameLower.Contains(TEXT("hotbar"));

		if (!bLooksLikeNarrativePlayerInventory)
		{
			OutRejectReason = TEXT("NotInAllowedList");
			return false;
		}
	}

	return true;
}

void USpellRiseFullLootSubsystem::RegisterTrackedLootBag(AActor* BagActor, UNarrativeInventoryComponent* InventoryComponent)
{
	if (!BagActor || !InventoryComponent)
	{
		UE_LOG(
			LogSpellRiseFullLoot,
			Warning,
			TEXT("[FullLoot] RegisterTrackedLootBag ignorado. Bag=%s Inventory=%s"),
			*GetNameSafe(BagActor),
			*GetNameSafe(InventoryComponent));
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FTrackedLootBag NewBag;
		NewBag.BagActor = BagActor;
		NewBag.InventoryComponent = InventoryComponent;
		NewBag.ExpireAtServerTimeSeconds = static_cast<double>(World->GetTimeSeconds()) + LootBagLifetimeSeconds;

		TrackedLootBags.Add(MoveTemp(NewBag));
		EnsureMonitorTimer();
	}
}

void USpellRiseFullLootSubsystem::EnsureMonitorTimer()
{
	UWorld* World = GetWorld();
	if (!World || !IsAuthorityWorld(World) || TrackedLootBags.Num() <= 0)
	{
		return;
	}

	if (!World->GetTimerManager().IsTimerActive(LootBagMonitorTimerHandle))
	{
		World->GetTimerManager().SetTimer(
			LootBagMonitorTimerHandle,
			this,
			&USpellRiseFullLootSubsystem::TickLootBags,
			FMath::Max(0.2f, LootBagMonitorIntervalSeconds),
			true);
	}
}

void USpellRiseFullLootSubsystem::StopMonitorTimerIfIdle()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (TrackedLootBags.Num() <= 0)
	{
		World->GetTimerManager().ClearTimer(LootBagMonitorTimerHandle);
	}
}

void USpellRiseFullLootSubsystem::TickLootBags()
{
	UWorld* World = GetWorld();
	if (!World || !IsAuthorityWorld(World))
	{
		return;
	}

	const double ServerTimeSeconds = static_cast<double>(World->GetTimeSeconds());

	for (int32 Index = TrackedLootBags.Num() - 1; Index >= 0; --Index)
	{
		FTrackedLootBag& TrackedBag = TrackedLootBags[Index];
		AActor* BagActor = TrackedBag.BagActor.Get();
		UNarrativeInventoryComponent* BagInventory = TrackedBag.InventoryComponent.Get();

		if (!BagActor || !BagInventory)
		{
			TrackedLootBags.RemoveAtSwap(Index);
			continue;
		}

		const bool bExpired = ServerTimeSeconds >= TrackedBag.ExpireAtServerTimeSeconds;
		const bool bEmpty = IsInventoryEmpty(BagInventory);

		if (bEmpty)
		{
			if (TrackedBag.EmptySinceServerTimeSeconds < 0.0)
			{
				TrackedBag.EmptySinceServerTimeSeconds = ServerTimeSeconds;
			}
		}
		else
		{
			TrackedBag.EmptySinceServerTimeSeconds = -1.0;
		}

		const bool bEmptyDelayElapsed =
			bEmpty
			&& TrackedBag.EmptySinceServerTimeSeconds >= 0.0
			&& (ServerTimeSeconds - TrackedBag.EmptySinceServerTimeSeconds) >= LootBagEmptyDespawnDelaySeconds;

		if (bExpired || bEmptyDelayElapsed)
		{
			const int32 ClearedLooters = ClearLootSourceReferencesToBag(World, BagInventory);
			UE_LOG(
				LogSpellRiseFullLoot,
				Log,
				TEXT("[FullLoot] Removendo bag=%s Motivo=%s QtyRestante=%d LootersReset=%d"),
				*GetNameSafe(BagActor),
				bExpired ? TEXT("timeout") : TEXT("vazia_delay"),
				CountTotalItemQuantity(BagInventory),
				ClearedLooters);

			BagActor->Destroy();
			TrackedLootBags.RemoveAtSwap(Index);
		}
	}

	StopMonitorTimerIfIdle();
}
