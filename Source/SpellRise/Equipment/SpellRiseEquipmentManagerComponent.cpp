#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "EquippableItem.h"
#include "EquipmentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "NarrativeItem.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
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
	QuickWeaponSlots.SetNum(2);
}

void USpellRiseEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	EquipmentList.OwnerComponent = this;
	BindInventoryRemovalHook();
}

void USpellRiseEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquipmentList);
	DOREPLIFETIME(ThisClass, QuickWeaponSlots);
	DOREPLIFETIME(ThisClass, ActiveQuickWeaponSlotIndex);
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

bool USpellRiseEquipmentManagerComponent::RequestEquipItem(UEquippableItem* Item)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	if (IsWeaponItem(Item))
	{
		if (OwnerActor->HasAuthority())
		{
			HandleWeaponEquipIntent(Item);
			return true;
		}

		ServerRequestEquipItem(Item);
		return true;
	}

	if (OwnerActor->HasAuthority())
	{
		return EquipItem(Item);
	}

	ServerRequestEquipItem(Item);
	return true;
}

bool USpellRiseEquipmentManagerComponent::RequestUnequipItem(UEquippableItem* Item)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	if (IsWeaponItem(Item))
	{
		if (OwnerActor->HasAuthority())
		{
			const int32 SlotIndex = FindQuickSlotByItem(Item);
			if (SlotIndex != INDEX_NONE)
			{
				RemoveQuickWeaponSlot_Server(SlotIndex, false);
				return true;
			}

			return false;
		}

		ServerRequestUnequipItem(Item);
		return true;
	}

	if (OwnerActor->HasAuthority())
	{
		return UnequipItem(Item);
	}

	ServerRequestUnequipItem(Item);
	return true;
}

void USpellRiseEquipmentManagerComponent::ServerRequestEquipItem_Implementation(UEquippableItem* Item)
{
	if (IsWeaponItem(Item))
	{
		HandleWeaponEquipIntent(Item);
		return;
	}

	EquipItem(Item);
}

void USpellRiseEquipmentManagerComponent::ServerRequestUnequipItem_Implementation(UEquippableItem* Item)
{
	if (IsWeaponItem(Item))
	{
		const int32 SlotIndex = FindQuickSlotByItem(Item);
		if (SlotIndex != INDEX_NONE)
		{
			RemoveQuickWeaponSlot_Server(SlotIndex, false);
		}
		return;
	}

	UnequipItem(Item);
}

bool USpellRiseEquipmentManagerComponent::RequestActivateQuickWeaponSlot(int32 QuickSlotIndex)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	if (OwnerActor->HasAuthority())
	{
		return ActivateQuickWeaponSlot_Server(QuickSlotIndex);
	}

	ServerRequestActivateQuickWeaponSlot(QuickSlotIndex);
	return true;
}

bool USpellRiseEquipmentManagerComponent::RequestAssignQuickWeaponSlot(UEquippableItem* Item, int32 QuickSlotIndex)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	if (OwnerActor->HasAuthority())
	{
		return AssignQuickWeaponSlot_Server(Item, QuickSlotIndex);
	}

	ServerRequestAssignQuickWeaponSlot(Item, QuickSlotIndex);
	return true;
}

void USpellRiseEquipmentManagerComponent::ServerRequestActivateQuickWeaponSlot_Implementation(int32 QuickSlotIndex)
{
	ActivateQuickWeaponSlot_Server(QuickSlotIndex);
}

void USpellRiseEquipmentManagerComponent::ServerRequestAssignQuickWeaponSlot_Implementation(UEquippableItem* Item, int32 QuickSlotIndex)
{
	AssignQuickWeaponSlot_Server(Item, QuickSlotIndex);
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

	SnapItemWeaponToSocket(Item, bEquip);
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

	ApplyGrantedAbilitiesForSlot(Item, SlotValue);

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

		RemoveGrantedAbilitiesForSlot(SlotValue);

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

bool USpellRiseEquipmentManagerComponent::ExtractAbilitiesToGrantFromItem(UEquippableItem* Item, TArray<FSpellRiseGrantedAbility>& OutAbilities) const
{
	OutAbilities.Reset();

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponConfigPtr || !WeaponConfigStruct)
	{
		return false;
	}

	FArrayProperty* AbilitiesArrayProperty = nullptr;
	for (TFieldIterator<FProperty> It(WeaponConfigStruct); It; ++It)
	{
		FArrayProperty* CandidateArray = CastField<FArrayProperty>(*It);
		if (!CandidateArray)
		{
			continue;
		}

		if (!CandidateArray->GetName().Contains(TEXT("AbilitiesToGrant")))
		{
			continue;
		}

		const FStructProperty* InnerStruct = CastField<FStructProperty>(CandidateArray->Inner);
		if (!InnerStruct || InnerStruct->Struct != FSpellRiseGrantedAbility::StaticStruct())
		{
			continue;
		}

		AbilitiesArrayProperty = CandidateArray;
		break;
	}

	if (!AbilitiesArrayProperty)
	{
		return false;
	}

	FScriptArrayHelper ArrayHelper(AbilitiesArrayProperty, AbilitiesArrayProperty->ContainerPtrToValuePtr<void>(WeaponConfigPtr));
	OutAbilities.Reserve(ArrayHelper.Num());
	for (int32 Index = 0; Index < ArrayHelper.Num(); ++Index)
	{
		const FSpellRiseGrantedAbility* AbilityEntry = reinterpret_cast<const FSpellRiseGrantedAbility*>(ArrayHelper.GetRawPtr(Index));
		if (AbilityEntry && AbilityEntry->Ability)
		{
			OutAbilities.Add(*AbilityEntry);
		}
	}

	return OutAbilities.Num() > 0;
}

