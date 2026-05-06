// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "UObject/SoftObjectPath.h"
#include "EquippableItem.h"
#include "EquipmentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "NarrativeItem.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Equipment/SpellRiseEquipmentInstance.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentReplication, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseWeaponAttach, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentTrace, Log, All);

namespace SpellRiseEquipmentAttach
{
	static USceneComponent* ResolveWeaponAttachSceneComponent(AActor* WeaponActor)
	{
		if (!IsValid(WeaponActor))
		{
			return nullptr;
		}

		static const FName WeaponComponentName(TEXT("Weapon"));
		TArray<USceneComponent*> SceneComponents;
		WeaponActor->GetComponents<USceneComponent>(SceneComponents);
		for (USceneComponent* Comp : SceneComponents)
		{
			if (IsValid(Comp) && Comp->GetFName() == WeaponComponentName)
			{
				return Comp;
			}
		}

		return WeaponActor->GetRootComponent();
	}

	static UPrimitiveComponent* ResolveWeaponAttachPrimitiveComponent(AActor* WeaponActor)
	{
		return Cast<UPrimitiveComponent>(ResolveWeaponAttachSceneComponent(WeaponActor));
	}

	static USceneComponent* FindSceneComponentByName(AActor* Actor, FName ComponentName)
	{
		if (!IsValid(Actor) || ComponentName.IsNone())
		{
			return nullptr;
		}

		TArray<USceneComponent*> SceneComponents;
		Actor->GetComponents<USceneComponent>(SceneComponents);
		for (USceneComponent* SceneComp : SceneComponents)
		{
			if (IsValid(SceneComp) && SceneComp->GetFName() == ComponentName)
			{
				return SceneComp;
			}
		}

		return nullptr;
	}
}

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
	DropPickupActorClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/NarrativeInventory/Blueprints/BP_BasicItemPickup.BP_BasicItemPickup_C")));
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
	DOREPLIFETIME(ThisClass, ActiveOffHandItem);
	DOREPLIFETIME(ThisClass, bOffHandSuppressedByTwoHandedWeapon);
	DOREPLIFETIME(ThisClass, EquippedWeapon);
	DOREPLIFETIME(ThisClass, EquippedOffHandWeapon);
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
	if (VisualSyncDepth > 0)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
			TEXT("RequestEquipItem ignorado por VisualSyncDepth. Owner=%s Item=%s Depth=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			VisualSyncDepth);
		return true;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning, TEXT("RequestEquipItem abortado: owner nulo. Item=%s"), *GetNameSafe(Item));
		return false;
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("RequestEquipItem. Owner=%s Item=%s IsWeapon=%s HasAuthority=%s ActiveQuickSlot=%d"),
		*GetNameSafe(OwnerActor),
		*GetNameSafe(Item),
		IsWeaponItem(Item) ? TEXT("true") : TEXT("false"),
		OwnerActor->HasAuthority() ? TEXT("true") : TEXT("false"),
		ActiveQuickWeaponSlotIndex);

	if (IsWeaponItem(Item))
	{
		if (IsOffHandWeaponItem(Item))
		{
			if (OwnerActor->HasAuthority())
			{
				return HandleOffHandEquipIntent(Item);
			}

			ServerRequestEquipItem(Item);
			return true;
		}

		if (OwnerActor->HasAuthority())
		{
			const bool bHandled = HandleWeaponEquipIntent(Item);
			UE_LOG(LogSpellRiseEquipmentTrace, Log,
				TEXT("RequestEquipItem weapon finalizado no servidor. Owner=%s Item=%s Handled=%s"),
				*GetNameSafe(OwnerActor),
				*GetNameSafe(Item),
				bHandled ? TEXT("true") : TEXT("false"));
			return bHandled;
		}

		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("RequestEquipItem weapon encaminhado para RPC. Owner=%s Item=%s"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(Item));
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
	if (VisualSyncDepth > 0)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
			TEXT("RequestUnequipItem ignorado por VisualSyncDepth. Owner=%s Item=%s Depth=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			VisualSyncDepth);
		return true;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning, TEXT("RequestUnequipItem abortado: owner nulo. Item=%s"), *GetNameSafe(Item));
		return false;
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("RequestUnequipItem. Owner=%s Item=%s IsWeapon=%s HasAuthority=%s ActiveQuickSlot=%d"),
		*GetNameSafe(OwnerActor),
		*GetNameSafe(Item),
		IsWeaponItem(Item) ? TEXT("true") : TEXT("false"),
		OwnerActor->HasAuthority() ? TEXT("true") : TEXT("false"),
		ActiveQuickWeaponSlotIndex);

	if (IsWeaponItem(Item))
	{
		if (Item == ActiveOffHandItem)
		{
			if (OwnerActor->HasAuthority())
			{
				RemoveOffHandWeapon_Server(false);
				return true;
			}

			ServerRequestUnequipItem(Item);
			return true;
		}

		if (OwnerActor->HasAuthority())
		{
			const int32 SlotIndex = FindQuickSlotByItem(Item);
			if (SlotIndex != INDEX_NONE)
			{
				UE_LOG(LogSpellRiseEquipmentTrace, Log,
					TEXT("RequestUnequipItem weapon removendo slot. Owner=%s Item=%s Slot=%d"),
					*GetNameSafe(OwnerActor),
					*GetNameSafe(Item),
					SlotIndex);
				RemoveQuickWeaponSlot_Server(SlotIndex, false);
				return true;
			}

			UE_LOG(LogSpellRiseEquipmentTrace, Warning,
				TEXT("RequestUnequipItem weapon falhou: item nao encontrado nos quick slots. Owner=%s Item=%s"),
				*GetNameSafe(OwnerActor),
				*GetNameSafe(Item));
			return false;
		}

		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("RequestUnequipItem weapon encaminhado para RPC. Owner=%s Item=%s"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(Item));
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
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("ServerRequestEquipItem. Owner=%s Item=%s IsWeapon=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		IsWeaponItem(Item) ? TEXT("true") : TEXT("false"));

	if (IsWeaponItem(Item))
	{
		if (IsOffHandWeaponItem(Item))
		{
			const bool bHandled = HandleOffHandEquipIntent(Item);
			UE_LOG(LogSpellRiseEquipmentTrace, Log,
				TEXT("ServerRequestEquipItem offhand result. Owner=%s Item=%s Handled=%s"),
				*GetNameSafe(GetOwner()),
				*GetNameSafe(Item),
				bHandled ? TEXT("true") : TEXT("false"));
			return;
		}

		const bool bHandled = HandleWeaponEquipIntent(Item);
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("ServerRequestEquipItem weapon result. Owner=%s Item=%s Handled=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			bHandled ? TEXT("true") : TEXT("false"));
		return;
	}

	EquipItem(Item);
}

void USpellRiseEquipmentManagerComponent::ServerRequestUnequipItem_Implementation(UEquippableItem* Item)
{
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("ServerRequestUnequipItem. Owner=%s Item=%s IsWeapon=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		IsWeaponItem(Item) ? TEXT("true") : TEXT("false"));

	if (IsWeaponItem(Item))
	{
		if (Item == ActiveOffHandItem)
		{
			RemoveOffHandWeapon_Server(false);
			return;
		}

		const int32 SlotIndex = FindQuickSlotByItem(Item);
		if (SlotIndex != INDEX_NONE)
		{
			UE_LOG(LogSpellRiseEquipmentTrace, Log,
				TEXT("ServerRequestUnequipItem removendo weapon slot. Owner=%s Item=%s Slot=%d"),
				*GetNameSafe(GetOwner()),
				*GetNameSafe(Item),
				SlotIndex);
			RemoveQuickWeaponSlot_Server(SlotIndex, false);
		}
		else
		{
			UE_LOG(LogSpellRiseEquipmentTrace, Warning,
				TEXT("ServerRequestUnequipItem nao encontrou item weapon nos quick slots. Owner=%s Item=%s"),
				*GetNameSafe(GetOwner()),
				*GetNameSafe(Item));
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

bool USpellRiseEquipmentManagerComponent::RequestDropItem(UNarrativeItem* Item, int32 QuantityToDrop)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return false;
	}

	if (OwnerActor->HasAuthority())
	{
		return DropItem_Server(Item, QuantityToDrop);
	}

	ServerRequestDropItem(Item, QuantityToDrop);
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

void USpellRiseEquipmentManagerComponent::ServerRequestDropItem_Implementation(UNarrativeItem* Item, int32 QuantityToDrop)
{
	DropItem_Server(Item, QuantityToDrop);
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
		return false;
	}

	const uint8 SlotValue = ResolveItemSlot(Item);
	if (SlotValue == 255)
	{
		return false;
	}

	RemoveEntryBySlot(SlotValue);
	FSpellRiseAppliedEquipmentEntry* AddedEntry = AddEntry(Item, SlotValue);
	if (!AddedEntry)
	{
		return false;
	}

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
		return false;
	}

	const uint8 SlotValue = ResolveItemSlot(Item);
	if (!RemoveEntryBySlot(SlotValue))
	{
		return false;
	}

	return true;
}

USpellRiseEquipmentInstance* USpellRiseEquipmentManagerComponent::GetEquippedInstanceBySlot(uint8 SlotValue) const
{
	const FSpellRiseAppliedEquipmentEntry* Entry = EquipmentList.FindBySlot(SlotValue);
	return Entry ? Entry->Instance : nullptr;
}

AActor* USpellRiseEquipmentManagerComponent::GetActiveEquippedWeaponActor() const
{
	if (!QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex))
	{
		return nullptr;
	}

	UEquippableItem* ActiveItem = QuickWeaponSlots[ActiveQuickWeaponSlotIndex];
	if (!ActiveItem)
	{
		return nullptr;
	}

	if (const TObjectPtr<AActor>* ExistingActor = SpawnedWeaponActorsByItem.Find(ActiveItem))
	{
		if (IsValid(*ExistingActor))
		{
			return *ExistingActor;
		}
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(ActiveItem, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponActorClass)
	{
		return nullptr;
	}

	return ResolveOwnedWeaponActor(WeaponActorClass);
}

bool USpellRiseEquipmentManagerComponent::GetActiveEquippedWeaponSpawnPointLocation(FVector& OutLocation) const
{
	if (AActor* WeaponActor = GetActiveEquippedWeaponActor())
	{
		if (USceneComponent* SpawnPoint = ResolveWeaponSpawnPointComponent(WeaponActor))
		{
			OutLocation = SpawnPoint->GetComponentLocation();
			return true;
		}
	}

	return false;
}

bool USpellRiseEquipmentManagerComponent::GetActiveEquippedWeaponSpawnPointTransform(FTransform& OutTransform) const
{
	if (AActor* WeaponActor = GetActiveEquippedWeaponActor())
	{
		if (USceneComponent* SpawnPoint = ResolveWeaponSpawnPointComponent(WeaponActor))
		{
			OutTransform = SpawnPoint->GetComponentTransform();
			return true;
		}
	}

	return false;
}

AActor* USpellRiseEquipmentManagerComponent::GetEquippedWeaponTyped(TSubclassOf<AActor> ExpectedClass) const
{
	if (!EquippedWeapon)
	{
		return nullptr;
	}

	UClass* ExpectedWeaponClass = ExpectedClass.Get();
	if (!ExpectedWeaponClass || EquippedWeapon->IsA(ExpectedWeaponClass))
	{
		return EquippedWeapon;
	}

	return nullptr;
}

FName USpellRiseEquipmentManagerComponent::GetHUDEquipmentSlotName(ESpellRiseHUDEquipmentSlot Slot)
{
	switch (Slot)
	{
	case ESpellRiseHUDEquipmentSlot::WeaponSlot1:
		return TEXT("WeaponSlot1");
	case ESpellRiseHUDEquipmentSlot::WeaponSlot2:
		return TEXT("WeaponSlot2");
	case ESpellRiseHUDEquipmentSlot::OffHand:
		return TEXT("OffHand");
	case ESpellRiseHUDEquipmentSlot::QuickSlot1:
		return TEXT("QuickSlot1");
	case ESpellRiseHUDEquipmentSlot::QuickSlot2:
		return TEXT("QuickSlot2");
	case ESpellRiseHUDEquipmentSlot::QuickSlot3:
		return TEXT("QuickSlot3");
	case ESpellRiseHUDEquipmentSlot::QuickSlot4:
		return TEXT("QuickSlot4");
	case ESpellRiseHUDEquipmentSlot::Backpack:
		return TEXT("BackpackItem");
	case ESpellRiseHUDEquipmentSlot::Necklace:
		return TEXT("NecklaceItem");
	case ESpellRiseHUDEquipmentSlot::Head:
		return TEXT("HeadItem");
	case ESpellRiseHUDEquipmentSlot::Torso:
		return TEXT("TorsoItem");
	case ESpellRiseHUDEquipmentSlot::Hands:
		return TEXT("HandsItem");
	case ESpellRiseHUDEquipmentSlot::Legs:
		return TEXT("LegsItem");
	case ESpellRiseHUDEquipmentSlot::Feet:
		return TEXT("FeetItem");
	default:
		return NAME_None;
	}
}

UEquippableItem* USpellRiseEquipmentManagerComponent::GetEquippedItemByNarrativeSlot(EEquippableSlot Slot) const
{
	const FSpellRiseAppliedEquipmentEntry* Entry = EquipmentList.FindBySlot(static_cast<uint8>(Slot));
	return Entry ? Entry->SourceItem : nullptr;
}

FSpellRiseHUDEquipmentSlotView USpellRiseEquipmentManagerComponent::GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot Slot) const
{
	FSpellRiseHUDEquipmentSlotView View;
	View.Slot = Slot;
	View.SlotName = GetHUDEquipmentSlotName(Slot);

	switch (Slot)
	{
	case ESpellRiseHUDEquipmentSlot::WeaponSlot1:
		View.Item = QuickWeaponSlots.IsValidIndex(0) ? QuickWeaponSlots[0] : nullptr;
		View.bIsActive = View.Item && ActiveQuickWeaponSlotIndex == 0;
		View.bIsStowed = View.Item && !View.bIsActive;
		break;
	case ESpellRiseHUDEquipmentSlot::WeaponSlot2:
		View.Item = QuickWeaponSlots.IsValidIndex(1) ? QuickWeaponSlots[1] : nullptr;
		View.bIsActive = View.Item && ActiveQuickWeaponSlotIndex == 1;
		View.bIsStowed = View.Item && !View.bIsActive;
		break;
	case ESpellRiseHUDEquipmentSlot::OffHand:
		View.Item = ActiveOffHandItem;
		View.bIsBlockedByTwoHandedWeapon = View.Item && bOffHandSuppressedByTwoHandedWeapon;
		View.bIsActive = View.Item && !View.bIsBlockedByTwoHandedWeapon;
		View.bIsStowed = View.Item && View.bIsBlockedByTwoHandedWeapon;
		break;
	case ESpellRiseHUDEquipmentSlot::QuickSlot1:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Custom2);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::QuickSlot2:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Custom3);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::QuickSlot3:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Custom4);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::QuickSlot4:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Custom5);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Backpack:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Backpack);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Necklace:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Necklace);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Head:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Helmet);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Torso:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Torso);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Hands:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Hands);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Legs:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Legs);
		View.bIsActive = View.Item != nullptr;
		break;
	case ESpellRiseHUDEquipmentSlot::Feet:
		View.Item = GetEquippedItemByNarrativeSlot(EEquippableSlot::ES_Feet);
		View.bIsActive = View.Item != nullptr;
		break;
	default:
		break;
	}

	View.bHasItem = View.Item != nullptr;
	View.bIsTwoHandedWeapon = View.Item && IsTwoHandedWeaponItem(View.Item);
	return View;
}

