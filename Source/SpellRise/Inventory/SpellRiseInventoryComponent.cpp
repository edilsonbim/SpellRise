#include "SpellRiseInventoryComponent.h"

#include "Engine/AssetManager.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRiseItemDefinition.h"
#include "SpellRise/Equipment/SpellRiseWeaponDefinition.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseInventory, Log, All);

void FSpellRiseInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32)
{
	if (!Owner) return;
	for (const int32 Index : AddedIndices)
	{
		if (Entries.IsValidIndex(Index)) Owner->NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Entries[Index]);
	}
}

void FSpellRiseInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32)
{
	if (!Owner) return;
	for (const int32 Index : ChangedIndices)
	{
		if (Entries.IsValidIndex(Index)) Owner->NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Entries[Index]);
	}
}

void FSpellRiseInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32)
{
	if (!Owner) return;
	for (const int32 Index : RemovedIndices)
	{
		if (Entries.IsValidIndex(Index)) Owner->NotifyReplicatedChange(ESpellRiseInventoryChangeType::Removed, Entries[Index]);
	}
}

USpellRiseInventoryComponent::USpellRiseInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
	Inventory.SetOwner(this);
}

void USpellRiseInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Inventory.SetOwner(this);
}

void USpellRiseInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(USpellRiseInventoryComponent, Inventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USpellRiseInventoryComponent, MaxSlots, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USpellRiseInventoryComponent, MaxWeight, COND_OwnerOnly);
}

bool USpellRiseInventoryComponent::FindItem(const FGuid& ItemInstanceId, FSpellRiseItemInstance& OutItem) const
{
	const int32 Index = FindEntryIndexById(ItemInstanceId);
	if (!Inventory.Entries.IsValidIndex(Index)) return false;
	OutItem = Inventory.Entries[Index];
	return true;
}

float USpellRiseInventoryComponent::GetCurrentWeight() const
{
	float Total = 0.0f;
	for (const FSpellRiseItemInstance& Entry : Inventory.Entries)
	{
		if (const USpellRiseItemDefinition* Definition = ResolveDefinition(Entry.DefinitionId))
		{
			Total += Definition->UnitWeight * Entry.Quantity;
		}
	}
	return Total;
}

void USpellRiseInventoryComponent::SetMaxWeight_Server(const float NewMaxWeight)
{
	if (HasServerAuthority() && FMath::IsFinite(NewMaxWeight))
	{
		MaxWeight = FMath::Max(0.0f, NewMaxWeight);
		ForceOwnerNetUpdate();
	}
}

void USpellRiseInventoryComponent::RequestMoveItem(FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity, int32 ClientRequestId)
{
	if (HasServerAuthority())
	{
		ServerRequestMoveItem_Implementation(ItemInstanceId, DestinationSlot, Quantity, ClientRequestId);
		return;
	}
	ServerRequestMoveItem(ItemInstanceId, DestinationSlot, Quantity, ClientRequestId);
}

void USpellRiseInventoryComponent::RequestDropItem(FGuid ItemInstanceId, int32 Quantity, int32 ClientRequestId)
{
	if (HasServerAuthority())
	{
		ServerRequestDropItem_Implementation(ItemInstanceId, Quantity, ClientRequestId);
		return;
	}
	ServerRequestDropItem(ItemInstanceId, Quantity, ClientRequestId);
}

void USpellRiseInventoryComponent::ServerRequestMoveItem_Implementation(FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity, int32 ClientRequestId)
{
	FString Reason;
	if (!IsRuntimeReady(Reason) || !ValidateRequestId(ClientRequestId, Reason) || !CheckRateLimit(MoveRateLimit, MoveRequestsPerWindow, Reason))
	{
		ResolveRequest(ClientRequestId, MapRejectReason(Reason), TEXT("MoveItem"), Reason);
		return;
	}
	const bool bSuccess = MoveItem_Server(ItemInstanceId, DestinationSlot, Quantity, Reason);
	ResolveRequest(ClientRequestId, bSuccess ? ESpellRiseInventoryRequestResult::Success : MapRejectReason(Reason), TEXT("MoveItem"), Reason);
}

