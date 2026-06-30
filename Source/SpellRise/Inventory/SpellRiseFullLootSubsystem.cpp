// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Inventory/SpellRiseFullLootSubsystem.h"

#include "Components/PrimitiveComponent.h"
#include "Containers/StringConv.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/UnrealType.h"

#include "InteractableComponent.h"
#include "NavigationMarkerComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Equipment/SpellRiseEquipmentComponent.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"
#include "SpellRise/Inventory/SpellRiseItemTypes.h"
#include "SpellRise/Inventory/SpellRiseLootBagActor.h"
#include "SpellRise/Inventory/SpellRiseLootBagNameComponent.h"
#include "SpellRise/Inventory/SpellRiseStorageComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseFullLoot, Log, All);

namespace
{
	constexpr TCHAR DefaultNativeBagClassPath[] = TEXT("/Game/UI/InventorySystem/BP_StorageMaster1.BP_StorageMaster1_C");

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
		const FVector AdjustedFallbackLocation = FallbackLocation + FVector(0.f, 0.f, 12.f);
		if (!World)
		{
			return AdjustedFallbackLocation;
		}

		const FVector TraceStart = TraceOriginLocation + FVector(0.f, 0.f, 120.f);
		const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, 5000.f);

		FHitResult Hit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FullLootGroundTrace), false, ActorToIgnore);
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams)
			|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams)
			|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams))
		{
			return Hit.ImpactPoint + FVector(0.f, 0.f, 8.f);
		}

		return AdjustedFallbackLocation;
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

	// Configura markers em bags Blueprint que já possuem UNavigationMarkerComponent nativo.
	// Bags sem o componente no Blueprint não recebem marker aqui — use ClientShowLootBagMarker via RPC.
	static void ConfigureLootMarkerForVictimOnly(AActor* LootBagActor)
	{
		if (!LootBagActor || !LootBagActor->HasAuthority())
		{
			return;
		}

		TArray<UNavigationMarkerComponent*> MarkerComponents;
		LootBagActor->GetComponents<UNavigationMarkerComponent>(MarkerComponents);

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
			return OverrideClass;
		}

		static TSoftClassPtr<AActor> DefaultBagClass{FSoftObjectPath(DefaultNativeBagClassPath)};
		return DefaultBagClass.LoadSynchronous();
	}

	static FString ResolveDeadPlayerName(const ASpellRiseCharacterBase* DeadCharacter)
	{
		if (!DeadCharacter)
		{
			return FString();
		}

		if (const APlayerState* PlayerState = DeadCharacter->GetPlayerState<APlayerState>())
		{
			const FString PlayerName = PlayerState->GetPlayerName();
			if (!PlayerName.IsEmpty())
			{
				return PlayerName;
			}
		}

		return DeadCharacter->GetName();
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

void USpellRiseFullLootSubsystem::HandleCharacterDeath(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride)
{
	UWorld* World = GetWorld();
	if (!World || !DeadCharacter || !DeadCharacter->HasAuthority() || !IsAuthorityWorld(World))
	{
		return;
	}

	const FVector DeathLocation = DeadCharacter->GetActorLocation();
	if (LootBagSpawnDelaySeconds > KINDA_SMALL_NUMBER)
	{
		const float DelaySeconds = LootBagSpawnDelaySeconds;
		TWeakObjectPtr<USpellRiseFullLootSubsystem> WeakSubsystem(this);
		TWeakObjectPtr<ASpellRiseCharacterBase> WeakDeadCharacter(DeadCharacter);

		FTimerDelegate SpawnDelayDelegate = FTimerDelegate::CreateLambda(
			[WeakSubsystem, WeakDeadCharacter, LootBagClassOverride, DeathLocation]()
			{
				USpellRiseFullLootSubsystem* Subsystem = WeakSubsystem.Get();
				ASpellRiseCharacterBase* ResolvedDeadCharacter = WeakDeadCharacter.Get();
				if (!Subsystem || !ResolvedDeadCharacter)
				{
					return;
				}

				Subsystem->ProcessCharacterDeathNow(ResolvedDeadCharacter, LootBagClassOverride, DeathLocation);
			});

		FTimerHandle DelayHandle;
		World->GetTimerManager().SetTimer(DelayHandle, SpawnDelayDelegate, DelaySeconds, false);

		return;
	}

	ProcessCharacterDeathNow(DeadCharacter, LootBagClassOverride, DeathLocation);
}

void USpellRiseFullLootSubsystem::HandleCharacterCorpseDespawn(
	ASpellRiseCharacterBase* DeadCharacter,
	TSubclassOf<AActor> LootBagClassOverride,
	const FVector& CorpseLocation)
{
	UWorld* World = GetWorld();
	if (!World || !DeadCharacter || !DeadCharacter->HasAuthority() || !IsAuthorityWorld(World))
	{
		return;
	}

	ProcessCharacterDeathNow(DeadCharacter, LootBagClassOverride, CorpseLocation);
}

void USpellRiseFullLootSubsystem::ProcessCharacterDeathNow(
	ASpellRiseCharacterBase* DeadCharacter,
	TSubclassOf<AActor> LootBagClassOverride,
	const FVector& DeathLocation)
{
	UWorld* World = GetWorld();
	if (!World || !DeadCharacter || !DeadCharacter->HasAuthority() || !IsAuthorityWorld(World))
	{
		return;
	}

	ASpellRisePlayerState* VictimPS = DeadCharacter->GetPlayerState<ASpellRisePlayerState>();
	USpellRiseInventoryComponent* SourceInventory = VictimPS ? VictimPS->GetInventoryComponent() : nullptr;
	USpellRiseEquipmentComponent* EquipmentComp = VictimPS ? VictimPS->GetEquipmentComponent() : nullptr;
	if (!SourceInventory)
	{
		UE_LOG(LogSpellRiseFullLoot, Warning, TEXT("ProcessCharacterDeathNow: sem InventoryComponent para %s"), *GetNameSafe(DeadCharacter));
		return;
	}

	UE_LOG(LogSpellRiseFullLoot, Log,
		TEXT("ProcessCharacterDeathNow: INICIO character=%s NativeEnabled=%d TotalItems=%d TotalEquip=%d"),
		*GetNameSafe(DeadCharacter),
		SourceInventory->IsNativeInventoryEnabled() ? 1 : 0,
		SourceInventory->GetItems().Num(),
		EquipmentComp ? EquipmentComp->GetPrivateEquipment().Entries.Num() : 0);

	// Snapshot de itens droppáveis do inventário
	TArray<FSpellRiseItemInstance> InventorySnapshot;
	for (const FSpellRiseItemInstance& Item : SourceInventory->GetItems())
	{
		const bool bNoDrop = (Item.Flags & static_cast<uint8>(ESpellRiseItemInstanceFlags::NoDrop)) != 0;
		UE_LOG(LogSpellRiseFullLoot, Verbose,
			TEXT("  Item=%s Qty=%d Flags=%d NoDrop=%d"),
			*Item.DefinitionId.ToString(), Item.Quantity, Item.Flags, bNoDrop ? 1 : 0);
		if (Item.Quantity <= 0 || bNoDrop)
		{
			continue;
		}
		InventorySnapshot.Add(Item);
	}

	// Snapshot de itens droppáveis do equipamento
	TArray<FSpellRiseItemInstance> EquipmentSnapshot;
	if (EquipmentComp)
	{
		for (const FSpellRiseEquippedItemEntry& Entry : EquipmentComp->GetPrivateEquipment().Entries)
		{
			if (!Entry.DefinitionId.IsValid() || (Entry.Flags & static_cast<uint8>(ESpellRiseItemInstanceFlags::NoDrop)))
			{
				continue;
			}
			FSpellRiseItemInstance Inst;
			Inst.ItemInstanceId = Entry.ItemInstanceId;
			Inst.DefinitionId   = Entry.DefinitionId;
			Inst.Quantity        = 1;
			Inst.Durability      = Entry.Durability;
			Inst.Flags           = Entry.Flags;
			EquipmentSnapshot.Add(Inst);
		}
	}

	if (InventorySnapshot.IsEmpty() && EquipmentSnapshot.IsEmpty())
	{
		UE_LOG(LogSpellRiseFullLoot, Warning,
			TEXT("ProcessCharacterDeathNow: nenhum item droppavel encontrado para %s (InvItems=%d EquipItems=%d). Verifique flag NoDrop."),
			*GetNameSafe(DeadCharacter),
			SourceInventory->GetItems().Num(),
			EquipmentComp ? EquipmentComp->GetPrivateEquipment().Entries.Num() : 0);
		return;
	}

	UE_LOG(LogSpellRiseFullLoot, Log,
		TEXT("ProcessCharacterDeathNow: dropando %d item(ns) de inventario + %d de equipment para %s"),
		InventorySnapshot.Num(), EquipmentSnapshot.Num(), *GetNameSafe(DeadCharacter));

	const TSubclassOf<AActor> LootBagClass = ResolveLootBagClass(LootBagClassOverride);
	if (!LootBagClass)
	{
		UE_LOG(LogSpellRiseFullLoot, Error, TEXT("ProcessCharacterDeathNow: classe de loot bag não encontrada"));
		return;
	}

	FTransform SpawnTransform = DeadCharacter->GetActorTransform();
	SpawnTransform.SetScale3D(FVector::OneVector);
	SpawnTransform.SetLocation(ResolveGroundSpawnLocation(World, DeadCharacter, DeathLocation, DeathLocation));

	AActor* LootBagOwner = nullptr;
	if (AController* VictimController = DeadCharacter->GetController())
	{
		LootBagOwner = VictimController;
	}
	else if (VictimPS)
	{
		LootBagOwner = VictimPS;
	}
	else
	{
		LootBagOwner = DeadCharacter;
	}

	AActor* LootBagActor = World->SpawnActorDeferred<AActor>(
		LootBagClass,
		SpawnTransform,
		LootBagOwner,
		DeadCharacter,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (!LootBagActor)
	{
		UE_LOG(LogSpellRiseFullLoot, Error, TEXT("ProcessCharacterDeathNow: falha ao spawnar loot bag para %s"), *GetNameSafe(DeadCharacter));
		return;
	}

	LootBagActor->SetReplicates(true);
	LootBagActor->SetReplicateMovement(true);
	LootBagActor->SetNetUpdateFrequency(FMath::Max(1.0f, LootBagNetUpdateFrequency));
	LootBagActor->SetMinNetUpdateFrequency(FMath::Max(1.0f, LootBagNetUpdateFrequency * 0.4f));
	LootBagActor->FinishSpawning(SpawnTransform);
	StabilizeLootBagActor(LootBagActor);
	ConfigureLootMarkerForVictimOnly(LootBagActor);

	const FString DeadPlayerDisplayName = SanitizeDeadPlayerNameForNet(
		ResolveDeadPlayerName(DeadCharacter),
		DeadPlayerNameMaxUtf8Bytes);
	SetLootBagDeadPlayerName(LootBagActor, DeadPlayerDisplayName);

	USpellRiseStorageComponent* LootBagStorage = LootBagActor->FindComponentByClass<USpellRiseStorageComponent>();
	if (!LootBagStorage)
	{
		UE_LOG(LogSpellRiseFullLoot, Error, TEXT("ProcessCharacterDeathNow: loot bag sem SpellRiseStorageComponent. Bag=%s"), *GetNameSafe(LootBagActor));
		LootBagActor->Destroy();
		return;
	}

	// Garante que o storage comporta todos os itens droppáveis
	LootBagStorage->SetMaxSlots_Authority(InventorySnapshot.Num() + EquipmentSnapshot.Num());

	// Remove todos os itens do equipment ANTES de transferir (NoDrop permanecem no respawn)
	if (EquipmentComp && !EquipmentSnapshot.IsEmpty())
	{
		EquipmentComp->ResetEquipment_Server();
	}

	int32 MovedItems = 0;

	// Transfere itens do equipamento diretamente para o loot bag
	for (const FSpellRiseItemInstance& Item : EquipmentSnapshot)
	{
		FString InsertReason;
		if (!LootBagStorage->InsertItem_Server(Item, INDEX_NONE, InsertReason))
		{
			UE_LOG(LogSpellRiseFullLoot, Warning, TEXT("ProcessCharacterDeathNow: InsertItem (equipment) falhou para %s — %s"),
				*Item.DefinitionId.ToString(), *InsertReason);
			continue;
		}
		++MovedItems;
	}

	// Transfere itens do inventário para o loot bag
	for (const FSpellRiseItemInstance& Snapshot : InventorySnapshot)
	{
		FSpellRiseItemInstance Extracted;
		FString ExtractReason;
		if (!SourceInventory->ExtractItem_Server(Snapshot.ItemInstanceId, Snapshot.Quantity, Extracted, ExtractReason))
		{
			UE_LOG(LogSpellRiseFullLoot, Warning, TEXT("ProcessCharacterDeathNow: ExtractItem falhou para %s — %s"),
				*Snapshot.DefinitionId.ToString(), *ExtractReason);
			continue;
		}

		FString InsertReason;
		if (!LootBagStorage->InsertItem_Server(Extracted, INDEX_NONE, InsertReason))
		{
			UE_LOG(LogSpellRiseFullLoot, Warning, TEXT("ProcessCharacterDeathNow: InsertItem (inventory) falhou para %s — %s. Devolvendo ao inventário."),
				*Extracted.DefinitionId.ToString(), *InsertReason);

			FString ReturnReason;
			if (!SourceInventory->InsertItem_Server(Extracted, Snapshot.SlotIndex, ReturnReason))
			{
				UE_LOG(LogSpellRiseFullLoot, Error, TEXT("ProcessCharacterDeathNow: falha ao devolver item %s ao inventário — %s. Item perdido."),
					*Extracted.DefinitionId.ToString(), *ReturnReason);
			}
			continue;
		}

		++MovedItems;
	}

	if (MovedItems <= 0)
	{
		LootBagActor->Destroy();
		return;
	}

	// Notifica somente o cliente da vítima para exibir o marker no Navigator (marker local, não replicado)
	if (ASpellRisePlayerController* VictimPC = Cast<ASpellRisePlayerController>(LootBagOwner))
	{
		VictimPC->ClientShowLootBagMarker(LootBagActor);
	}

	RegisterTrackedLootBag(LootBagActor, LootBagStorage);
}

void USpellRiseFullLootSubsystem::RegisterTrackedLootBag(AActor* BagActor, USpellRiseStorageComponent* StorageComponent)
{
	if (!BagActor || !StorageComponent)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FTrackedLootBag NewBag;
		NewBag.BagActor = BagActor;
		NewBag.StorageComponent = StorageComponent;
		NewBag.ExpireAtServerTimeSeconds = static_cast<double>(World->GetTimeSeconds()) + LootBagLifetimeSeconds;

		TrackedLootBags.Add(MoveTemp(NewBag));
		EnsureMonitorTimer();
	}
}

void USpellRiseFullLootSubsystem::RegisterTrackedLootBag(AActor* BagActor)
{
	if (!BagActor)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FTrackedLootBag NewBag;
		NewBag.BagActor = BagActor;
		NewBag.StorageComponent = nullptr;
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
		USpellRiseStorageComponent* BagStorage = TrackedBag.StorageComponent.Get();

		if (!BagActor)
		{
			TrackedLootBags.RemoveAtSwap(Index);
			continue;
		}

		const bool bExpired = ServerTimeSeconds >= TrackedBag.ExpireAtServerTimeSeconds;
		const bool bEmpty = BagStorage
			? !BagStorage->GetItems().ContainsByPredicate([](const FSpellRiseItemInstance& I) { return I.Quantity > 0; })
			: false;

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
			BagActor->Destroy();
			TrackedLootBags.RemoveAtSwap(Index);
		}
	}

	StopMonitorTimerIfIdle();
}