bool USpellRiseEquipmentManagerComponent::ResolveWeaponActorClassFromItem(UEquippableItem* Item, UClass*& OutWeaponActorClass, const void*& OutWeaponConfigPtr, const UStruct*& OutWeaponConfigStruct) const
{
	OutWeaponActorClass = nullptr;
	OutWeaponConfigPtr = nullptr;
	OutWeaponConfigStruct = nullptr;

	if (!Item)
	{
		return false;
	}

	const FProperty* WeaponClassProperty = Item->GetClass()->FindPropertyByName(TEXT("WeaponClass"));
	const FClassProperty* ClassProperty = CastField<FClassProperty>(WeaponClassProperty);
	if (!ClassProperty)
	{
		return false;
	}

	const void* ClassPtr = ClassProperty->ContainerPtrToValuePtr<void>(Item);
	UObject* WeaponActorClassObject = ClassProperty->GetObjectPropertyValue(ClassPtr);
	UClass* WeaponActorClass = Cast<UClass>(WeaponActorClassObject);
	if (!WeaponActorClass)
	{
		return false;
	}

	const UObject* WeaponCDO = WeaponActorClass->GetDefaultObject();
	if (!WeaponCDO)
	{
		return false;
	}

	const FStructProperty* WeaponConfigProperty = CastField<FStructProperty>(WeaponActorClass->FindPropertyByName(TEXT("WeaponConfig")));
	if (!WeaponConfigProperty)
	{
		return false;
	}

	const void* WeaponConfigPtr = WeaponConfigProperty->ContainerPtrToValuePtr<void>(WeaponCDO);
	if (!WeaponConfigPtr)
	{
		return false;
	}

	OutWeaponActorClass = WeaponActorClass;
	OutWeaponConfigPtr = WeaponConfigPtr;
	OutWeaponConfigStruct = WeaponConfigProperty->Struct;
	return true;
}

bool USpellRiseEquipmentManagerComponent::ResolveWeaponSocketsFromConfig(const void* WeaponConfigPtr, const UStruct* WeaponConfigStruct, FName& OutEquippedSocket, FName& OutStowedSocket) const
{
	OutEquippedSocket = NAME_None;
	OutStowedSocket = NAME_None;

	if (!WeaponConfigPtr || !WeaponConfigStruct)
	{
		return false;
	}

	for (TFieldIterator<FProperty> It(WeaponConfigStruct); It; ++It)
	{
		const FString PropertyName = It->GetName();
		const FNameProperty* NameProperty = CastField<FNameProperty>(*It);
		if (!NameProperty)
		{
			continue;
		}

		// UserDefinedStruct fields can include generated suffixes in internal names.
		if (PropertyName.Contains(TEXT("EquippedSocketName")))
		{
			OutEquippedSocket = NameProperty->GetPropertyValue_InContainer(WeaponConfigPtr);
		}
		else if (PropertyName.Contains(TEXT("StowedSocketName")))
		{
			OutStowedSocket = NameProperty->GetPropertyValue_InContainer(WeaponConfigPtr);
		}
	}

	// Conservative fallback for legacy assets when config doesn't provide explicit sockets.
	if (OutEquippedSocket == NAME_None)
	{
		OutEquippedSocket = TEXT("hand_r");
	}
	if (OutStowedSocket == NAME_None)
	{
		OutStowedSocket = TEXT("stowed");
	}

	return OutEquippedSocket != NAME_None || OutStowedSocket != NAME_None;
}