void USpellRiseInventoryComponent::ServerRequestDropItem_Implementation(FGuid ItemInstanceId, int32 Quantity, int32 ClientRequestId)
{
	FString Reason;
	if (!IsRuntimeReady(Reason) || !ValidateRequestId(ClientRequestId, Reason) || !CheckRateLimit(DropRateLimit, DropRequestsPerWindow, Reason))
	{
		ResolveRequest(ClientRequestId, MapRejectReason(Reason), TEXT("DropItem"), Reason);
		return;
	}

	FSpellRiseItemInstance Extracted;
	if (!ExtractItem_Server(ItemInstanceId, Quantity, Extracted, Reason))
	{
		ResolveRequest(ClientRequestId, MapRejectReason(Reason), TEXT("DropItem"), Reason);
		return;
	}

	if (!CommitDrop_Server(Extracted))
	{
		FString RollbackReason;
		if (!RestoreExtractedItem_Server(Extracted, RollbackReason))
		{
			UE_LOG(LogSpellRiseInventory, Error,
				TEXT("[Inventory][RollbackFailed] Owner=%s Item=%s Reason=%s"),
				*GetNameSafe(GetOwner()),
				*Extracted.ItemInstanceId.ToString(),
				*RollbackReason);
			ResolveRequest(ClientRequestId, ESpellRiseInventoryRequestResult::TransactionFailed, TEXT("DropItem"), RollbackReason);
			return;
		}
		Reason = TEXT("drop_unavailable");
		ResolveRequest(ClientRequestId, ESpellRiseInventoryRequestResult::DropUnavailable, TEXT("DropItem"), Reason);
		return;
	}

	ResolveRequest(ClientRequestId, ESpellRiseInventoryRequestResult::Success, TEXT("DropItem"), FString());
}

bool USpellRiseInventoryComponent::AddItem_Server(const FPrimaryAssetId& DefinitionId, int32 Quantity, int32 PreferredSlot, FGuid& OutItemInstanceId, FString& OutRejectReason)
{
	OutItemInstanceId.Invalidate();
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	const USpellRiseItemDefinition* Definition = ResolveDefinition(DefinitionId);
	if (!Definition) { OutRejectReason = TEXT("invalid_definition"); return false; }
	if (Quantity <= 0 || Quantity > MaxQuantityPerRequest || Quantity > Definition->MaxStackSize) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (!CanAddWeight(Definition, Quantity)) { OutRejectReason = TEXT("weight_exceeded"); return false; }
	const int32 Slot = FindFreeSlot(PreferredSlot);
	if (Slot == INDEX_NONE) { OutRejectReason = TEXT("capacity_exceeded"); return false; }

	FSpellRiseItemInstance& Entry = Inventory.Entries.AddDefaulted_GetRef();
	Entry.ItemInstanceId = FGuid::NewGuid();
	Entry.DefinitionId = DefinitionId;
	Entry.SlotIndex = Slot;
	Entry.Quantity = Quantity;
	Entry.Durability = Definition->MaxDurability;
	Entry.Revision = NextRevision++;
	Inventory.MarkItemDirty(Entry);
	OutItemInstanceId = Entry.ItemInstanceId;
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Entry);
	ForceOwnerNetUpdate();
	return true;
}

bool USpellRiseInventoryComponent::RemoveItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FString& OutRejectReason)
{
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	const int32 Index = FindEntryIndexById(ItemInstanceId);
	if (!Inventory.Entries.IsValidIndex(Index)) { OutRejectReason = TEXT("invalid_item"); return false; }
	FSpellRiseItemInstance& Entry = Inventory.Entries[Index];
	if (Quantity <= 0 || Quantity > Entry.Quantity || Quantity > MaxQuantityPerRequest) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (Quantity == Entry.Quantity)
	{
		const FSpellRiseItemInstance Removed = Entry;
		Inventory.Entries.RemoveAt(Index);
		Inventory.MarkArrayDirty();
		NotifyReplicatedChange(ESpellRiseInventoryChangeType::Removed, Removed);
	}
	else
	{
		Entry.Quantity -= Quantity;
		MarkEntryChanged(Entry);
		NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Entry);
	}
	ForceOwnerNetUpdate();
	return true;
}

