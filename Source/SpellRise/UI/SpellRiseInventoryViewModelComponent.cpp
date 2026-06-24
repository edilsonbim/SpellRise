#include "SpellRise/UI/SpellRiseInventoryViewModelComponent.h"

#include "SpellRise/Equipment/SpellRiseEquipmentComponent.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"

namespace SpellRiseInventoryViewModel
{
	const FName MoveRequest(TEXT("move"));
	const FName EquipRequest(TEXT("equip"));
	const FName UnequipRequest(TEXT("unequip"));
	const FName DropRequest(TEXT("drop"));
	const FName InvalidGuid(TEXT("invalid_guid"));
	const FName InvalidSlot(TEXT("invalid_slot"));
	const FName InvalidQuantity(TEXT("invalid_quantity"));
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
	}
	Super::EndPlay(EndPlayReason);
}

void USpellRiseInventoryViewModelComponent::ApplyAuthoritativeInventorySnapshot(
	const FSpellRiseInventoryViewSnapshot& Snapshot)
{
	if (Snapshot.Revision < InventorySnapshot.Revision)
	{
		return;
	}

	InventorySnapshot = Snapshot;
	OnInventorySnapshotChanged.Broadcast(InventorySnapshot);
}

void USpellRiseInventoryViewModelComponent::ApplyAuthoritativeEquipmentSnapshot(
	const FSpellRiseEquipmentViewSnapshot& Snapshot)
{
	if (Snapshot.Revision < EquipmentSnapshot.Revision)
	{
		return;
	}

	EquipmentSnapshot = Snapshot;
	OnEquipmentSnapshotChanged.Broadcast(EquipmentSnapshot);
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
	if (USpellRiseInventoryComponent* Inventory = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseInventoryComponent>() : nullptr)
	{
		Inventory->RequestMoveItem(ItemInstanceId, DestinationSlot, Quantity, AllocateRequestId());
	}
	return true;
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
	if (USpellRiseInventoryComponent* Inventory = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseInventoryComponent>() : nullptr)
	{
		Inventory->RequestDropItem(ItemInstanceId, Quantity, AllocateRequestId());
	}
	return true;
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

int32 USpellRiseInventoryViewModelComponent::AllocateRequestId()
{
	if (NextClientRequestId <= 0)
	{
		NextClientRequestId = 1;
	}
	return NextClientRequestId++;
}

void USpellRiseInventoryViewModelComponent::HandleInventoryChanged(
	ESpellRiseInventoryChangeType,
	FSpellRiseItemInstance)
{
	RefreshFromAuthoritativeComponents();
}

void USpellRiseInventoryViewModelComponent::HandleEquipmentChanged()
{
	RefreshFromAuthoritativeComponents();
}
