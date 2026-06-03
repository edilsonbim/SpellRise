#include "SpellRise/Equipment/SpellRiseWeaponComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "EquippableItem.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/UnrealType.h"

#include "SpellRise/Equipment/SpellRiseWeaponBase.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseWeaponComponent, Log, All);

namespace SpellRiseWeaponComponentPrivate
{
	constexpr uint8 MainHandSlotValue = 100;
	constexpr uint8 OffHandSlotValue = 110;

	static bool IsFiniteMagnitude(float Value)
	{
		return FMath::IsFinite(Value);
	}

	static const FProperty* FindPropertyByAuthoredName(const UStruct* Struct, const TArray<FName>& PropertyNames)
	{
		if (!Struct)
		{
			return nullptr;
		}

		for (const FName PropertyName : PropertyNames)
		{
			if (const FProperty* ExactProperty = Struct->FindPropertyByName(PropertyName))
			{
				return ExactProperty;
			}
		}

		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			const FProperty* Property = *It;
			if (!Property)
			{
				continue;
			}

			const FString InternalName = Property->GetName();
			const FString DisplayName = Property->GetDisplayNameText().ToString();
			for (const FName PropertyName : PropertyNames)
			{
				const FString AuthoredName = PropertyName.ToString();
				if (InternalName == AuthoredName || DisplayName == AuthoredName || InternalName.StartsWith(AuthoredName + TEXT("_")))
				{
					return Property;
				}
			}
		}

		return nullptr;
	}

	static bool ReadNameProperty(const void* ContainerPtr, const UStruct* Struct, const TArray<FName>& PropertyNames, FName& OutValue)
	{
		if (!ContainerPtr || !Struct)
		{
			return false;
		}

		if (const FNameProperty* NameProperty = CastField<FNameProperty>(FindPropertyByAuthoredName(Struct, PropertyNames)))
		{
			const FName Value = NameProperty->GetPropertyValue_InContainer(ContainerPtr);
			if (Value != NAME_None)
			{
				OutValue = Value;
				return true;
			}
		}

		return false;
	}

	static UObject* ReadObjectLikeProperty(const void* ContainerPtr, const FProperty* Property)
	{
		if (!ContainerPtr || !Property)
		{
			return nullptr;
		}

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ContainerPtr);
		if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			return ObjectProperty->GetObjectPropertyValue(ValuePtr);
		}

		if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
		{
			FSoftObjectPtr SoftObjectPtr = SoftObjectProperty->GetPropertyValue(ValuePtr);
			UObject* ResolvedObject = SoftObjectPtr.Get();
			return ResolvedObject ? ResolvedObject : SoftObjectPtr.LoadSynchronous();
		}

		return nullptr;
	}

	static UObject* ReadObjectLikeProperty(const void* ContainerPtr, const UStruct* Struct, const TArray<FName>& PropertyNames)
	{
		return ReadObjectLikeProperty(ContainerPtr, FindPropertyByAuthoredName(Struct, PropertyNames));
	}

	static UClass* ReadClassLikeProperty(const void* ContainerPtr, const FProperty* Property, const UClass* RequiredParentClass)
	{
		if (!ContainerPtr || !Property)
		{
			return nullptr;
		}

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ContainerPtr);
		UClass* ResolvedClass = nullptr;
		if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
		{
			ResolvedClass = Cast<UClass>(ClassProperty->GetObjectPropertyValue(ValuePtr));
		}
		else if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
		{
			FSoftObjectPtr SoftClassPtr = SoftClassProperty->GetPropertyValue(ValuePtr);
			UObject* ResolvedObject = SoftClassPtr.Get();
			ResolvedClass = Cast<UClass>(ResolvedObject ? ResolvedObject : SoftClassPtr.LoadSynchronous());
		}

		return (ResolvedClass && (!RequiredParentClass || ResolvedClass->IsChildOf(RequiredParentClass))) ? ResolvedClass : nullptr;
	}

	static const void* ResolveWeaponConfigPtr(const UClass* WeaponActorClass, const UStruct*& OutConfigStruct)
	{
		OutConfigStruct = nullptr;
		const UObject* CDO = WeaponActorClass ? WeaponActorClass->GetDefaultObject() : nullptr;
		if (!CDO)
		{
			return nullptr;
		}

		const FStructProperty* WeaponConfigProperty = CastField<FStructProperty>(WeaponActorClass->FindPropertyByName(TEXT("WeaponConfig")));
		if (!WeaponConfigProperty)
		{
			return nullptr;
		}

		OutConfigStruct = WeaponConfigProperty->Struct;
		return WeaponConfigProperty->ContainerPtrToValuePtr<void>(CDO);
	}
}

USpellRiseWeaponComponent::USpellRiseWeaponComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	DefaultWeaponActorClass = TSoftClassPtr<AActor>(FSoftObjectPath(TEXT("/Game/Combat/Weapons/Blueprint/BP_Weapon_Base.BP_Weapon_Base_C")));
}

void USpellRiseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	QuickWeaponSlots.SetNum(FMath::Max(QuickSlotCount, 1));
	BindAnimNotifyDelegate();
}

void USpellRiseWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAnimNotifyDelegate();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AnimNotifyFallbackTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void USpellRiseWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
	DOREPLIFETIME(ThisClass, ActiveWeaponItem);
	DOREPLIFETIME(ThisClass, ActiveQuickSlotIndex);
	DOREPLIFETIME(ThisClass, QuickWeaponSlots);
	DOREPLIFETIME(ThisClass, bWeaponDrawn);
	DOREPLIFETIME(ThisClass, OffHandWeapon);
}

bool USpellRiseWeaponComponent::EquipWeapon(UEquippableItem* Item)
{
	if (!Item)
	{
		return false;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		return EquipWeapon_Server(Item);
	}

	ServerEquipWeapon(Item);
	return true;
}

bool USpellRiseWeaponComponent::EquipWeaponClass(TSubclassOf<AActor> WeaponActorClass, int32 SlotIndex, bool bDrawImmediately)
{
	if (!WeaponActorClass)
	{
		return false;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipClassRejected] Reason=not_authority Owner=%s WeaponClass=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActorClass.Get()));
		return false;
	}

	return EquipWeaponClass_Server(WeaponActorClass, SlotIndex, bDrawImmediately);
}

bool USpellRiseWeaponComponent::UnequipWeapon()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		return UnequipWeapon_Server(ActiveWeaponItem);
	}

	ServerUnequipWeapon();
	return true;
}

bool USpellRiseWeaponComponent::UnequipWeaponItem(UEquippableItem* Item)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		return UnequipWeapon_Server(Item);
	}

	ServerUnequipWeaponItem(Item);
	return true;
}

bool USpellRiseWeaponComponent::ActivateQuickSlot(int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		return ActivateQuickSlot_Server(SlotIndex);
	}

	ServerActivateQuickSlot(SlotIndex);
	return true;
}

bool USpellRiseWeaponComponent::AssignQuickSlot(UEquippableItem* Item, int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		return AssignQuickSlot_Server(Item, SlotIndex);
	}

	ServerAssignQuickSlot(Item, SlotIndex);
	return true;
}

void USpellRiseWeaponComponent::HandleWeaponAnimNotify(FName NotifyName)
{
	const FString ReceivedNotifyName = NotifyName.ToString();
	if (ReceivedNotifyName.Equals(EquipNotifyName.ToString(), ESearchCase::IgnoreCase))
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			CompleteWeaponDrawTransition_Server(true);
		}
		return;
	}

	if (ReceivedNotifyName.Equals(UnequipNotifyName.ToString(), ESearchCase::IgnoreCase))
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			CompleteWeaponDrawTransition_Server(false);
		}
	}
}

bool USpellRiseWeaponComponent::IsWeaponItem(UEquippableItem* Item) const
{
	if (!Item)
	{
		return false;
	}

	USpellRiseWeaponDefinition* Definition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, Definition) && Definition)
	{
		return true;
	}

	static const TArray<FName> WeaponPropertyNames = {
		TEXT("WeaponDefinition"),
		TEXT("WeaponClass"),
		TEXT("WeaponActorClass"),
		TEXT("WeaponActor"),
		TEXT("Weapon")
	};

	for (const FName PropertyName : WeaponPropertyNames)
	{
		if (Item->GetClass()->FindPropertyByName(PropertyName))
		{
			return true;
		}
	}

	return false;
}

