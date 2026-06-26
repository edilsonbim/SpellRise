#include "SpellRise/Inventory/SpellRiseStorageComponent.h"

#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"
#include "SpellRise/Inventory/SpellRiseItemDefinition.h"
#include "SpellRise/Inventory/SpellRiseItemDefinitionResolver.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseStorage, Log, All);

void FSpellRiseStorageList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32)
{
	if (!Owner) return;
	for (const int32 Index : AddedIndices)
	{
		if (Entries.IsValidIndex(Index)) Owner->NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Entries[Index]);
	}
}

void FSpellRiseStorageList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32)
{
	if (!Owner) return;
	for (const int32 Index : ChangedIndices)
	{
		if (Entries.IsValidIndex(Index)) Owner->NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Entries[Index]);
	}
}

void FSpellRiseStorageList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32)
{
	if (!Owner) return;
	for (const int32 Index : RemovedIndices)
	{
		if (Entries.IsValidIndex(Index)) Owner->NotifyReplicatedChange(ESpellRiseInventoryChangeType::Removed, Entries[Index]);
	}
}

USpellRiseStorageComponent::USpellRiseStorageComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
	Storage.SetOwner(this);
}

void USpellRiseStorageComponent::BeginPlay()
{
	Super::BeginPlay();
	Storage.SetOwner(this);
}

void USpellRiseStorageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellRiseStorageComponent, Storage);
}

bool USpellRiseStorageComponent::FindItem(const FGuid& ItemInstanceId, FSpellRiseItemInstance& OutItem) const
{
	const int32 Index = FindEntryIndexById(ItemInstanceId);
	if (!Storage.Entries.IsValidIndex(Index)) return false;
	OutItem = Storage.Entries[Index];
	return true;
}

float USpellRiseStorageComponent::GetCurrentWeight() const
{
	float Total = 0.0f;
	for (const FSpellRiseItemInstance& Entry : Storage.Entries)
	{
		if (const USpellRiseItemDefinition* Definition = ResolveDefinition(Entry.DefinitionId))
		{
			Total += Definition->UnitWeight * Entry.Quantity;
		}
	}
	return Total;
}

bool USpellRiseStorageComponent::AddItem_Server(
	const FPrimaryAssetId& DefinitionId,
	const int32 Quantity,
	const int32 PreferredSlot,
	FGuid& OutItemInstanceId,
	FString& OutRejectReason)
{
	OutItemInstanceId.Invalidate();
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	const USpellRiseItemDefinition* Definition = ResolveDefinition(DefinitionId);
	if (!Definition) { OutRejectReason = TEXT("invalid_definition"); return false; }
	if (Quantity <= 0 || Quantity > MaxQuantityPerOperation || Quantity > Definition->MaxStackSize) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (!CanAddWeight(Definition, Quantity)) { OutRejectReason = TEXT("weight_exceeded"); return false; }
	const int32 Slot = FindFreeSlot(PreferredSlot);
	if (Slot == INDEX_NONE) { OutRejectReason = TEXT("capacity_exceeded"); return false; }

	FSpellRiseItemInstance& Entry = Storage.Entries.AddDefaulted_GetRef();
	Entry.ItemInstanceId = FGuid::NewGuid();
	Entry.DefinitionId = DefinitionId;
	Entry.SlotIndex = Slot;
	Entry.Quantity = Quantity;
	Entry.Durability = Definition->MaxDurability;
	Entry.Revision = NextRevision++;
	Storage.MarkItemDirty(Entry);
	OutItemInstanceId = Entry.ItemInstanceId;
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Entry);
	ForceNetUpdate();
	return true;
}

bool USpellRiseStorageComponent::RemoveItem_Server(const FGuid& ItemInstanceId, const int32 Quantity, FString& OutRejectReason)
{
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	const int32 Index = FindEntryIndexById(ItemInstanceId);
	if (!Storage.Entries.IsValidIndex(Index)) { OutRejectReason = TEXT("invalid_item"); return false; }
	FSpellRiseItemInstance& Entry = Storage.Entries[Index];
	if (Quantity <= 0 || Quantity > Entry.Quantity || Quantity > MaxQuantityPerOperation) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (Quantity == Entry.Quantity)
	{
		const FSpellRiseItemInstance Removed = Entry;
		Storage.Entries.RemoveAt(Index);
		Storage.MarkArrayDirty();
		NotifyReplicatedChange(ESpellRiseInventoryChangeType::Removed, Removed);
	}
	else
	{
		Entry.Quantity -= Quantity;
		MarkEntryChanged(Entry);
		NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Entry);
	}
	ForceNetUpdate();
	return true;
}