bool USpellRiseInventoryComponent::MoveItem_Server(const FGuid& ItemInstanceId, int32 DestinationSlot, int32 Quantity, FString& OutRejectReason)
{
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	if (!ValidateSlot(DestinationSlot)) { OutRejectReason = TEXT("invalid_slot"); return false; }
	const int32 SourceIndex = FindEntryIndexById(ItemInstanceId);
	if (!Inventory.Entries.IsValidIndex(SourceIndex)) { OutRejectReason = TEXT("invalid_item"); return false; }
	FSpellRiseItemInstance& Source = Inventory.Entries[SourceIndex];
	if (Quantity <= 0 || Quantity > Source.Quantity || Quantity > MaxQuantityPerRequest) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (Source.SlotIndex == DestinationSlot) return true;

	const int32 DestinationIndex = FindEntryIndexBySlot(DestinationSlot);
	if (DestinationIndex == INDEX_NONE)
	{
		if (Quantity == Source.Quantity)
		{
			Source.SlotIndex = DestinationSlot;
			MarkEntryChanged(Source);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Source);
		}
		else
		{
			FSpellRiseItemInstance Split = Source;
			Split.ItemInstanceId = FGuid::NewGuid();
			Split.SlotIndex = DestinationSlot;
			Split.Quantity = Quantity;
			Split.Revision = NextRevision++;
			Source.Quantity -= Quantity;
			MarkEntryChanged(Source);
			FSpellRiseItemInstance& Added = Inventory.Entries.Add_GetRef(Split);
			Inventory.MarkItemDirty(Added);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Source);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Added);
		}
	}
	else
	{
		FSpellRiseItemInstance& Destination = Inventory.Entries[DestinationIndex];
		if (Destination.DefinitionId != Source.DefinitionId) { OutRejectReason = TEXT("stack_mismatch"); return false; }
		const USpellRiseItemDefinition* Definition = ResolveDefinition(Source.DefinitionId);
		if (!Definition || Destination.Quantity + Quantity > Definition->MaxStackSize) { OutRejectReason = TEXT("capacity_exceeded"); return false; }
		Destination.Quantity += Quantity;
		MarkEntryChanged(Destination);
		const FSpellRiseItemInstance DestinationSnapshot = Destination;
		if (Quantity == Source.Quantity)
		{
			const FSpellRiseItemInstance Removed = Source;
			Inventory.Entries.RemoveAt(SourceIndex);
			Inventory.MarkArrayDirty();
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Removed, Removed);
		}
		else
		{
			Source.Quantity -= Quantity;
			MarkEntryChanged(Source);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Source);
		}
		NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, DestinationSnapshot);
	}
	ForceOwnerNetUpdate();
	return true;
}

bool USpellRiseInventoryComponent::ExtractItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FSpellRiseItemInstance& OutExtractedItem, FString& OutRejectReason)
{
	const int32 Index = FindEntryIndexById(ItemInstanceId);
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	if (!Inventory.Entries.IsValidIndex(Index)) { OutRejectReason = TEXT("invalid_item"); return false; }
	const FSpellRiseItemInstance Original = Inventory.Entries[Index];
	if (Quantity <= 0 || Quantity > Original.Quantity || Quantity > MaxQuantityPerRequest) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	OutExtractedItem = Original;
	OutExtractedItem.Quantity = Quantity;
	if (Quantity < Original.Quantity) OutExtractedItem.ItemInstanceId = FGuid::NewGuid();
	return RemoveItem_Server(ItemInstanceId, Quantity, OutRejectReason);
}

bool USpellRiseInventoryComponent::InsertItem_Server(const FSpellRiseItemInstance& Item, int32 PreferredSlot, FString& OutRejectReason)
{
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	if (!Item.ItemInstanceId.IsValid() || FindEntryIndexById(Item.ItemInstanceId) != INDEX_NONE) { OutRejectReason = TEXT("invalid_item"); return false; }
	const USpellRiseItemDefinition* Definition = ResolveDefinition(Item.DefinitionId);
	if (!Definition) { OutRejectReason = TEXT("invalid_definition"); return false; }
	if (Item.Quantity <= 0 || Item.Quantity > Definition->MaxStackSize) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (!CanAddWeight(Definition, Item.Quantity)) { OutRejectReason = TEXT("weight_exceeded"); return false; }
	const int32 Slot = FindFreeSlot(PreferredSlot);
	if (Slot == INDEX_NONE) { OutRejectReason = TEXT("capacity_exceeded"); return false; }
	FSpellRiseItemInstance Copy = Item;
	Copy.SlotIndex = Slot;
	Copy.Revision = NextRevision++;
	FSpellRiseItemInstance& Added = Inventory.Entries.Add_GetRef(Copy);
	Inventory.MarkItemDirty(Added);
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Added);
	ForceOwnerNetUpdate();
	return true;
}

void USpellRiseInventoryComponent::ResetInventory_Server()
{
	if (!HasServerAuthority())
	{
		return;
	}
	Inventory.Entries.Reset();
	Inventory.MarkArrayDirty();
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Reset, FSpellRiseItemInstance());
	ForceOwnerNetUpdate();
}

