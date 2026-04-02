#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "EquippableItem.h"
#include "EquipmentComponent.h"
#include "SpellRise/Equipment/SpellRiseEquipmentInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentReplication, Log, All);

void FSpellRiseEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	if (!OwnerComponent)
	{
		return;
	}

	for (const int32 Index : RemovedIndices)
	{
		if (!Entries.IsValidIndex(Index))
		{
			continue;
		}

		OwnerComponent->HandleEntryRemoved(Entries[Index]);
	}
}

void FSpellRiseEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	if (!OwnerComponent)
	{
		return;
	}

	for (const int32 Index : AddedIndices)
	{
		if (!Entries.IsValidIndex(Index))
		{
			continue;
		}

		OwnerComponent->HandleEntryAdded(Entries[Index]);
	}
}

void FSpellRiseEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	if (!OwnerComponent)
	{
		return;
	}

	for (const int32 Index : ChangedIndices)
	{
		if (!Entries.IsValidIndex(Index))
		{
			continue;
		}

		OwnerComponent->HandleEntryAdded(Entries[Index]);
	}
}

FSpellRiseAppliedEquipmentEntry* FSpellRiseEquipmentList::FindBySlot(uint8 SlotValue)
{
	for (FSpellRiseAppliedEquipmentEntry& Entry : Entries)
	{
		if (Entry.SlotValue == SlotValue)
		{
			return &Entry;
		}
	}

	return nullptr;
}

const FSpellRiseAppliedEquipmentEntry* FSpellRiseEquipmentList::FindBySlot(uint8 SlotValue) const
{
	for (const FSpellRiseAppliedEquipmentEntry& Entry : Entries)
	{
		if (Entry.SlotValue == SlotValue)
		{
			return &Entry;
		}
	}

	return nullptr;
}

USpellRiseEquipmentManagerComponent::USpellRiseEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
}

void USpellRiseEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	EquipmentList.OwnerComponent = this;
}

void USpellRiseEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool USpellRiseEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FSpellRiseAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (IsValid(Entry.Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Entry.Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void USpellRiseEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (!IsUsingRegisteredSubObjectList())
	{
		return;
	}

	for (const FSpellRiseAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (IsValid(Entry.Instance))
		{
			AddReplicatedSubObject(Entry.Instance);
		}
	}
}

bool USpellRiseEquipmentManagerComponent::EquipItem(UEquippableItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		UE_LOG(LogSpellRiseEquipmentReplication, Warning, TEXT("[Equipment][EquipRejected] Owner=%s Item=%s Reason=%s"), *GetNameSafe(GetOwner()), *GetNameSafe(Item), *ValidationReason);
		return false;
	}

	const uint8 SlotValue = ResolveItemSlot(Item);
	if (SlotValue == 255)
	{
		UE_LOG(LogSpellRiseEquipmentReplication, Warning, TEXT("[Equipment][EquipRejected] Owner=%s Item=%s Reason=invalid_slot"), *GetNameSafe(GetOwner()), *GetNameSafe(Item));
		return false;
	}

	RemoveEntryBySlot(SlotValue);
	FSpellRiseAppliedEquipmentEntry* AddedEntry = AddEntry(Item, SlotValue);
	if (!AddedEntry)
	{
		return false;
	}

	UE_LOG(LogSpellRiseEquipmentReplication, Log, TEXT("[Equipment][EquipCommittedServer] Owner=%s Item=%s Slot=%d"), *GetNameSafe(GetOwner()), *GetNameSafe(Item), static_cast<int32>(SlotValue));
	return true;
}

bool USpellRiseEquipmentManagerComponent::UnequipItem(UEquippableItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		UE_LOG(LogSpellRiseEquipmentReplication, Warning, TEXT("[Equipment][UnequipRejected] Owner=%s Item=%s Reason=%s"), *GetNameSafe(GetOwner()), *GetNameSafe(Item), *ValidationReason);
		return false;
	}

	const uint8 SlotValue = ResolveItemSlot(Item);
	if (!RemoveEntryBySlot(SlotValue))
	{
		return false;
	}

	UE_LOG(LogSpellRiseEquipmentReplication, Log, TEXT("[Equipment][UnequipCommittedServer] Owner=%s Item=%s Slot=%d"), *GetNameSafe(GetOwner()), *GetNameSafe(Item), static_cast<int32>(SlotValue));
	return true;
}

USpellRiseEquipmentInstance* USpellRiseEquipmentManagerComponent::GetEquippedInstanceBySlot(uint8 SlotValue) const
{
	const FSpellRiseAppliedEquipmentEntry* Entry = EquipmentList.FindBySlot(SlotValue);
	return Entry ? Entry->Instance : nullptr;
}

void USpellRiseEquipmentManagerComponent::ApplyReplicatedEquipmentVisual(USpellRiseEquipmentInstance& EquipmentInstance, bool bEquipped)
{
	ApplyVisualForItem(EquipmentInstance.GetSourceItem(), bEquipped);
}