bool USpellRiseWeaponComponent::IsOffHandWeaponItem(UEquippableItem* Item) const
{
	USpellRiseWeaponDefinition* Definition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, Definition) && Definition)
	{
		return Definition->HandPolicy == ESpellRiseWeaponHandPolicy::OffHand;
	}
	return false;
}

bool USpellRiseWeaponComponent::IsTwoHandedWeaponItem(UEquippableItem* Item) const
{
	USpellRiseWeaponDefinition* Definition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, Definition) && Definition)
	{
		return Definition->HandPolicy == ESpellRiseWeaponHandPolicy::TwoHanded;
	}
	return false;
}

ASpellRiseWeaponBase* USpellRiseWeaponComponent::GetEquippedWeaponBase() const
{
	return Cast<ASpellRiseWeaponBase>(EquippedWeapon);
}

bool USpellRiseWeaponComponent::GetActiveEquippedWeaponSpawnPointTransform(FTransform& OutTransform) const
{
	if (!bWeaponDrawn || !EquippedWeapon)
	{
		return false;
	}

	if (const USceneComponent* SpawnPoint = ResolveWeaponSpawnPoint(EquippedWeapon))
	{
		OutTransform = SpawnPoint->GetComponentTransform();
		return true;
	}

	OutTransform = EquippedWeapon->GetActorTransform();
	return true;
}

void USpellRiseWeaponComponent::ServerEquipWeapon_Implementation(UEquippableItem* Item)
{
	FString RejectReason;
	if (!CheckServerWeaponRpcRateLimit(TEXT("ServerEquipWeapon"), EquipRpcRateState, RejectReason))
	{
		AuditRejectedWeaponRpc(TEXT("ServerEquipWeapon"), RejectReason);
		return;
	}

	EquipWeapon_Server(Item);
}

void USpellRiseWeaponComponent::ServerUnequipWeapon_Implementation()
{
	FString RejectReason;
	if (!CheckServerWeaponRpcRateLimit(TEXT("ServerUnequipWeapon"), UnequipRpcRateState, RejectReason))
	{
		AuditRejectedWeaponRpc(TEXT("ServerUnequipWeapon"), RejectReason);
		return;
	}

	UnequipWeapon_Server(ActiveWeaponItem);
}

void USpellRiseWeaponComponent::ServerUnequipWeaponItem_Implementation(UEquippableItem* Item)
{
	FString RejectReason;
	if (!CheckServerWeaponRpcRateLimit(TEXT("ServerUnequipWeaponItem"), UnequipRpcRateState, RejectReason))
	{
		AuditRejectedWeaponRpc(TEXT("ServerUnequipWeaponItem"), RejectReason);
		return;
	}

	UnequipWeapon_Server(Item);
}

void USpellRiseWeaponComponent::ServerActivateQuickSlot_Implementation(int32 SlotIndex)
{
	FString RejectReason;
	if (!CheckServerWeaponRpcRateLimit(TEXT("ServerActivateQuickSlot"), ActivateQuickSlotRpcRateState, RejectReason))
	{
		AuditRejectedWeaponRpc(TEXT("ServerActivateQuickSlot"), RejectReason);
		return;
	}

	ActivateQuickSlot_Server(SlotIndex);
}

void USpellRiseWeaponComponent::ServerAssignQuickSlot_Implementation(UEquippableItem* Item, int32 SlotIndex)
{
	FString RejectReason;
	if (!CheckServerWeaponRpcRateLimit(TEXT("ServerAssignQuickSlot"), AssignQuickSlotRpcRateState, RejectReason))
	{
		AuditRejectedWeaponRpc(TEXT("ServerAssignQuickSlot"), RejectReason);
		return;
	}

	AssignQuickSlot_Server(Item, SlotIndex);
}

void USpellRiseWeaponComponent::MulticastPlayWeaponTransition_Implementation(UAnimMontage* MontageToPlay, bool bDraw)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		return;
	}

	PlayWeaponTransitionLocally(MontageToPlay, bDraw);
}

void USpellRiseWeaponComponent::HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	HandleWeaponAnimNotify(NotifyName);
}

void USpellRiseWeaponComponent::OnRep_EquippedWeapon()
{
	AttachCurrentWeaponVisual();
	BroadcastWeaponState();
}

void USpellRiseWeaponComponent::OnRep_ActiveWeaponItem()
{
	AttachCurrentWeaponVisual();
	BroadcastWeaponState();
}

void USpellRiseWeaponComponent::OnRep_ActiveQuickSlotIndex()
{
	AttachCurrentWeaponVisual();
	OnQuickSlotsChanged.Broadcast();
}

void USpellRiseWeaponComponent::OnRep_QuickWeaponSlots()
{
	AttachCurrentWeaponVisual();
	OnQuickSlotsChanged.Broadcast();
}

void USpellRiseWeaponComponent::OnRep_WeaponDrawn()
{
	AttachCurrentWeaponVisual();
	ApplyOverlayState();
	OnWeaponDrawStateChanged.Broadcast(bWeaponDrawn);
}

void USpellRiseWeaponComponent::OnRep_OffHandState()
{
	AttachWeaponVisual(OffHandWeapon.WeaponActor, OffHandWeapon.Item, OffHandWeapon.WeaponDefinition, !OffHandWeapon.bIsSuppressed, true);
}

bool USpellRiseWeaponComponent::EquipWeapon_Server(UEquippableItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item || !IsWeaponItem(Item))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipRejected] Reason=invalid_context Owner=%s Item=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item));
		return false;
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipRejected] Reason=%s Owner=%s Item=%s"),
			*ValidationReason,
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item));
		return false;
	}

	if (IsOffHandWeaponItem(Item))
	{
		FSpellRiseWeaponSlotState NewOffHandState;
		if (!BuildWeaponSlotState(Item, NewOffHandState))
		{
			return false;
		}

		NewOffHandState.bIsOffHand = true;
		NewOffHandState.WeaponActor = GetOrSpawnWeaponActorForSlot(NewOffHandState);
		RemoveOffHandWeaponGrants_Server();
		OffHandWeapon = NewOffHandState;
		RefreshOffHandSuppression_Server();
		AttachWeaponVisual(OffHandWeapon.WeaponActor, OffHandWeapon.Item, OffHandWeapon.WeaponDefinition, !OffHandWeapon.bIsSuppressed, true);
		ForceOwnerNetUpdate();
		return true;
	}

	const int32 PreferredSlot = GetPreferredQuickSlotForItem(Item);
	const int32 SlotToUse = PreferredSlot != INDEX_NONE ? PreferredSlot : 0;
	return AssignQuickSlot_Server(Item, SlotToUse);
}

bool USpellRiseWeaponComponent::EquipWeaponClass_Server(TSubclassOf<AActor> WeaponActorClass, int32 SlotIndex, bool bDrawImmediately)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !WeaponActorClass || !QuickWeaponSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipClassRejected] Reason=invalid_context Owner=%s WeaponClass=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActorClass.Get()),
			SlotIndex);
		return false;
	}

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsPlayerControlled() && !bAllowWeaponClassEquipOnPlayerControlledOwner)
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipClassRejected] Reason=player_class_equip_disabled Owner=%s WeaponClass=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActorClass.Get()),
			SlotIndex);
		return false;
	}

	FSpellRiseWeaponSlotState NewSlotState;
	if (!BuildWeaponSlotStateFromClass(WeaponActorClass, NewSlotState))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipClassRejected] Reason=invalid_weapon_class Owner=%s WeaponClass=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActorClass.Get()),
			SlotIndex);
		return false;
	}

	NewSlotState.WeaponActor = GetOrSpawnWeaponActorForSlot(NewSlotState);
	if (!NewSlotState.WeaponActor)
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][EquipClassRejected] Reason=spawn_failed Owner=%s WeaponClass=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActorClass.Get()),
			SlotIndex);
		return false;
	}

	if (QuickWeaponSlots[SlotIndex].WeaponActor)
	{
		DestroyWeaponActor(QuickWeaponSlots[SlotIndex].WeaponActor);
	}

	if (ActiveQuickSlotIndex == SlotIndex)
	{
		RemoveActiveWeaponGrants_Server();
	}

	QuickWeaponSlots[SlotIndex] = NewSlotState;
	ActiveQuickSlotIndex = SlotIndex;
	bWeaponDrawn = bDrawImmediately;
	RefreshEquippedWeaponReference_Server();
	AttachCurrentWeaponVisual();
	if (bWeaponDrawn)
	{
		ApplyActiveWeaponGrants_Server();
	}
	else
	{
		RemoveActiveWeaponGrants_Server();
	}
	RefreshOffHandSuppression_Server();
	ApplyOverlayState();
	BroadcastWeaponState();
	OnQuickSlotsChanged.Broadcast();
	ForceOwnerNetUpdate();
	return true;
}