TArray<FSpellRiseHUDEquipmentSlotView> USpellRiseEquipmentManagerComponent::GetHUDEquipmentSlotViews() const
{
	TArray<FSpellRiseHUDEquipmentSlotView> Views;
	Views.Reserve(14);
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::WeaponSlot1));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::WeaponSlot2));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::OffHand));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::QuickSlot1));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::QuickSlot2));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::QuickSlot3));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::QuickSlot4));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Backpack));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Necklace));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Head));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Torso));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Hands));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Legs));
	Views.Add(GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot::Feet));
	return Views;
}

TArray<FSpellRiseEquipmentAbilityPreview> USpellRiseEquipmentManagerComponent::GetAbilitiesToGrantPreviewForItem(UEquippableItem* Item) const
{
	TArray<FSpellRiseEquipmentAbilityPreview> PreviewAbilities;

	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	if (!ExtractAbilitiesToGrantFromItem(Item, AbilitiesToGrant))
	{
		return PreviewAbilities;
	}

	const ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(GetOwner());
	const USpellRiseAbilitySystemComponent* ASC = CharacterOwner
		? Cast<USpellRiseAbilitySystemComponent>(CharacterOwner->GetAbilitySystemComponent())
		: nullptr;
	const FGameplayAbilityActorInfo* ActorInfo = ASC ? ASC->AbilityActorInfo.Get() : nullptr;

	PreviewAbilities.Reserve(AbilitiesToGrant.Num());
	for (const FSpellRiseGrantedAbility& AbilityToGrant : AbilitiesToGrant)
	{
		if (!AbilityToGrant.Ability)
		{
			continue;
		}

		FSpellRiseEquipmentAbilityPreview& Preview = PreviewAbilities.AddDefaulted_GetRef();
		Preview.Ability = AbilityToGrant.Ability;
		Preview.AbilityLevel = AbilityToGrant.AbilityLevel;
		Preview.InputTag = AbilityToGrant.InputTag;
		Preview.bAutoActivateIfNoInputTag = AbilityToGrant.bAutoActivateIfNoInputTag;

		if (!ASC)
		{
			continue;
		}

		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (!Spec.Ability || Spec.Ability->GetClass() != AbilityToGrant.Ability)
			{
				continue;
			}

			Preview.GrantedSpecHandle = Spec.Handle;
			Preview.bIsCurrentlyGranted = true;
			Preview.bIsActive = Spec.IsActive();

			if (ActorInfo)
			{
				float TimeRemaining = 0.f;
				float CooldownDuration = 0.f;
				Spec.Ability->GetCooldownTimeRemainingAndDuration(Spec.Handle, ActorInfo, TimeRemaining, CooldownDuration);
				Preview.CooldownRemaining = FMath::Max(0.f, TimeRemaining);
				Preview.CooldownDuration = FMath::Max(0.f, CooldownDuration);
				Preview.bIsOnCooldown = Preview.CooldownRemaining > KINDA_SMALL_NUMBER;
				if (Preview.bIsOnCooldown)
				{
					const double NowSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
					Preview.CooldownCapturedWorldTimeSeconds = NowSeconds;
					Preview.CooldownEndWorldTimeSeconds = NowSeconds + static_cast<double>(Preview.CooldownRemaining);
					Preview.CooldownStartWorldTimeSeconds = Preview.CooldownDuration > KINDA_SMALL_NUMBER
						? Preview.CooldownEndWorldTimeSeconds - static_cast<double>(Preview.CooldownDuration)
						: NowSeconds;
				}
			}
			break;
		}
	}

	return PreviewAbilities;
}

float USpellRiseEquipmentManagerComponent::GetAbilityPreviewCooldownRemainingAtCurrentTime(
	const UObject* WorldContextObject,
	const FSpellRiseEquipmentAbilityPreview& Preview)
{
	if (!Preview.bIsOnCooldown)
	{
		return 0.f;
	}

	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	const double NowSeconds = World ? World->GetTimeSeconds() : Preview.CooldownCapturedWorldTimeSeconds;
	if (Preview.CooldownEndWorldTimeSeconds > 0.0)
	{
		return FMath::Max(0.f, static_cast<float>(Preview.CooldownEndWorldTimeSeconds - NowSeconds));
	}

	return FMath::Max(0.f, Preview.CooldownRemaining);
}

float USpellRiseEquipmentManagerComponent::GetAbilityPreviewCooldownPercentAtCurrentTime(
	const UObject* WorldContextObject,
	const FSpellRiseEquipmentAbilityPreview& Preview,
	const bool bReadyProgress)
{
	if (Preview.CooldownDuration <= KINDA_SMALL_NUMBER)
	{
		return bReadyProgress ? 1.f : 0.f;
	}

	const float Remaining = GetAbilityPreviewCooldownRemainingAtCurrentTime(WorldContextObject, Preview);
	const float RemainingPercent = FMath::Clamp(Remaining / Preview.CooldownDuration, 0.f, 1.f);
	return bReadyProgress ? 1.f - RemainingPercent : RemainingPercent;
}

void USpellRiseEquipmentManagerComponent::ApplyReplicatedEquipmentVisual(USpellRiseEquipmentInstance& EquipmentInstance, bool bEquipped)
{
	ApplyVisualForItem(EquipmentInstance.GetSourceItem(), bEquipped);
}