AActor* USpellRiseEquipmentManagerComponent::ResolveOwnedWeaponActor(UClass* WeaponActorClass) const
{
	AActor* OwnerActor = GetOwner();
	UWorld* World = GetWorld();
	if (!OwnerActor || !World || !WeaponActorClass)
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(World, WeaponActorClass); It; ++It)
	{
		AActor* Candidate = *It;
		if (!Candidate)
		{
			continue;
		}

		if (Candidate->GetOwner() == OwnerActor || Candidate->GetInstigator() == Cast<APawn>(OwnerActor))
		{
			return Candidate;
		}
	}

	return nullptr;
}

USkeletalMeshComponent* USpellRiseEquipmentManagerComponent::ResolveEquipmentAttachMesh() const
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	if (const ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(OwnerActor))
	{
		if (USkeletalMeshComponent* EquipmentMesh = CharacterOwner->GetEquipmentAttachMeshComponent())
		{
			return EquipmentMesh;
		}
	}

	return OwnerActor->FindComponentByClass<USkeletalMeshComponent>();
}

void USpellRiseEquipmentManagerComponent::SnapItemWeaponToSocket(UEquippableItem* Item, bool bEquip)
{
	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct))
	{
		return;
	}

	FName EquippedSocket = NAME_None;
	FName StowedSocket = NAME_None;
	ResolveWeaponSocketsFromConfig(WeaponConfigPtr, WeaponConfigStruct, EquippedSocket, StowedSocket);

	const FName TargetSocket = bEquip ? EquippedSocket : StowedSocket;
	if (TargetSocket == NAME_None)
	{
		return;
	}

	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh();
	AActor* WeaponActor = ResolveOwnedWeaponActor(WeaponActorClass);
	if (!AttachMesh || !WeaponActor)
	{
		return;
	}

	WeaponActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetSocket);
}

void USpellRiseEquipmentManagerComponent::ApplyGrantedAbilitiesForSlot(UEquippableItem* Item, uint8 SlotValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	if (!ExtractAbilitiesToGrantFromItem(Item, AbilitiesToGrant))
	{
		return;
	}

	RemoveGrantedAbilitiesForSlot(SlotValue);
	const TArray<FGameplayAbilitySpecHandle> Handles = CharacterOwner->GrantAbilities(AbilitiesToGrant);
	if (Handles.Num() > 0)
	{
		GrantedAbilityHandlesBySlot.Add(SlotValue, Handles);
	}
}

void USpellRiseEquipmentManagerComponent::RemoveGrantedAbilitiesForSlot(uint8 SlotValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> HandlesToRemove;
	if (!GrantedAbilityHandlesBySlot.RemoveAndCopyValue(SlotValue, HandlesToRemove))
	{
		return;
	}

	if (HandlesToRemove.Num() > 0)
	{
		CharacterOwner->RemoveAbilities(HandlesToRemove);
	}
}

bool USpellRiseEquipmentManagerComponent::IsWeaponItem(UEquippableItem* Item) const
{
	if (!Item)
	{
		return false;
	}

	const FProperty* WeaponClassProperty = Item->GetClass()->FindPropertyByName(TEXT("WeaponClass"));
	return CastField<FClassProperty>(WeaponClassProperty) != nullptr;
}