bool USpellRiseWeaponComponent::UnequipWeapon_Server(UEquippableItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!Item)
	{
		if (bWeaponDrawn)
		{
			return SetWeaponDrawn_Server(false);
		}
		return true;
	}

	if (OffHandWeapon.Item == Item)
	{
		RemoveOffHandWeaponGrants_Server();
		DestroyWeaponActor(OffHandWeapon.WeaponActor);
		OffHandWeapon = FSpellRiseWeaponSlotState();
		ForceOwnerNetUpdate();
		return true;
	}

	const int32 SlotIndex = FindQuickSlotByItem(Item);
	if (!QuickWeaponSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Verbose,
			TEXT("[Weapon][UnequipIgnored] Reason=item_not_in_quick_slots Owner=%s Item=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item));
		return true;
	}

	const bool bWasActive = ActiveQuickSlotIndex == SlotIndex;
	if (bWasActive)
	{
		RemoveActiveWeaponGrants_Server();
		bWeaponDrawn = false;
		ActiveWeaponItem = nullptr;
		EquippedWeapon = nullptr;
		ActiveQuickSlotIndex = INDEX_NONE;
	}

	DestroyWeaponActor(QuickWeaponSlots[SlotIndex].WeaponActor);
	QuickWeaponSlots[SlotIndex] = FSpellRiseWeaponSlotState();
	if (Item->bActive)
	{
		Item->SetActive(false);
	}
	RefreshEquippedWeaponReference_Server();
	BroadcastWeaponState();
	OnQuickSlotsChanged.Broadcast();
	ForceOwnerNetUpdate();
	return true;
}

bool USpellRiseWeaponComponent::AssignQuickSlot_Server(UEquippableItem* Item, int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item || !QuickWeaponSlots.IsValidIndex(SlotIndex) || IsOffHandWeaponItem(Item))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][AssignRejected] Reason=invalid_context Owner=%s Item=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotIndex);
		return false;
	}

	FString ValidationReason;
	if (!ValidateItemOwnership(Item, ValidationReason))
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][AssignRejected] Reason=%s Owner=%s Item=%s Slot=%d"),
			*ValidationReason,
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotIndex);
		return false;
	}

	FSpellRiseWeaponSlotState NewSlotState;
	if (!BuildWeaponSlotState(Item, NewSlotState))
	{
		return false;
	}

	TArray<TObjectPtr<UEquippableItem>> ItemsToDeactivate;
	const int32 ExistingSlot = FindQuickSlotByItem(Item);
	if (QuickWeaponSlots.IsValidIndex(ExistingSlot) && ExistingSlot != SlotIndex)
	{
		if (ExistingSlot == ActiveQuickSlotIndex)
		{
			RemoveActiveWeaponGrants_Server();
			bWeaponDrawn = false;
			ActiveQuickSlotIndex = INDEX_NONE;
			ActiveWeaponItem = nullptr;
			EquippedWeapon = nullptr;
		}
		QuickWeaponSlots[ExistingSlot] = FSpellRiseWeaponSlotState();
	}

	if (QuickWeaponSlots[SlotIndex].Item && QuickWeaponSlots[SlotIndex].Item != Item)
	{
		ItemsToDeactivate.AddUnique(QuickWeaponSlots[SlotIndex].Item);
		if (SlotIndex == ActiveQuickSlotIndex)
		{
			RemoveActiveWeaponGrants_Server();
			bWeaponDrawn = false;
			ActiveQuickSlotIndex = INDEX_NONE;
			ActiveWeaponItem = nullptr;
			EquippedWeapon = nullptr;
		}
		DestroyWeaponActor(QuickWeaponSlots[SlotIndex].WeaponActor);
	}

	NewSlotState.WeaponActor = GetOrSpawnWeaponActorForSlot(NewSlotState);
	QuickWeaponSlots[SlotIndex] = NewSlotState;
	for (UEquippableItem* ReplacedItem : ItemsToDeactivate)
	{
		if (ReplacedItem && ReplacedItem->bActive)
		{
			ReplacedItem->SetActive(false);
		}
	}

	if (ActiveQuickSlotIndex == INDEX_NONE)
	{
		ActiveQuickSlotIndex = SlotIndex;
		RefreshEquippedWeaponReference_Server();
		if (!bWeaponDrawn)
		{
			SetWeaponDrawn_Server(true);
		}
		else
		{
			AttachCurrentWeaponVisual();
		}
	}
	else
	{
		AttachWeaponVisual(NewSlotState.WeaponActor, NewSlotState.Item, NewSlotState.WeaponDefinition, false, false);
	}

	RefreshOffHandSuppression_Server();
	BroadcastWeaponState();
	OnQuickSlotsChanged.Broadcast();
	ForceOwnerNetUpdate();
	UE_LOG(LogSpellRiseWeaponComponent, Log,
		TEXT("[Weapon][AssignQuickSlot] Owner=%s Item=%s Slot=%d ActiveSlot=%d Drawn=%s Weapon=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		SlotIndex,
		ActiveQuickSlotIndex,
		bWeaponDrawn ? TEXT("true") : TEXT("false"),
		*GetNameSafe(NewSlotState.WeaponActor));
	return true;
}

bool USpellRiseWeaponComponent::ActivateQuickSlot_Server(int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !QuickWeaponSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	FSpellRiseWeaponSlotState& TargetSlot = QuickWeaponSlots[SlotIndex];
	if (!TargetSlot.Item && !TargetSlot.WeaponActorClass)
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][ActivateRejected] Reason=empty_slot Owner=%s Slot=%d"),
			*GetNameSafe(GetOwner()),
			SlotIndex);
		return false;
	}

	if (ActiveQuickSlotIndex == SlotIndex)
	{
		return SetWeaponDrawn_Server(!bWeaponDrawn);
	}

	if (QuickWeaponSlots.IsValidIndex(ActiveQuickSlotIndex))
	{
		FSpellRiseWeaponSlotState& PreviousSlot = QuickWeaponSlots[ActiveQuickSlotIndex];
		AttachWeaponVisual(PreviousSlot.WeaponActor, PreviousSlot.Item, PreviousSlot.WeaponDefinition, false, false);
	}

	RemoveActiveWeaponGrants_Server();
	bWeaponDrawn = false;
	ActiveQuickSlotIndex = SlotIndex;
	RefreshEquippedWeaponReference_Server();
	RefreshOffHandSuppression_Server();
	BroadcastWeaponState();
	OnQuickSlotsChanged.Broadcast();
	ForceOwnerNetUpdate();

	return SetWeaponDrawn_Server(true);
}

bool USpellRiseWeaponComponent::SetWeaponDrawn_Server(bool bDraw)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!GetActiveSlotState())
	{
		return false;
	}

	if (bDraw && !EquippedWeapon)
	{
		RefreshEquippedWeaponReference_Server();
	}

	if (bDraw && !EquippedWeapon)
	{
		return false;
	}

	if (bWeaponDrawn == bDraw && !bTransitionInProgress)
	{
		AttachCurrentWeaponVisual();
		return true;
	}

	return StartWeaponDrawTransition_Server(bDraw);
}

