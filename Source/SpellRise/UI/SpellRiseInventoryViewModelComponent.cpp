#include "SpellRise/UI/SpellRiseInventoryViewModelComponent.h"

#include "SpellRise/Equipment/SpellRiseEquipmentComponent.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"
#include "SpellRise/Inventory/SpellRiseItemDefinition.h"
#include "SpellRise/Inventory/SpellRiseItemDefinitionResolver.h"
#include "SpellRise/Inventory/SpellRiseStorageComponent.h"

namespace SpellRiseInventoryViewModel
{
	const FName MoveRequest(TEXT("move"));
	const FName TransferRequest(TEXT("transfer"));
	const FName EquipRequest(TEXT("equip"));
	const FName UnequipRequest(TEXT("unequip"));
	const FName SwapEquipmentRequest(TEXT("swap_equipment"));
	const FName DropRequest(TEXT("drop"));
	const FName UseRequest(TEXT("use"));
	const FName DestroyRequest(TEXT("destroy"));
	const FName DepositStorageRequest(TEXT("deposit_storage"));
	const FName WithdrawStorageRequest(TEXT("withdraw_storage"));
	const FName MoveStorageRequest(TEXT("move_storage"));
	const FName InvalidGuid(TEXT("invalid_guid"));
	const FName InvalidSlot(TEXT("invalid_slot"));
	const FName InvalidQuantity(TEXT("invalid_quantity"));
	const FName InvalidStorage(TEXT("invalid_storage"));
	const FName InvalidSource(TEXT("invalid_source"));
	const FName SameSlot(TEXT("same_slot"));
	const FName SlotOccupied(TEXT("slot_occupied"));
	const FName StackFull(TEXT("stack_full"));
	const FName WeightExceeded(TEXT("weight_exceeded"));
	const FName InvalidDefinition(TEXT("invalid_definition"));
}

USpellRiseInventoryViewModelComponent::USpellRiseInventoryViewModelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void USpellRiseInventoryViewModelComponent::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* OwnerActor = GetOwner())
	{
		if (USpellRiseInventoryComponent* Inventory = OwnerActor->FindComponentByClass<USpellRiseInventoryComponent>())
		{
			Inventory->OnInventoryChanged.AddDynamic(this, &ThisClass::HandleInventoryChanged);
		}
		if (USpellRiseEquipmentComponent* Equipment = OwnerActor->FindComponentByClass<USpellRiseEquipmentComponent>())
		{
			Equipment->OnEquipmentChanged.AddDynamic(this, &ThisClass::HandleEquipmentChanged);
		}
		if (USpellRiseStorageComponent* Storage = OwnerActor->FindComponentByClass<USpellRiseStorageComponent>())
		{
			Storage->OnStorageChanged.AddDynamic(this, &ThisClass::HandleStorageChanged);
		}
	}
	RefreshFromAuthoritativeComponents();
}

void USpellRiseInventoryViewModelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (USpellRiseInventoryComponent* Inventory = OwnerActor->FindComponentByClass<USpellRiseInventoryComponent>())
		{
			Inventory->OnInventoryChanged.RemoveDynamic(this, &ThisClass::HandleInventoryChanged);
		}
		if (USpellRiseEquipmentComponent* Equipment = OwnerActor->FindComponentByClass<USpellRiseEquipmentComponent>())
		{
			Equipment->OnEquipmentChanged.RemoveDynamic(this, &ThisClass::HandleEquipmentChanged);
		}
		if (USpellRiseStorageComponent* Storage = OwnerActor->FindComponentByClass<USpellRiseStorageComponent>())
		{
			Storage->OnStorageChanged.RemoveDynamic(this, &ThisClass::HandleStorageChanged);
		}
	}
	for (USpellRiseStorageComponent* Storage : WatchedStorageComponents)
	{
		if (Storage)
		{
			Storage->OnStorageChanged.RemoveDynamic(this, &ThisClass::HandleStorageChanged);
		}
	}
	WatchedStorageComponents.Reset();
	Super::EndPlay(EndPlayReason);
}