void USpellRiseEquipmentManagerComponent::HandleEntryAdded(const FSpellRiseAppliedEquipmentEntry& Entry)
{
	if (Entry.Instance)
	{
		Entry.Instance->Initialize(this, Entry.SourceItem);
		Entry.Instance->OnEquipped();
		SyncNarrativeEquipmentComponentState(Entry.SourceItem, true);
	}
	else
	{
		ApplyVisualForItem(Entry.SourceItem, true);
		SyncNarrativeEquipmentComponentState(Entry.SourceItem, true);
	}

	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::HandleEntryRemoved(const FSpellRiseAppliedEquipmentEntry& Entry)
{
	if (Entry.Instance)
	{
		Entry.Instance->Initialize(this, Entry.SourceItem);
		Entry.Instance->OnUnequipped();
		SyncNarrativeEquipmentComponentState(Entry.SourceItem, false);
	}
	else
	{
		ApplyVisualForItem(Entry.SourceItem, false);
		SyncNarrativeEquipmentComponentState(Entry.SourceItem, false);
	}

	OnHUDEquipmentSlotsChanged.Broadcast();
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
			const bool bOwnedByController = (Pawn->GetController() != nullptr && Item->GetOwningController() == Pawn->GetController());
			if (!bOwnedByController)
			{
				OutReason = TEXT("item_not_owned_by_pawn_or_controller");
				return false;
			}
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

void USpellRiseEquipmentManagerComponent::SyncNarrativeEquipmentComponentState(UEquippableItem* Item, bool bEquipped)
{
	if (!Item)
	{
		return;
	}

	UEquipmentComponent* EquipmentComponent = ResolveEquipmentComponent();
	if (!EquipmentComponent)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("SyncNarrativeEquipmentComponentState falhou: EquipmentComponent ausente. Owner=%s Item=%s Equipped=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			bEquipped ? TEXT("true") : TEXT("false"));
		return;
	}

	EquipmentComponent->SyncEquippedItemState(Item, bEquipped);
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("SyncNarrativeEquipmentComponentState sincronizou no Narrative. Owner=%s Item=%s Equipped=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		bEquipped ? TEXT("true") : TEXT("false"));
}

void USpellRiseEquipmentManagerComponent::ApplyVisualForItem(UEquippableItem* Item, bool bEquip)
{
	if (!Item)
	{
		return;
	}

	++VisualSyncDepth;

	const FName VisualFunctionName = bEquip ? FName(TEXT("HandleEquip")) : FName(TEXT("HandleUnequip"));
	if (UFunction* VisualFunction = Item->FindFunction(VisualFunctionName))
	{
		Item->ProcessEvent(VisualFunction, nullptr);
	}

	SnapItemWeaponToSocket(Item, bEquip);
	--VisualSyncDepth;
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
	SyncNarrativeEquipmentComponentState(Item, true);

	EquipmentList.MarkItemDirty(NewEntry);
	if (AActor* NetOwnerActor = GetOwner())
	{
		NetOwnerActor->ForceNetUpdate();
	}
	OnHUDEquipmentSlotsChanged.Broadcast();
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
		SyncNarrativeEquipmentComponentState(Entry.SourceItem, false);

		EquipmentList.Entries.RemoveAt(Index);
		EquipmentList.MarkArrayDirty();
		if (AActor* OwnerActor = GetOwner())
		{
			OwnerActor->ForceNetUpdate();
		}
		OnHUDEquipmentSlotsChanged.Broadcast();
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
		UE_LOG(LogSpellRiseEquipmentTrace, Warning, TEXT("ResolveWeaponActorClassFromItem abortado: Item nulo."));
		return false;
	}

	auto ResolveWeaponClassFromProperty = [Item](const FProperty* Property) -> UClass*
	{
		if (!Property)
		{
			return nullptr;
		}

		if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
		{
			if (!ClassProperty->MetaClass || !ClassProperty->MetaClass->IsChildOf(AActor::StaticClass()))
			{
				return nullptr;
			}

			const void* ClassPtr = ClassProperty->ContainerPtrToValuePtr<void>(Item);
			UObject* WeaponActorClassObject = ClassProperty->GetObjectPropertyValue(ClassPtr);
			return Cast<UClass>(WeaponActorClassObject);
		}

		if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
		{
			if (!SoftClassProperty->MetaClass || !SoftClassProperty->MetaClass->IsChildOf(AActor::StaticClass()))
			{
				return nullptr;
			}

			const void* ValuePtr = SoftClassProperty->ContainerPtrToValuePtr<void>(Item);
			const FSoftObjectPtr& SoftObjectPtr = SoftClassProperty->GetPropertyValue(ValuePtr);
			UObject* ResolvedObject = SoftObjectPtr.Get();
			if (!ResolvedObject)
			{
				ResolvedObject = SoftObjectPtr.LoadSynchronous();
			}

			return Cast<UClass>(ResolvedObject);
		}

		return nullptr;
	};

	UClass* WeaponActorClass = nullptr;

	static const TArray<FName> PreferredNames = {
		TEXT("WeaponClass"),
		TEXT("Weapon Class"),
		TEXT("WeaponActorClass"),
		TEXT("WeaponActor"),
		TEXT("Weapon")
	};

	for (const FName PropertyName : PreferredNames)
	{
		WeaponActorClass = ResolveWeaponClassFromProperty(Item->GetClass()->FindPropertyByName(PropertyName));
		if (WeaponActorClass)
		{
			break;
		}
	}

	if (!WeaponActorClass)
	{
		for (TFieldIterator<FProperty> It(Item->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
		{
			FProperty* Property = *It;
			const FString PropertyName = Property ? Property->GetName() : FString();
			if (!Property || !PropertyName.Contains(TEXT("Weapon"), ESearchCase::IgnoreCase))
			{
				continue;
			}

			WeaponActorClass = ResolveWeaponClassFromProperty(Property);
			if (WeaponActorClass)
			{
				break;
			}
		}
	}

	if (!WeaponActorClass)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ResolveWeaponActorClassFromItem falhou: nenhuma WeaponClass encontrada. Item=%s ItemClass=%s"),
			*GetNameSafe(Item),
			*GetNameSafe(Item->GetClass()));
		return false;
	}

	const UObject* WeaponCDO = WeaponActorClass->GetDefaultObject();
	if (!WeaponCDO)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ResolveWeaponActorClassFromItem sem CDO. Item=%s WeaponClass=%s"),
			*GetNameSafe(Item),
			*GetNameSafe(WeaponActorClass));
		OutWeaponActorClass = WeaponActorClass;
		return true;
	}

	const FStructProperty* WeaponConfigProperty = CastField<FStructProperty>(WeaponActorClass->FindPropertyByName(TEXT("WeaponConfig")));
	if (!WeaponConfigProperty)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ResolveWeaponActorClassFromItem sem WeaponConfig. Item=%s WeaponClass=%s"),
			*GetNameSafe(Item),
			*GetNameSafe(WeaponActorClass));
		OutWeaponActorClass = WeaponActorClass;
		return true;
	}

	const void* WeaponConfigPtr = WeaponConfigProperty->ContainerPtrToValuePtr<void>(WeaponCDO);
	if (!WeaponConfigPtr)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ResolveWeaponActorClassFromItem WeaponConfig nulo. Item=%s WeaponClass=%s"),
			*GetNameSafe(Item),
			*GetNameSafe(WeaponActorClass));
		OutWeaponActorClass = WeaponActorClass;
		return true;
	}

	OutWeaponActorClass = WeaponActorClass;
	OutWeaponConfigPtr = WeaponConfigPtr;
	OutWeaponConfigStruct = WeaponConfigProperty->Struct;
	UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
		TEXT("ResolveWeaponActorClassFromItem ok. Item=%s WeaponClass=%s WeaponConfigStruct=%s"),
		*GetNameSafe(Item),
		*GetNameSafe(WeaponActorClass),
		*GetNameSafe(WeaponConfigProperty->Struct));
	return true;
}

bool USpellRiseEquipmentManagerComponent::ResolveWeaponSocketsFromConfig(const void* WeaponConfigPtr, const UStruct* WeaponConfigStruct, FName& OutEquippedSocket, FName& OutStowedSocket) const
{
	OutEquippedSocket = TEXT("hand_r");
	OutStowedSocket = TEXT("holster_r");

	if (!WeaponConfigPtr || !WeaponConfigStruct)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ResolveWeaponSocketsFromConfig usando sockets default. ConfigPtr=%s ConfigStruct=%s Equipped=%s Stowed=%s"),
			WeaponConfigPtr ? TEXT("yes") : TEXT("no"),
			*GetNameSafe(WeaponConfigStruct),
			*OutEquippedSocket.ToString(),
			*OutStowedSocket.ToString());
		return true;
	}

	for (TFieldIterator<FProperty> It(WeaponConfigStruct); It; ++It)
	{
		const FString PropertyName = It->GetName();
		const FNameProperty* NameProperty = CastField<FNameProperty>(*It);
		if (!NameProperty)
		{
			continue;
		}


		if (PropertyName.Contains(TEXT("EquippedSocketName")))
		{
			OutEquippedSocket = NameProperty->GetPropertyValue_InContainer(WeaponConfigPtr);
		}
		else if (PropertyName.Contains(TEXT("HolsterSocketName")) || PropertyName.Contains(TEXT("StowedSocketName")))
		{
			OutStowedSocket = NameProperty->GetPropertyValue_InContainer(WeaponConfigPtr);
		}
	}


	UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
		TEXT("ResolveWeaponSocketsFromConfig. ConfigStruct=%s Equipped=%s Stowed=%s"),
		*GetNameSafe(WeaponConfigStruct),
		*OutEquippedSocket.ToString(),
		*OutStowedSocket.ToString());
	return OutEquippedSocket != NAME_None || OutStowedSocket != NAME_None;
}

bool USpellRiseEquipmentManagerComponent::ResolveWeaponSocketsForItem(UEquippableItem* Item, bool bOffHand, FName& OutEquippedSocket, FName& OutHolsterSocket) const
{
	OutEquippedSocket = bOffHand ? TEXT("hand_l") : TEXT("hand_r");
	OutHolsterSocket = bOffHand ? TEXT("holster_l") : TEXT("holster_r");

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct))
	{
		return false;
	}

	if (!WeaponConfigPtr || !WeaponConfigStruct)
	{
		return true;
	}

	auto ReadNameProperty = [WeaponConfigPtr, WeaponConfigStruct](const TArray<FString>& PreferredNames, FName& OutValue) -> bool
	{
		for (const FString& PreferredName : PreferredNames)
		{
			for (TFieldIterator<FProperty> It(WeaponConfigStruct); It; ++It)
			{
				const FNameProperty* NameProperty = CastField<FNameProperty>(*It);
				if (!NameProperty || !It->GetName().Contains(PreferredName, ESearchCase::IgnoreCase))
				{
					continue;
				}

				const FName Value = NameProperty->GetPropertyValue_InContainer(WeaponConfigPtr);
				if (Value != NAME_None)
				{
					OutValue = Value;
					return true;
				}
			}
		}

		return false;
	};

	if (bOffHand)
	{
		ReadNameProperty({ TEXT("OffHandEquippedSocketName"), TEXT("OffHandEquippedSocket"), TEXT("EquippedSocketName") }, OutEquippedSocket);
		ReadNameProperty({ TEXT("OffHandHolsterSocketName"), TEXT("OffHandHolsterSocket"), TEXT("HolsterSocketName"), TEXT("StowedSocketName") }, OutHolsterSocket);
	}
	else if (IsTwoHandedWeaponItem(Item))
	{
		ReadNameProperty({ TEXT("TwoHandEquippedSocketName"), TEXT("TwoHandEquippedSocket"), TEXT("MainHandEquippedSocketName"), TEXT("EquippedSocketName") }, OutEquippedSocket);
		ReadNameProperty({ TEXT("TwoHandHolsterSocketName"), TEXT("TwoHandHolsterSocket"), TEXT("MainHandHolsterSocketName"), TEXT("HolsterSocketName"), TEXT("StowedSocketName") }, OutHolsterSocket);
	}
	else
	{
		ReadNameProperty({ TEXT("MainHandEquippedSocketName"), TEXT("MainHandEquippedSocket"), TEXT("EquippedSocketName") }, OutEquippedSocket);
		ReadNameProperty({ TEXT("MainHandHolsterSocketName"), TEXT("MainHandHolsterSocket"), TEXT("HolsterSocketName"), TEXT("StowedSocketName") }, OutHolsterSocket);
	}

	return OutEquippedSocket != NAME_None || OutHolsterSocket != NAME_None;
}