bool USpellRiseStorageComponent::MoveItem_Server(
	const FGuid& ItemInstanceId,
	const int32 DestinationSlot,
	const int32 Quantity,
	FString& OutRejectReason)
{
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	if (!ValidateSlot(DestinationSlot)) { OutRejectReason = TEXT("invalid_slot"); return false; }
	const int32 SourceIndex = FindEntryIndexById(ItemInstanceId);
	if (!Storage.Entries.IsValidIndex(SourceIndex)) { OutRejectReason = TEXT("invalid_item"); return false; }
	FSpellRiseItemInstance& Source = Storage.Entries[SourceIndex];
	if (Quantity <= 0 || Quantity > Source.Quantity || Quantity > MaxQuantityPerOperation) { OutRejectReason = TEXT("invalid_quantity"); return false; }
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
			FSpellRiseItemInstance& Added = Storage.Entries.Add_GetRef(Split);
			Storage.MarkItemDirty(Added);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Source);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Added);
		}
	}
	else
	{
		FSpellRiseItemInstance& Destination = Storage.Entries[DestinationIndex];
		if (Destination.DefinitionId != Source.DefinitionId)
		{
			if (Quantity != Source.Quantity) { OutRejectReason = TEXT("stack_mismatch"); return false; }
			const int32 SourceSlot = Source.SlotIndex;
			Source.SlotIndex = Destination.SlotIndex;
			Destination.SlotIndex = SourceSlot;
			MarkEntryChanged(Source);
			MarkEntryChanged(Destination);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Source);
			NotifyReplicatedChange(ESpellRiseInventoryChangeType::Changed, Destination);
			ForceNetUpdate();
			return true;
		}
		const USpellRiseItemDefinition* Definition = ResolveDefinition(Source.DefinitionId);
		if (!Definition || Destination.Quantity + Quantity > Definition->MaxStackSize) { OutRejectReason = TEXT("capacity_exceeded"); return false; }
		Destination.Quantity += Quantity;
		MarkEntryChanged(Destination);
		const FSpellRiseItemInstance DestinationSnapshot = Destination;
		if (Quantity == Source.Quantity)
		{
			const FSpellRiseItemInstance Removed = Source;
			Storage.Entries.RemoveAt(SourceIndex);
			Storage.MarkArrayDirty();
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
	ForceNetUpdate();
	return true;
}

bool USpellRiseStorageComponent::DepositFromInventory_Server(
	USpellRiseInventoryComponent* Inventory,
	const FGuid& InventoryItemInstanceId,
	const int32 PreferredStorageSlot,
	const int32 Quantity,
	FString& OutRejectReason)
{
	if (!HasServerAuthority() || !Inventory) { OutRejectReason = TEXT("invalid_context"); return false; }
	FSpellRiseItemInstance Extracted;
	if (!Inventory->ExtractItem_Server(InventoryItemInstanceId, Quantity, Extracted, OutRejectReason)) return false;
	if (InsertItem_Server(Extracted, PreferredStorageSlot, OutRejectReason)) return true;

	FString RollbackReason;
	if (!Inventory->InsertItem_Server(Extracted, INDEX_NONE, RollbackReason))
	{
		UE_LOG(LogSpellRiseStorage, Error, TEXT("[Storage][DepositRollbackFailed] Owner=%s Item=%s Reason=%s"),
			*GetNameSafe(GetOwner()), *Extracted.ItemInstanceId.ToString(), *RollbackReason);
	}
	return false;
}

bool USpellRiseStorageComponent::WithdrawToInventory_Server(
	USpellRiseInventoryComponent* Inventory,
	const FGuid& StorageItemInstanceId,
	const int32 PreferredInventorySlot,
	const int32 Quantity,
	FString& OutRejectReason)
{
	if (!HasServerAuthority() || !Inventory) { OutRejectReason = TEXT("invalid_context"); return false; }
	FSpellRiseItemInstance Extracted;
	if (!ExtractItem_Server(StorageItemInstanceId, Quantity, Extracted, OutRejectReason)) return false;
	if (Inventory->InsertItem_Server(Extracted, PreferredInventorySlot, OutRejectReason)) return true;

	FString RollbackReason;
	if (!RestoreExtractedItem_Server(Extracted, RollbackReason))
	{
		UE_LOG(LogSpellRiseStorage, Error, TEXT("[Storage][WithdrawRollbackFailed] Owner=%s Item=%s Reason=%s"),
			*GetNameSafe(GetOwner()), *Extracted.ItemInstanceId.ToString(), *RollbackReason);
	}
	return false;
}

void USpellRiseStorageComponent::ResetStorage_Server()
{
	if (!HasServerAuthority()) return;
	Storage.Entries.Reset();
	Storage.MarkArrayDirty();
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Reset, FSpellRiseItemInstance());
	ForceNetUpdate();
}

void USpellRiseStorageComponent::NotifyReplicatedChange(
	const ESpellRiseInventoryChangeType ChangeType,
	const FSpellRiseItemInstance& Item)
{
	OnStorageChanged.Broadcast(ChangeType, Item);
}

bool USpellRiseStorageComponent::HasServerAuthority() const
{
	return GetOwner() && GetOwner()->HasAuthority();
}