FSpellRiseInventoryContainerViewSnapshot USpellRiseInventoryViewModelComponent::GetPlayerInventoryContainerSnapshot() const
{
	FSpellRiseInventoryContainerViewSnapshot Snapshot;
	Snapshot.Source = ESpellRiseInventoryContainerSource::PlayerInventory;
	Snapshot.Slots = InventorySnapshot.Slots;
	Snapshot.MaxSlots = InventorySnapshot.MaxSlots;
	Snapshot.CurrentWeight = InventorySnapshot.CurrentWeight;
	Snapshot.MaxWeight = InventorySnapshot.MaxWeight;
	Snapshot.Revision = InventorySnapshot.Revision;
	return Snapshot;
}

FSpellRiseInventoryContainerViewSnapshot USpellRiseInventoryViewModelComponent::GetStorageSnapshot(
	USpellRiseStorageComponent* Storage) const
{
	FSpellRiseInventoryContainerViewSnapshot Snapshot;
	Snapshot.Source = ESpellRiseInventoryContainerSource::Storage;
	Snapshot.Storage = Storage;
	if (!Storage)
	{
		return Snapshot;
	}

	Snapshot.MaxSlots = Storage->GetMaxSlots();
	Snapshot.CurrentWeight = Storage->GetCurrentWeight();
	Snapshot.MaxWeight = Storage->GetMaxWeight();
	for (const FSpellRiseItemInstance& Item : Storage->GetItems())
	{
		FSpellRiseInventorySlotView& View = Snapshot.Slots.AddDefaulted_GetRef();
		View.ItemInstanceId = Item.ItemInstanceId;
		View.DefinitionId = Item.DefinitionId;
		View.SlotIndex = Item.SlotIndex;
		View.Quantity = Item.Quantity;
		View.Durability = Item.Durability;
		View.Flags = Item.Flags;
		View.bDropLocked = (Item.Flags & static_cast<uint8>(ESpellRiseItemInstanceFlags::NoDrop)) != 0;
		Snapshot.Revision = FMath::Max(Snapshot.Revision, Item.Revision);
	}
	return Snapshot;
}

void USpellRiseInventoryViewModelComponent::ApplyAuthoritativeInventorySnapshot(
	const FSpellRiseInventoryViewSnapshot& Snapshot)
{
	InventorySnapshot = Snapshot;
	OnInventorySnapshotChanged.Broadcast(InventorySnapshot);
}

void USpellRiseInventoryViewModelComponent::ApplyAuthoritativeEquipmentSnapshot(
	const FSpellRiseEquipmentViewSnapshot& Snapshot)
{
	EquipmentSnapshot = Snapshot;
	OnEquipmentSnapshotChanged.Broadcast(EquipmentSnapshot);
}

void USpellRiseInventoryViewModelComponent::RefreshPresentationFromAuthoritativeState()
{
	RefreshFromAuthoritativeComponents();
	for (USpellRiseStorageComponent* Storage : WatchedStorageComponents)
	{
		BroadcastStorageSnapshot(Storage);
	}
}