bool USpellRiseWeaponComponent::StartWeaponDrawTransition_Server(bool bDraw)
{
	UAnimMontage* EquipMontage = nullptr;
	UAnimMontage* UnequipMontage = nullptr;
	const FSpellRiseWeaponSlotState* ActiveSlot = GetActiveSlotState();
	UClass* WeaponActorClass = nullptr;
	if (ActiveSlot)
	{
		ResolveWeaponMontagesForSlot(*ActiveSlot, EquipMontage, UnequipMontage);
		WeaponActorClass = ActiveSlot->WeaponActorClass;
		if (!WeaponActorClass && ActiveSlot->WeaponActor)
		{
			WeaponActorClass = ActiveSlot->WeaponActor->GetClass();
		}
		if (!WeaponActorClass && ActiveSlot->Item)
		{
			ResolveWeaponActorClassFromItem(ActiveSlot->Item, WeaponActorClass);
		}
	}

	UAnimMontage* MontageToPlay = bDraw ? EquipMontage : UnequipMontage;
	UE_LOG(LogSpellRiseWeaponComponent, Log,
		TEXT("[Weapon][Transition] Owner=%s Draw=%s Slot=%d WeaponClass=%s EquipMontage=%s UnequipMontage=%s SelectedMontage=%s"),
		*GetNameSafe(GetOwner()),
		bDraw ? TEXT("true") : TEXT("false"),
		ActiveQuickSlotIndex,
		*GetNameSafe(WeaponActorClass),
		*GetNameSafe(EquipMontage),
		*GetNameSafe(UnequipMontage),
		*GetNameSafe(MontageToPlay));
	bTransitionInProgress = true;
	bPendingDrawState = bDraw;
	PlayWeaponTransitionLocally(MontageToPlay, bDraw);
	MulticastPlayWeaponTransition(MontageToPlay, bDraw);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AnimNotifyFallbackTimerHandle);
		World->GetTimerManager().SetTimer(
			AnimNotifyFallbackTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, bDraw]()
			{
				if (bTransitionInProgress && bPendingDrawState == bDraw)
				{
					UE_LOG(LogSpellRiseWeaponComponent, Warning,
						TEXT("[Weapon][AnimNotifyFallback] Owner=%s Draw=%s Notify=%s"),
						*GetNameSafe(GetOwner()),
						bDraw ? TEXT("true") : TEXT("false"),
						*(bDraw ? EquipNotifyName : UnequipNotifyName).ToString());
					CompleteWeaponDrawTransition_Server(bDraw);
				}
			}),
			AnimNotifyFallbackDelaySeconds,
			false);
	}

	return true;
}

void USpellRiseWeaponComponent::CompleteWeaponDrawTransition_Server(bool bDraw)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AnimNotifyFallbackTimerHandle);
	}

	if (bWeaponDrawn != bDraw)
	{
		RemoveActiveWeaponGrants_Server();
	}

	bTransitionInProgress = false;
	bPendingDrawState = false;
	bWeaponDrawn = bDraw;
	RefreshEquippedWeaponReference_Server();
	AttachCurrentWeaponVisual();
	if (bWeaponDrawn)
	{
		ApplyActiveWeaponGrants_Server();
	}
	else
	{
		RemoveActiveWeaponGrants_Server();
		EquippedWeapon = nullptr;
		ActiveWeaponItem = nullptr;
	}
	RefreshOffHandSuppression_Server();
	ApplyOverlayState();
	BroadcastWeaponState();
	ForceOwnerNetUpdate();
}

void USpellRiseWeaponComponent::RemoveActiveWeaponGrants_Server()
{
	USpellRiseAbilitySystemComponent* ASC = ResolveSpellRiseASC();
	if (!ASC)
	{
		ActiveWeaponAbilityHandles.Reset();
		ActiveWeaponEffectHandles.Reset();
		return;
	}

	for (const FActiveGameplayEffectHandle& Handle : ActiveWeaponEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	ActiveWeaponEffectHandles.Reset();

	for (const FGameplayAbilitySpecHandle& Handle : ActiveWeaponAbilityHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}
	ActiveWeaponAbilityHandles.Reset();
}

void USpellRiseWeaponComponent::ApplyActiveWeaponGrants_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !bWeaponDrawn)
	{
		return;
	}

	FSpellRiseWeaponSlotState* ActiveSlot = GetActiveSlotState();
	USpellRiseAbilitySystemComponent* ASC = ResolveSpellRiseASC();
	if (!ActiveSlot || !ASC)
	{
		return;
	}

	RemoveActiveWeaponGrants_Server();

	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	ExtractGrantedAbilities(ActiveSlot->Item, ActiveSlot->WeaponDefinition, AbilitiesToGrant);
	for (const FSpellRiseGrantedAbility& AbilityEntry : AbilitiesToGrant)
	{
		UClass* AbilityClass = AbilityEntry.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		UObject* SourceObject = ActiveSlot->WeaponActor ? static_cast<UObject*>(ActiveSlot->WeaponActor.Get()) : static_cast<UObject*>(ActiveSlot->Item.Get());
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, SourceObject);
		if (AbilityEntry.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(AbilityEntry.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		if (Handle.IsValid())
		{
			ActiveWeaponAbilityHandles.Add(Handle);
		}
	}

	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;
	ExtractGrantedEffects(ActiveSlot->Item, ActiveSlot->WeaponDefinition, EffectsToApply);
	TMap<FGameplayTag, float> SetByCallerMagnitudes;
	ExtractSetByCallerMagnitudes(ActiveSlot->Item, ActiveSlot->WeaponDefinition, SetByCallerMagnitudes);

	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApply)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		UObject* SourceObject = ActiveSlot->WeaponActor ? static_cast<UObject*>(ActiveSlot->WeaponActor.Get()) : static_cast<UObject*>(ActiveSlot->Item.Get());
		Context.AddSourceObject(SourceObject);
		Context.AddInstigator(GetOwner(), GetOwner());
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
		if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
		{
			continue;
		}

		for (const TPair<FGameplayTag, float>& Magnitude : SetByCallerMagnitudes)
		{
			if (Magnitude.Key.IsValid() && SpellRiseWeaponComponentPrivate::IsFiniteMagnitude(Magnitude.Value))
			{
				SpecHandle.Data->SetSetByCallerMagnitude(Magnitude.Key, Magnitude.Value);
			}
		}

		const FActiveGameplayEffectHandle AppliedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (AppliedHandle.IsValid())
		{
			ActiveWeaponEffectHandles.Add(AppliedHandle);
		}
	}

	UE_LOG(LogSpellRiseWeaponComponent, Log,
		TEXT("[Weapon][GrantsApplied] Owner=%s Item=%s Abilities=%d Effects=%d"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(ActiveSlot->Item),
		ActiveWeaponAbilityHandles.Num(),
		ActiveWeaponEffectHandles.Num());
}