void USpellRiseEquipmentManagerComponent::RefreshEquippedWeaponReference()
{
	EquippedWeapon = nullptr;

	if (!QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex))
	{
		return;
	}

	UEquippableItem* ActiveItem = QuickWeaponSlots[ActiveQuickWeaponSlotIndex];
	if (!ActiveItem)
	{
		return;
	}

	EquippedWeapon = GetOrSpawnWeaponActorForItem(ActiveItem);
}

void USpellRiseEquipmentManagerComponent::RefreshEquippedOffHandWeaponReference()
{
	EquippedOffHandWeapon = ActiveOffHandItem ? GetOrSpawnWeaponActorForItem(ActiveOffHandItem) : nullptr;
}

void USpellRiseEquipmentManagerComponent::BroadcastWeaponChangedIfNeeded()
{
	if (LastBroadcastEquippedWeapon == EquippedWeapon)
	{
		return;
	}

	LastBroadcastEquippedWeapon = EquippedWeapon;
	OnWeaponChanged.Broadcast(EquippedWeapon);
	BroadcastWeaponLoadoutChangedIfNeeded();
}

void USpellRiseEquipmentManagerComponent::BroadcastOffHandWeaponChangedIfNeeded()
{
	if (LastBroadcastEquippedOffHandWeapon == EquippedOffHandWeapon)
	{
		return;
	}

	LastBroadcastEquippedOffHandWeapon = EquippedOffHandWeapon;
	OnOffHandWeaponChanged.Broadcast(EquippedOffHandWeapon);
	BroadcastWeaponLoadoutChangedIfNeeded();
}

void USpellRiseEquipmentManagerComponent::BroadcastWeaponLoadoutChangedIfNeeded()
{
	if (LastBroadcastLoadoutMainHandWeapon == EquippedWeapon && LastBroadcastLoadoutOffHandWeapon == EquippedOffHandWeapon)
	{
		return;
	}

	LastBroadcastLoadoutMainHandWeapon = EquippedWeapon;
	LastBroadcastLoadoutOffHandWeapon = EquippedOffHandWeapon;
	OnWeaponLoadoutChanged.Broadcast(EquippedWeapon, EquippedOffHandWeapon);
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

USceneComponent* USpellRiseEquipmentManagerComponent::ResolveWeaponSpawnPointComponent(AActor* WeaponActor) const
{
	if (!IsValid(WeaponActor))
	{
		return nullptr;
	}

	if (USceneComponent* SpawnPointComponent = SpellRiseEquipmentAttach::FindSceneComponentByName(WeaponActor, TEXT("SpawnPoint")))
	{
		return SpawnPointComponent;
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

		if (USkeletalMeshComponent* BaseMesh = CharacterOwner->GetMesh())
		{
			return BaseMesh;
		}
	}

	return OwnerActor->FindComponentByClass<USkeletalMeshComponent>();
}

void USpellRiseEquipmentManagerComponent::PrepareWeaponActorForAttachment(AActor* WeaponActor) const
{
	if (!IsValid(WeaponActor))
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	WeaponActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComp : PrimitiveComponents)
	{
		if (!IsValid(PrimitiveComp))
		{
			continue;
		}

		if (PrimitiveComp->IsSimulatingPhysics())
		{
			PrimitiveComp->SetSimulatePhysics(false);
		}
	}

	WeaponActor->SetReplicateMovement(false);
}

bool USpellRiseEquipmentManagerComponent::AttachWeaponActorToSocket(AActor* WeaponActor, USkeletalMeshComponent* AttachMesh, FName TargetSocket) const
{
	if (!IsValid(WeaponActor) || !IsValid(AttachMesh) || TargetSocket == NAME_None)
	{
		UE_LOG(LogSpellRiseWeaponAttach, Warning,
			TEXT("AttachWeaponActorToSocket aborted (invalid args). WeaponActor=%s AttachMesh=%s Socket=%s"),
			*GetNameSafe(WeaponActor),
			*GetNameSafe(AttachMesh),
			*TargetSocket.ToString());
		return false;
	}

	PrepareWeaponActorForAttachment(WeaponActor);

	USceneComponent* WeaponComponent = SpellRiseEquipmentAttach::ResolveWeaponAttachSceneComponent(WeaponActor);
	if (!IsValid(WeaponComponent))
	{
		UE_LOG(LogSpellRiseWeaponAttach, Warning,
			TEXT("AttachWeaponActorToSocket aborted (no Weapon/root scene component). WeaponActor=%s"),
			*GetNameSafe(WeaponActor));
		return false;
	}

	if (UPrimitiveComponent* WeaponPrimitive = SpellRiseEquipmentAttach::ResolveWeaponAttachPrimitiveComponent(WeaponActor))
	{
		WeaponPrimitive->SetSimulatePhysics(false);
	}

	const FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;

	const bool bSocketOnMesh = AttachMesh->DoesSocketExist(TargetSocket);
	const bool bAttached = WeaponComponent->AttachToComponent(AttachMesh, AttachRules, TargetSocket);

	if (!bAttached)
	{
		UE_LOG(LogSpellRiseWeaponAttach, Warning,
			TEXT("AttachToComponent FAILED (weapon stays at spawn/world transform). DoesSocketExist=%s | WeaponActor=%s WeaponComp=%s ParentMesh=%s Socket=%s WeaponWorldLoc=%s"),
			bSocketOnMesh ? TEXT("yes") : TEXT("NO — wrong mesh or socket name"),
			*GetNameSafe(WeaponActor),
			*WeaponComponent->GetName(),
			*AttachMesh->GetName(),
			*TargetSocket.ToString(),
			*WeaponActor->GetActorLocation().ToString());
	}
	else
	{
		UE_LOG(LogSpellRiseWeaponAttach, Log,
			TEXT("AttachToComponent OK. WeaponActor=%s WeaponComp=%s ParentMesh=%s Socket=%s DoesSocketExist=%s"),
			*GetNameSafe(WeaponActor),
			*WeaponComponent->GetName(),
			*AttachMesh->GetName(),
			*TargetSocket.ToString(),
			bSocketOnMesh ? TEXT("yes") : TEXT("no (bone/socket naming — attach still succeeded)"));
	}

	return bAttached;
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
	FName HolsterSocket = NAME_None;
	ResolveWeaponSocketsForItem(Item, IsOffHandWeaponItem(Item), EquippedSocket, HolsterSocket);

	const FName TargetSocket = bEquip ? EquippedSocket : HolsterSocket;
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

	AttachWeaponActorToSocket(WeaponActor, AttachMesh, TargetSocket);
}

void USpellRiseEquipmentManagerComponent::RefreshQuickSlotVisual_Local(int32 QuickSlotIndex, bool bEquipped)
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

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponActorClass)
	{
		return;
	}

	FName EquippedSocket = NAME_None;
	FName HolsterSocket = NAME_None;
	ResolveWeaponSocketsForItem(Item, false, EquippedSocket, HolsterSocket);
	const FName TargetSocket = bEquipped ? EquippedSocket : HolsterSocket;
	if (TargetSocket == NAME_None)
	{
		return;
	}

	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh();
	AActor* WeaponActor = ResolveOwnedWeaponActor(WeaponActorClass);
	if (!AttachMesh || !WeaponActor)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
			TEXT("RefreshQuickSlotVisual_Local aguardando actor/mesh. Owner=%s Item=%s Slot=%d Equipped=%s AttachMesh=%s WeaponActor=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"),
			*GetNameSafe(AttachMesh),
			*GetNameSafe(WeaponActor));
		return;
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("RefreshQuickSlotVisual_Local attachando em cliente/replica. Owner=%s Item=%s WeaponActor=%s Slot=%d Equipped=%s Socket=%s Mesh=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		*GetNameSafe(WeaponActor),
		QuickSlotIndex,
		bEquipped ? TEXT("true") : TEXT("false"),
		*TargetSocket.ToString(),
		*GetNameSafe(AttachMesh));
	AttachWeaponActorToSocket(WeaponActor, AttachMesh, TargetSocket);
}

void USpellRiseEquipmentManagerComponent::ReconcileReplicatedQuickSlotVisuals()
{
	for (int32 SlotIndex = 0; SlotIndex < QuickWeaponSlots.Num(); ++SlotIndex)
	{
		if (!QuickWeaponSlots[SlotIndex])
		{
			continue;
		}

		const bool bShouldBeEquipped = (SlotIndex == ActiveQuickWeaponSlotIndex);
		RefreshQuickSlotVisual_Local(SlotIndex, bShouldBeEquipped);
	}

	RefreshOffHandVisual_Local(IsOffHandGameplayActive());
}