bool USpellRiseInventoryViewModelComponent::RequestMoveItem(
	const FGuid ItemInstanceId,
	const int32 DestinationSlot,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(MoveRequest, InvalidGuid);
	}
	if (DestinationSlot < 0 || (InventorySnapshot.MaxSlots > 0 && DestinationSlot >= InventorySnapshot.MaxSlots))
	{
		return RejectRequest(MoveRequest, InvalidSlot);
	}
	if (Quantity <= 0)
	{
		return RejectRequest(MoveRequest, InvalidQuantity);
	}

	OnMoveItemRequested.Broadcast(ItemInstanceId, DestinationSlot, Quantity);
	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestMoveItem(ItemInstanceId, DestinationSlot, Quantity, AllocateRequestId());
	}
	else if (USpellRiseStorageComponent* Storage = GetOwnerStorageComponent())
	{
		return RequestMoveStorageItem(Storage, ItemInstanceId, DestinationSlot, Quantity);
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestMoveItemFromInventory(
	USpellRiseInventoryViewModelComponent* SourceInventoryVM,
	const FGuid ItemInstanceId,
	const int32 DestinationSlot,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!SourceInventoryVM)
	{
		return RejectRequest(TransferRequest, InvalidSource);
	}
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(TransferRequest, InvalidGuid);
	}
	if (DestinationSlot < 0 || (InventorySnapshot.MaxSlots > 0 && DestinationSlot >= InventorySnapshot.MaxSlots))
	{
		return RejectRequest(TransferRequest, InvalidSlot);
	}
	if (Quantity <= 0)
	{
		return RejectRequest(TransferRequest, InvalidQuantity);
	}
	if (SourceInventoryVM == this)
	{
		for (USpellRiseStorageComponent* Watched : WatchedStorageComponents)
		{
			FSpellRiseItemInstance FoundItem;
			if (Watched && Watched->FindItem(ItemInstanceId, FoundItem))
			{
				const bool bRequested = RequestMoveStorageItem(Watched, ItemInstanceId, DestinationSlot, Quantity);
				RefreshPresentationFromAuthoritativeState();
				return bRequested;
			}
		}
		return RequestMoveItem(ItemInstanceId, DestinationSlot, Quantity);
	}

	USpellRiseInventoryComponent* SourceInventory = SourceInventoryVM->GetOwnerInventoryComponent();
	USpellRiseStorageComponent* SourceStorage = SourceInventoryVM->GetOwnerStorageComponent();
	USpellRiseInventoryComponent* TargetInventory = GetOwnerInventoryComponent();
	USpellRiseStorageComponent* TargetStorage = GetOwnerStorageComponent();

	if (SourceInventory && TargetStorage)
	{
		const bool bRequested = SourceInventoryVM->RequestDepositToStorage(TargetStorage, ItemInstanceId, DestinationSlot, Quantity);
		if (bRequested)
		{
			SourceInventoryVM->OptimisticallyRemoveFromInventorySnapshot(ItemInstanceId, Quantity);
		}
		RefreshPresentationFromAuthoritativeState();
		return bRequested;
	}
	if (SourceStorage && TargetInventory)
	{
		const bool bRequested = RequestWithdrawFromStorage(SourceStorage, ItemInstanceId, DestinationSlot, Quantity);
		if (bRequested)
		{
			SourceInventoryVM->OptimisticallyRemoveFromInventorySnapshot(ItemInstanceId, Quantity);
		}
		RefreshPresentationFromAuthoritativeState();
		return bRequested;
	}
	if (SourceInventory && TargetInventory)
	{
		const bool bRequested = RequestMoveItem(ItemInstanceId, DestinationSlot, Quantity);
		SourceInventoryVM->RefreshPresentationFromAuthoritativeState();
		RefreshPresentationFromAuthoritativeState();
		return bRequested;
	}
	if (SourceStorage && TargetStorage && SourceStorage == TargetStorage)
	{
		if (TransferAuthorityInventoryVM)
		{
			const bool bRequested = TransferAuthorityInventoryVM->RequestMoveStorageItem(TargetStorage, ItemInstanceId, DestinationSlot, Quantity);
			TransferAuthorityInventoryVM->RefreshPresentationFromAuthoritativeState();
			RefreshPresentationFromAuthoritativeState();
			return bRequested;
		}
		return RejectRequest(TransferRequest, InvalidSource);
	}

	return RejectRequest(TransferRequest, InvalidSource);
}