int32 USpellRiseEquipmentManagerComponent::FindQuickSlotByItem(UEquippableItem* Item) const
{
	for (int32 Index = 0; Index < QuickWeaponSlots.Num(); ++Index)
	{
		if (QuickWeaponSlots[Index] == Item)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 USpellRiseEquipmentManagerComponent::FindFirstFreeQuickSlot() const
{
	for (int32 Index = 0; Index < QuickWeaponSlots.Num(); ++Index)
	{
		if (!QuickWeaponSlots[Index])
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

void USpellRiseEquipmentManagerComponent::HandleWeaponEquipIntent(UEquippableItem* Item)
{
	if (!Item || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	const int32 ExistingSlot = FindQuickSlotByItem(Item);
	if (ExistingSlot != INDEX_NONE)
	{
		ActivateQuickWeaponSlot_Server(ExistingSlot);
		return;
	}

	int32 SlotToUse = FindFirstFreeQuickSlot();
	if (SlotToUse == INDEX_NONE)
	{
		SlotToUse = (ActiveQuickWeaponSlotIndex == 0) ? 1 : 0;
	}

	AssignQuickWeaponSlot_Server(Item, SlotToUse);
}

bool USpellRiseEquipmentManagerComponent::AssignQuickWeaponSlot_Server(UEquippableItem* Item, int32 QuickSlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item || !IsWeaponItem(Item) || !QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		return false;
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		return false;
	}

	const int32 ExistingSlot = FindQuickSlotByItem(Item);
	if (ExistingSlot != INDEX_NONE && ExistingSlot != QuickSlotIndex)
	{
		if (UEquippableItem* ExistingItem = QuickWeaponSlots[ExistingSlot])
		{
			SetNarrativeItemActiveState(ExistingItem, false);
		}
		QuickWeaponSlots[ExistingSlot] = nullptr;
		RemoveGrantedAbilitiesForSlot(static_cast<uint8>(200 + ExistingSlot));
	}

	if (QuickWeaponSlots[QuickSlotIndex] && QuickWeaponSlots[QuickSlotIndex] != Item)
	{
		RemoveQuickWeaponSlot_Server(QuickSlotIndex, true);
	}

	QuickWeaponSlots[QuickSlotIndex] = Item;
	SetNarrativeItemActiveState(Item, true);
	GetOrSpawnWeaponActorForItem(Item);

	if (ActiveQuickWeaponSlotIndex == INDEX_NONE)
	{
		ActiveQuickWeaponSlotIndex = QuickSlotIndex;
		RefreshQuickSlotVisual_Server(QuickSlotIndex, true);
		ApplyGrantedAbilitiesForSlot(Item, static_cast<uint8>(200 + QuickSlotIndex));
	}
	else
	{
		RefreshQuickSlotVisual_Server(QuickSlotIndex, false);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();

	return true;
}

bool USpellRiseEquipmentManagerComponent::ActivateQuickWeaponSlot_Server(int32 QuickSlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		return false;
	}

	UEquippableItem* ItemToActivate = QuickWeaponSlots[QuickSlotIndex];
	if (!ItemToActivate)
	{
		return false;
	}

	if (ActiveQuickWeaponSlotIndex == QuickSlotIndex)
	{
		return true;
	}

	if (QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex))
	{
		if (UEquippableItem* PreviouslyActive = QuickWeaponSlots[ActiveQuickWeaponSlotIndex])
		{
			RefreshQuickSlotVisual_Server(ActiveQuickWeaponSlotIndex, false);
			RemoveGrantedAbilitiesForSlot(static_cast<uint8>(200 + ActiveQuickWeaponSlotIndex));
		}
	}

	ActiveQuickWeaponSlotIndex = QuickSlotIndex;
	RefreshQuickSlotVisual_Server(QuickSlotIndex, true);
	ApplyGrantedAbilitiesForSlot(ItemToActivate, static_cast<uint8>(200 + QuickSlotIndex));

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();

	return true;
}

void USpellRiseEquipmentManagerComponent::RemoveQuickWeaponSlot_Server(int32 QuickSlotIndex, bool bDestroyWeaponActor)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		return;
	}

	UEquippableItem* RemovedItem = QuickWeaponSlots[QuickSlotIndex];
	QuickWeaponSlots[QuickSlotIndex] = nullptr;
	SetNarrativeItemActiveState(RemovedItem, false);
	RemoveGrantedAbilitiesForSlot(static_cast<uint8>(200 + QuickSlotIndex));

	if (bDestroyWeaponActor && RemovedItem)
	{
		DestroyWeaponActorForItem(RemovedItem);
	}

	if (ActiveQuickWeaponSlotIndex == QuickSlotIndex)
	{
		ActiveQuickWeaponSlotIndex = INDEX_NONE;
		int32 FallbackSlot = INDEX_NONE;
		for (int32 Index = 0; Index < QuickWeaponSlots.Num(); ++Index)
		{
			if (QuickWeaponSlots[Index])
			{
				FallbackSlot = Index;
				break;
			}
		}

		if (FallbackSlot != INDEX_NONE)
		{
			ActivateQuickWeaponSlot_Server(FallbackSlot);
		}
	}

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	CleanupOrphanedWeaponActors_Server();
	OnQuickWeaponSlotsChanged.Broadcast();
}

AActor* USpellRiseEquipmentManagerComponent::GetOrSpawnWeaponActorForItem(UEquippableItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item)
	{
		return nullptr;
	}

	if (TObjectPtr<AActor>* ExistingActor = SpawnedWeaponActorsByItem.Find(Item))
	{
		if (IsValid(*ExistingActor))
		{
			return *ExistingActor;
		}
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponActorClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedWeapon = World->SpawnActor<AActor>(WeaponActorClass, FTransform::Identity, SpawnParams);
	if (!SpawnedWeapon)
	{
		return nullptr;
	}

	SpawnedWeaponActorsByItem.Add(Item, SpawnedWeapon);
	return SpawnedWeapon;
}

void USpellRiseEquipmentManagerComponent::DestroyWeaponActorForItem(UEquippableItem* Item)
{
	if (!Item)
	{
		return;
	}

	TObjectPtr<AActor> SpawnedActor = nullptr;
	if (!SpawnedWeaponActorsByItem.RemoveAndCopyValue(Item, SpawnedActor))
	{
		return;
	}

	if (IsValid(SpawnedActor))
	{
		SpawnedActor->Destroy();
	}
}

void USpellRiseEquipmentManagerComponent::RefreshQuickSlotVisual_Server(int32 QuickSlotIndex, bool bEquipped)
{
	if (!QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		return;
	}

	UEquippableItem* Item = QuickWeaponSlots[QuickSlotIndex];
	if (!Item)
	{
		return;
	}

	AActor* WeaponActor = GetOrSpawnWeaponActorForItem(Item);
	if (!WeaponActor)
	{
		return;
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct))
	{
		return;
	}

	FName EquippedSocket = NAME_None;
	FName StowedSocket = NAME_None;
	ResolveWeaponSocketsFromConfig(WeaponConfigPtr, WeaponConfigStruct, EquippedSocket, StowedSocket);
	const FName TargetSocket = bEquipped ? EquippedSocket : StowedSocket;
	if (TargetSocket == NAME_None)
	{
		return;
	}

	if (USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh())
	{
		WeaponActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetSocket);
	}
}