void USpellRiseEquipmentManagerComponent::ApplyGrantedAbilitiesForSlot(UEquippableItem* Item, uint8 SlotValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ApplyGrantedAbilitiesForSlot abortado. Owner=%s Item=%s SlotValue=%d HasAuthority=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotValue,
			(GetOwner() && GetOwner()->HasAuthority()) ? TEXT("true") : TEXT("false"));
		return;
	}

	ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(GetOwner());
	if (!CharacterOwner)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ApplyGrantedAbilitiesForSlot abortado: owner nao e ASpellRiseCharacterBase. Owner=%s Item=%s SlotValue=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotValue);
		return;
	}

	RemoveGrantedAbilitiesForSlot(SlotValue);

	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	const bool bHasAbilitiesToGrant = ExtractAbilitiesToGrantFromItem(Item, AbilitiesToGrant);
	if (bHasAbilitiesToGrant)
	{
		const TArray<FGameplayAbilitySpecHandle> Handles = CharacterOwner->GrantAbilities(AbilitiesToGrant);
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("ApplyGrantedAbilitiesForSlot abilities aplicadas. Owner=%s Item=%s SlotValue=%d Requested=%d Granted=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotValue,
			AbilitiesToGrant.Num(),
			Handles.Num());
		if (Handles.Num() > 0)
		{
			GrantedAbilityHandlesBySlot.Add(SlotValue, Handles);
		}
	}

	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;
	if (!ExtractGrantedEffectsFromItem(Item, EffectsToApply))
	{
		if (!bHasAbilitiesToGrant)
		{
			UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
				TEXT("ApplyGrantedAbilitiesForSlot sem grants configurados. Owner=%s Item=%s SlotValue=%d"),
				*GetNameSafe(GetOwner()),
				*GetNameSafe(Item),
				SlotValue);
		}
		return;
	}

	USpellRiseAbilitySystemComponent* ASC = Cast<USpellRiseAbilitySystemComponent>(CharacterOwner->GetAbilitySystemComponent());
	if (!ASC)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ApplyGrantedAbilitiesForSlot abortado: ASC ausente para efeitos. Owner=%s Item=%s SlotValue=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotValue);
		return;
	}

	TMap<FGameplayTag, float> SetByCallerMagnitudes;
	ExtractSetByCallerMagnitudesFromItem(Item, SetByCallerMagnitudes);

	TArray<FActiveGameplayEffectHandle> AppliedEffectHandles;
	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApply)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(Item);
		EffectContext.AddInstigator(CharacterOwner, CharacterOwner);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.f, EffectContext);
		if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
		{
			continue;
		}

		for (const TPair<FGameplayTag, float>& Magnitude : SetByCallerMagnitudes)
		{
			if (Magnitude.Key.IsValid() && FMath::IsFinite(Magnitude.Value))
			{
				SpecHandle.Data->SetSetByCallerMagnitude(Magnitude.Key, Magnitude.Value);
			}
		}

		const FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (ActiveHandle.IsValid())
		{
			AppliedEffectHandles.Add(ActiveHandle);
		}
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("ApplyGrantedAbilitiesForSlot effects aplicados. Owner=%s Item=%s SlotValue=%d Requested=%d Applied=%d SetByCaller=%d"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		SlotValue,
		EffectsToApply.Num(),
		AppliedEffectHandles.Num(),
		SetByCallerMagnitudes.Num());
	if (AppliedEffectHandles.Num() > 0)
	{
		GrantedEffectHandlesBySlot.Add(SlotValue, AppliedEffectHandles);
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

	if (USpellRiseAbilitySystemComponent* ASC = Cast<USpellRiseAbilitySystemComponent>(CharacterOwner->GetAbilitySystemComponent()))
	{
		TArray<FActiveGameplayEffectHandle> EffectHandlesToRemove;
		if (GrantedEffectHandlesBySlot.RemoveAndCopyValue(SlotValue, EffectHandlesToRemove))
		{
			for (const FActiveGameplayEffectHandle& EffectHandle : EffectHandlesToRemove)
			{
				if (EffectHandle.IsValid())
				{
					ASC->RemoveActiveGameplayEffect(EffectHandle);
				}
			}
		}
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

bool USpellRiseEquipmentManagerComponent::ExtractGrantedEffectsFromItem(
	UEquippableItem* Item,
	TArray<TSubclassOf<UGameplayEffect>>& OutEffects) const
{
	OutEffects.Reset();
	if (!Item)
	{
		return false;
	}

	static const FName GrantedEffectsPropertyName(TEXT("GrantedEffectsWhileEquipped"));
	const FProperty* Property = Item->GetClass()->FindPropertyByName(GrantedEffectsPropertyName);
	if (!Property)
	{
		return false;
	}

	const void* PropertyValuePtr = Property->ContainerPtrToValuePtr<void>(Item);
	if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper ArrayHelper(ArrayProperty, PropertyValuePtr);
		for (int32 Index = 0; Index < ArrayHelper.Num(); ++Index)
		{
			UClass* EffectClass = nullptr;
			if (const FClassProperty* ClassProperty = CastField<FClassProperty>(ArrayProperty->Inner))
			{
				if (ClassProperty->MetaClass && ClassProperty->MetaClass->IsChildOf(UGameplayEffect::StaticClass()))
				{
					EffectClass = Cast<UClass>(ClassProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(Index)));
				}
			}
			else if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(ArrayProperty->Inner))
			{
				const FSoftObjectPtr SoftClassPtr = SoftClassProperty->GetPropertyValue(ArrayHelper.GetRawPtr(Index));
				EffectClass = Cast<UClass>(SoftClassPtr.LoadSynchronous());
			}

			if (EffectClass && EffectClass->IsChildOf(UGameplayEffect::StaticClass()))
			{
				OutEffects.AddUnique(EffectClass);
			}
		}
	}
	else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
	{
		if (ClassProperty->MetaClass && ClassProperty->MetaClass->IsChildOf(UGameplayEffect::StaticClass()))
		{
			UClass* EffectClass = Cast<UClass>(ClassProperty->GetObjectPropertyValue(PropertyValuePtr));
			if (EffectClass && EffectClass->IsChildOf(UGameplayEffect::StaticClass()))
			{
				OutEffects.AddUnique(EffectClass);
			}
		}
	}
	else if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
	{
		const FSoftObjectPtr SoftClassPtr = SoftClassProperty->GetPropertyValue(PropertyValuePtr);
		UClass* EffectClass = Cast<UClass>(SoftClassPtr.LoadSynchronous());
		if (EffectClass && EffectClass->IsChildOf(UGameplayEffect::StaticClass()))
		{
			OutEffects.AddUnique(EffectClass);
		}
	}

	return OutEffects.Num() > 0;
}

bool USpellRiseEquipmentManagerComponent::ExtractSetByCallerMagnitudesFromItem(
	UEquippableItem* Item,
	TMap<FGameplayTag, float>& OutMagnitudes) const
{
	OutMagnitudes.Reset();
	if (!Item)
	{
		return false;
	}

	static const FName LegacyMagnitudeMapName(TEXT("SetSetByCallerMagnitude"));
	static const FName MagnitudeMapName(TEXT("SetByCallerMagnitude"));
	const FMapProperty* MapProperty = CastField<FMapProperty>(Item->GetClass()->FindPropertyByName(LegacyMagnitudeMapName));
	if (!MapProperty)
	{
		MapProperty = CastField<FMapProperty>(Item->GetClass()->FindPropertyByName(MagnitudeMapName));
	}

	if (!MapProperty)
	{
		return false;
	}

	const FStructProperty* KeyProperty = CastField<FStructProperty>(MapProperty->KeyProp);
	if (!KeyProperty || KeyProperty->Struct != FGameplayTag::StaticStruct())
	{
		return false;
	}

	const void* MapValuePtr = MapProperty->ContainerPtrToValuePtr<void>(Item);
	FScriptMapHelper MapHelper(MapProperty, MapValuePtr);
	for (int32 Index = 0; Index < MapHelper.GetMaxIndex(); ++Index)
	{
		if (!MapHelper.IsValidIndex(Index))
		{
			continue;
		}

		const FGameplayTag* Tag = reinterpret_cast<const FGameplayTag*>(MapHelper.GetKeyPtr(Index));
		if (!Tag || !Tag->IsValid())
		{
			continue;
		}

		const uint8* ValuePtr = MapHelper.GetValuePtr(Index);
		float Magnitude = 0.f;
		if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(MapProperty->ValueProp))
		{
			Magnitude = FloatProperty->GetPropertyValue(ValuePtr);
		}
		else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(MapProperty->ValueProp))
		{
			Magnitude = static_cast<float>(DoubleProperty->GetPropertyValue(ValuePtr));
		}
		else if (const FIntProperty* IntProperty = CastField<FIntProperty>(MapProperty->ValueProp))
		{
			Magnitude = static_cast<float>(IntProperty->GetPropertyValue(ValuePtr));
		}
		else
		{
			continue;
		}

		if (FMath::IsFinite(Magnitude))
		{
			OutMagnitudes.Add(*Tag, Magnitude);
		}
	}

	return OutMagnitudes.Num() > 0;
}

bool USpellRiseEquipmentManagerComponent::IsWeaponItem(UEquippableItem* Item) const
{
	if (!Item)
	{
		return false;
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	return ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) && WeaponActorClass != nullptr;
}

bool USpellRiseEquipmentManagerComponent::IsOffHandWeaponItem(UEquippableItem* Item) const
{
	if (!IsWeaponItem(Item))
	{
		return false;
	}

	auto ReadBoolMetadata = [](const void* ContainerPtr, const UStruct* StructType, const TArray<FString>& Names) -> bool
	{
		if (!ContainerPtr || !StructType)
		{
			return false;
		}

		for (TFieldIterator<FProperty> It(StructType); It; ++It)
		{
			const FBoolProperty* BoolProperty = CastField<FBoolProperty>(*It);
			if (!BoolProperty)
			{
				continue;
			}

			for (const FString& Name : Names)
			{
				if (It->GetName().Contains(Name, ESearchCase::IgnoreCase) && BoolProperty->GetPropertyValue_InContainer(ContainerPtr))
				{
					return true;
				}
			}
		}

		return false;
	};

	auto ReadTextMetadata = [](const void* ContainerPtr, const UStruct* StructType, const TArray<FString>& Names, const TArray<FString>& ExpectedValues) -> bool
	{
		if (!ContainerPtr || !StructType)
		{
			return false;
		}

		for (TFieldIterator<FProperty> It(StructType); It; ++It)
		{
			const FString PropertyName = It->GetName();
			bool bNameMatches = false;
			for (const FString& Name : Names)
			{
				if (PropertyName.Contains(Name, ESearchCase::IgnoreCase))
				{
					bNameMatches = true;
					break;
				}
			}
			if (!bNameMatches)
			{
				continue;
			}

			FString Value;
			if (const FStrProperty* StrProperty = CastField<FStrProperty>(*It))
			{
				Value = StrProperty->GetPropertyValue_InContainer(ContainerPtr);
			}
			else if (const FNameProperty* NameProperty = CastField<FNameProperty>(*It))
			{
				Value = NameProperty->GetPropertyValue_InContainer(ContainerPtr).ToString();
			}
			else if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(*It))
			{
				const void* ValuePtr = EnumProperty->ContainerPtrToValuePtr<void>(ContainerPtr);
				const int64 IntValue = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(ValuePtr);
				Value = EnumProperty->GetEnum()->GetNameStringByValue(IntValue);
			}
			else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(*It))
			{
				const UEnum* Enum = ByteProperty->Enum;
				const void* ValuePtr = ByteProperty->ContainerPtrToValuePtr<void>(ContainerPtr);
				const int64 IntValue = ByteProperty->GetUnsignedIntPropertyValue(ValuePtr);
				Value = Enum ? Enum->GetNameStringByValue(IntValue) : FString::FromInt(static_cast<int32>(IntValue));
			}

			for (const FString& ExpectedValue : ExpectedValues)
			{
				if (Value.Contains(ExpectedValue, ESearchCase::IgnoreCase))
				{
					return true;
				}
			}
		}

		return false;
	};

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct);

	const TArray<FString> OffHandNames = { TEXT("OffHand"), TEXT("SecondaryHand"), TEXT("AllowedHand"), TEXT("WeaponHand"), TEXT("WeaponGrip") };
	const TArray<FString> OffHandValues = { TEXT("OffHand"), TEXT("Secondary"), TEXT("Shield"), TEXT("OffHandOnly") };
	if (ReadBoolMetadata(Item, Item->GetClass(), { TEXT("OffHand"), TEXT("OffHandOnly"), TEXT("Shield") }) ||
		ReadBoolMetadata(WeaponConfigPtr, WeaponConfigStruct, { TEXT("OffHand"), TEXT("OffHandOnly"), TEXT("Shield") }) ||
		ReadTextMetadata(Item, Item->GetClass(), OffHandNames, OffHandValues) ||
		ReadTextMetadata(WeaponConfigPtr, WeaponConfigStruct, OffHandNames, OffHandValues))
	{
		return true;
	}

	return ResolveItemSlot(Item) == 9;
}