bool USpellRiseInventoryViewModelComponent::CanPreviewMoveItemFromInventory(
	USpellRiseInventoryViewModelComponent* SourceInventoryVM,
	const FSpellRiseInventorySlotView& SourceSlotView,
	const int32 DestinationSlot,
	const int32 Quantity,
	FName& OutReason) const
{
	using namespace SpellRiseInventoryViewModel;
	OutReason = NAME_None;
	if (!SourceInventoryVM)
	{
		OutReason = InvalidSource;
		return false;
	}
	if (!SourceSlotView.ItemInstanceId.IsValid())
	{
		OutReason = InvalidGuid;
		return false;
	}
	if (DestinationSlot < 0 || (InventorySnapshot.MaxSlots > 0 && DestinationSlot >= InventorySnapshot.MaxSlots))
	{
		OutReason = InvalidSlot;
		return false;
	}
	if (Quantity <= 0 || Quantity > SourceSlotView.Quantity)
	{
		OutReason = InvalidQuantity;
		return false;
	}
	if (SourceInventoryVM == this && SourceSlotView.SlotIndex == DestinationSlot)
	{
		OutReason = SameSlot;
		return false;
	}

	const FSpellRiseInventorySlotView* DestinationItem = FindSnapshotSlotByIndex(DestinationSlot);
	if (DestinationItem)
	{
		if (DestinationItem->ItemInstanceId == SourceSlotView.ItemInstanceId)
		{
			OutReason = SameSlot;
			return false;
		}
		if (DestinationItem->DefinitionId != SourceSlotView.DefinitionId)
		{
			if (Quantity != SourceSlotView.Quantity)
			{
				OutReason = SlotOccupied;
				return false;
			}
			return true;
		}

		const USpellRiseItemDefinition* Definition = SpellRiseItemDefinitionResolver::ResolveItemDefinition(SourceSlotView.DefinitionId);
		if (!Definition)
		{
			OutReason = InvalidDefinition;
			return false;
		}

		const int32 MergeRoom = Definition->MaxStackSize - DestinationItem->Quantity;
		const int32 MergeAmount = FMath::Min(Quantity, FMath::Max(0, MergeRoom));
		int32 RemainingAfterMerge = Quantity - MergeAmount;
		if (RemainingAfterMerge > 0)
		{
			const int32 SlotsNeeded = FMath::DivideAndRoundUp(RemainingAfterMerge, Definition->MaxStackSize);
			if (CountFreeSlotsInSnapshot() < SlotsNeeded)
			{
				OutReason = StackFull;
				return false;
			}
		}
		else if (MergeAmount <= 0)
		{
			OutReason = StackFull;
			return false;
		}
	}
	if (SourceInventoryVM != this && InventorySnapshot.MaxWeight > 0.0f)
	{
		const USpellRiseItemDefinition* Definition = SpellRiseItemDefinitionResolver::ResolveItemDefinition(SourceSlotView.DefinitionId);
		if (!Definition)
		{
			OutReason = InvalidDefinition;
			return false;
		}
		const float AddedWeight = Definition->UnitWeight * Quantity;
		if (InventorySnapshot.CurrentWeight + AddedWeight > InventorySnapshot.MaxWeight + KINDA_SMALL_NUMBER)
		{
			OutReason = WeightExceeded;
			return false;
		}
	}
	return true;
}

void USpellRiseInventoryViewModelComponent::SetTransferAuthorityInventoryViewModel(
	USpellRiseInventoryViewModelComponent* InventoryVM)
{
	TransferAuthorityInventoryVM = InventoryVM;
}