void USpellRiseWeaponComponent::RemoveOffHandWeaponGrants_Server()
{
	USpellRiseAbilitySystemComponent* ASC = ResolveSpellRiseASC();
	if (!ASC)
	{
		OffHandWeaponAbilityHandles.Reset();
		OffHandWeaponEffectHandles.Reset();
		return;
	}

	for (const FActiveGameplayEffectHandle& Handle : OffHandWeaponEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	OffHandWeaponEffectHandles.Reset();

	for (const FGameplayAbilitySpecHandle& Handle : OffHandWeaponAbilityHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}
	OffHandWeaponAbilityHandles.Reset();
}

void USpellRiseWeaponComponent::ApplyOffHandWeaponGrants_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !OffHandWeapon.Item || OffHandWeapon.bIsSuppressed)
	{
		return;
	}

	USpellRiseAbilitySystemComponent* ASC = ResolveSpellRiseASC();
	if (!ASC)
	{
		return;
	}

	RemoveOffHandWeaponGrants_Server();

	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	ExtractGrantedAbilities(OffHandWeapon.Item, OffHandWeapon.WeaponDefinition, AbilitiesToGrant);
	for (const FSpellRiseGrantedAbility& AbilityEntry : AbilitiesToGrant)
	{
		UClass* AbilityClass = AbilityEntry.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		UObject* SourceObject = OffHandWeapon.WeaponActor ? static_cast<UObject*>(OffHandWeapon.WeaponActor.Get()) : static_cast<UObject*>(OffHandWeapon.Item.Get());
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, SourceObject);
		if (AbilityEntry.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(AbilityEntry.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		if (Handle.IsValid())
		{
			OffHandWeaponAbilityHandles.Add(Handle);
		}
	}

	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;
	ExtractGrantedEffects(OffHandWeapon.Item, OffHandWeapon.WeaponDefinition, EffectsToApply);
	TMap<FGameplayTag, float> SetByCallerMagnitudes;
	ExtractSetByCallerMagnitudes(OffHandWeapon.Item, OffHandWeapon.WeaponDefinition, SetByCallerMagnitudes);

	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectsToApply)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		UObject* SourceObject = OffHandWeapon.WeaponActor ? static_cast<UObject*>(OffHandWeapon.WeaponActor.Get()) : static_cast<UObject*>(OffHandWeapon.Item.Get());
		Context.AddSourceObject(SourceObject);
		Context.AddInstigator(GetOwner(), GetOwner());
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
		if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
		{
			continue;
		}

		for (const TPair<FGameplayTag, float>& Magnitude : SetByCallerMagnitudes)
		{
			if (Magnitude.Key.IsValid() && SpellRiseWeaponComponentPrivate::IsFiniteMagnitude(Magnitude.Value))
			{
				SpecHandle.Data->SetSetByCallerMagnitude(Magnitude.Key, Magnitude.Value);
			}
		}

		const FActiveGameplayEffectHandle AppliedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (AppliedHandle.IsValid())
		{
			OffHandWeaponEffectHandles.Add(AppliedHandle);
		}
	}
}

void USpellRiseWeaponComponent::RefreshEquippedWeaponReference_Server()
{
	EquippedWeapon = nullptr;
	ActiveWeaponItem = nullptr;

	FSpellRiseWeaponSlotState* ActiveSlot = GetActiveSlotState();
	if (!ActiveSlot)
	{
		return;
	}

	EquippedWeapon = GetOrSpawnWeaponActorForSlot(*ActiveSlot);
	ActiveSlot->WeaponActor = EquippedWeapon;
	ActiveWeaponItem = ActiveSlot->Item;
}

void USpellRiseWeaponComponent::RefreshOffHandSuppression_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	OffHandWeapon.bIsSuppressed = bWeaponDrawn && IsTwoHandedWeaponItem(ActiveWeaponItem);
	if (OffHandWeapon.Item)
	{
		if (OffHandWeapon.bIsSuppressed)
		{
			RemoveOffHandWeaponGrants_Server();
		}
		else
		{
			ApplyOffHandWeaponGrants_Server();
		}
		AttachWeaponVisual(OffHandWeapon.WeaponActor, OffHandWeapon.Item, OffHandWeapon.WeaponDefinition, !OffHandWeapon.bIsSuppressed, true);
	}
}

void USpellRiseWeaponComponent::AttachCurrentWeaponVisual()
{
	const FSpellRiseWeaponSlotState* ActiveSlot = GetActiveSlotState();
	if (!ActiveSlot)
	{
		return;
	}

	AttachWeaponVisual(EquippedWeapon ? EquippedWeapon.Get() : ActiveSlot->WeaponActor.Get(), ActiveSlot->Item, ActiveSlot->WeaponDefinition, bWeaponDrawn, false);
}

void USpellRiseWeaponComponent::AttachWeaponVisual(AActor* WeaponActor, UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, bool bDraw, bool bOffHand)
{
	if (!WeaponActor)
	{
		return;
	}

	FName EquippedSocket = NAME_None;
	FName StowedSocket = NAME_None;
	USpellRiseWeaponDefinition* EffectiveDefinition = WeaponDefinition;
	if (!EffectiveDefinition)
	{
		ExtractWeaponDefinitionFromObject(WeaponActor, EffectiveDefinition);
	}
	ResolveWeaponSockets(Item, EffectiveDefinition, bOffHand, EquippedSocket, StowedSocket);
	if (!Item)
	{
		const UStruct* ConfigStruct = nullptr;
		const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActor->GetClass(), ConfigStruct);
		SpellRiseWeaponComponentPrivate::ReadNameProperty(ConfigPtr, ConfigStruct, { TEXT("EquippedSocket"), TEXT("EquippedSocketName"), TEXT("MainHandEquippedSocketName") }, EquippedSocket);
		SpellRiseWeaponComponentPrivate::ReadNameProperty(ConfigPtr, ConfigStruct, { TEXT("StowedSocket"), TEXT("StowedSocketName"), TEXT("HolsterSocketName"), TEXT("MainHandStowedSocketName") }, StowedSocket);
	}
	const FName TargetSocket = bDraw ? EquippedSocket : StowedSocket;
	if (TargetSocket == NAME_None)
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][AttachRejected] Reason=no_socket Owner=%s Weapon=%s Draw=%s OffHand=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActor),
			bDraw ? TEXT("true") : TEXT("false"),
			bOffHand ? TEXT("true") : TEXT("false"));
		return;
	}

	USkeletalMeshComponent* AttachMesh = ResolveAttachMesh(TargetSocket);
	USceneComponent* WeaponRoot = WeaponActor->GetRootComponent();
	if (!AttachMesh || !WeaponRoot)
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][AttachRejected] Reason=missing_attach_target Owner=%s Weapon=%s Socket=%s Mesh=%s Root=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActor),
			*TargetSocket.ToString(),
			*GetNameSafe(AttachMesh),
			*GetNameSafe(WeaponRoot));
		return;
	}

	WeaponActor->SetActorEnableCollision(false);
	WeaponRoot->SetMobility(EComponentMobility::Movable);
	WeaponActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetSocket);
	UE_LOG(LogSpellRiseWeaponComponent, Log,
		TEXT("[Weapon][AttachOK] Owner=%s Weapon=%s Socket=%s Mesh=%s Draw=%s OffHand=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(WeaponActor),
		*TargetSocket.ToString(),
		*GetNameSafe(AttachMesh),
		bDraw ? TEXT("true") : TEXT("false"),
		bOffHand ? TEXT("true") : TEXT("false"));
}

void USpellRiseWeaponComponent::PlayWeaponTransitionLocally(UAnimMontage* MontageToPlay, bool bDraw)
{
	if (!MontageToPlay)
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][PlayMontageSkipped] Owner=%s Draw=%s Reason=no_montage"),
			*GetNameSafe(GetOwner()),
			bDraw ? TEXT("true") : TEXT("false"));
		return;
	}

	BindAnimNotifyDelegate();

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh();
		UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
		const float Duration = CharacterOwner->PlayAnimMontage(MontageToPlay);
		UE_LOG(LogSpellRiseWeaponComponent, Log,
			TEXT("[Weapon][PlayMontage] Owner=%s Draw=%s Montage=%s Duration=%.3f Mesh=%s AnimInstance=%s"),
			*GetNameSafe(GetOwner()),
			bDraw ? TEXT("true") : TEXT("false"),
			*GetNameSafe(MontageToPlay),
			Duration,
			*GetNameSafe(Mesh),
			*GetNameSafe(AnimInstance));
	}
	else
	{
		UE_LOG(LogSpellRiseWeaponComponent, Warning,
			TEXT("[Weapon][PlayMontageSkipped] Owner=%s Draw=%s Reason=owner_not_character Montage=%s"),
			*GetNameSafe(GetOwner()),
			bDraw ? TEXT("true") : TEXT("false"),
			*GetNameSafe(MontageToPlay));
	}
}

void USpellRiseWeaponComponent::BindAnimNotifyDelegate()
{
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::HandleMontageNotifyBegin);
				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::HandleMontageNotifyBegin);
			}
		}
	}
}

void USpellRiseWeaponComponent::UnbindAnimNotifyDelegate()
{
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::HandleMontageNotifyBegin);
			}
		}
	}
}