bool USpellRiseEquipmentManagerComponent::IsTwoHandedWeaponItem(UEquippableItem* Item) const
{
	if (!IsWeaponItem(Item))
	{
		return false;
	}

	auto ReadTwoHandedMetadata = [](const void* ContainerPtr, const UStruct* StructType) -> bool
	{
		if (!ContainerPtr || !StructType)
		{
			return false;
		}

		for (TFieldIterator<FProperty> It(StructType); It; ++It)
		{
			const FString PropertyName = It->GetName();
			if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(*It))
			{
				if ((PropertyName.Contains(TEXT("TwoHand"), ESearchCase::IgnoreCase) || PropertyName.Contains(TEXT("TwoHanded"), ESearchCase::IgnoreCase)) &&
					BoolProperty->GetPropertyValue_InContainer(ContainerPtr))
				{
					return true;
				}
			}

			FString Value;
			if (const FStrProperty* StrProperty = CastField<FStrProperty>(*It))
			{
				Value = StrProperty->GetPropertyValue_InContainer(ContainerPtr);
			}
			else if (const FNameProperty* NameProperty = CastField<FNameProperty>(*It))
			{
				Value = NameProperty->GetPropertyValue_InContainer(ContainerPtr).ToString();
			}
			else if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(*It))
			{
				const void* ValuePtr = EnumProperty->ContainerPtrToValuePtr<void>(ContainerPtr);
				const int64 IntValue = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(ValuePtr);
				Value = EnumProperty->GetEnum()->GetNameStringByValue(IntValue);
			}

			if ((PropertyName.Contains(TEXT("Grip"), ESearchCase::IgnoreCase) || PropertyName.Contains(TEXT("Hand"), ESearchCase::IgnoreCase)) &&
				Value.Contains(TEXT("TwoHand"), ESearchCase::IgnoreCase))
			{
				return true;
			}
		}

		return false;
	};

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct);

	return ReadTwoHandedMetadata(Item, Item->GetClass()) || ReadTwoHandedMetadata(WeaponConfigPtr, WeaponConfigStruct);
}

bool USpellRiseEquipmentManagerComponent::IsOffHandGameplayActive() const
{
	return ActiveOffHandItem != nullptr && !bOffHandSuppressedByTwoHandedWeapon;
}

void USpellRiseEquipmentManagerComponent::RefreshOffHandSuppression_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!ActiveOffHandItem)
	{
		RemoveGrantedAbilitiesForSlot(241);
		bOffHandSuppressedByTwoHandedWeapon = false;
		EquippedOffHandWeapon = nullptr;
		BroadcastOffHandWeaponChangedIfNeeded();
		OnHUDEquipmentSlotsChanged.Broadcast();
		return;
	}

	UEquippableItem* ActiveMainHandItem = QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex)
		? QuickWeaponSlots[ActiveQuickWeaponSlotIndex]
		: nullptr;
	const bool bShouldSuppress = IsTwoHandedWeaponItem(ActiveMainHandItem);
	bOffHandSuppressedByTwoHandedWeapon = bShouldSuppress;

	GetOrSpawnWeaponActorForItem(ActiveOffHandItem);
	if (bShouldSuppress)
	{
		RemoveGrantedAbilitiesForSlot(241);
		SetNarrativeItemActiveState(ActiveOffHandItem, false);
		RefreshOffHandVisual_Server(false);
		RefreshEquippedOffHandWeaponReference();
	}
	else
	{
		SetNarrativeItemActiveState(ActiveOffHandItem, true);
		RefreshOffHandVisual_Server(true);
		ApplyGrantedAbilitiesForSlot(ActiveOffHandItem, 241);
		RefreshEquippedOffHandWeaponReference();
	}

	BroadcastOffHandWeaponChangedIfNeeded();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
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

int32 USpellRiseEquipmentManagerComponent::GetPreferredQuickWeaponSlotForItem(UEquippableItem* Item) const
{
	if (!Item)
	{
		return INDEX_NONE;
	}

	const uint8 SlotValue = ResolveItemSlot(Item);
	if (SlotValue == static_cast<uint8>(EEquippableSlot::ES_Weapon))
	{
		return 0;
	}
	if (SlotValue == static_cast<uint8>(EEquippableSlot::ES_Holster))
	{
		return 1;
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

bool USpellRiseEquipmentManagerComponent::HandleWeaponEquipIntent(UEquippableItem* Item)
{
	if (!Item || !GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("HandleWeaponEquipIntent abortado. Owner=%s Item=%s HasAuthority=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			(GetOwner() && GetOwner()->HasAuthority()) ? TEXT("true") : TEXT("false"));
		return false;
	}

	const int32 ExistingSlot = FindQuickSlotByItem(Item);
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("HandleWeaponEquipIntent. Owner=%s Item=%s ExistingSlot=%d ActiveQuickSlot=%d"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		ExistingSlot,
		ActiveQuickWeaponSlotIndex);

	if (ExistingSlot != INDEX_NONE)
	{
		const bool bActivated = ActivateQuickWeaponSlot_Server(ExistingSlot);
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("HandleWeaponEquipIntent reutilizou slot existente. Owner=%s Item=%s Slot=%d Activated=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			ExistingSlot,
			bActivated ? TEXT("true") : TEXT("false"));
		return bActivated;
	}

	int32 SlotToUse = GetPreferredQuickWeaponSlotForItem(Item);
	if (SlotToUse != INDEX_NONE && QuickWeaponSlots.IsValidIndex(SlotToUse) && QuickWeaponSlots[SlotToUse] == Item)
	{
		return ActivateQuickWeaponSlot_Server(SlotToUse);
	}
	if (SlotToUse != INDEX_NONE && (!QuickWeaponSlots.IsValidIndex(SlotToUse)))
	{
		SlotToUse = INDEX_NONE;
	}
	if (SlotToUse != INDEX_NONE && QuickWeaponSlots.IsValidIndex(SlotToUse) && QuickWeaponSlots[SlotToUse] && QuickWeaponSlots[SlotToUse] != Item)
	{
		const int32 FreeSlot = FindFirstFreeQuickSlot();
		if (FreeSlot != INDEX_NONE)
		{
			SlotToUse = FreeSlot;
		}
	}
	if (SlotToUse == INDEX_NONE)
	{
		SlotToUse = FindFirstFreeQuickSlot();
	}
	if (SlotToUse == INDEX_NONE)
	{
		SlotToUse = GetPreferredQuickWeaponSlotForItem(Item);
	}
	if (SlotToUse == INDEX_NONE)
	{
		SlotToUse = (ActiveQuickWeaponSlotIndex == 0) ? 1 : 0;
	}

	const bool bAssigned = AssignQuickWeaponSlot_Server(Item, SlotToUse);
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("HandleWeaponEquipIntent tentou assign. Owner=%s Item=%s Slot=%d Assigned=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		SlotToUse,
		bAssigned ? TEXT("true") : TEXT("false"));
	return bAssigned;
}

bool USpellRiseEquipmentManagerComponent::HandleOffHandEquipIntent(UEquippableItem* Item)
{
	if (!Item || !GetOwner() || !GetOwner()->HasAuthority() || !IsOffHandWeaponItem(Item))
	{
		return false;
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("HandleOffHandEquipIntent rejeitado por ownership. Owner=%s Item=%s Reason=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			*ValidationReason);
		return false;
	}

	UEquippableItem* ActiveMainHandItem = QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex)
		? QuickWeaponSlots[ActiveQuickWeaponSlotIndex]
		: nullptr;
	const bool bSuppressedByTwoHanded = IsTwoHandedWeaponItem(ActiveMainHandItem);

	if (ActiveOffHandItem && ActiveOffHandItem != Item)
	{
		RemoveOffHandWeapon_Server(false);
	}

	ActiveOffHandItem = Item;
	bOffHandSuppressedByTwoHandedWeapon = bSuppressedByTwoHanded;
	SetNarrativeItemActiveState(Item, !bSuppressedByTwoHanded);
	GetOrSpawnWeaponActorForItem(Item);
	RefreshOffHandVisual_Server(!bSuppressedByTwoHanded);
	if (bSuppressedByTwoHanded)
	{
		RemoveGrantedAbilitiesForSlot(241);
	}
	else
	{
		ApplyGrantedAbilitiesForSlot(Item, 241);
	}
	RefreshEquippedOffHandWeaponReference();
	BroadcastOffHandWeaponChangedIfNeeded();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
	return true;
}

bool USpellRiseEquipmentManagerComponent::AssignQuickWeaponSlot_Server(UEquippableItem* Item, int32 QuickSlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item || !IsWeaponItem(Item) || !QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("AssignQuickWeaponSlot abortado. Owner=%s Item=%s Slot=%d HasAuthority=%s IsWeapon=%s SlotValid=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			(GetOwner() && GetOwner()->HasAuthority()) ? TEXT("true") : TEXT("false"),
			IsWeaponItem(Item) ? TEXT("true") : TEXT("false"),
			QuickWeaponSlots.IsValidIndex(QuickSlotIndex) ? TEXT("true") : TEXT("false"));
		return false;
	}

	if (IsOffHandWeaponItem(Item))
	{
		return HandleOffHandEquipIntent(Item);
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("AssignQuickWeaponSlot rejeitado por ownership. Owner=%s Item=%s Slot=%d Reason=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			*ValidationReason);
		return false;
	}

	const int32 ExistingSlot = FindQuickSlotByItem(Item);
	const bool bMovedFromActiveSlot = (ExistingSlot != INDEX_NONE && ExistingSlot != QuickSlotIndex && ActiveQuickWeaponSlotIndex == ExistingSlot);
	const bool bReplacingActiveSlot = (QuickWeaponSlots[QuickSlotIndex] && QuickWeaponSlots[QuickSlotIndex] != Item && ActiveQuickWeaponSlotIndex == QuickSlotIndex);
	const bool bShouldKeepAssignedSlotActive = (ActiveQuickWeaponSlotIndex == INDEX_NONE) || bMovedFromActiveSlot || bReplacingActiveSlot;
	if (ExistingSlot != INDEX_NONE && ExistingSlot != QuickSlotIndex)
	{
		if (UEquippableItem* ExistingItem = QuickWeaponSlots[ExistingSlot])
		{
			SetNarrativeItemActiveState(ExistingItem, false);
		}
		QuickWeaponSlots[ExistingSlot] = nullptr;
		RemoveGrantedAbilitiesForSlot(static_cast<uint8>(200 + ExistingSlot));
		if (bMovedFromActiveSlot)
		{
			ActiveQuickWeaponSlotIndex = INDEX_NONE;
		}
	}

	if (QuickWeaponSlots[QuickSlotIndex] && QuickWeaponSlots[QuickSlotIndex] != Item)
	{
		UEquippableItem* ReplacedItem = QuickWeaponSlots[QuickSlotIndex];
		QuickWeaponSlots[QuickSlotIndex] = nullptr;
		SetNarrativeItemActiveState(ReplacedItem, false);
		RemoveGrantedAbilitiesForSlot(static_cast<uint8>(200 + QuickSlotIndex));
		if (bReplacingActiveSlot)
		{
			ActiveQuickWeaponSlotIndex = INDEX_NONE;
		}
		DestroyWeaponActorForItem(ReplacedItem);
	}

	QuickWeaponSlots[QuickSlotIndex] = Item;
	SetNarrativeItemActiveState(Item, true);
	AActor* SpawnedWeaponActor = GetOrSpawnWeaponActorForItem(Item);

	if (bShouldKeepAssignedSlotActive)
	{
		ActiveQuickWeaponSlotIndex = QuickSlotIndex;
		RefreshQuickSlotVisual_Server(QuickSlotIndex, true);
		ApplyGrantedAbilitiesForSlot(Item, static_cast<uint8>(200 + QuickSlotIndex));
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("AssignQuickWeaponSlot item ficou ativo. Owner=%s Item=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex);
	}
	else
	{
		RefreshQuickSlotVisual_Server(QuickSlotIndex, false);
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("AssignQuickWeaponSlot item entrou stowed. Owner=%s Item=%s Slot=%d ActiveQuickSlot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			ActiveQuickWeaponSlotIndex);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	RefreshEquippedWeaponReference();
	BroadcastWeaponChangedIfNeeded();
	RefreshOffHandSuppression_Server();
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("AssignQuickWeaponSlot concluido. Owner=%s Item=%s Slot=%d ActiveQuickSlot=%d SpawnedWeapon=%s Slot0=%s Slot1=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		QuickSlotIndex,
		ActiveQuickWeaponSlotIndex,
		*GetNameSafe(SpawnedWeaponActor),
		*GetNameSafe(GetQuickWeaponItemByIndex(0)),
		*GetNameSafe(GetQuickWeaponItemByIndex(1)));

	return true;
}

bool USpellRiseEquipmentManagerComponent::ActivateQuickWeaponSlot_Server(int32 QuickSlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ActivateQuickWeaponSlot abortado. Owner=%s Slot=%d HasAuthority=%s SlotValid=%s"),
			*GetNameSafe(GetOwner()),
			QuickSlotIndex,
			(GetOwner() && GetOwner()->HasAuthority()) ? TEXT("true") : TEXT("false"),
			QuickWeaponSlots.IsValidIndex(QuickSlotIndex) ? TEXT("true") : TEXT("false"));
		return false;
	}

	UEquippableItem* ItemToActivate = QuickWeaponSlots[QuickSlotIndex];
	if (!ItemToActivate)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ActivateQuickWeaponSlot abortado: slot vazio. Owner=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			QuickSlotIndex);
		return false;
	}

	if (ActiveQuickWeaponSlotIndex == QuickSlotIndex)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("ActivateQuickWeaponSlot ignorado: slot ja ativo. Owner=%s Slot=%d Item=%s"),
			*GetNameSafe(GetOwner()),
			QuickSlotIndex,
			*GetNameSafe(ItemToActivate));
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
	RefreshEquippedWeaponReference();
	BroadcastWeaponChangedIfNeeded();
	RefreshOffHandSuppression_Server();
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("ActivateQuickWeaponSlot concluido. Owner=%s Slot=%d Item=%s EquippedWeapon=%s"),
		*GetNameSafe(GetOwner()),
		QuickSlotIndex,
		*GetNameSafe(ItemToActivate),
		*GetNameSafe(EquippedWeapon));

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

	CleanupOrphanedWeaponActors_Server();
	RefreshEquippedWeaponReference();
	BroadcastWeaponChangedIfNeeded();
	RefreshOffHandSuppression_Server();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::RemoveOffHandWeapon_Server(bool bDestroyWeaponActor)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ActiveOffHandItem)
	{
		return;
	}

	UEquippableItem* RemovedItem = ActiveOffHandItem;
	RefreshOffHandVisual_Server(false);
	SetNarrativeItemActiveState(RemovedItem, false);
	RemoveGrantedAbilitiesForSlot(241);
	ActiveOffHandItem = nullptr;
	bOffHandSuppressedByTwoHandedWeapon = false;

	if (bDestroyWeaponActor)
	{
		DestroyWeaponActorForItem(RemovedItem);
	}

	RefreshEquippedOffHandWeaponReference();
	BroadcastOffHandWeaponChangedIfNeeded();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