const USpellRiseItemDefinition* USpellRiseInventoryComponent::ResolveDefinition(const FPrimaryAssetId& DefinitionId) const
{
	if (!DefinitionId.IsValid()) return nullptr;
	UAssetManager& Manager = UAssetManager::Get();
	if (const UObject* Loaded = Manager.GetPrimaryAssetObject(DefinitionId))
	{
		return Cast<USpellRiseItemDefinition>(Loaded);
	}
	const FSoftObjectPath Path = Manager.GetPrimaryAssetPath(DefinitionId);
	return Cast<USpellRiseItemDefinition>(Path.TryLoad());
}

bool USpellRiseInventoryComponent::Equipment_TakeItem(
	const FGuid& ItemInstanceId,
	FSpellRiseEquipmentItemData& OutItem,
	FString& OutRejectReason)
{
	FSpellRiseItemInstance Instance;
	if (!FindItem(ItemInstanceId, Instance))
	{
		OutRejectReason = TEXT("invalid_item");
		return false;
	}

	const USpellRiseItemDefinition* Definition = ResolveDefinition(Instance.DefinitionId);
	if (!Definition || Instance.Quantity != 1)
	{
		OutRejectReason = Definition ? TEXT("invalid_quantity") : TEXT("invalid_definition");
		return false;
	}

	OutItem = FSpellRiseEquipmentItemData();
	OutItem.ItemInstanceId = Instance.ItemInstanceId;
	OutItem.DefinitionId = Instance.DefinitionId;
	OutItem.Durability = Instance.Durability;
	OutItem.UnitWeight = Definition->UnitWeight;

	if (const USpellRiseWeaponItemDefinition* WeaponItem = Cast<USpellRiseWeaponItemDefinition>(Definition))
	{
		OutItem.WeaponDefinition = WeaponItem->WeaponDefinition;
		const USpellRiseWeaponDefinition* WeaponDefinition = WeaponItem->WeaponDefinition.LoadSynchronous();
		if (!WeaponDefinition)
		{
			OutRejectReason = TEXT("invalid_definition");
			return false;
		}
		OutItem.bTwoHanded = WeaponDefinition->HandPolicy == ESpellRiseWeaponHandPolicy::TwoHanded;
		OutItem.AllowedSlots.Add(
			WeaponDefinition->HandPolicy == ESpellRiseWeaponHandPolicy::OffHand
				? ESpellRiseEquipmentSlot::OffHand
				: ESpellRiseEquipmentSlot::MainHand);
		OutItem.GrantedAbilities = WeaponDefinition->AbilitiesToGrant;
		OutItem.GrantedEffects = WeaponDefinition->GrantedEffectClassesWhileEquipped;
		OutItem.SetByCallerMagnitudes = WeaponDefinition->SetByCallerMagnitudes;
	}
	else if (const USpellRiseArmorItemDefinition* Armor = Cast<USpellRiseArmorItemDefinition>(Definition))
	{
		static const TPair<const TCHAR*, ESpellRiseEquipmentSlot> SlotTags[] = {
			{ TEXT("Equipment.Slot.Head"), ESpellRiseEquipmentSlot::Head },
			{ TEXT("Equipment.Slot.Chest"), ESpellRiseEquipmentSlot::Chest },
			{ TEXT("Equipment.Slot.Hands"), ESpellRiseEquipmentSlot::Hands },
			{ TEXT("Equipment.Slot.Legs"), ESpellRiseEquipmentSlot::Legs },
			{ TEXT("Equipment.Slot.Feet"), ESpellRiseEquipmentSlot::Feet },
			{ TEXT("Equipment.Slot.Necklace"), ESpellRiseEquipmentSlot::Necklace },
			{ TEXT("Equipment.Slot.Backpack"), ESpellRiseEquipmentSlot::Backpack }
		};
		for (const TPair<const TCHAR*, ESpellRiseEquipmentSlot>& Pair : SlotTags)
		{
			const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(Pair.Key, false);
			if (Tag.IsValid() && Armor->AllowedEquipmentSlots.HasTagExact(Tag))
			{
				OutItem.AllowedSlots.Add(Pair.Value);
			}
		}
		OutItem.GrantedEffects = Armor->EquippedEffects;
	}

	if (OutItem.AllowedSlots.IsEmpty())
	{
		OutRejectReason = TEXT("slot_incompatible");
		return false;
	}

	FSpellRiseItemInstance Extracted;
	return ExtractItem_Server(ItemInstanceId, 1, Extracted, OutRejectReason);
}