bool USpellRiseInventoryViewModelComponent::RequestEquipItem(
	const FGuid ItemInstanceId,
	const ESpellRiseUIEquipmentSlot EquipmentSlot)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(EquipRequest, InvalidGuid);
	}

	OnEquipItemRequested.Broadcast(ItemInstanceId, EquipmentSlot);
	if (USpellRiseEquipmentComponent* Equipment = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseEquipmentComponent>() : nullptr)
	{
		Equipment->RequestEquipItem(ItemInstanceId, static_cast<ESpellRiseEquipmentSlot>(EquipmentSlot), AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestUnequipItem(
	const ESpellRiseUIEquipmentSlot EquipmentSlot,
	const int32 PreferredInventorySlot)
{
	using namespace SpellRiseInventoryViewModel;
	if (PreferredInventorySlot < INDEX_NONE
		|| (InventorySnapshot.MaxSlots > 0 && PreferredInventorySlot >= InventorySnapshot.MaxSlots))
	{
		return RejectRequest(UnequipRequest, InvalidSlot);
	}

	OnUnequipItemRequested.Broadcast(EquipmentSlot, PreferredInventorySlot);
	if (USpellRiseEquipmentComponent* Equipment = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseEquipmentComponent>() : nullptr)
	{
		Equipment->RequestUnequipItem(static_cast<ESpellRiseEquipmentSlot>(EquipmentSlot), PreferredInventorySlot, AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestSwapEquipmentSlots(
	const ESpellRiseUIEquipmentSlot FromSlot,
	const ESpellRiseUIEquipmentSlot ToSlot)
{
	using namespace SpellRiseInventoryViewModel;
	if (FromSlot == ToSlot)
	{
		return true;
	}

	if (USpellRiseEquipmentComponent* Equipment = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseEquipmentComponent>() : nullptr)
	{
		Equipment->RequestSwapEquipmentSlots(
			static_cast<ESpellRiseEquipmentSlot>(FromSlot),
			static_cast<ESpellRiseEquipmentSlot>(ToSlot),
			AllocateRequestId());
		return true;
	}
	return RejectRequest(SwapEquipmentRequest, InvalidSlot);
}

bool USpellRiseInventoryViewModelComponent::RequestDropItem(
	const FGuid ItemInstanceId,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(DropRequest, InvalidGuid);
	}
	if (Quantity <= 0)
	{
		return RejectRequest(DropRequest, InvalidQuantity);
	}

	OnDropItemRequested.Broadcast(ItemInstanceId, Quantity);
	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestDropItem(ItemInstanceId, Quantity, AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestUseItem(const FGuid ItemInstanceId)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(UseRequest, InvalidGuid);
	}

	OnUseItemRequested.Broadcast(ItemInstanceId);
	for (const FSpellRiseEquipmentSlotView& EquipmentSlot : EquipmentSnapshot.Slots)
	{
		if (EquipmentSlot.bOccupied && EquipmentSlot.ItemInstanceId == ItemInstanceId)
		{
			if (USpellRiseEquipmentComponent* Equipment = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseEquipmentComponent>() : nullptr)
			{
				Equipment->RequestUnequipItem(static_cast<ESpellRiseEquipmentSlot>(EquipmentSlot.Slot), INDEX_NONE, AllocateRequestId());
			}
			return true;
		}
	}

	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestUseItem(ItemInstanceId, AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestDestroyItem(
	const FGuid ItemInstanceId,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(DestroyRequest, InvalidGuid);
	}
	if (Quantity <= 0)
	{
		return RejectRequest(DestroyRequest, InvalidQuantity);
	}

	OnDestroyItemRequested.Broadcast(ItemInstanceId, Quantity);
	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestDestroyItem(ItemInstanceId, Quantity, AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestDepositToStorage(
	USpellRiseStorageComponent* Storage,
	const FGuid InventoryItemInstanceId,
	const int32 PreferredStorageSlot,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ValidateStorageRequest_Local(DepositStorageRequest, Storage, InventoryItemInstanceId, PreferredStorageSlot, Quantity, true))
	{
		return false;
	}
	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestDepositToStorage(Storage, InventoryItemInstanceId, PreferredStorageSlot, Quantity, AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestWithdrawFromStorage(
	USpellRiseStorageComponent* Storage,
	const FGuid StorageItemInstanceId,
	const int32 PreferredInventorySlot,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ValidateStorageRequest_Local(WithdrawStorageRequest, Storage, StorageItemInstanceId, INDEX_NONE, Quantity, true))
	{
		return false;
	}
	if (PreferredInventorySlot < INDEX_NONE
		|| (PreferredInventorySlot != INDEX_NONE && InventorySnapshot.MaxSlots > 0 && PreferredInventorySlot >= InventorySnapshot.MaxSlots))
	{
		return RejectRequest(WithdrawStorageRequest, InvalidSlot);
	}
	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestWithdrawFromStorage(Storage, StorageItemInstanceId, PreferredInventorySlot, Quantity, AllocateRequestId());
	}
	return true;
}

bool USpellRiseInventoryViewModelComponent::RequestMoveStorageItem(
	USpellRiseStorageComponent* Storage,
	const FGuid StorageItemInstanceId,
	const int32 DestinationSlot,
	const int32 Quantity)
{
	using namespace SpellRiseInventoryViewModel;
	if (!ValidateStorageRequest_Local(MoveStorageRequest, Storage, StorageItemInstanceId, DestinationSlot, Quantity, false))
	{
		return false;
	}
	if (USpellRiseInventoryComponent* Inventory = GetOwnerInventoryComponent())
	{
		Inventory->RequestMoveStorageItem(Storage, StorageItemInstanceId, DestinationSlot, Quantity, AllocateRequestId());
		return true;
	}
	if (TransferAuthorityInventoryVM)
	{
		return TransferAuthorityInventoryVM->RequestMoveStorageItem(Storage, StorageItemInstanceId, DestinationSlot, Quantity);
	}
	return RejectRequest(MoveStorageRequest, InvalidSource);
}

void USpellRiseInventoryViewModelComponent::WatchStorage(USpellRiseStorageComponent* Storage)
{
	if (!Storage || WatchedStorageComponents.Contains(Storage))
	{
		return;
	}
	WatchedStorageComponents.Add(Storage);
	Storage->OnStorageChanged.AddDynamic(this, &ThisClass::HandleStorageChanged);
	BroadcastStorageSnapshot(Storage);
}

void USpellRiseInventoryViewModelComponent::UnwatchStorage(USpellRiseStorageComponent* Storage)
{
	if (!Storage)
	{
		return;
	}
	if (WatchedStorageComponents.Remove(Storage) > 0)
	{
		Storage->OnStorageChanged.RemoveDynamic(this, &ThisClass::HandleStorageChanged);
	}
}

bool USpellRiseInventoryViewModelComponent::RejectRequest(const FName RequestType, const FName Reason)
{
	OnLocalRequestRejected.Broadcast(RequestType, Reason);
	return false;
}

void USpellRiseInventoryViewModelComponent::RefreshFromAuthoritativeComponents()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (const USpellRiseInventoryComponent* Inventory = OwnerActor->FindComponentByClass<USpellRiseInventoryComponent>())
	{
		FSpellRiseInventoryViewSnapshot Snapshot;
		Snapshot.MaxSlots = Inventory->GetMaxSlots();
		Snapshot.CurrentWeight = Inventory->GetCurrentWeight();
		Snapshot.MaxWeight = Inventory->GetMaxWeight();
		for (const FSpellRiseItemInstance& Item : Inventory->GetItems())
		{
			FSpellRiseInventorySlotView& View = Snapshot.Slots.AddDefaulted_GetRef();
			View.ItemInstanceId = Item.ItemInstanceId;
			View.DefinitionId = Item.DefinitionId;
			View.SlotIndex = Item.SlotIndex;
			View.Quantity = Item.Quantity;
			View.Durability = Item.Durability;
			View.Flags = Item.Flags;
			View.bDropLocked = (Item.Flags & static_cast<uint8>(ESpellRiseItemInstanceFlags::NoDrop)) != 0;
			Snapshot.Revision = FMath::Max(Snapshot.Revision, Item.Revision);
		}
		ApplyAuthoritativeInventorySnapshot(Snapshot);
	}
	else if (const USpellRiseStorageComponent* Storage = OwnerActor->FindComponentByClass<USpellRiseStorageComponent>())
	{
		FSpellRiseInventoryViewSnapshot Snapshot;
		Snapshot.MaxSlots = Storage->GetMaxSlots();
		Snapshot.CurrentWeight = Storage->GetCurrentWeight();
		Snapshot.MaxWeight = Storage->GetMaxWeight();
		for (const FSpellRiseItemInstance& Item : Storage->GetItems())
		{
			FSpellRiseInventorySlotView& View = Snapshot.Slots.AddDefaulted_GetRef();
			View.ItemInstanceId = Item.ItemInstanceId;
			View.DefinitionId = Item.DefinitionId;
			View.SlotIndex = Item.SlotIndex;
			View.Quantity = Item.Quantity;
			View.Durability = Item.Durability;
			View.Flags = Item.Flags;
			View.bDropLocked = (Item.Flags & static_cast<uint8>(ESpellRiseItemInstanceFlags::NoDrop)) != 0;
			Snapshot.Revision = FMath::Max(Snapshot.Revision, Item.Revision);
		}
		ApplyAuthoritativeInventorySnapshot(Snapshot);
	}

	if (const USpellRiseEquipmentComponent* Equipment = OwnerActor->FindComponentByClass<USpellRiseEquipmentComponent>())
	{
		FSpellRiseEquipmentViewSnapshot Snapshot;
		Snapshot.EquippedWeight = Equipment->GetEquippedWeight();
		for (const FSpellRiseEquippedItemEntry& Item : Equipment->GetPrivateEquipment().Entries)
		{
			FSpellRiseEquipmentSlotView& View = Snapshot.Slots.AddDefaulted_GetRef();
			View.Slot = static_cast<ESpellRiseUIEquipmentSlot>(Item.Slot);
			View.ItemInstanceId = Item.ItemInstanceId;
			View.DefinitionId = Item.DefinitionId;
			View.Durability = Item.Durability;
			View.bOccupied = true;
			Snapshot.Revision = FMath::Max(Snapshot.Revision, Item.Revision);
		}
		ApplyAuthoritativeEquipmentSnapshot(Snapshot);
	}
}

void USpellRiseInventoryViewModelComponent::BroadcastStorageSnapshot(USpellRiseStorageComponent* Storage)
{
	if (Storage)
	{
		OnStorageSnapshotChanged.Broadcast(Storage, GetStorageSnapshot(Storage));
	}
}

int32 USpellRiseInventoryViewModelComponent::AllocateRequestId()
{
	if (NextClientRequestId <= 0)
	{
		NextClientRequestId = 1;
	}
	return NextClientRequestId++;
}

bool USpellRiseInventoryViewModelComponent::ValidateStorageRequest_Local(
	const FName RequestType,
	USpellRiseStorageComponent* Storage,
	const FGuid& ItemInstanceId,
	const int32 Slot,
	const int32 Quantity,
	const bool bAllowAutoSlot)
{
	using namespace SpellRiseInventoryViewModel;
	if (!Storage)
	{
		return RejectRequest(RequestType, InvalidStorage);
	}
	if (!ItemInstanceId.IsValid())
	{
		return RejectRequest(RequestType, InvalidGuid);
	}
	if (Quantity <= 0)
	{
		return RejectRequest(RequestType, InvalidQuantity);
	}
	if ((!bAllowAutoSlot && Slot < 0) || (Slot < INDEX_NONE))
	{
		return RejectRequest(RequestType, InvalidSlot);
	}
	if (Slot != INDEX_NONE && Storage->GetMaxSlots() > 0 && Slot >= Storage->GetMaxSlots())
	{
		return RejectRequest(RequestType, InvalidSlot);
	}
	return true;
}

const FSpellRiseInventorySlotView* USpellRiseInventoryViewModelComponent::FindSnapshotSlotByIndex(const int32 SlotIndex) const
{
	return InventorySnapshot.Slots.FindByPredicate([SlotIndex](const FSpellRiseInventorySlotView& Slot)
	{
		return Slot.SlotIndex == SlotIndex;
	});
}

int32 USpellRiseInventoryViewModelComponent::CountFreeSlotsInSnapshot() const
{
	return FMath::Max(0, InventorySnapshot.MaxSlots - InventorySnapshot.Slots.Num());
}

USpellRiseInventoryComponent* USpellRiseInventoryViewModelComponent::GetOwnerInventoryComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseInventoryComponent>() : nullptr;
}

USpellRiseStorageComponent* USpellRiseInventoryViewModelComponent::GetOwnerStorageComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseStorageComponent>() : nullptr;
}

void USpellRiseInventoryViewModelComponent::OptimisticallyRemoveFromInventorySnapshot(
	const FGuid& ItemInstanceId,
	const int32 Quantity)
{
	FSpellRiseInventorySlotView* Slot = InventorySnapshot.Slots.FindByPredicate(
		[&ItemInstanceId](const FSpellRiseInventorySlotView& Candidate)
		{
			return Candidate.ItemInstanceId == ItemInstanceId;
		});
	if (!Slot || Quantity <= 0)
	{
		return;
	}

	if (Quantity >= Slot->Quantity)
	{
		const int32 RemovedSlotIndex = Slot->SlotIndex;
		InventorySnapshot.Slots.RemoveAll(
			[&ItemInstanceId](const FSpellRiseInventorySlotView& Candidate)
			{
				return Candidate.ItemInstanceId == ItemInstanceId;
			});
		OnInventorySlotRemoved.Broadcast(RemovedSlotIndex, ItemInstanceId);
	}
	else
	{
		Slot->Quantity -= Quantity;
	}

	OnInventorySnapshotChanged.Broadcast(InventorySnapshot);
}

void USpellRiseInventoryViewModelComponent::HandleInventoryChanged(
	const ESpellRiseInventoryChangeType ChangeType,
	const FSpellRiseItemInstance Item)
{
	if (ChangeType == ESpellRiseInventoryChangeType::Removed)
	{
		OnInventorySlotRemoved.Broadcast(Item.SlotIndex, Item.ItemInstanceId);
	}
	RefreshFromAuthoritativeComponents();
}

void USpellRiseInventoryViewModelComponent::HandleEquipmentChanged()
{
	RefreshFromAuthoritativeComponents();
}

void USpellRiseInventoryViewModelComponent::HandleStorageChanged(
	const ESpellRiseInventoryChangeType ChangeType,
	const FSpellRiseItemInstance Item)
{
	if (ChangeType == ESpellRiseInventoryChangeType::Removed)
	{
		USpellRiseStorageComponent* ChangedStorage = GetOwnerStorageComponent();
		if (!ChangedStorage && WatchedStorageComponents.Num() == 1)
		{
			ChangedStorage = WatchedStorageComponents.Array()[0];
		}
		OnStorageSlotRemoved.Broadcast(ChangedStorage, Item.SlotIndex, Item.ItemInstanceId);
	}

	RefreshFromAuthoritativeComponents();
	for (USpellRiseStorageComponent* Storage : WatchedStorageComponents)
	{
		BroadcastStorageSnapshot(Storage);
	}
}