bool USpellRiseEquipmentManagerComponent::DropItem_Server(UNarrativeItem* Item, int32 QuantityToDrop)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority() || !Item)
	{
		return false;
	}

	UNarrativeInventoryComponent* OwnerInventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(OwnerActor);
	if (!OwnerInventory || Item->OwningInventory != OwnerInventory)
	{
		return false;
	}

	if (!Item->CanBeRemoved())
	{
		return false;
	}

	const int32 StackQuantity = FMath::Max(1, Item->GetQuantity());
	const int32 SafeDropQuantity = FMath::Clamp(QuantityToDrop, 1, StackQuantity);

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	const FVector Origin = OwnerPawn ? OwnerPawn->GetActorLocation() : OwnerActor->GetActorLocation();
	const FVector Forward = OwnerPawn ? OwnerPawn->GetActorForwardVector() : OwnerActor->GetActorForwardVector();
	const FVector SpawnLocation = Origin + Forward * DropSpawnForwardDistance;
	const FRotator SpawnRotation = OwnerPawn ? OwnerPawn->GetActorRotation() : OwnerActor->GetActorRotation();
	const TSubclassOf<UNarrativeItem> ItemClass = Item->GetClass();


	if (Cast<UEquippableItem>(Item))
	{
		if (SafeDropQuantity < StackQuantity)
		{
			return false;
		}
	}


	UClass* DropPickupClass = DropPickupActorClass.LoadSynchronous();
	if (!DropPickupClass)
	{

		DropPickupClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/NarrativeInventory/Blueprints/BP_BasicItemPickup.BP_BasicItemPickup_C"))).LoadSynchronous();
	}
	if (!DropPickupClass)
	{

		DropPickupClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/Game/UI/InventorySystem/BP_BasicItemPickup.BP_BasicItemPickup_C"))).LoadSynchronous();
	}
	if (!DropPickupClass)
	{
		return false;
	}

	const bool bRemovingEntireStack = (SafeDropQuantity >= StackQuantity);
	bool bRemoved = false;
	if (bRemovingEntireStack)
	{
		bRemoved = OwnerInventory->RemoveItem(Item);
	}
	else
	{
		Item->SetQuantity(StackQuantity - SafeDropQuantity);
		bRemoved = true;
	}

	if (!bRemoved)
	{
		return false;
	}

	if (!SpawnPickupActorForDroppedItem_Server(ItemClass, SafeDropQuantity, SpawnLocation, SpawnRotation))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Error,
			TEXT("DropItem_Server falhou ao spawnar pickup. Owner=%s ItemClass=%s Quantity=%d"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(ItemClass),
			SafeDropQuantity);

		if (bRemovingEntireStack)
		{
			const FItemAddResult RestoreResult = OwnerInventory->TryAddItemFromClass(ItemClass, SafeDropQuantity, false);
			if (RestoreResult.AmountGiven != SafeDropQuantity)
			{
				UE_LOG(LogSpellRiseEquipmentTrace, Error,
					TEXT("Rollback de drop falhou parcialmente. Owner=%s ItemClass=%s Esperado=%d Restaurado=%d"),
					*GetNameSafe(OwnerActor),
					*GetNameSafe(ItemClass),
					SafeDropQuantity,
					RestoreResult.AmountGiven);
			}
		}
		else
		{
			Item->SetQuantity(StackQuantity);
		}

		if (UEquippableItem* EquippableItem = Cast<UEquippableItem>(Item))
		{
			if (FindQuickSlotByItem(EquippableItem) == INDEX_NONE)
			{
				SetNarrativeItemActiveState(EquippableItem, false);
			}
		}

		return false;
	}

	return true;
}

bool USpellRiseEquipmentManagerComponent::SpawnPickupActorForDroppedItem_Server(TSubclassOf<UNarrativeItem> ItemClass, int32 QuantityToDrop, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ItemClass || QuantityToDrop <= 0)
	{
		return false;
	}

	UClass* DropPickupClass = DropPickupActorClass.LoadSynchronous();
	if (!DropPickupClass)
	{
		DropPickupClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/NarrativeInventory/Blueprints/BP_BasicItemPickup.BP_BasicItemPickup_C"))).LoadSynchronous();
	}
	if (!DropPickupClass)
	{
		DropPickupClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/Game/UI/InventorySystem/BP_BasicItemPickup.BP_BasicItemPickup_C"))).LoadSynchronous();
	}
	if (!DropPickupClass)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	AActor* PickupActor = World->SpawnActorDeferred<AActor>(
		DropPickupClass,
		SpawnTransform,
		GetOwner(),
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn,
		ESpawnActorScaleMethod::OverrideRootScale);
	if (!PickupActor)
	{
		return false;
	}


	auto TrySetItemClassPropertyByName = [&](const FName PropertyName) -> bool
	{
		if (FProperty* ItemClassProperty = PickupActor->GetClass()->FindPropertyByName(PropertyName))
		{
			if (FClassProperty* ClassProperty = CastField<FClassProperty>(ItemClassProperty))
			{
				void* ValuePtr = ClassProperty->ContainerPtrToValuePtr<void>(PickupActor);
				ClassProperty->SetPropertyValue(ValuePtr, *ItemClass);
				return true;
			}

			if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(ItemClassProperty))
			{
				void* ValuePtr = ObjectProperty->ContainerPtrToValuePtr<void>(PickupActor);
				ObjectProperty->SetObjectPropertyValue(ValuePtr, *ItemClass);
				return true;
			}
		}

		return false;
	};

	auto TrySetItemClassPropertyByType = [&]() -> bool
	{
		for (TFieldIterator<FProperty> It(PickupActor->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
		{
			FProperty* Property = *It;
			if (FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
			{
				UClass* MetaClass = ClassProperty->MetaClass;
				if (MetaClass && MetaClass->IsChildOf(UNarrativeItem::StaticClass()))
				{
					void* ValuePtr = ClassProperty->ContainerPtrToValuePtr<void>(PickupActor);
					ClassProperty->SetPropertyValue(ValuePtr, *ItemClass);
					return true;
				}
			}
		}

		return false;
	};

	auto TrySetQuantityProperty = [&](const FName PropertyName) -> bool
	{
		if (FProperty* QuantityProperty = PickupActor->GetClass()->FindPropertyByName(PropertyName))
		{
			if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(QuantityProperty))
			{
				void* ValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(PickupActor);
				NumericProperty->SetIntPropertyValue(ValuePtr, static_cast<int64>(QuantityToDrop));
				return true;
			}
		}

		return false;
	};

	auto TrySetQuantityPropertyByType = [&]() -> bool
	{
		for (TFieldIterator<FProperty> It(PickupActor->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
		{
			FProperty* Property = *It;
			if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
			{
				const FString PropertyName = Property->GetName();
				if (PropertyName.Contains(TEXT("Quantity"), ESearchCase::IgnoreCase))
				{
					void* ValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(PickupActor);
					NumericProperty->SetIntPropertyValue(ValuePtr, static_cast<int64>(QuantityToDrop));
					return true;
				}
			}
		}

		return false;
	};

	const bool bSetItemClass =
		TrySetItemClassPropertyByName(TEXT("ItemClass")) ||
		TrySetItemClassPropertyByName(TEXT("Item Class")) ||
		TrySetItemClassPropertyByName(TEXT("Class")) ||
		TrySetItemClassPropertyByType();

	const bool bSetQuantity =
		TrySetQuantityProperty(TEXT("QuantityToGive")) ||
		TrySetQuantityProperty(TEXT("Quantity to Give")) ||
		TrySetQuantityProperty(TEXT("Quantity")) ||
		TrySetQuantityPropertyByType();


	if (UFunction* InitializeFunction = PickupActor->FindFunction(TEXT("Initialize")))
	{
		uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(InitializeFunction->ParmsSize));
		FMemory::Memzero(ParamsBuffer, InitializeFunction->ParmsSize);

		if (FProperty* ClassParam = InitializeFunction->FindPropertyByName(TEXT("Class")))
		{
			if (FClassProperty* ClassProperty = CastField<FClassProperty>(ClassParam))
			{
				void* ValuePtr = ClassProperty->ContainerPtrToValuePtr<void>(ParamsBuffer);
				ClassProperty->SetPropertyValue(ValuePtr, *ItemClass);
			}
			else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(ClassParam))
			{
				void* ValuePtr = ObjectProperty->ContainerPtrToValuePtr<void>(ParamsBuffer);
				ObjectProperty->SetObjectPropertyValue(ValuePtr, *ItemClass);
			}
		}
		else
		{
			for (TFieldIterator<FProperty> It(InitializeFunction); It && (It->PropertyFlags & CPF_Parm); ++It)
			{
				if (FClassProperty* ClassProperty = CastField<FClassProperty>(*It))
				{
					UClass* MetaClass = ClassProperty->MetaClass;
					if (MetaClass && MetaClass->IsChildOf(UNarrativeItem::StaticClass()))
					{
						void* ValuePtr = ClassProperty->ContainerPtrToValuePtr<void>(ParamsBuffer);
						ClassProperty->SetPropertyValue(ValuePtr, *ItemClass);
						break;
					}
				}
			}
		}

		if (FProperty* QuantityParam = InitializeFunction->FindPropertyByName(TEXT("Quantity")))
		{
			if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(QuantityParam))
			{
				void* ValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(ParamsBuffer);
				NumericProperty->SetIntPropertyValue(ValuePtr, static_cast<int64>(QuantityToDrop));
			}
		}
		else
		{
			for (TFieldIterator<FProperty> It(InitializeFunction); It && (It->PropertyFlags & CPF_Parm); ++It)
			{
				if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(*It))
				{
					void* ValuePtr = NumericProperty->ContainerPtrToValuePtr<void>(ParamsBuffer);
					NumericProperty->SetIntPropertyValue(ValuePtr, static_cast<int64>(QuantityToDrop));
					break;
				}
			}
		}

		PickupActor->ProcessEvent(InitializeFunction, ParamsBuffer);
	}

	UGameplayStatics::FinishSpawningActor(PickupActor, SpawnTransform);

	return true;
}