bool USpellRiseInventoryComponent::Equipment_ReturnItem(
	const FSpellRiseEquipmentItemData& Item,
	const int32 PreferredInventorySlot,
	FString& OutRejectReason)
{
	FSpellRiseItemInstance Instance;
	Instance.ItemInstanceId = Item.ItemInstanceId;
	Instance.DefinitionId = Item.DefinitionId;
	Instance.Quantity = 1;
	Instance.Durability = Item.Durability;
	return InsertItem_Server(Instance, PreferredInventorySlot, OutRejectReason);
}

bool USpellRiseInventoryComponent::Equipment_CanReturnItem(
	const FSpellRiseEquipmentItemData& Item,
	const int32 PreferredInventorySlot,
	FString& OutRejectReason) const
{
	const USpellRiseItemDefinition* Definition = ResolveDefinition(Item.DefinitionId);
	if (!Definition)
	{
		OutRejectReason = TEXT("invalid_definition");
		return false;
	}
	if (FindFreeSlot(PreferredInventorySlot) == INDEX_NONE)
	{
		OutRejectReason = TEXT("capacity_exceeded");
		return false;
	}
	if (!CanAddWeight(Definition, 1))
	{
		OutRejectReason = TEXT("weight_exceeded");
		return false;
	}
	return true;
}

bool USpellRiseInventoryComponent::CommitDrop_Server_Implementation(const FSpellRiseItemInstance&)
{
	return false;
}

void USpellRiseInventoryComponent::NotifyReplicatedChange(ESpellRiseInventoryChangeType ChangeType, const FSpellRiseItemInstance& Item)
{
	OnInventoryChanged.Broadcast(ChangeType, Item);
}

bool USpellRiseInventoryComponent::HasServerAuthority() const
{
	return GetOwner() && GetOwner()->HasAuthority();
}

bool USpellRiseInventoryComponent::ValidateRequestId(int32 ClientRequestId, FString& OutRejectReason)
{
	if (ClientRequestId <= 0) { OutRejectReason = TEXT("stale_request"); return false; }
	if (RecentRequestIds.Contains(ClientRequestId)) { OutRejectReason = TEXT("duplicate_request"); return false; }
	RecentRequestIds.Add(ClientRequestId);
	RecentRequestOrder.Add(ClientRequestId);
	while (RecentRequestOrder.Num() > 128)
	{
		RecentRequestIds.Remove(RecentRequestOrder[0]);
		RecentRequestOrder.RemoveAt(0);
	}
	return true;
}

bool USpellRiseInventoryComponent::CheckRateLimit(FSpellRiseInventoryRateLimitState& State, int32 MaxRequests, FString& OutRejectReason)
{
	const UWorld* World = GetWorld();
	if (!World) { OutRejectReason = TEXT("invalid_context"); return false; }
	const double Now = World->GetTimeSeconds();
	if (Now - State.WindowStartSeconds > RateLimitWindowSeconds)
	{
		State.WindowStartSeconds = Now;
		State.RequestsInWindow = 0;
	}
	if (++State.RequestsInWindow > MaxRequests) { OutRejectReason = TEXT("rate_limited"); return false; }
	return true;
}

bool USpellRiseInventoryComponent::ValidateSlot(int32 SlotIndex) const
{
	return SlotIndex >= 0 && SlotIndex < MaxSlots;
}

int32 USpellRiseInventoryComponent::FindFreeSlot(int32 PreferredSlot) const
{
	if (ValidateSlot(PreferredSlot) && FindEntryIndexBySlot(PreferredSlot) == INDEX_NONE) return PreferredSlot;
	for (int32 Slot = 0; Slot < MaxSlots; ++Slot)
	{
		if (FindEntryIndexBySlot(Slot) == INDEX_NONE) return Slot;
	}
	return INDEX_NONE;
}

int32 USpellRiseInventoryComponent::FindEntryIndexById(const FGuid& ItemInstanceId) const
{
	return Inventory.Entries.IndexOfByPredicate([&](const FSpellRiseItemInstance& Entry) { return Entry.ItemInstanceId == ItemInstanceId; });
}

int32 USpellRiseInventoryComponent::FindEntryIndexBySlot(int32 SlotIndex) const
{
	return Inventory.Entries.IndexOfByPredicate([&](const FSpellRiseItemInstance& Entry) { return Entry.SlotIndex == SlotIndex; });
}