bool USpellRiseStorageComponent::ValidateSlot(const int32 SlotIndex) const
{
	return SlotIndex >= 0 && SlotIndex < MaxSlots;
}

int32 USpellRiseStorageComponent::FindFreeSlot(const int32 PreferredSlot) const
{
	if (ValidateSlot(PreferredSlot) && FindEntryIndexBySlot(PreferredSlot) == INDEX_NONE) return PreferredSlot;
	for (int32 Slot = 0; Slot < MaxSlots; ++Slot)
	{
		if (FindEntryIndexBySlot(Slot) == INDEX_NONE) return Slot;
	}
	return INDEX_NONE;
}

int32 USpellRiseStorageComponent::FindEntryIndexById(const FGuid& ItemInstanceId) const
{
	return Storage.Entries.IndexOfByPredicate([&ItemInstanceId](const FSpellRiseItemInstance& Entry)
	{
		return Entry.ItemInstanceId == ItemInstanceId;
	});
}

int32 USpellRiseStorageComponent::FindEntryIndexBySlot(const int32 SlotIndex) const
{
	return Storage.Entries.IndexOfByPredicate([SlotIndex](const FSpellRiseItemInstance& Entry)
	{
		return Entry.SlotIndex == SlotIndex;
	});
}

bool USpellRiseStorageComponent::CanAddWeight(const USpellRiseItemDefinition* Definition, const int32 Quantity) const
{
	return Definition && (MaxWeight <= 0.0f || GetCurrentWeight() + Definition->UnitWeight * Quantity <= MaxWeight + KINDA_SMALL_NUMBER);
}

bool USpellRiseStorageComponent::ExtractItem_Server(
	const FGuid& ItemInstanceId,
	const int32 Quantity,
	FSpellRiseItemInstance& OutExtractedItem,
	FString& OutRejectReason)
{
	const int32 Index = FindEntryIndexById(ItemInstanceId);
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	if (!Storage.Entries.IsValidIndex(Index)) { OutRejectReason = TEXT("invalid_item"); return false; }
	const FSpellRiseItemInstance Original = Storage.Entries[Index];
	if (Quantity <= 0 || Quantity > Original.Quantity || Quantity > MaxQuantityPerOperation) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	OutExtractedItem = Original;
	OutExtractedItem.Quantity = Quantity;
	if (Quantity < Original.Quantity) OutExtractedItem.ItemInstanceId = FGuid::NewGuid();
	return RemoveItem_Server(ItemInstanceId, Quantity, OutRejectReason);
}

bool USpellRiseStorageComponent::InsertItem_Server(
	const FSpellRiseItemInstance& Item,
	const int32 PreferredSlot,
	FString& OutRejectReason)
{
	if (!HasServerAuthority()) { OutRejectReason = TEXT("invalid_context"); return false; }
	if (!Item.ItemInstanceId.IsValid() || FindEntryIndexById(Item.ItemInstanceId) != INDEX_NONE) { OutRejectReason = TEXT("invalid_item"); return false; }
	const USpellRiseItemDefinition* Definition = ResolveDefinition(Item.DefinitionId);
	if (!Definition) { OutRejectReason = TEXT("invalid_definition"); return false; }
	if (Item.Quantity <= 0 || Item.Quantity > Definition->MaxStackSize || Item.Quantity > MaxQuantityPerOperation) { OutRejectReason = TEXT("invalid_quantity"); return false; }
	if (!CanAddWeight(Definition, Item.Quantity)) { OutRejectReason = TEXT("weight_exceeded"); return false; }
	const int32 Slot = FindFreeSlot(PreferredSlot);
	if (Slot == INDEX_NONE) { OutRejectReason = TEXT("capacity_exceeded"); return false; }
	FSpellRiseItemInstance Copy = Item;
	Copy.SlotIndex = Slot;
	Copy.Revision = NextRevision++;
	FSpellRiseItemInstance& Added = Storage.Entries.Add_GetRef(Copy);
	Storage.MarkItemDirty(Added);
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Added);
	ForceNetUpdate();
	return true;
}

bool USpellRiseStorageComponent::RestoreExtractedItem_Server(
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
	FSpellRiseItemInstance& Added = Storage.Entries.Add_GetRef(Copy);
	Storage.MarkItemDirty(Added);
	NotifyReplicatedChange(ESpellRiseInventoryChangeType::Added, Added);
	ForceNetUpdate();
	return true;
}

const USpellRiseItemDefinition* USpellRiseStorageComponent::ResolveDefinition(const FPrimaryAssetId& DefinitionId) const
{
	return SpellRiseItemDefinitionResolver::ResolveItemDefinition(DefinitionId);
}

void USpellRiseStorageComponent::MarkEntryChanged(FSpellRiseItemInstance& Entry)
{
	Entry.Revision = NextRevision++;
	Storage.MarkItemDirty(Entry);
}

void USpellRiseStorageComponent::ForceNetUpdate() const
{
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
}