AActor* USpellRiseEquipmentManagerComponent::GetOrSpawnWeaponActorForItem(UEquippableItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("GetOrSpawnWeaponActorForItem abortado. Owner=%s Item=%s HasAuthority=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			(GetOwner() && GetOwner()->HasAuthority()) ? TEXT("true") : TEXT("false"));
		return nullptr;
	}

	if (TObjectPtr<AActor>* ExistingActor = SpawnedWeaponActorsByItem.Find(Item))
	{
		if (IsValid(*ExistingActor))
		{
			UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
				TEXT("GetOrSpawnWeaponActorForItem reutilizou actor existente. Item=%s WeaponActor=%s"),
				*GetNameSafe(Item),
				*GetNameSafe(*ExistingActor));
			return *ExistingActor;
		}
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponActorClass)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("GetOrSpawnWeaponActorForItem falhou ao resolver WeaponClass. Item=%s"),
			*GetNameSafe(Item));
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
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("GetOrSpawnWeaponActorForItem falhou no SpawnActor. Item=%s WeaponClass=%s"),
			*GetNameSafe(Item),
			*GetNameSafe(WeaponActorClass));
		return nullptr;
	}

	SpawnedWeaponActorsByItem.Add(Item, SpawnedWeapon);
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("GetOrSpawnWeaponActorForItem spawnou arma. Item=%s WeaponActor=%s WeaponClass=%s"),
		*GetNameSafe(Item),
		*GetNameSafe(SpawnedWeapon),
		*GetNameSafe(WeaponActorClass));
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

void USpellRiseEquipmentManagerComponent::RefreshOffHandVisual_Local(bool bEquipped)
{
	UEquippableItem* Item = ActiveOffHandItem;
	if (!Item)
	{
		return;
	}

	FName EquippedSocket = NAME_None;
	FName HolsterSocket = NAME_None;
	if (!ResolveWeaponSocketsForItem(Item, true, EquippedSocket, HolsterSocket))
	{
		return;
	}

	const FName TargetSocket = bEquipped ? EquippedSocket : HolsterSocket;
	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh();

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponActorClass)
	{
		return;
	}

	AActor* WeaponActor = ResolveOwnedWeaponActor(WeaponActorClass);
	if (!AttachMesh || !WeaponActor || TargetSocket == NAME_None)
	{
		return;
	}

	AttachWeaponActorToSocket(WeaponActor, AttachMesh, TargetSocket);
}

void USpellRiseEquipmentManagerComponent::RefreshOffHandVisual_Server(bool bEquipped)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ActiveOffHandItem)
	{
		return;
	}

	AActor* WeaponActor = GetOrSpawnWeaponActorForItem(ActiveOffHandItem);
	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh();
	FName EquippedSocket = NAME_None;
	FName HolsterSocket = NAME_None;
	if (!WeaponActor || !AttachMesh || !ResolveWeaponSocketsForItem(ActiveOffHandItem, true, EquippedSocket, HolsterSocket))
	{
		return;
	}

	const FName TargetSocket = bEquipped ? EquippedSocket : HolsterSocket;
	if (TargetSocket != NAME_None)
	{
		AttachWeaponActorToSocket(WeaponActor, AttachMesh, TargetSocket);
	}
}

void USpellRiseEquipmentManagerComponent::RefreshQuickSlotVisual_Server(int32 QuickSlotIndex, bool bEquipped)
{
	if (!QuickWeaponSlots.IsValidIndex(QuickSlotIndex))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("RefreshQuickSlotVisual abortado: slot invalido. Owner=%s Slot=%d Equipped=%s"),
			*GetNameSafe(GetOwner()),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"));
		return;
	}

	UEquippableItem* Item = QuickWeaponSlots[QuickSlotIndex];
	if (!Item)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("RefreshQuickSlotVisual abortado: slot vazio. Owner=%s Slot=%d Equipped=%s"),
			*GetNameSafe(GetOwner()),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"));
		return;
	}

	AActor* WeaponActor = GetOrSpawnWeaponActorForItem(Item);
	if (!WeaponActor)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("RefreshQuickSlotVisual abortado: sem WeaponActor. Owner=%s Item=%s Slot=%d Equipped=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"));
		return;
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("RefreshQuickSlotVisual abortado: falha ao resolver weapon class/config. Owner=%s Item=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex);
		return;
	}

	FName EquippedSocket = NAME_None;
	FName HolsterSocket = NAME_None;
	ResolveWeaponSocketsForItem(Item, false, EquippedSocket, HolsterSocket);
	const FName TargetSocket = bEquipped ? EquippedSocket : HolsterSocket;
	if (TargetSocket == NAME_None)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("RefreshQuickSlotVisual abortado: TargetSocket none. Owner=%s Item=%s Slot=%d Equipped=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"));
		return;
	}

	if (USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh())
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("RefreshQuickSlotVisual attachando. Owner=%s Item=%s WeaponActor=%s Slot=%d Equipped=%s Socket=%s Mesh=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			*GetNameSafe(WeaponActor),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"),
			*TargetSocket.ToString(),
			*GetNameSafe(AttachMesh));
		AttachWeaponActorToSocket(WeaponActor, AttachMesh, TargetSocket);
	}
	else
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("RefreshQuickSlotVisual falhou: mesh de attach nula. Owner=%s Item=%s WeaponActor=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			*GetNameSafe(WeaponActor),
			QuickSlotIndex);
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

	if (RemovedEquippableItem == ActiveOffHandItem)
	{
		RemoveOffHandWeapon_Server(true);
		return;
	}


	DestroyWeaponActorForItem(RemovedEquippableItem);



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



	const UClass* RemovedItemClass = RemovedEquippableItem->GetClass();
	for (auto It = SpawnedWeaponActorsByItem.CreateIterator(); It; ++It)
	{
		UEquippableItem* MappedItem = It.Key().Get();
		AActor* MappedActor = It.Value().Get();
		if (!MappedItem || MappedItem->GetClass() != RemovedItemClass)
		{
			continue;
		}

		bool bStillInQuickSlots = false;
		for (UEquippableItem* SlotItem : QuickWeaponSlots)
		{
			if (SlotItem == MappedItem)
			{
				bStillInQuickSlots = true;
				break;
			}
		}
		if (MappedItem == ActiveOffHandItem)
		{
			bStillInQuickSlots = true;
		}

		if (!bStillInQuickSlots)
		{
			if (IsValid(MappedActor))
			{
				MappedActor->Destroy();
			}
			It.RemoveCurrent();
		}
	}

	CleanupOrphanedWeaponActors_Server();
}

void USpellRiseEquipmentManagerComponent::OnRep_QuickWeaponSlots()
{
	ReconcileReplicatedQuickSlotVisuals();
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::OnRep_ActiveQuickSlotIndex()
{
	RefreshEquippedWeaponReference();
	ReconcileReplicatedQuickSlotVisuals();
	if (!QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex) || !QuickWeaponSlots[ActiveQuickWeaponSlotIndex])
	{
		BroadcastWeaponChangedIfNeeded();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::OnRep_EquippedWeapon()
{
	ReconcileReplicatedQuickSlotVisuals();
	BroadcastWeaponChangedIfNeeded();
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::OnRep_ActiveOffHandItem()
{
	if (!ActiveOffHandItem)
	{
		EquippedOffHandWeapon = nullptr;
	}
	RefreshOffHandVisual_Local(IsOffHandGameplayActive());
	if (!ActiveOffHandItem)
	{
		BroadcastOffHandWeaponChangedIfNeeded();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::OnRep_OffHandSuppressedByTwoHandedWeapon()
{
	RefreshOffHandVisual_Local(IsOffHandGameplayActive());
	BroadcastOffHandWeaponChangedIfNeeded();
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::OnRep_EquippedOffHandWeapon()
{
	RefreshOffHandVisual_Local(IsOffHandGameplayActive());
	BroadcastOffHandWeaponChangedIfNeeded();
	OnQuickWeaponSlotsChanged.Broadcast();
	OnHUDEquipmentSlotsChanged.Broadcast();
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
		if (MappedItem == ActiveOffHandItem)
		{
			bItemStillInQuickSlots = true;
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