bool USpellRiseInventoryComponent::CanAddWeight(const USpellRiseItemDefinition* Definition, int32 Quantity) const
{
	return Definition && (MaxWeight <= 0.0f || GetCurrentWeight() + Definition->UnitWeight * Quantity <= MaxWeight + KINDA_SMALL_NUMBER);
}

void USpellRiseInventoryComponent::MarkEntryChanged(FSpellRiseItemInstance& Entry)
{
	Entry.Revision = NextRevision++;
	Inventory.MarkItemDirty(Entry);
}

void USpellRiseInventoryComponent::ResolveRequest(int32 ClientRequestId, ESpellRiseInventoryRequestResult Result, const TCHAR* RpcName, const FString& Reason)
{
	if (Result != ESpellRiseInventoryRequestResult::Success)
	{
		UE_LOG(LogSpellRiseInventory, Warning, TEXT("[Inventory][RpcRejected] Rpc=%s Owner=%s RequestId=%d Reason=%s"),
			RpcName, *GetNameSafe(GetOwner()), ClientRequestId, *Reason);
	}
	OnRequestResolved.Broadcast(ClientRequestId, Result);
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ClientResolveRequest(ClientRequestId, Result);
	}
}

void USpellRiseInventoryComponent::ClientResolveRequest_Implementation(
	const int32 ClientRequestId,
	const ESpellRiseInventoryRequestResult Result)
{
	OnRequestResolved.Broadcast(ClientRequestId, Result);
}

bool USpellRiseInventoryComponent::IsRuntimeReady(FString& OutRejectReason) const
{
	if (!bNativeInventoryEnabled)
	{
		OutRejectReason = TEXT("feature_disabled");
		return false;
	}
	const ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner());
	if (!PlayerState || !PlayerState->IsPersistenceProfileApplied())
	{
		OutRejectReason = TEXT("persistence_not_ready");
		return false;
	}
	return true;
}

bool USpellRiseInventoryComponent::RestoreExtractedItem_Server(
	const FSpellRiseItemInstance& Item,
	FString& OutRejectReason)
{
	if (!HasServerAuthority() || !Item.ItemInstanceId.IsValid() || FindEntryIndexById(Item.ItemInstanceId) != INDEX_NONE)
	{
		OutRejectReason = TEXT("rollback_invalid_item");
		return false;
	}
	if (FindEntryIndexBySlot(Item.SlotIndex) != INDEX_NONE)
	{
		OutRejectReason = TEXT("rollback_slot_occupied");
		return false;
	}
	FSpellRiseItemInstance Copy = Item;
	Copy.Revision = NextRevision++;
	FSpellRiseItemInstance& Added = Inventory.Entries.Add_GetRef(Copy);
	Inventory.MarkItemDirty(Added);
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Added);
	ForceOwnerNetUpdate();
	return true;
}

ESpellRiseInventoryRequestResult USpellRiseInventoryComponent::MapRejectReason(const FString& Reason) const
{
	if (Reason == TEXT("invalid_context")) return ESpellRiseInventoryRequestResult::InvalidContext;
	if (Reason == TEXT("invalid_item")) return ESpellRiseInventoryRequestResult::InvalidItem;
	if (Reason == TEXT("invalid_definition")) return ESpellRiseInventoryRequestResult::InvalidDefinition;
	if (Reason == TEXT("invalid_slot")) return ESpellRiseInventoryRequestResult::InvalidSlot;
	if (Reason == TEXT("invalid_quantity")) return ESpellRiseInventoryRequestResult::InvalidQuantity;
	if (Reason == TEXT("stack_mismatch")) return ESpellRiseInventoryRequestResult::StackMismatch;
	if (Reason == TEXT("capacity_exceeded")) return ESpellRiseInventoryRequestResult::CapacityExceeded;
	if (Reason == TEXT("weight_exceeded")) return ESpellRiseInventoryRequestResult::WeightExceeded;
	if (Reason == TEXT("item_locked")) return ESpellRiseInventoryRequestResult::ItemLocked;
	if (Reason == TEXT("rate_limited")) return ESpellRiseInventoryRequestResult::RateLimited;
	if (Reason == TEXT("duplicate_request") || Reason == TEXT("stale_request")) return ESpellRiseInventoryRequestResult::DuplicateRequest;
	if (Reason == TEXT("feature_disabled") || Reason == TEXT("persistence_not_ready")) return ESpellRiseInventoryRequestResult::InvalidContext;
	return ESpellRiseInventoryRequestResult::TransactionFailed;
}

void USpellRiseInventoryComponent::ForceOwnerNetUpdate() const
{
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
}