void USpellRiseWeaponComponent::BroadcastWeaponState()
{
	OnWeaponChanged.Broadcast(EquippedWeapon);
	OnWeaponDrawStateChanged.Broadcast(bWeaponDrawn);
}

void USpellRiseWeaponComponent::ApplyOverlayState()
{
	const FSpellRiseWeaponSlotState* ActiveSlot = GetActiveSlotState();
	const uint8 OverlayValue = ActiveSlot ? ResolveOverlayStateValueForSlot(*ActiveSlot) : 0;
	BP_ApplyWeaponOverlayState(OverlayValue, bWeaponDrawn && ActiveSlot != nullptr);
}

void USpellRiseWeaponComponent::ForceOwnerNetUpdate() const
{
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
}

bool USpellRiseWeaponComponent::ValidateItemOwnership(UEquippableItem* Item, FString& OutReason) const
{
	OutReason.Reset();
	if (!Item)
	{
		OutReason = TEXT("item_null");
		return false;
	}

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		OutReason = TEXT("owner_not_pawn");
		return false;
	}

	if (Item->GetOwningPawn() == PawnOwner)
	{
		return true;
	}

	if (Item->GetOwningController() && Item->GetOwningController() == PawnOwner->GetController())
	{
		return true;
	}

	if (!Item->GetOwningPawn() && !Item->GetOwningController())
	{
		OutReason = TEXT("item_has_no_owner");
		return false;
	}

	OutReason = TEXT("ownership_mismatch");
	return false;
}

bool USpellRiseWeaponComponent::CheckServerWeaponRpcRateLimit(const TCHAR* RpcName, FSpellRiseWeaponRpcRateLimitState& RateState, FString& OutRejectReason)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		OutRejectReason = TEXT("world_null");
		return false;
	}

	const double Now = World->GetTimeSeconds();
	if (Now - RateState.WindowStartServerTimeSeconds > WeaponRpcRateLimitWindowSeconds)
	{
		RateState.WindowStartServerTimeSeconds = Now;
		RateState.RequestsInWindow = 0;
	}

	++RateState.RequestsInWindow;
	if (RateState.RequestsInWindow > WeaponRpcRateLimitMaxRequestsPerWindow)
	{
		++RateState.RejectCount;
		OutRejectReason = FString::Printf(TEXT("rate_limited:%s count=%d reject_count=%d"), RpcName, RateState.RequestsInWindow, RateState.RejectCount);
		return false;
	}

	return true;
}

void USpellRiseWeaponComponent::AuditRejectedWeaponRpc(const TCHAR* RpcName, const FString& RejectReason) const
{
	UE_LOG(LogSpellRiseWeaponComponent, Warning,
		TEXT("[Weapon][RpcRejected] Rpc=%s Owner=%s Reason=%s"),
		RpcName,
		*GetNameSafe(GetOwner()),
		*RejectReason);
}