void USpellRiseEquipmentManagerComponent::HandleEntryAdded(const FSpellRiseAppliedEquipmentEntry& Entry)
{
	if (Entry.Instance)
	{
		UE_LOG(LogSpellRiseEquipmentReplication, Verbose, TEXT("[Equipment][OnRep_EquipmentApplied] Owner=%s Item=%s Slot=%d"), *GetNameSafe(GetOwner()), *GetNameSafe(Entry.SourceItem), static_cast<int32>(Entry.SlotValue));
		Entry.Instance->OnEquipped();
	}
	else
	{
		ApplyVisualForItem(Entry.SourceItem, true);
	}
}

void USpellRiseEquipmentManagerComponent::HandleEntryRemoved(const FSpellRiseAppliedEquipmentEntry& Entry)
{
	if (Entry.Instance)
	{
		UE_LOG(LogSpellRiseEquipmentReplication, Verbose, TEXT("[Equipment][OnRep_EquipmentRemoved] Owner=%s Item=%s Slot=%d"), *GetNameSafe(GetOwner()), *GetNameSafe(Entry.SourceItem), static_cast<int32>(Entry.SlotValue));
		Entry.Instance->OnUnequipped();
	}
	else
	{
		ApplyVisualForItem(Entry.SourceItem, false);
	}
}

bool USpellRiseEquipmentManagerComponent::ValidateItemOwnership(UEquippableItem* Item, FString& OutReason) const
{
	OutReason.Reset();

	if (!Item)
	{
		OutReason = TEXT("null_item");
		return false;
	}

	if (!GetOwner())
	{
		OutReason = TEXT("missing_owner");
		return false;
	}

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (Item->GetOwningPawn() != Pawn)
		{
			OutReason = TEXT("item_not_owned_by_pawn");
			return false;
		}
	}

	return true;
}

uint8 USpellRiseEquipmentManagerComponent::ResolveItemSlot(UEquippableItem* Item) const
{
	if (!Item)
	{
		return 255;
	}

	FProperty* SlotProperty = Item->GetClass()->FindPropertyByName(TEXT("EquippableSlot"));
	if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(SlotProperty))
	{
		const void* ValuePtr = EnumProperty->ContainerPtrToValuePtr<void>(Item);
		if (const FNumericProperty* NumericProperty = EnumProperty->GetUnderlyingProperty())
		{
			return static_cast<uint8>(NumericProperty->GetUnsignedIntPropertyValue(ValuePtr));
		}
	}
	else if (FByteProperty* ByteProperty = CastField<FByteProperty>(SlotProperty))
	{
		const void* ValuePtr = ByteProperty->ContainerPtrToValuePtr<void>(Item);
		return static_cast<uint8>(ByteProperty->GetUnsignedIntPropertyValue(ValuePtr));
	}

	return 255;
}

UEquipmentComponent* USpellRiseEquipmentManagerComponent::ResolveEquipmentComponent() const
{
	AActor* OwnerActor = GetOwner();
	return OwnerActor ? OwnerActor->FindComponentByClass<UEquipmentComponent>() : nullptr;
}

void USpellRiseEquipmentManagerComponent::ApplyVisualForItem(UEquippableItem* Item, bool bEquip)
{
	if (!Item)
	{
		return;
	}

	if (UEquipmentComponent* EquipmentComponent = ResolveEquipmentComponent())
	{
		EquipmentComponent->ApplyEquippableVisual(Item, bEquip);
	}
}

FSpellRiseAppliedEquipmentEntry* USpellRiseEquipmentManagerComponent::AddEntry(UEquippableItem* Item, uint8 SlotValue)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !Item)
	{
		return nullptr;
	}

	FSpellRiseAppliedEquipmentEntry& NewEntry = EquipmentList.Entries.AddDefaulted_GetRef();
	NewEntry.SourceItem = Item;
	NewEntry.SlotValue = SlotValue;
	NewEntry.ItemClass = Item->GetClass();
	NewEntry.Instance = NewObject<USpellRiseEquipmentInstance>(OwnerActor);
	if (NewEntry.Instance)
	{
		NewEntry.Instance->Initialize(this, Item);
		NewEntry.Instance->OnEquipped();
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(NewEntry.Instance);
		}
	}

	EquipmentList.MarkItemDirty(NewEntry);
	if (AActor* NetOwnerActor = GetOwner())
	{
		NetOwnerActor->ForceNetUpdate();
	}
	return &NewEntry;
}

bool USpellRiseEquipmentManagerComponent::RemoveEntryBySlot(uint8 SlotValue)
{
	for (int32 Index = 0; Index < EquipmentList.Entries.Num(); ++Index)
	{
		FSpellRiseAppliedEquipmentEntry& Entry = EquipmentList.Entries[Index];
		if (Entry.SlotValue != SlotValue)
		{
			continue;
		}

		if (Entry.Instance)
		{
			Entry.Instance->OnUnequipped();
			if (IsUsingRegisteredSubObjectList())
			{
				RemoveReplicatedSubObject(Entry.Instance);
			}
		}
		else
		{
			ApplyVisualForItem(Entry.SourceItem, false);
		}

		EquipmentList.Entries.RemoveAt(Index);
		EquipmentList.MarkArrayDirty();
		if (AActor* OwnerActor = GetOwner())
		{
			OwnerActor->ForceNetUpdate();
		}
		return true;
	}

	return false;
}