void USpellRiseEquipmentManagerComponent::BindInventoryRemovalHook()
{
	if (CachedInventoryComponent)
	{
		return;
	}

	if (AActor* OwnerActor = GetOwner())
	{
		CachedInventoryComponent = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(OwnerActor);
		if (CachedInventoryComponent)
		{
			CachedInventoryComponent->OnItemRemoved.AddDynamic(this, &USpellRiseEquipmentManagerComponent::HandleInventoryItemRemoved);
		}
	}
}

void USpellRiseEquipmentManagerComponent::HandleInventoryItemRemoved(UNarrativeItem* RemovedItem, int32 Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	UEquippableItem* RemovedEquippableItem = Cast<UEquippableItem>(RemovedItem);
	if (!RemovedEquippableItem)
	{
		return;
	}

	const int32 SlotIndex = FindQuickSlotByItem(RemovedEquippableItem);
	if (SlotIndex != INDEX_NONE)
	{
		RemoveQuickWeaponSlot_Server(SlotIndex, true);
		return;
	}

	// Fallback: if references diverge after inventory mutations, purge any quick-slot item
	// that no longer validates as owned by this actor.
	for (int32 Index = 0; Index < QuickWeaponSlots.Num(); ++Index)
	{
		UEquippableItem* SlotItem = QuickWeaponSlots[Index];
		if (!SlotItem)
		{
			continue;
		}

		FString ValidationReason;
		if (!ValidateItemOwnership(SlotItem, ValidationReason))
		{
			RemoveQuickWeaponSlot_Server(Index, true);
		}
	}

	CleanupOrphanedWeaponActors_Server();
}

void USpellRiseEquipmentManagerComponent::OnRep_QuickWeaponSlots()
{
	OnQuickWeaponSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::OnRep_ActiveQuickSlotIndex()
{
	OnQuickWeaponSlotsChanged.Broadcast();
}

UEquippableItem* USpellRiseEquipmentManagerComponent::GetQuickWeaponItemByIndex(int32 QuickSlotIndex) const
{
	return QuickWeaponSlots.IsValidIndex(QuickSlotIndex) ? QuickWeaponSlots[QuickSlotIndex] : nullptr;
}

void USpellRiseEquipmentManagerComponent::SetNarrativeItemActiveState(UEquippableItem* Item, bool bShouldBeActive)
{
	if (!Item)
	{
		return;
	}

	Item->SetActive(bShouldBeActive);
}

void USpellRiseEquipmentManagerComponent::CleanupOrphanedWeaponActors_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (auto It = SpawnedWeaponActorsByItem.CreateIterator(); It; ++It)
	{
		UEquippableItem* MappedItem = It.Key().Get();
		AActor* MappedActor = It.Value().Get();

		bool bItemStillInQuickSlots = false;
		for (UEquippableItem* SlotItem : QuickWeaponSlots)
		{
			if (SlotItem == MappedItem)
			{
				bItemStillInQuickSlots = true;
				break;
			}
		}

		if (!MappedItem || !bItemStillInQuickSlots)
		{
			if (IsValid(MappedActor))
			{
				MappedActor->Destroy();
			}
			It.RemoveCurrent();
		}
	}
}