int32 USpellRiseWeaponComponent::FindQuickSlotByItem(UEquippableItem* Item) const
{
	if (!Item)
	{
		return INDEX_NONE;
	}

	for (int32 Index = 0; Index < QuickWeaponSlots.Num(); ++Index)
	{
		if (QuickWeaponSlots[Index].Item == Item)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 USpellRiseWeaponComponent::FindFirstFreeQuickSlot() const
{
	for (int32 Index = 0; Index < QuickWeaponSlots.Num(); ++Index)
	{
		if (!QuickWeaponSlots[Index].Item)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

int32 USpellRiseWeaponComponent::GetPreferredQuickSlotForItem(UEquippableItem* Item) const
{
	const int32 ExistingSlot = FindQuickSlotByItem(Item);
	if (ExistingSlot != INDEX_NONE)
	{
		return ExistingSlot;
	}

	const int32 FreeSlot = FindFirstFreeQuickSlot();
	if (FreeSlot != INDEX_NONE)
	{
		return FreeSlot;
	}

	return ActiveQuickSlotIndex == 0 ? 1 : 0;
}

FSpellRiseWeaponSlotState* USpellRiseWeaponComponent::GetActiveSlotState()
{
	return QuickWeaponSlots.IsValidIndex(ActiveQuickSlotIndex) ? &QuickWeaponSlots[ActiveQuickSlotIndex] : nullptr;
}

const FSpellRiseWeaponSlotState* USpellRiseWeaponComponent::GetActiveSlotState() const
{
	return QuickWeaponSlots.IsValidIndex(ActiveQuickSlotIndex) ? &QuickWeaponSlots[ActiveQuickSlotIndex] : nullptr;
}

bool USpellRiseWeaponComponent::BuildWeaponSlotState(UEquippableItem* Item, FSpellRiseWeaponSlotState& OutState)
{
	OutState = FSpellRiseWeaponSlotState();
	OutState.Item = Item;
	USpellRiseWeaponDefinition* ResolvedDefinition = nullptr;
	ExtractWeaponDefinitionFromItem(Item, ResolvedDefinition);
	OutState.WeaponDefinition = ResolvedDefinition;
	OutState.bIsOffHand = IsOffHandWeaponItem(Item);
	OutState.bIsTwoHanded = IsTwoHandedWeaponItem(Item);
	return IsWeaponItem(Item);
}

bool USpellRiseWeaponComponent::BuildWeaponSlotStateFromClass(TSubclassOf<AActor> WeaponActorClass, FSpellRiseWeaponSlotState& OutState)
{
	OutState = FSpellRiseWeaponSlotState();
	if (!WeaponActorClass)
	{
		return false;
	}

	OutState.WeaponActorClass = WeaponActorClass;
	USpellRiseWeaponDefinition* ResolvedDefinition = nullptr;
	ExtractWeaponDefinitionFromObject(WeaponActorClass->GetDefaultObject(), ResolvedDefinition);
	OutState.WeaponDefinition = ResolvedDefinition;
	OutState.bIsOffHand = ResolvedDefinition && ResolvedDefinition->HandPolicy == ESpellRiseWeaponHandPolicy::OffHand;
	OutState.bIsTwoHanded = ResolvedDefinition && ResolvedDefinition->HandPolicy == ESpellRiseWeaponHandPolicy::TwoHanded;
	return true;
}

AActor* USpellRiseWeaponComponent::GetOrSpawnWeaponActorForSlot(FSpellRiseWeaponSlotState& SlotState)
{
	if (SlotState.WeaponActor)
	{
		return SlotState.WeaponActor;
	}

	if (SlotState.Item)
	{
		if (TObjectPtr<AActor>* Existing = SpawnedWeaponActorsByItem.Find(SlotState.Item))
		{
			if (IsValid(*Existing))
			{
				SlotState.WeaponActor = *Existing;
				return SlotState.WeaponActor;
			}
		}
	}

	UClass* WeaponActorClass = nullptr;
	if (SlotState.WeaponActorClass)
	{
		WeaponActorClass = SlotState.WeaponActorClass;
	}
	else if (!ResolveWeaponActorClassFromItem(SlotState.Item, WeaponActorClass) || !WeaponActorClass)
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

	SpawnedWeapon->SetReplicates(true);
	SpawnedWeapon->SetReplicateMovement(false);
	SpawnedWeapon->bNetUseOwnerRelevancy = true;
	SpawnedWeapon->SetNetUpdateFrequency(15.0f);
	SlotState.WeaponActor = SpawnedWeapon;
	if (SlotState.Item)
	{
		SpawnedWeaponActorsByItem.Add(SlotState.Item, SpawnedWeapon);
	}
	return SpawnedWeapon;
}

void USpellRiseWeaponComponent::DestroyWeaponActor(AActor* WeaponActor)
{
	if (IsValid(WeaponActor) && WeaponActor->HasAuthority())
	{
		WeaponActor->Destroy();
	}
}

bool USpellRiseWeaponComponent::ResolveWeaponActorClassFromItem(UEquippableItem* Item, UClass*& OutWeaponActorClass) const
{
	OutWeaponActorClass = nullptr;

	USpellRiseWeaponDefinition* Definition = nullptr;
	if (Item)
	{
		if (UObject* DefinitionObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(Item, Item->GetClass()->FindPropertyByName(TEXT("WeaponDefinition"))))
		{
			Definition = Cast<USpellRiseWeaponDefinition>(DefinitionObject);
		}
	}
	if (Definition)
	{
		OutWeaponActorClass = Definition->WeaponActorClassRef.LoadSynchronous();
		if (OutWeaponActorClass)
		{
			return true;
		}
	}

	if (Item)
	{
		static const TArray<FName> ClassPropertyNames = {
			TEXT("WeaponClass"),
			TEXT("WeaponActorClass"),
			TEXT("WeaponActor"),
			TEXT("Weapon")
		};

		for (const FName PropertyName : ClassPropertyNames)
		{
			if (UClass* WeaponClass = SpellRiseWeaponComponentPrivate::ReadClassLikeProperty(Item, Item->GetClass()->FindPropertyByName(PropertyName), AActor::StaticClass()))
			{
				OutWeaponActorClass = WeaponClass;
				return true;
			}
		}
	}

	OutWeaponActorClass = DefaultWeaponActorClass.LoadSynchronous();
	return OutWeaponActorClass != nullptr;
}

bool USpellRiseWeaponComponent::ExtractWeaponDefinitionFromItem(UEquippableItem* Item, USpellRiseWeaponDefinition*& OutWeaponDefinition) const
{
	OutWeaponDefinition = nullptr;
	if (!Item)
	{
		return false;
	}

	if (UObject* DefinitionObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(Item, Item->GetClass()->FindPropertyByName(TEXT("WeaponDefinition"))))
	{
		OutWeaponDefinition = Cast<USpellRiseWeaponDefinition>(DefinitionObject);
		if (OutWeaponDefinition)
		{
			return true;
		}
	}

	static const TArray<FName> ClassPropertyNames = {
		TEXT("WeaponClass"),
		TEXT("WeaponActorClass"),
		TEXT("WeaponActor"),
		TEXT("Weapon")
	};

	for (const FName PropertyName : ClassPropertyNames)
	{
		if (UClass* WeaponActorClass = SpellRiseWeaponComponentPrivate::ReadClassLikeProperty(Item, Item->GetClass()->FindPropertyByName(PropertyName), AActor::StaticClass()))
		{
			return ExtractWeaponDefinitionFromObject(WeaponActorClass->GetDefaultObject(), OutWeaponDefinition);
		}
	}

	return false;
}

bool USpellRiseWeaponComponent::ExtractWeaponDefinitionFromObject(const UObject* SourceObject, USpellRiseWeaponDefinition*& OutWeaponDefinition) const
{
	OutWeaponDefinition = nullptr;
	if (!SourceObject)
	{
		return false;
	}

	if (const ASpellRiseWeaponBase* WeaponBase = Cast<ASpellRiseWeaponBase>(SourceObject))
	{
		OutWeaponDefinition = WeaponBase->WeaponDefinition;
		return OutWeaponDefinition != nullptr;
	}

	if (UObject* DefinitionObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(SourceObject, SourceObject->GetClass()->FindPropertyByName(TEXT("WeaponDefinition"))))
	{
		OutWeaponDefinition = Cast<USpellRiseWeaponDefinition>(DefinitionObject);
	}

	return OutWeaponDefinition != nullptr;
}

bool USpellRiseWeaponComponent::ExtractWeaponDefinitionFromSlot(const FSpellRiseWeaponSlotState& SlotState, USpellRiseWeaponDefinition*& OutWeaponDefinition) const
{
	OutWeaponDefinition = SlotState.WeaponDefinition;
	if (OutWeaponDefinition)
	{
		return true;
	}

	if (SlotState.Item && ExtractWeaponDefinitionFromItem(SlotState.Item, OutWeaponDefinition))
	{
		return true;
	}

	if (SlotState.WeaponActor && ExtractWeaponDefinitionFromObject(SlotState.WeaponActor, OutWeaponDefinition))
	{
		return true;
	}

	if (SlotState.WeaponActorClass && ExtractWeaponDefinitionFromObject(SlotState.WeaponActorClass->GetDefaultObject(), OutWeaponDefinition))
	{
		return true;
	}

	return false;
}

bool USpellRiseWeaponComponent::ExtractGrantedEffects(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, TArray<TSubclassOf<UGameplayEffect>>& OutEffects) const
{
	OutEffects.Reset();
	if (WeaponDefinition)
	{
		for (const TSoftClassPtr<UGameplayEffect>& EffectClassRef : WeaponDefinition->GrantedEffectClassesWhileEquipped)
		{
			if (UClass* EffectClass = EffectClassRef.LoadSynchronous())
			{
				OutEffects.AddUnique(EffectClass);
			}
		}
	}

	if (Item)
	{
		const FProperty* Property = Item->GetClass()->FindPropertyByName(TEXT("GrantedEffectsWhileEquipped"));
		if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
		{
			FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Item));
			for (int32 Index = 0; Index < ArrayHelper.Num(); ++Index)
			{
				UClass* EffectClass = nullptr;
				if (const FClassProperty* ClassProperty = CastField<FClassProperty>(ArrayProperty->Inner))
				{
					EffectClass = Cast<UClass>(ClassProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(Index)));
				}
				else if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(ArrayProperty->Inner))
				{
					FSoftObjectPtr SoftClassPtr = SoftClassProperty->GetPropertyValue(ArrayHelper.GetRawPtr(Index));
					UObject* ResolvedObject = SoftClassPtr.Get();
					EffectClass = Cast<UClass>(ResolvedObject ? ResolvedObject : SoftClassPtr.LoadSynchronous());
				}

				if (EffectClass && EffectClass->IsChildOf(UGameplayEffect::StaticClass()))
				{
					OutEffects.AddUnique(EffectClass);
				}
			}
		}
	}

	return OutEffects.Num() > 0;
}

bool USpellRiseWeaponComponent::ExtractGrantedAbilities(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, TArray<FSpellRiseGrantedAbility>& OutAbilities) const
{
	OutAbilities.Reset();
	if (WeaponDefinition)
	{
		OutAbilities.Append(WeaponDefinition->AbilitiesToGrant);
	}
	return OutAbilities.Num() > 0;
}

void USpellRiseWeaponComponent::ExtractSetByCallerMagnitudes(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, TMap<FGameplayTag, float>& OutMagnitudes) const
{
	OutMagnitudes.Reset();
	if (WeaponDefinition)
	{
		OutMagnitudes.Append(WeaponDefinition->SetByCallerMagnitudes);
	}
}

bool USpellRiseWeaponComponent::ResolveWeaponSockets(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, bool bOffHand, FName& OutEquippedSocket, FName& OutStowedSocket) const
{
	OutEquippedSocket = bOffHand ? TEXT("hand_l") : DefaultEquippedSocket;
	OutStowedSocket = bOffHand ? TEXT("stowed_l") : DefaultStowedSocket;

	if (WeaponDefinition)
	{
		if (WeaponDefinition->EquippedSocket != NAME_None)
		{
			OutEquippedSocket = WeaponDefinition->EquippedSocket;
		}
		if (WeaponDefinition->StowedSocket != NAME_None)
		{
			OutStowedSocket = WeaponDefinition->StowedSocket;
		}
		return true;
	}

	UClass* WeaponActorClass = nullptr;
	if (ResolveWeaponActorClassFromItem(Item, WeaponActorClass) && WeaponActorClass)
	{
		const UStruct* ConfigStruct = nullptr;
		const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActorClass, ConfigStruct);
		SpellRiseWeaponComponentPrivate::ReadNameProperty(ConfigPtr, ConfigStruct, { TEXT("EquippedSocket"), TEXT("EquippedSocketName"), TEXT("MainHandEquippedSocketName") }, OutEquippedSocket);
		SpellRiseWeaponComponentPrivate::ReadNameProperty(ConfigPtr, ConfigStruct, { TEXT("StowedSocket"), TEXT("StowedSocketName"), TEXT("HolsterSocketName"), TEXT("MainHandStowedSocketName") }, OutStowedSocket);
	}

	return OutEquippedSocket != NAME_None || OutStowedSocket != NAME_None;
}

bool USpellRiseWeaponComponent::ResolveWeaponSocketsForSlot(const FSpellRiseWeaponSlotState& SlotState, bool bOffHand, FName& OutEquippedSocket, FName& OutStowedSocket) const
{
	USpellRiseWeaponDefinition* EffectiveDefinition = nullptr;
	ExtractWeaponDefinitionFromSlot(SlotState, EffectiveDefinition);
	ResolveWeaponSockets(SlotState.Item, EffectiveDefinition, bOffHand, OutEquippedSocket, OutStowedSocket);

	UClass* WeaponActorClass = SlotState.WeaponActorClass;
	if (!WeaponActorClass && SlotState.WeaponActor)
	{
		WeaponActorClass = SlotState.WeaponActor->GetClass();
	}
	if (!WeaponActorClass && SlotState.Item)
	{
		ResolveWeaponActorClassFromItem(SlotState.Item, WeaponActorClass);
	}

	if (WeaponActorClass)
	{
		const UStruct* ConfigStruct = nullptr;
		const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActorClass, ConfigStruct);
		SpellRiseWeaponComponentPrivate::ReadNameProperty(ConfigPtr, ConfigStruct, { TEXT("EquippedSocket"), TEXT("EquippedSocketName"), TEXT("MainHandEquippedSocketName") }, OutEquippedSocket);
		SpellRiseWeaponComponentPrivate::ReadNameProperty(ConfigPtr, ConfigStruct, { TEXT("StowedSocket"), TEXT("StowedSocketName"), TEXT("HolsterSocketName"), TEXT("MainHandStowedSocketName") }, OutStowedSocket);
	}

	return OutEquippedSocket != NAME_None || OutStowedSocket != NAME_None;
}

bool USpellRiseWeaponComponent::ResolveWeaponMontages(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, UAnimMontage*& OutEquipMontage, UAnimMontage*& OutUnequipMontage) const
{
	OutEquipMontage = nullptr;
	OutUnequipMontage = nullptr;

	UClass* WeaponActorClass = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass) || !WeaponActorClass)
	{
		return false;
	}

	const UStruct* ConfigStruct = nullptr;
	const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActorClass, ConfigStruct);
	if (!ConfigPtr || !ConfigStruct)
	{
		return false;
	}

	if (UObject* EquipObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(ConfigPtr, ConfigStruct, { TEXT("EquipMontage") }))
	{
		OutEquipMontage = Cast<UAnimMontage>(EquipObject);
	}
	if (UObject* UnequipObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(ConfigPtr, ConfigStruct, { TEXT("UnequipMontage") }))
	{
		OutUnequipMontage = Cast<UAnimMontage>(UnequipObject);
	}

	return OutEquipMontage != nullptr || OutUnequipMontage != nullptr;
}

bool USpellRiseWeaponComponent::ResolveWeaponMontagesForSlot(const FSpellRiseWeaponSlotState& SlotState, UAnimMontage*& OutEquipMontage, UAnimMontage*& OutUnequipMontage) const
{
	OutEquipMontage = nullptr;
	OutUnequipMontage = nullptr;

	UClass* WeaponActorClass = SlotState.WeaponActorClass;
	if (!WeaponActorClass && SlotState.WeaponActor)
	{
		WeaponActorClass = SlotState.WeaponActor->GetClass();
	}
	if (!WeaponActorClass && SlotState.Item)
	{
		ResolveWeaponActorClassFromItem(SlotState.Item, WeaponActorClass);
	}
	if (!WeaponActorClass)
	{
		return false;
	}

	const UStruct* ConfigStruct = nullptr;
	const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActorClass, ConfigStruct);
	if (!ConfigPtr || !ConfigStruct)
	{
		return false;
	}

	if (UObject* EquipObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(ConfigPtr, ConfigStruct, { TEXT("EquipMontage") }))
	{
		OutEquipMontage = Cast<UAnimMontage>(EquipObject);
	}
	if (UObject* UnequipObject = SpellRiseWeaponComponentPrivate::ReadObjectLikeProperty(ConfigPtr, ConfigStruct, { TEXT("UnequipMontage") }))
	{
		OutUnequipMontage = Cast<UAnimMontage>(UnequipObject);
	}

	return OutEquipMontage != nullptr || OutUnequipMontage != nullptr;
}

uint8 USpellRiseWeaponComponent::ResolveOverlayStateValue(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition) const
{
	UClass* WeaponActorClass = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass) || !WeaponActorClass)
	{
		return 0;
	}

	const UStruct* ConfigStruct = nullptr;
	const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActorClass, ConfigStruct);
	if (!ConfigPtr || !ConfigStruct)
	{
		return 0;
	}

	for (const FName PropertyName : { TEXT("OverlayState"), TEXT("OverlayStates"), TEXT("AnimClass") })
	{
		const FProperty* Property = ConfigStruct->FindPropertyByName(PropertyName);
		if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			return static_cast<uint8>(EnumProperty->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(EnumProperty->ContainerPtrToValuePtr<void>(ConfigPtr)));
		}
		if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			return ByteProperty->GetPropertyValue_InContainer(ConfigPtr);
		}
	}

	return 0;
}

uint8 USpellRiseWeaponComponent::ResolveOverlayStateValueForSlot(const FSpellRiseWeaponSlotState& SlotState) const
{
	UClass* WeaponActorClass = SlotState.WeaponActorClass;
	if (!WeaponActorClass && SlotState.WeaponActor)
	{
		WeaponActorClass = SlotState.WeaponActor->GetClass();
	}
	if (!WeaponActorClass && SlotState.Item)
	{
		ResolveWeaponActorClassFromItem(SlotState.Item, WeaponActorClass);
	}
	if (!WeaponActorClass)
	{
		return 0;
	}

	const UStruct* ConfigStruct = nullptr;
	const void* ConfigPtr = SpellRiseWeaponComponentPrivate::ResolveWeaponConfigPtr(WeaponActorClass, ConfigStruct);
	if (!ConfigPtr || !ConfigStruct)
	{
		return 0;
	}

	for (const FName PropertyName : { TEXT("OverlayState"), TEXT("OverlayStates"), TEXT("AnimClass") })
	{
		const FProperty* Property = ConfigStruct->FindPropertyByName(PropertyName);
		if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			return static_cast<uint8>(EnumProperty->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(EnumProperty->ContainerPtrToValuePtr<void>(ConfigPtr)));
		}
		if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			return ByteProperty->GetPropertyValue_InContainer(ConfigPtr);
		}
	}

	return 0;
}

USkeletalMeshComponent* USpellRiseWeaponComponent::ResolveAttachMesh(FName TargetSocket) const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		TArray<USkeletalMeshComponent*> Meshes;
		OwnerActor->GetComponents<USkeletalMeshComponent>(Meshes);
		for (USkeletalMeshComponent* Mesh : Meshes)
		{
			if (Mesh && TargetSocket != NAME_None && Mesh->DoesSocketExist(TargetSocket))
			{
				return Mesh;
			}
		}
	}

	if (const ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		return CharacterOwner->GetMesh();
	}

	return nullptr;
}

USceneComponent* USpellRiseWeaponComponent::ResolveWeaponSpawnPoint(AActor* WeaponActor) const
{
	if (!WeaponActor)
	{
		return nullptr;
	}

	if (ASpellRiseWeaponBase* WeaponBase = Cast<ASpellRiseWeaponBase>(WeaponActor))
	{
		return WeaponBase->GetWeaponSpawnPointComponent();
	}

	TArray<USceneComponent*> SceneComponents;
	WeaponActor->GetComponents<USceneComponent>(SceneComponents);
	for (USceneComponent* SceneComponent : SceneComponents)
	{
		if (SceneComponent && SceneComponent->GetFName() == TEXT("SpawnPoint"))
		{
			return SceneComponent;
		}
	}

	return WeaponActor->GetRootComponent();
}

USpellRiseAbilitySystemComponent* USpellRiseWeaponComponent::ResolveSpellRiseASC() const
{
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		return Cast<USpellRiseAbilitySystemComponent>(AbilitySystemInterface->GetAbilitySystemComponent());
	}
	return nullptr;
}
