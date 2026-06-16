// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

#include "Abilities/GameplayAbility.h"
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
#include "TimerManager.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Equipment/SpellRiseEquipmentInstance.h"
#include "SpellRise/Equipment/SpellRiseWeaponComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentReplication, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseWeaponAttach, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentTrace, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentSecurity, Log, All);

namespace SpellRiseEquipmentAbilityPreview
{
	static void CaptureCooldown(
		const UObject* WorldContextObject,
		const UAbilitySystemComponent* ASC,
		const UGameplayAbility* Ability,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpecHandle& AbilityHandle,
		FSpellRiseEquipmentAbilityPreview& Preview)
	{
		if (!ASC || !Ability)
		{
			return;
		}

		float TimeRemaining = 0.f;
		float CooldownDuration = 0.f;
		if (ActorInfo && AbilityHandle.IsValid())
		{
			Ability->GetCooldownTimeRemainingAndDuration(AbilityHandle, ActorInfo, TimeRemaining, CooldownDuration);
		}

		if (TimeRemaining <= KINDA_SMALL_NUMBER)
		{
			const FGameplayTagContainer* CooldownTags = Ability->GetCooldownTags();
			if (CooldownTags && CooldownTags->Num() > 0)
			{
				const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CooldownTags);
				const TArray<TPair<float, float>> DurationAndTimeRemaining = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);
				for (const TPair<float, float>& CooldownPair : DurationAndTimeRemaining)
				{
					if (CooldownPair.Key > TimeRemaining)
					{
						TimeRemaining = CooldownPair.Key;
						CooldownDuration = CooldownPair.Value;
					}
				}
			}
		}

		Preview.CooldownRemaining = FMath::Max(0.f, TimeRemaining);
		Preview.CooldownDuration = FMath::Max(0.f, CooldownDuration);
		Preview.bIsOnCooldown = Preview.CooldownRemaining > KINDA_SMALL_NUMBER;
		if (!Preview.bIsOnCooldown)
		{
			Preview.CooldownCapturedWorldTimeSeconds = 0.0;
			Preview.CooldownStartWorldTimeSeconds = 0.0;
			Preview.CooldownEndWorldTimeSeconds = 0.0;
			return;
		}

		const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
		const double NowSeconds = World ? World->GetTimeSeconds() : 0.0;
		Preview.CooldownCapturedWorldTimeSeconds = NowSeconds;
		Preview.CooldownEndWorldTimeSeconds = NowSeconds + static_cast<double>(Preview.CooldownRemaining);
		Preview.CooldownStartWorldTimeSeconds = Preview.CooldownDuration > KINDA_SMALL_NUMBER
			? Preview.CooldownEndWorldTimeSeconds - static_cast<double>(Preview.CooldownDuration)
			: NowSeconds;
	}
}

namespace SpellRiseEquipmentSlots
{
	constexpr uint8 MainHandSlotBase = 200;
	constexpr uint8 OffHandSlot = 241;
}

namespace SpellRiseEquipmentAttach
{
	static USceneComponent* FindSceneComponentByName(AActor* Actor, FName ComponentName);

	static USceneComponent* ResolveWeaponAttachSceneComponent(AActor* WeaponActor)
	{
		if (!IsValid(WeaponActor))
		{
			return nullptr;
		}

		static const FName WeaponComponentName(TEXT("Weapon"));
		static const FName SpawnPointComponentName(TEXT("SpawnPoint"));
		USceneComponent* WeaponComponent = nullptr;
		TArray<USceneComponent*> SceneComponents;
		WeaponActor->GetComponents<USceneComponent>(SceneComponents);
		for (USceneComponent* Comp : SceneComponents)
		{
			if (IsValid(Comp) && Comp->GetFName() == WeaponComponentName)
			{
				WeaponComponent = Comp;
				break;
			}
		}

		USceneComponent* RootComponent = WeaponActor->GetRootComponent();
		USceneComponent* SpawnPointComponent = FindSceneComponentByName(WeaponActor, SpawnPointComponentName);
		if (IsValid(WeaponComponent))
		{
			if (IsValid(SpawnPointComponent) && !SpawnPointComponent->IsAttachedTo(WeaponComponent) && IsValid(RootComponent))
			{
				return RootComponent;
			}

			return WeaponComponent;
		}

		return RootComponent;
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

namespace SpellRiseDropPickup
{
	static UClass* ResolveDropPickupClass(const TSoftClassPtr<AActor>& ConfiguredDropPickupActorClass)
	{
		UClass* DropPickupClass = ConfiguredDropPickupActorClass.LoadSynchronous();
		if (!DropPickupClass)
		{
			DropPickupClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/NarrativeInventory/Blueprints/BP_BasicItemPickup.BP_BasicItemPickup_C"))).LoadSynchronous();
		}
		if (!DropPickupClass)
		{
			DropPickupClass = TSoftClassPtr<AActor>(FSoftClassPath(TEXT("/Game/UI/InventorySystem/BP_BasicItemPickup.BP_BasicItemPickup_C"))).LoadSynchronous();
		}

		return DropPickupClass;
	}

	static bool IsNarrativeItemClassProperty(const FProperty* Property)
	{
		if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
		{
			return ClassProperty->MetaClass && ClassProperty->MetaClass->IsChildOf(UNarrativeItem::StaticClass());
		}

		if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
		{
			return SoftClassProperty->MetaClass && SoftClassProperty->MetaClass->IsChildOf(UNarrativeItem::StaticClass());
		}

		if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			return ObjectProperty->PropertyClass && ObjectProperty->PropertyClass->IsChildOf(UClass::StaticClass());
		}

		return false;
	}

	static bool SetNarrativeItemClassProperty(void* Container, FProperty* Property, TSubclassOf<UNarrativeItem> ItemClass)
	{
		if (!Container || !Property || !ItemClass)
		{
			return false;
		}

		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
		if (FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
		{
			if (ClassProperty->MetaClass && ClassProperty->MetaClass->IsChildOf(UNarrativeItem::StaticClass()))
			{
				ClassProperty->SetPropertyValue(ValuePtr, ItemClass.Get());
				return true;
			}
		}
		else if (FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
		{
			if (SoftClassProperty->MetaClass && SoftClassProperty->MetaClass->IsChildOf(UNarrativeItem::StaticClass()))
			{
				SoftClassProperty->SetPropertyValue(ValuePtr, FSoftObjectPtr(ItemClass.Get()));
				return true;
			}
		}
		else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			if (ObjectProperty->PropertyClass && ObjectProperty->PropertyClass->IsChildOf(UClass::StaticClass()))
			{
				ObjectProperty->SetObjectPropertyValue(ValuePtr, ItemClass.Get());
				return true;
			}
		}

		return false;
	}

	static bool SetQuantityProperty(void* Container, FProperty* Property, const int32 QuantityToDrop)
	{
		if (!Container || !Property)
		{
			return false;
		}

		if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
		{
			if (NumericProperty->IsInteger())
			{
				NumericProperty->SetIntPropertyValue(Property->ContainerPtrToValuePtr<void>(Container), static_cast<int64>(QuantityToDrop));
				return true;
			}
		}

		return false;
	}

	static bool SetNamedClassProperty(UObject* Object, const FName PropertyName, TSubclassOf<UNarrativeItem> ItemClass)
	{
		return Object && SetNarrativeItemClassProperty(Object, Object->GetClass()->FindPropertyByName(PropertyName), ItemClass);
	}

	static bool SetAnyNarrativeItemClassProperty(UObject* Object, TSubclassOf<UNarrativeItem> ItemClass)
	{
		if (!Object || !ItemClass)
		{
			return false;
		}

		bool bSetAny = false;
		for (TFieldIterator<FProperty> It(Object->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
		{
			FProperty* Property = *It;
			if (IsNarrativeItemClassProperty(Property))
			{
				bSetAny |= SetNarrativeItemClassProperty(Object, Property, ItemClass);
			}
		}

		return bSetAny;
	}

	static bool SetNamedQuantityProperty(UObject* Object, const FName PropertyName, const int32 QuantityToDrop)
	{
		return Object && SetQuantityProperty(Object, Object->GetClass()->FindPropertyByName(PropertyName), QuantityToDrop);
	}

	static bool SetAnyQuantityProperty(UObject* Object, const int32 QuantityToDrop)
	{
		if (!Object)
		{
			return false;
		}

		bool bSetAny = false;
		for (TFieldIterator<FProperty> It(Object->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
		{
			FProperty* Property = *It;
			if (Property->GetName().Contains(TEXT("Quantity"), ESearchCase::IgnoreCase) ||
				Property->GetName().Contains(TEXT("Amount"), ESearchCase::IgnoreCase))
			{
				bSetAny |= SetQuantityProperty(Object, Property, QuantityToDrop);
			}
		}

		return bSetAny;
	}

	static bool ApplyPickupRuntimeData(AActor* PickupActor, TSubclassOf<UNarrativeItem> ItemClass, const int32 QuantityToDrop)
	{
		if (!PickupActor || !ItemClass || QuantityToDrop <= 0)
		{
			return false;
		}

		const bool bSetItemClass =
			SetNamedClassProperty(PickupActor, FName(TEXT("ItemClass")), ItemClass) |
			SetNamedClassProperty(PickupActor, FName(TEXT("Item Class")), ItemClass) |
			SetNamedClassProperty(PickupActor, FName(TEXT("Class")), ItemClass) |
			SetNamedClassProperty(PickupActor, FName(TEXT("Item")), ItemClass) |
			SetAnyNarrativeItemClassProperty(PickupActor, ItemClass);

		const bool bSetQuantity =
			SetNamedQuantityProperty(PickupActor, FName(TEXT("QuantityToGive")), QuantityToDrop) |
			SetNamedQuantityProperty(PickupActor, FName(TEXT("Quantity to Give")), QuantityToDrop) |
			SetNamedQuantityProperty(PickupActor, FName(TEXT("Quantity")), QuantityToDrop) |
			SetNamedQuantityProperty(PickupActor, FName(TEXT("Amount")), QuantityToDrop) |
			SetAnyQuantityProperty(PickupActor, QuantityToDrop);

		return bSetItemClass && bSetQuantity;
	}

	static bool InvokeInitialize(AActor* PickupActor, TSubclassOf<UNarrativeItem> ItemClass, const int32 QuantityToDrop)
	{
		if (!PickupActor || !ItemClass || QuantityToDrop <= 0)
		{
			return false;
		}

		UFunction* InitializeFunction = PickupActor->FindFunction(TEXT("Initialize"));
		if (!InitializeFunction)
		{
			return false;
		}

		uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(InitializeFunction->ParmsSize));
		FMemory::Memzero(ParamsBuffer, InitializeFunction->ParmsSize);

		bool bSetItemClass = false;
		bool bSetQuantity = false;
		for (TFieldIterator<FProperty> It(InitializeFunction); It && (It->PropertyFlags & CPF_Parm); ++It)
		{
			FProperty* Param = *It;
			if (!bSetItemClass && IsNarrativeItemClassProperty(Param))
			{
				bSetItemClass = SetNarrativeItemClassProperty(ParamsBuffer, Param, ItemClass);
				continue;
			}

			if (!bSetQuantity && SetQuantityProperty(ParamsBuffer, Param, QuantityToDrop))
			{
				bSetQuantity = true;
			}
		}

		if (!bSetItemClass || !bSetQuantity)
		{
			return false;
		}

		PickupActor->ProcessEvent(InitializeFunction, ParamsBuffer);
		return true;
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
	QuickWeaponActors.SetNum(2);
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
	DOREPLIFETIME(ThisClass, QuickWeaponActors);
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

	for (const TPair<uint8, TObjectPtr<USpellRiseEquipmentInstance>>& Pair : EquipmentInstancesBySlot)
	{
		if (IsValid(Pair.Value))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Pair.Value, *Bunch, *RepFlags);
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

	for (const TPair<uint8, TObjectPtr<USpellRiseEquipmentInstance>>& Pair : EquipmentInstancesBySlot)
	{
		if (IsValid(Pair.Value))
		{
			AddReplicatedSubObject(Pair.Value);
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
		if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
		{
			return WeaponComponent->EquipWeapon(Item);
		}

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
		if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
		{
			return WeaponComponent->UnequipWeaponItem(Item);
		}

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
	FString RejectReason;
	if (!CheckServerEquipmentRpcRateLimit(TEXT("ServerRequestEquipItem"), EquipItemRpcRateState, EquipmentRpcRateLimitWindowSeconds, EquipmentRpcRateLimitMaxRequestsPerWindow, RejectReason))
	{
		AuditRejectedEquipmentRpc(TEXT("ServerRequestEquipItem"), RejectReason);
		return;
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("ServerRequestEquipItem. Owner=%s Item=%s IsWeapon=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		IsWeaponItem(Item) ? TEXT("true") : TEXT("false"));

	if (IsWeaponItem(Item))
	{
		if (AActor* OwnerActor = GetOwner())
		{
			if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
			{
				WeaponComponent->EquipWeapon(Item);
				return;
			}
		}

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
	FString RejectReason;
	if (!CheckServerEquipmentRpcRateLimit(TEXT("ServerRequestUnequipItem"), UnequipItemRpcRateState, EquipmentRpcRateLimitWindowSeconds, EquipmentRpcRateLimitMaxRequestsPerWindow, RejectReason))
	{
		AuditRejectedEquipmentRpc(TEXT("ServerRequestUnequipItem"), RejectReason);
		return;
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("ServerRequestUnequipItem. Owner=%s Item=%s IsWeapon=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		IsWeaponItem(Item) ? TEXT("true") : TEXT("false"));

	if (IsWeaponItem(Item))
	{
		if (AActor* OwnerActor = GetOwner())
		{
			if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
			{
				WeaponComponent->UnequipWeaponItem(Item);
				return;
			}
		}

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

	if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
	{
		return WeaponComponent->ActivateQuickSlot(QuickSlotIndex);
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

	if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
	{
		return WeaponComponent->AssignQuickSlot(Item, QuickSlotIndex);
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
	FString RejectReason;
	if (!CheckServerEquipmentRpcRateLimit(TEXT("ServerRequestActivateQuickWeaponSlot"), ActivateQuickWeaponSlotRpcRateState, EquipmentRpcRateLimitWindowSeconds, EquipmentRpcRateLimitMaxRequestsPerWindow, RejectReason))
	{
		AuditRejectedEquipmentRpc(TEXT("ServerRequestActivateQuickWeaponSlot"), RejectReason);
		return;
	}

	if (AActor* OwnerActor = GetOwner())
	{
		if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
		{
			WeaponComponent->ActivateQuickSlot(QuickSlotIndex);
			return;
		}
	}

	ActivateQuickWeaponSlot_Server(QuickSlotIndex);
}

void USpellRiseEquipmentManagerComponent::ServerRequestAssignQuickWeaponSlot_Implementation(UEquippableItem* Item, int32 QuickSlotIndex)
{
	FString RejectReason;
	if (!CheckServerEquipmentRpcRateLimit(TEXT("ServerRequestAssignQuickWeaponSlot"), AssignQuickWeaponSlotRpcRateState, EquipmentRpcRateLimitWindowSeconds, EquipmentRpcRateLimitMaxRequestsPerWindow, RejectReason))
	{
		AuditRejectedEquipmentRpc(TEXT("ServerRequestAssignQuickWeaponSlot"), RejectReason);
		return;
	}

	if (AActor* OwnerActor = GetOwner())
	{
		if (USpellRiseWeaponComponent* WeaponComponent = OwnerActor->FindComponentByClass<USpellRiseWeaponComponent>())
		{
			WeaponComponent->AssignQuickSlot(Item, QuickSlotIndex);
			return;
		}
	}

	AssignQuickWeaponSlot_Server(Item, QuickSlotIndex);
}

void USpellRiseEquipmentManagerComponent::ServerRequestDropItem_Implementation(UNarrativeItem* Item, int32 QuantityToDrop)
{
	FString RejectReason;
	if (!CheckServerEquipmentRpcRateLimit(TEXT("ServerRequestDropItem"), DropItemRpcRateState, DropRpcRateLimitWindowSeconds, DropRpcRateLimitMaxRequestsPerWindow, RejectReason))
	{
		AuditRejectedEquipmentRpc(TEXT("ServerRequestDropItem"), RejectReason);
		return;
	}

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
	if (Entry)
	{
		return Entry->Instance;
	}

	const TObjectPtr<USpellRiseEquipmentInstance>* Instance = EquipmentInstancesBySlot.Find(SlotValue);
	return Instance ? Instance->Get() : nullptr;
}

AActor* USpellRiseEquipmentManagerComponent::GetActiveEquippedWeaponActor() const
{
	if (const USpellRiseWeaponComponent* WeaponComponent = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseWeaponComponent>() : nullptr)
	{
		return WeaponComponent->bWeaponDrawn ? WeaponComponent->EquippedWeapon : nullptr;
	}

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

AActor* USpellRiseEquipmentManagerComponent::GetEquippedWeapon(TSubclassOf<AActor> ExpectedClass) const
{
	if (const USpellRiseWeaponComponent* WeaponComponent = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseWeaponComponent>() : nullptr)
	{
		AActor* ActiveWeapon = WeaponComponent->bWeaponDrawn ? WeaponComponent->EquippedWeapon.Get() : nullptr;
		UClass* ExpectedWeaponClass = ExpectedClass.Get();
		if (!ActiveWeapon || ExpectedWeaponClass && !ActiveWeapon->IsA(ExpectedWeaponClass))
		{
			return nullptr;
		}
		return ActiveWeapon;
	}

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

TArray<FSpellRiseWeaponLoadoutSlotView> USpellRiseEquipmentManagerComponent::GetWeaponLoadoutSlotViews() const
{
	TArray<FSpellRiseWeaponLoadoutSlotView> Views;
	Views.Reserve(3);

	if (const USpellRiseWeaponComponent* WeaponComponent = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseWeaponComponent>() : nullptr)
	{
		for (int32 SlotIndex = 0; SlotIndex < 2; ++SlotIndex)
		{
			FSpellRiseWeaponLoadoutSlotView& View = Views.AddDefaulted_GetRef();
			View.Slot = SlotIndex == 0 ? ESpellRiseWeaponLoadoutSlot::WeaponSlot1 : ESpellRiseWeaponLoadoutSlot::WeaponSlot2;
			if (WeaponComponent->QuickWeaponSlots.IsValidIndex(SlotIndex))
			{
				const FSpellRiseWeaponSlotState& SlotState = WeaponComponent->QuickWeaponSlots[SlotIndex];
				View.Item = SlotState.Item;
				View.WeaponActor = SlotState.WeaponActor;
				View.WeaponDefinition = SlotState.WeaponDefinition ? SlotState.WeaponDefinition.Get() : GetWeaponDefinitionForItem(View.Item);
				View.bIsActive = View.Item && WeaponComponent->ActiveQuickSlotIndex == SlotIndex && WeaponComponent->bWeaponDrawn;
			}
			if (View.WeaponDefinition)
			{
				View.WeaponTag = View.WeaponDefinition->WeaponTag;
				View.WeaponProgressionTag = View.WeaponDefinition->WeaponProgressionTag;
				View.HandPolicy = View.WeaponDefinition->HandPolicy;
			}
		}

		FSpellRiseWeaponLoadoutSlotView& OffHandView = Views.AddDefaulted_GetRef();
		OffHandView.Slot = ESpellRiseWeaponLoadoutSlot::OffHand;
		OffHandView.Item = WeaponComponent->OffHandWeapon.Item;
		OffHandView.WeaponActor = WeaponComponent->OffHandWeapon.WeaponActor;
		OffHandView.WeaponDefinition = WeaponComponent->OffHandWeapon.WeaponDefinition ? WeaponComponent->OffHandWeapon.WeaponDefinition.Get() : GetWeaponDefinitionForItem(OffHandView.Item);
		if (OffHandView.WeaponDefinition)
		{
			OffHandView.WeaponTag = OffHandView.WeaponDefinition->WeaponTag;
			OffHandView.WeaponProgressionTag = OffHandView.WeaponDefinition->WeaponProgressionTag;
			OffHandView.HandPolicy = OffHandView.WeaponDefinition->HandPolicy;
		}
		OffHandView.bIsActive = OffHandView.Item && !WeaponComponent->OffHandWeapon.bIsSuppressed;
		OffHandView.bIsSuppressed = WeaponComponent->OffHandWeapon.bIsSuppressed;

		return Views;
	}

	for (int32 SlotIndex = 0; SlotIndex < 2; ++SlotIndex)
	{
		FSpellRiseWeaponLoadoutSlotView& View = Views.AddDefaulted_GetRef();
		View.Slot = SlotIndex == 0 ? ESpellRiseWeaponLoadoutSlot::WeaponSlot1 : ESpellRiseWeaponLoadoutSlot::WeaponSlot2;
		View.Item = QuickWeaponSlots.IsValidIndex(SlotIndex) ? QuickWeaponSlots[SlotIndex] : nullptr;
		View.WeaponActor = QuickWeaponActors.IsValidIndex(SlotIndex) ? QuickWeaponActors[SlotIndex] : nullptr;
		View.WeaponDefinition = GetWeaponDefinitionForItem(View.Item);
		if (View.WeaponDefinition)
		{
			View.WeaponTag = View.WeaponDefinition->WeaponTag;
			View.WeaponProgressionTag = View.WeaponDefinition->WeaponProgressionTag;
			View.HandPolicy = View.WeaponDefinition->HandPolicy;
		}
		View.bIsActive = ActiveQuickWeaponSlotIndex == SlotIndex;
	}

	FSpellRiseWeaponLoadoutSlotView& OffHandView = Views.AddDefaulted_GetRef();
	OffHandView.Slot = ESpellRiseWeaponLoadoutSlot::OffHand;
	OffHandView.Item = ActiveOffHandItem;
	OffHandView.WeaponActor = EquippedOffHandWeapon;
	OffHandView.WeaponDefinition = GetWeaponDefinitionForItem(OffHandView.Item);
	if (OffHandView.WeaponDefinition)
	{
		OffHandView.WeaponTag = OffHandView.WeaponDefinition->WeaponTag;
		OffHandView.WeaponProgressionTag = OffHandView.WeaponDefinition->WeaponProgressionTag;
		OffHandView.HandPolicy = OffHandView.WeaponDefinition->HandPolicy;
	}
	OffHandView.bIsActive = IsOffHandGameplayActive();
	OffHandView.bIsSuppressed = bOffHandSuppressedByTwoHandedWeapon;

	return Views;
}

USpellRiseWeaponDefinition* USpellRiseEquipmentManagerComponent::GetWeaponDefinitionForItem(UEquippableItem* Item) const
{
	if (!Item)
	{
		return nullptr;
	}

	const USpellRiseWeaponDefinition* WeaponDefinition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, WeaponDefinition))
	{
		return const_cast<USpellRiseWeaponDefinition*>(WeaponDefinition);
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) &&
		WeaponActorClass &&
		ExtractWeaponDefinitionFromObject(WeaponActorClass->GetDefaultObject(), WeaponDefinition))
	{
		return const_cast<USpellRiseWeaponDefinition*>(WeaponDefinition);
	}

	return nullptr;
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
	return GetEquippedWeapon(ExpectedClass);
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

	if (const USpellRiseWeaponComponent* WeaponComponent = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseWeaponComponent>() : nullptr)
	{
		const auto FillWeaponSlotView = [this, WeaponComponent, &View](const int32 SlotIndex)
		{
			if (WeaponComponent->QuickWeaponSlots.IsValidIndex(SlotIndex))
			{
				const FSpellRiseWeaponSlotState& SlotState = WeaponComponent->QuickWeaponSlots[SlotIndex];
				View.Item = SlotState.Item;
				View.bIsActive = View.Item && WeaponComponent->ActiveQuickSlotIndex == SlotIndex && WeaponComponent->bWeaponDrawn;
				View.bIsStowed = View.Item && !View.bIsActive;
				View.bIsTwoHandedWeapon = View.Item && WeaponComponent->IsTwoHandedWeaponItem(View.Item);
			}
		};

		switch (Slot)
		{
		case ESpellRiseHUDEquipmentSlot::WeaponSlot1:
			FillWeaponSlotView(0);
			break;
		case ESpellRiseHUDEquipmentSlot::WeaponSlot2:
			FillWeaponSlotView(1);
			break;
		case ESpellRiseHUDEquipmentSlot::OffHand:
			View.Item = WeaponComponent->OffHandWeapon.Item;
			View.bIsBlockedByTwoHandedWeapon = View.Item && WeaponComponent->OffHandWeapon.bIsSuppressed;
			View.bIsActive = View.Item && !View.bIsBlockedByTwoHandedWeapon;
			View.bIsStowed = View.Item && View.bIsBlockedByTwoHandedWeapon;
			View.bIsTwoHandedWeapon = false;
			break;
		default:
			break;
		}

		if (Slot == ESpellRiseHUDEquipmentSlot::WeaponSlot1 ||
			Slot == ESpellRiseHUDEquipmentSlot::WeaponSlot2 ||
			Slot == ESpellRiseHUDEquipmentSlot::OffHand)
		{
			View.bHasItem = View.Item != nullptr;
			return View;
		}
	}

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
		UClass* AbilityClass = AbilityToGrant.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		FSpellRiseEquipmentAbilityPreview& Preview = PreviewAbilities.AddDefaulted_GetRef();
		Preview.Ability = AbilityClass;
		Preview.InputTag = AbilityToGrant.InputTag;
		Preview.bAutoActivateIfNoInputTag = AbilityToGrant.bAutoActivateIfNoInputTag;

		if (!ASC)
		{
			continue;
		}

		const UGameplayAbility* CooldownAbility = Cast<UGameplayAbility>(AbilityClass->GetDefaultObject());
		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (!Spec.Ability || Spec.Ability->GetClass() != AbilityClass)
			{
				continue;
			}

			if (!DoesAbilitySpecBelongToItem(Spec, Item))
			{
				continue;
			}

			Preview.GrantedSpecHandle = Spec.Handle;
			Preview.bIsCurrentlyGranted = true;
			Preview.bIsActive = Spec.IsActive();
			CooldownAbility = Spec.Ability;

			SpellRiseEquipmentAbilityPreview::CaptureCooldown(this, ASC, CooldownAbility, ActorInfo, Spec.Handle, Preview);
			break;
		}

		if (!Preview.bIsCurrentlyGranted)
		{
			SpellRiseEquipmentAbilityPreview::CaptureCooldown(this, ASC, CooldownAbility, ActorInfo, Preview.GrantedSpecHandle, Preview);
		}
	}

	return PreviewAbilities;
}

TArray<FSpellRiseEquipmentAbilityPreview> USpellRiseEquipmentManagerComponent::GetAbilitiesToGrantPreviewForQuickWeaponSlot(
	const int32 QuickSlotIndex) const
{
	return GetAbilitiesToGrantPreviewForItem(GetQuickWeaponItemByIndex(QuickSlotIndex));
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

bool USpellRiseEquipmentManagerComponent::DoesAbilitySpecBelongToItem(
	const FGameplayAbilitySpec& Spec,
	UEquippableItem* Item) const
{
	if (!Item)
	{
		return false;
	}

	UObject* SourceObject = Spec.SourceObject.Get();
	if (SourceObject == Item)
	{
		return true;
	}

	const USpellRiseEquipmentInstance* EquipmentInstance = Cast<USpellRiseEquipmentInstance>(SourceObject);
	return EquipmentInstance && EquipmentInstance->GetSourceItem() == Item;
}

bool USpellRiseEquipmentManagerComponent::AreGrantedAbilitySpecsReadyForItem(UEquippableItem* Item) const
{
	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	if (!ExtractAbilitiesToGrantFromItem(Item, AbilitiesToGrant))
	{
		return true;
	}

	const ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(GetOwner());
	const USpellRiseAbilitySystemComponent* ASC = CharacterOwner
		? Cast<USpellRiseAbilitySystemComponent>(CharacterOwner->GetAbilitySystemComponent())
		: nullptr;
	if (!ASC)
	{
		return false;
	}

	for (const FSpellRiseGrantedAbility& AbilityToGrant : AbilitiesToGrant)
	{
		UClass* AbilityClass = AbilityToGrant.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		bool bFoundSpecForItem = false;
		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.Ability->GetClass() == AbilityClass && DoesAbilitySpecBelongToItem(Spec, Item))
			{
				bFoundSpecForItem = true;
				break;
			}
		}

		if (!bFoundSpecForItem)
		{
			return false;
		}
	}

	return true;
}

void USpellRiseEquipmentManagerComponent::BroadcastHUDEquipmentSlotsChanged()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || OwnerActor->HasAuthority())
	{
		OnHUDEquipmentSlotsChanged.Broadcast();
		return;
	}

	const APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
	{
		OnHUDEquipmentSlotsChanged.Broadcast();
		return;
	}

	UEquippableItem* ActiveItem = QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex)
		? QuickWeaponSlots[ActiveQuickWeaponSlotIndex]
		: nullptr;
	if (!ActiveItem || AreGrantedAbilitySpecsReadyForItem(ActiveItem))
	{
		bPendingHUDEquipmentAbilityRefresh = false;
		PendingHUDEquipmentAbilityRefreshAttempts = 0;
		OnHUDEquipmentSlotsChanged.Broadcast();
		return;
	}

	if (PendingHUDEquipmentAbilityRefreshAttempts >= 8)
	{
		bPendingHUDEquipmentAbilityRefresh = false;
		PendingHUDEquipmentAbilityRefreshAttempts = 0;
		OnHUDEquipmentSlotsChanged.Broadcast();
		return;
	}

	if (bPendingHUDEquipmentAbilityRefresh)
	{
		return;
	}

	bPendingHUDEquipmentAbilityRefresh = true;
	++PendingHUDEquipmentAbilityRefreshAttempts;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &ThisClass::BroadcastHUDEquipmentSlotsChangedWhenAbilitySpecsReady));
		return;
	}

	bPendingHUDEquipmentAbilityRefresh = false;
	PendingHUDEquipmentAbilityRefreshAttempts = 0;
	OnHUDEquipmentSlotsChanged.Broadcast();
}

void USpellRiseEquipmentManagerComponent::BroadcastHUDEquipmentSlotsChangedWhenAbilitySpecsReady()
{
	bPendingHUDEquipmentAbilityRefresh = false;
	BroadcastHUDEquipmentSlotsChanged();
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

	BroadcastHUDEquipmentSlotsChanged();
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

	BroadcastHUDEquipmentSlotsChanged();
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
	BroadcastHUDEquipmentSlotsChanged();
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
		BroadcastHUDEquipmentSlotsChanged();
		return true;
	}

	return false;
}

bool USpellRiseEquipmentManagerComponent::ExtractAbilitiesToGrantFromItem(UEquippableItem* Item, TArray<FSpellRiseGrantedAbility>& OutAbilities) const
{
	OutAbilities.Reset();

	const USpellRiseWeaponDefinition* WeaponDefinition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, WeaponDefinition) && WeaponDefinition)
	{
		for (const FSpellRiseGrantedAbility& AbilityGrant : WeaponDefinition->AbilitiesToGrant)
		{
			if (!AbilityGrant.AbilityClass.IsNull())
			{
				OutAbilities.Add(AbilityGrant);
			}
		}
		return OutAbilities.Num() > 0;
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct))
	{
		return false;
	}

	if (WeaponActorClass && ExtractWeaponDefinitionFromObject(WeaponActorClass->GetDefaultObject(), WeaponDefinition) && WeaponDefinition)
	{
		for (const FSpellRiseGrantedAbility& AbilityGrant : WeaponDefinition->AbilitiesToGrant)
		{
			if (!AbilityGrant.AbilityClass.IsNull())
			{
				OutAbilities.Add(AbilityGrant);
			}
		}
		return OutAbilities.Num() > 0;
	}

	if (!WeaponConfigPtr || !WeaponConfigStruct)
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
		if (AbilityEntry && !AbilityEntry->AbilityClass.IsNull())
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

	const USpellRiseWeaponDefinition* WeaponDefinition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, WeaponDefinition) && WeaponDefinition)
	{
		OutWeaponActorClass = WeaponDefinition->WeaponActorClassRef.LoadSynchronous();
		if (!OutWeaponActorClass)
		{
			UE_LOG(LogSpellRiseEquipmentTrace, Warning,
				TEXT("ResolveWeaponActorClassFromItem falhou: WeaponDefinition sem WeaponActorClass carregavel. Item=%s WeaponDefinition=%s"),
				*GetNameSafe(Item),
				*GetNameSafe(WeaponDefinition));
		}
		return OutWeaponActorClass != nullptr;
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
		TEXT("ResolveWeaponActorClassFromItem usando fallback legado sem WeaponDefinition. Item=%s ItemClass=%s"),
		*GetNameSafe(Item),
		*GetNameSafe(Item->GetClass()));

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
	OutStowedSocket = TEXT("stowed_r");

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
		else if (PropertyName.Contains(TEXT("StowedSocketName")) || PropertyName.Contains(TEXT("HolsterSocketName")))
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

bool USpellRiseEquipmentManagerComponent::ResolveWeaponSocketsForItem(UEquippableItem* Item, bool bOffHand, FName& OutEquippedSocket, FName& OutStowedSocket) const
{
	OutEquippedSocket = bOffHand ? TEXT("hand_l") : TEXT("hand_r");
	OutStowedSocket = bOffHand ? TEXT("stowed_l") : TEXT("stowed_r");

	const USpellRiseWeaponDefinition* WeaponDefinition = GetWeaponDefinitionForItem(Item);
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
		ReadNameProperty({ TEXT("OffHandStowedSocketName"), TEXT("OffHandStowedSocket"), TEXT("StowedSocketName"), TEXT("OffHandHolsterSocketName"), TEXT("OffHandHolsterSocket"), TEXT("HolsterSocketName") }, OutStowedSocket);
	}
	else if (IsTwoHandedWeaponItem(Item))
	{
		ReadNameProperty({ TEXT("TwoHandEquippedSocketName"), TEXT("TwoHandEquippedSocket"), TEXT("MainHandEquippedSocketName"), TEXT("EquippedSocketName") }, OutEquippedSocket);
		ReadNameProperty({ TEXT("TwoHandStowedSocketName"), TEXT("TwoHandStowedSocket"), TEXT("MainHandStowedSocketName"), TEXT("StowedSocketName"), TEXT("TwoHandHolsterSocketName"), TEXT("TwoHandHolsterSocket"), TEXT("MainHandHolsterSocketName"), TEXT("HolsterSocketName") }, OutStowedSocket);
	}
	else
	{
		ReadNameProperty({ TEXT("MainHandEquippedSocketName"), TEXT("MainHandEquippedSocket"), TEXT("EquippedSocketName") }, OutEquippedSocket);
		ReadNameProperty({ TEXT("MainHandStowedSocketName"), TEXT("MainHandStowedSocket"), TEXT("StowedSocketName"), TEXT("MainHandHolsterSocketName"), TEXT("MainHandHolsterSocket"), TEXT("HolsterSocketName") }, OutStowedSocket);
	}

	return OutEquippedSocket != NAME_None || OutStowedSocket != NAME_None;
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

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		EquippedWeapon = GetOrSpawnWeaponActorForItem(ActiveItem);
		if (QuickWeaponActors.IsValidIndex(ActiveQuickWeaponSlotIndex))
		{
			QuickWeaponActors[ActiveQuickWeaponSlotIndex] = EquippedWeapon;
		}
		return;
	}

	if (QuickWeaponActors.IsValidIndex(ActiveQuickWeaponSlotIndex))
	{
		EquippedWeapon = QuickWeaponActors[ActiveQuickWeaponSlotIndex];
	}
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

USkeletalMeshComponent* USpellRiseEquipmentManagerComponent::ResolveEquipmentAttachMesh(FName TargetSocket) const
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	if (const ASpellRiseCharacterBase* CharacterOwner = Cast<ASpellRiseCharacterBase>(OwnerActor))
	{
		return CharacterOwner->GetEquipmentAttachMeshComponentForSocket(TargetSocket);
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	OwnerActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		if (IsValid(MeshComp) && (TargetSocket == NAME_None || MeshComp->DoesSocketExist(TargetSocket)))
		{
			return MeshComp;
		}
	}

	return nullptr;
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
	WeaponActor->bNetUseOwnerRelevancy = true;
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
	if (!bSocketOnMesh)
	{
		UE_LOG(LogSpellRiseWeaponAttach, Warning,
			TEXT("AttachWeaponActorToSocket aborted (socket ausente no mesh de attach). WeaponActor=%s WeaponComp=%s ParentMesh=%s Socket=%s"),
			*GetNameSafe(WeaponActor),
			*WeaponComponent->GetName(),
			*AttachMesh->GetName(),
			*TargetSocket.ToString());
		return false;
	}

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
			TEXT("yes"));
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

	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh(TargetSocket);
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

	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh(TargetSocket);
	AActor* WeaponActor = QuickWeaponActors.IsValidIndex(QuickSlotIndex)
		? QuickWeaponActors[QuickSlotIndex].Get()
		: nullptr;
	if (!AttachMesh || !WeaponActor)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
			TEXT("RefreshQuickSlotVisual_Local aguardando actor/mesh replicado do slot. Owner=%s Item=%s Slot=%d Equipped=%s AttachMesh=%s WeaponActor=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			QuickSlotIndex,
			bEquipped ? TEXT("true") : TEXT("false"),
			*GetNameSafe(AttachMesh),
			*GetNameSafe(WeaponActor));
		if (UWorld* World = GetWorld())
		{
			const TWeakObjectPtr<USpellRiseEquipmentManagerComponent> WeakThis(this);
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakThis]()
			{
				if (USpellRiseEquipmentManagerComponent* Component = WeakThis.Get())
				{
					Component->ReconcileReplicatedQuickSlotVisuals();
				}
			}));
		}
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
	ASpellRisePlayerState* OwnerPlayerState = CharacterOwner ? CharacterOwner->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (!CharacterOwner || !OwnerPlayerState)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Warning,
			TEXT("ApplyGrantedAbilitiesForSlot abortado: owner sem Character/PlayerState. Owner=%s Item=%s SlotValue=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotValue);
		return;
	}

	RemoveGrantedAbilitiesForSlot(SlotValue);

	TArray<FSpellRiseGrantedAbility> AbilitiesToGrant;
	const bool bHasAbilitiesToGrant = ExtractAbilitiesToGrantFromItem(Item, AbilitiesToGrant);
	USpellRiseEquipmentInstance* SourceInstance = GetOrCreateEquipmentInstanceForSlot(Item, SlotValue);
	UObject* AbilitySourceObject = SourceInstance ? static_cast<UObject*>(SourceInstance) : static_cast<UObject*>(Item);
	if (bHasAbilitiesToGrant)
	{
		const TArray<FGameplayAbilitySpecHandle> Handles = OwnerPlayerState->GrantAbilitiesFromSource(AbilitiesToGrant, AbilitySourceObject, 1, true);
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("ApplyGrantedAbilitiesForSlot abilities aplicadas. Owner=%s Item=%s SlotValue=%d Source=%s Requested=%d Granted=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			SlotValue,
			*GetNameSafe(AbilitySourceObject),
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
		EffectContext.AddSourceObject(AbilitySourceObject ? AbilitySourceObject : Item);
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
			UE_LOG(LogSpellRiseEquipmentTrace, Log,
				TEXT("RemoveGrantedAbilitiesForSlot effects removidos. Owner=%s SlotValue=%d Count=%d"),
				*GetNameSafe(GetOwner()),
				SlotValue,
				EffectHandlesToRemove.Num());
		}
	}

	TArray<FGameplayAbilitySpecHandle> HandlesToRemove;
	if (!GrantedAbilityHandlesBySlot.RemoveAndCopyValue(SlotValue, HandlesToRemove))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
			TEXT("RemoveGrantedAbilitiesForSlot sem ability handles. Owner=%s SlotValue=%d"),
			*GetNameSafe(GetOwner()),
			SlotValue);
		return;
	}

	if (HandlesToRemove.Num() > 0)
	{
		if (ASpellRisePlayerState* OwnerPlayerState = CharacterOwner->GetPlayerState<ASpellRisePlayerState>())
		{
			OwnerPlayerState->RemoveAbilities(HandlesToRemove);
		}
		UE_LOG(LogSpellRiseEquipmentTrace, Log,
			TEXT("RemoveGrantedAbilitiesForSlot abilities removidas. Owner=%s SlotValue=%d Count=%d"),
			*GetNameSafe(GetOwner()),
			SlotValue,
			HandlesToRemove.Num());
	}
}

USpellRiseEquipmentInstance* USpellRiseEquipmentManagerComponent::GetOrCreateEquipmentInstanceForSlot(UEquippableItem* Item, uint8 SlotValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Item)
	{
		return nullptr;
	}

	if (const FSpellRiseAppliedEquipmentEntry* ExistingEntry = EquipmentList.FindBySlot(SlotValue))
	{
		return ExistingEntry->Instance;
	}

	if (TObjectPtr<USpellRiseEquipmentInstance>* ExistingInstance = EquipmentInstancesBySlot.Find(SlotValue))
	{
		if (IsValid(ExistingInstance->Get()))
		{
			return ExistingInstance->Get();
		}
		EquipmentInstancesBySlot.Remove(SlotValue);
	}

	USpellRiseEquipmentInstance* NewInstance = NewObject<USpellRiseEquipmentInstance>(GetOwner());
	if (!NewInstance)
	{
		return nullptr;
	}

	NewInstance->Initialize(this, Item);
	EquipmentInstancesBySlot.Add(SlotValue, NewInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(NewInstance);
	}

	return NewInstance;
}

void USpellRiseEquipmentManagerComponent::RemoveEquipmentInstanceForSlot(uint8 SlotValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	TObjectPtr<USpellRiseEquipmentInstance> RemovedInstance = nullptr;
	if (!EquipmentInstancesBySlot.RemoveAndCopyValue(SlotValue, RemovedInstance))
	{
		return;
	}

	if (RemovedInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(RemovedInstance);
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

	const USpellRiseWeaponDefinition* WeaponDefinition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, WeaponDefinition) && WeaponDefinition)
	{
		for (const TSoftClassPtr<UGameplayEffect>& EffectClassRef : WeaponDefinition->GrantedEffectClassesWhileEquipped)
		{
			UClass* EffectClass = EffectClassRef.LoadSynchronous();
			if (EffectClass)
			{
				OutEffects.AddUnique(EffectClass);
			}
		}
		return OutEffects.Num() > 0;
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) &&
		WeaponActorClass &&
		ExtractWeaponDefinitionFromObject(WeaponActorClass->GetDefaultObject(), WeaponDefinition) &&
		WeaponDefinition)
	{
		for (const TSoftClassPtr<UGameplayEffect>& EffectClassRef : WeaponDefinition->GrantedEffectClassesWhileEquipped)
		{
			UClass* EffectClass = EffectClassRef.LoadSynchronous();
			if (EffectClass)
			{
				OutEffects.AddUnique(EffectClass);
			}
		}
		return OutEffects.Num() > 0;
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

	const USpellRiseWeaponDefinition* WeaponDefinition = nullptr;
	if (ExtractWeaponDefinitionFromItem(Item, WeaponDefinition) && WeaponDefinition)
	{
		for (const TPair<FGameplayTag, float>& Magnitude : WeaponDefinition->SetByCallerMagnitudes)
		{
			if (Magnitude.Key.IsValid() && FMath::IsFinite(Magnitude.Value))
			{
				OutMagnitudes.Add(Magnitude.Key, Magnitude.Value);
			}
		}
		return OutMagnitudes.Num() > 0;
	}

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) &&
		WeaponActorClass &&
		ExtractWeaponDefinitionFromObject(WeaponActorClass->GetDefaultObject(), WeaponDefinition) &&
		WeaponDefinition)
	{
		for (const TPair<FGameplayTag, float>& Magnitude : WeaponDefinition->SetByCallerMagnitudes)
		{
			if (Magnitude.Key.IsValid() && FMath::IsFinite(Magnitude.Value))
			{
				OutMagnitudes.Add(Magnitude.Key, Magnitude.Value);
			}
		}
		return OutMagnitudes.Num() > 0;
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

bool USpellRiseEquipmentManagerComponent::ExtractWeaponDefinitionFromObject(const UObject* SourceObject, const USpellRiseWeaponDefinition*& OutWeaponDefinition) const
{
	OutWeaponDefinition = nullptr;
	if (!SourceObject)
	{
		return false;
	}

	static const TArray<FName> PreferredNames = {
		TEXT("WeaponDefinition"),
		TEXT("WeaponData"),
		TEXT("WeaponDefinitionAsset")
	};

	auto ResolveDefinitionFromProperty = [SourceObject](const FProperty* Property) -> const USpellRiseWeaponDefinition*
	{
		if (!Property)
		{
			return nullptr;
		}

		if (const FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
		{
			if (!ObjectProperty->PropertyClass || !ObjectProperty->PropertyClass->IsChildOf(USpellRiseWeaponDefinition::StaticClass()))
			{
				return nullptr;
			}

			return Cast<USpellRiseWeaponDefinition>(ObjectProperty->GetObjectPropertyValue_InContainer(SourceObject));
		}

		if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
		{
			if (!SoftObjectProperty->PropertyClass || !SoftObjectProperty->PropertyClass->IsChildOf(USpellRiseWeaponDefinition::StaticClass()))
			{
				return nullptr;
			}

			const void* ValuePtr = SoftObjectProperty->ContainerPtrToValuePtr<void>(SourceObject);
			const FSoftObjectPtr SoftObjectPtr = SoftObjectProperty->GetPropertyValue(ValuePtr);
			UObject* ResolvedObject = SoftObjectPtr.Get();
			if (!ResolvedObject)
			{
				ResolvedObject = SoftObjectPtr.LoadSynchronous();
			}
			return Cast<USpellRiseWeaponDefinition>(ResolvedObject);
		}

		return nullptr;
	};

	for (const FName PropertyName : PreferredNames)
	{
		if (const USpellRiseWeaponDefinition* Definition = ResolveDefinitionFromProperty(SourceObject->GetClass()->FindPropertyByName(PropertyName)))
		{
			OutWeaponDefinition = Definition;
			return true;
		}
	}

	for (TFieldIterator<FProperty> It(SourceObject->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		const FString PropertyName = It->GetName();
		if (!PropertyName.Contains(TEXT("Weapon"), ESearchCase::IgnoreCase) &&
			!PropertyName.Contains(TEXT("Definition"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		if (const USpellRiseWeaponDefinition* Definition = ResolveDefinitionFromProperty(*It))
		{
			OutWeaponDefinition = Definition;
			return true;
		}
	}

	return false;
}

bool USpellRiseEquipmentManagerComponent::ExtractWeaponDefinitionFromItem(UEquippableItem* Item, const USpellRiseWeaponDefinition*& OutWeaponDefinition) const
{
	return ExtractWeaponDefinitionFromObject(Item, OutWeaponDefinition);
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

	const USpellRiseWeaponDefinition* WeaponDefinition = GetWeaponDefinitionForItem(Item);
	if (WeaponDefinition)
	{
		return WeaponDefinition->HandPolicy == ESpellRiseWeaponHandPolicy::OffHand;
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

	const USpellRiseWeaponDefinition* WeaponDefinition = GetWeaponDefinitionForItem(Item);
	if (WeaponDefinition)
	{
		return WeaponDefinition->HandPolicy == ESpellRiseWeaponHandPolicy::TwoHanded;
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
				const UEnum* Enum = EnumProperty->GetEnum();
				Value = Enum ? Enum->GetNameStringByValue(IntValue) : FString();
				if (Enum)
				{
					Value += TEXT(" ");
					Value += Enum->GetDisplayNameTextByValue(IntValue).ToString();
				}
			}
			else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(*It))
			{
				const void* ValuePtr = ByteProperty->ContainerPtrToValuePtr<void>(ContainerPtr);
				const int64 IntValue = ByteProperty->GetUnsignedIntPropertyValue(ValuePtr);
				if (ByteProperty->Enum)
				{
					Value = ByteProperty->Enum->GetNameStringByValue(IntValue);
					Value += TEXT(" ");
					Value += ByteProperty->Enum->GetDisplayNameTextByValue(IntValue).ToString();
				}
				else
				{
					Value = FString::FromInt(static_cast<int32>(IntValue));
				}
			}
			else if (const FIntProperty* IntProperty = CastField<FIntProperty>(*It))
			{
				const void* ValuePtr = IntProperty->ContainerPtrToValuePtr<void>(ContainerPtr);
				const int32 IntValue = IntProperty->GetPropertyValue(ValuePtr);
				if ((PropertyName.Contains(TEXT("Hand"), ESearchCase::IgnoreCase) ||
					PropertyName.Contains(TEXT("Grip"), ESearchCase::IgnoreCase)) &&
					IntValue >= 2)
				{
					return true;
				}
			}

			if ((PropertyName.Contains(TEXT("Grip"), ESearchCase::IgnoreCase) || PropertyName.Contains(TEXT("Hand"), ESearchCase::IgnoreCase)) &&
				(Value.Contains(TEXT("TwoHand"), ESearchCase::IgnoreCase) ||
					Value.Contains(TEXT("Two Hand"), ESearchCase::IgnoreCase) ||
					Value.Contains(TEXT("BothHand"), ESearchCase::IgnoreCase) ||
					Value.Contains(TEXT("Both Hand"), ESearchCase::IgnoreCase) ||
					Value.Contains(TEXT("2H"), ESearchCase::IgnoreCase)))
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

	const bool bTwoHanded = ReadTwoHandedMetadata(Item, Item->GetClass()) || ReadTwoHandedMetadata(WeaponConfigPtr, WeaponConfigStruct);
	UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
		TEXT("IsTwoHandedWeaponItem. Owner=%s Item=%s WeaponClass=%s Result=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Item),
		*GetNameSafe(WeaponActorClass),
		bTwoHanded ? TEXT("true") : TEXT("false"));
	return bTwoHanded;
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
		RemoveGrantedAbilitiesForSlot(SpellRiseEquipmentSlots::OffHandSlot);
		bOffHandSuppressedByTwoHandedWeapon = false;
		EquippedOffHandWeapon = nullptr;
		BroadcastOffHandWeaponChangedIfNeeded();
		BroadcastHUDEquipmentSlotsChanged();
		return;
	}

	UEquippableItem* ActiveMainHandItem = QuickWeaponSlots.IsValidIndex(ActiveQuickWeaponSlotIndex)
		? QuickWeaponSlots[ActiveQuickWeaponSlotIndex]
		: nullptr;
	const bool bShouldSuppress = IsTwoHandedWeaponItem(ActiveMainHandItem);
	const bool bWasSuppressed = bOffHandSuppressedByTwoHandedWeapon;
	bOffHandSuppressedByTwoHandedWeapon = bShouldSuppress;

	GetOrSpawnWeaponActorForItem(ActiveOffHandItem);
	if (bShouldSuppress)
	{
		RemoveGrantedAbilitiesForSlot(SpellRiseEquipmentSlots::OffHandSlot);
		SetNarrativeItemActiveState(ActiveOffHandItem, true);
		RefreshOffHandVisual_Server(false);
		RefreshEquippedOffHandWeaponReference();
	}
	else
	{
		SetNarrativeItemActiveState(ActiveOffHandItem, true);
		RefreshOffHandVisual_Server(true);
		ApplyGrantedAbilitiesForSlot(ActiveOffHandItem, SpellRiseEquipmentSlots::OffHandSlot);
		RefreshEquippedOffHandWeaponReference();
	}

	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("RefreshOffHandSuppression. Owner=%s OffHand=%s MainHand=%s WasSuppressed=%s NowSuppressed=%s OffHandActor=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(ActiveOffHandItem),
		*GetNameSafe(ActiveMainHandItem),
		bWasSuppressed ? TEXT("true") : TEXT("false"),
		bOffHandSuppressedByTwoHandedWeapon ? TEXT("true") : TEXT("false"),
		*GetNameSafe(EquippedOffHandWeapon));

	BroadcastOffHandWeaponChangedIfNeeded();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
}

void USpellRiseEquipmentManagerComponent::RefreshWeaponLoadoutVisuals_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (int32 SlotIndex = 0; SlotIndex < QuickWeaponSlots.Num(); ++SlotIndex)
	{
		UEquippableItem* SlotItem = QuickWeaponSlots[SlotIndex];
		if (!SlotItem)
		{
			continue;
		}

		const bool bSlotEquipped = (SlotIndex == ActiveQuickWeaponSlotIndex);
		SetNarrativeItemActiveState(SlotItem, true);
		RefreshQuickSlotVisual_Server(SlotIndex, bSlotEquipped);
	}

	if (ActiveOffHandItem)
	{
		const bool bOffHandEquippedWithMainHand = IsOffHandGameplayActive();
		SetNarrativeItemActiveState(ActiveOffHandItem, true);
		RefreshOffHandVisual_Server(bOffHandEquippedWithMainHand);
	}
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
	SetNarrativeItemActiveState(Item, true);
	GetOrSpawnWeaponActorForItem(Item);
	RefreshOffHandVisual_Server(!bSuppressedByTwoHanded);
	if (bSuppressedByTwoHanded)
	{
		RemoveGrantedAbilitiesForSlot(SpellRiseEquipmentSlots::OffHandSlot);
	}
	else
	{
		ApplyGrantedAbilitiesForSlot(Item, SpellRiseEquipmentSlots::OffHandSlot);
	}
	RefreshEquippedOffHandWeaponReference();
	BroadcastOffHandWeaponChangedIfNeeded();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
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
		QuickWeaponSlots[ExistingSlot] = nullptr;
		if (QuickWeaponActors.IsValidIndex(ExistingSlot))
		{
			QuickWeaponActors[ExistingSlot] = nullptr;
		}
		const uint8 ExistingEquipmentSlot = static_cast<uint8>(SpellRiseEquipmentSlots::MainHandSlotBase + ExistingSlot);
		RemoveGrantedAbilitiesForSlot(ExistingEquipmentSlot);
		RemoveEquipmentInstanceForSlot(ExistingEquipmentSlot);
		if (bMovedFromActiveSlot)
		{
			ActiveQuickWeaponSlotIndex = INDEX_NONE;
		}
	}

	if (QuickWeaponSlots[QuickSlotIndex] && QuickWeaponSlots[QuickSlotIndex] != Item)
	{
		UEquippableItem* ReplacedItem = QuickWeaponSlots[QuickSlotIndex];
		QuickWeaponSlots[QuickSlotIndex] = nullptr;
		if (QuickWeaponActors.IsValidIndex(QuickSlotIndex))
		{
			QuickWeaponActors[QuickSlotIndex] = nullptr;
		}
		SetNarrativeItemActiveState(ReplacedItem, false);
		const uint8 ReplacedEquipmentSlot = static_cast<uint8>(SpellRiseEquipmentSlots::MainHandSlotBase + QuickSlotIndex);
		RemoveGrantedAbilitiesForSlot(ReplacedEquipmentSlot);
		RemoveEquipmentInstanceForSlot(ReplacedEquipmentSlot);
		if (bReplacingActiveSlot)
		{
			ActiveQuickWeaponSlotIndex = INDEX_NONE;
		}
		DestroyWeaponActorForItem(ReplacedItem);
	}

	QuickWeaponSlots[QuickSlotIndex] = Item;
	SetNarrativeItemActiveState(Item, true);
	AActor* SpawnedWeaponActor = GetOrSpawnWeaponActorForItem(Item);
	if (QuickWeaponActors.IsValidIndex(QuickSlotIndex))
	{
		QuickWeaponActors[QuickSlotIndex] = SpawnedWeaponActor;
	}

	if (bShouldKeepAssignedSlotActive)
	{
		ActiveQuickWeaponSlotIndex = QuickSlotIndex;
		RefreshQuickSlotVisual_Server(QuickSlotIndex, true);
		ApplyGrantedAbilitiesForSlot(Item, static_cast<uint8>(SpellRiseEquipmentSlots::MainHandSlotBase + QuickSlotIndex));
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
	RefreshWeaponLoadoutVisuals_Server();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();

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
		RefreshEquippedWeaponReference();
		RefreshOffHandSuppression_Server();
		RefreshWeaponLoadoutVisuals_Server();
		BroadcastWeaponChangedIfNeeded();
		BroadcastOffHandWeaponChangedIfNeeded();
		OnQuickWeaponSlotsChanged.Broadcast();
		BroadcastHUDEquipmentSlotsChanged();
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
			SetNarrativeItemActiveState(PreviouslyActive, true);
			RefreshQuickSlotVisual_Server(ActiveQuickWeaponSlotIndex, false);
			RemoveGrantedAbilitiesForSlot(static_cast<uint8>(SpellRiseEquipmentSlots::MainHandSlotBase + ActiveQuickWeaponSlotIndex));
		}
	}

	ActiveQuickWeaponSlotIndex = QuickSlotIndex;
	SetNarrativeItemActiveState(ItemToActivate, true);
	RefreshQuickSlotVisual_Server(QuickSlotIndex, true);
	ApplyGrantedAbilitiesForSlot(ItemToActivate, static_cast<uint8>(SpellRiseEquipmentSlots::MainHandSlotBase + QuickSlotIndex));

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	RefreshEquippedWeaponReference();
	BroadcastWeaponChangedIfNeeded();
	RefreshOffHandSuppression_Server();
	RefreshWeaponLoadoutVisuals_Server();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
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
	if (QuickWeaponActors.IsValidIndex(QuickSlotIndex))
	{
		QuickWeaponActors[QuickSlotIndex] = nullptr;
	}
	SetNarrativeItemActiveState(RemovedItem, false);
	const uint8 RemovedEquipmentSlot = static_cast<uint8>(SpellRiseEquipmentSlots::MainHandSlotBase + QuickSlotIndex);
	RemoveGrantedAbilitiesForSlot(RemovedEquipmentSlot);
	RemoveEquipmentInstanceForSlot(RemovedEquipmentSlot);

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
	RefreshWeaponLoadoutVisuals_Server();
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
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
	RemoveGrantedAbilitiesForSlot(SpellRiseEquipmentSlots::OffHandSlot);
	RemoveEquipmentInstanceForSlot(SpellRiseEquipmentSlots::OffHandSlot);
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
	BroadcastHUDEquipmentSlotsChanged();
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


	if (!SpellRiseDropPickup::ResolveDropPickupClass(DropPickupActorClass))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Error,
			TEXT("DropItem_Server abortado: classe de pickup indisponivel. Owner=%s ItemClass=%s"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(ItemClass));
		return false;
	}

	AActor* SpawnedPickupActor = nullptr;
	if (!SpawnPickupActorForDroppedItem_Server(ItemClass, SafeDropQuantity, SpawnLocation, SpawnRotation, SpawnedPickupActor))
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Error,
			TEXT("DropItem_Server falhou ao spawnar pickup antes da remocao. Owner=%s ItemClass=%s Quantity=%d"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(ItemClass),
			SafeDropQuantity);
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
		if (SpawnedPickupActor)
		{
			SpawnedPickupActor->Destroy();
		}
		UE_LOG(LogSpellRiseEquipmentTrace, Error,
			TEXT("DropItem_Server falhou ao remover item apos spawn do pickup. Owner=%s ItemClass=%s Quantity=%d"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(ItemClass),
			SafeDropQuantity);
		return false;
	}

	return true;
}

bool USpellRiseEquipmentManagerComponent::CheckServerEquipmentRpcRateLimit(
	const TCHAR* RpcName,
	FSpellRiseEquipmentRpcRateLimitState& RateState,
	const float WindowSeconds,
	const int32 MaxRequestsPerWindow,
	FString& OutRejectReason)
{
	OutRejectReason.Reset();
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		OutRejectReason = TEXT("not_authority");
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		OutRejectReason = TEXT("missing_world_context");
		return false;
	}

	const double NowSeconds = World->GetTimeSeconds();
	const double EffectiveWindowSeconds = static_cast<double>(FMath::Max(0.01f, WindowSeconds));
	const int32 EffectiveMaxRequests = FMath::Max(1, MaxRequestsPerWindow);

	if ((NowSeconds - RateState.WindowStartServerTimeSeconds) > EffectiveWindowSeconds)
	{
		RateState.WindowStartServerTimeSeconds = NowSeconds;
		RateState.RequestsInWindow = 0;
	}

	++RateState.RequestsInWindow;
	if (RateState.RequestsInWindow > EffectiveMaxRequests)
	{
		++RateState.RejectCount;
		OutRejectReason = FString::Printf(
			TEXT("rate_limited rpc=%s window=%.2f max=%d count=%d reject_count=%d"),
			RpcName ? RpcName : TEXT("unknown"),
			EffectiveWindowSeconds,
			EffectiveMaxRequests,
			RateState.RequestsInWindow,
			RateState.RejectCount);
		return false;
	}

	OutRejectReason = TEXT("accepted");
	return true;
}

void USpellRiseEquipmentManagerComponent::AuditRejectedEquipmentRpc(const TCHAR* RpcName, const FString& RejectReason)
{
	UE_LOG(LogSpellRiseEquipmentSecurity, Warning,
		TEXT("[RPC][Rejected] Rpc=%s Reason=%s Owner=%s"),
		RpcName ? RpcName : TEXT("unknown"),
		*RejectReason,
		*GetNameSafe(GetOwner()));
}

bool USpellRiseEquipmentManagerComponent::SpawnPickupActorForDroppedItem_Server(TSubclassOf<UNarrativeItem> ItemClass, int32 QuantityToDrop, const FVector& SpawnLocation, const FRotator& SpawnRotation, AActor*& OutSpawnedPickupActor)
{
	OutSpawnedPickupActor = nullptr;
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ItemClass || QuantityToDrop <= 0)
	{
		return false;
	}

	UClass* DropPickupClass = SpellRiseDropPickup::ResolveDropPickupClass(DropPickupActorClass);
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

	PickupActor->SetReplicates(true);
	PickupActor->SetReplicateMovement(true);
	PickupActor->SetNetUpdateFrequency(10.0f);

	const bool bInitializedBeforeConstruction = SpellRiseDropPickup::ApplyPickupRuntimeData(PickupActor, ItemClass, QuantityToDrop);
	if (!bInitializedBeforeConstruction)
	{
		PickupActor->Destroy();
		UE_LOG(LogSpellRiseEquipmentTrace, Error,
			TEXT("SpawnPickupActorForDroppedItem_Server abortado: nao foi possivel aplicar ItemClass/Quantity antes da construcao. PickupClass=%s ItemClass=%s Quantity=%d"),
			*GetNameSafe(DropPickupClass),
			*GetNameSafe(ItemClass),
			QuantityToDrop);
		return false;
	}

	UGameplayStatics::FinishSpawningActor(PickupActor, SpawnTransform);

	const bool bInitializedAfterConstruction = SpellRiseDropPickup::ApplyPickupRuntimeData(PickupActor, ItemClass, QuantityToDrop);
	const bool bInvokedInitialize = SpellRiseDropPickup::InvokeInitialize(PickupActor, ItemClass, QuantityToDrop);
	if (!bInitializedAfterConstruction)
	{
		PickupActor->Destroy();
		UE_LOG(LogSpellRiseEquipmentTrace, Error,
			TEXT("SpawnPickupActorForDroppedItem_Server abortado: ItemClass/Quantity perdidos apos FinishSpawning. Pickup=%s ItemClass=%s Quantity=%d"),
			*GetNameSafe(PickupActor),
			*GetNameSafe(ItemClass),
			QuantityToDrop);
		return false;
	}

	PickupActor->ForceNetUpdate();
	OutSpawnedPickupActor = PickupActor;
	UE_LOG(LogSpellRiseEquipmentTrace, Log,
		TEXT("Drop pickup spawnado no servidor. Pickup=%s PickupClass=%s ItemClass=%s Quantity=%d Initialize=%s"),
		*GetNameSafe(PickupActor),
		*GetNameSafe(DropPickupClass),
		*GetNameSafe(ItemClass),
		QuantityToDrop,
		bInvokedInitialize ? TEXT("true") : TEXT("false"));
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

	SpawnedWeapon->SetReplicates(true);
	SpawnedWeapon->SetReplicateMovement(false);
	SpawnedWeapon->bNetUseOwnerRelevancy = true;
	SpawnedWeapon->SetNetUpdateFrequency(15.0f);

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
	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh(TargetSocket);

	UClass* WeaponActorClass = nullptr;
	const void* WeaponConfigPtr = nullptr;
	const UStruct* WeaponConfigStruct = nullptr;
	if (!ResolveWeaponActorClassFromItem(Item, WeaponActorClass, WeaponConfigPtr, WeaponConfigStruct) || !WeaponActorClass)
	{
		return;
	}

	AActor* WeaponActor = EquippedOffHandWeapon;
	if (!IsValid(WeaponActor))
	{
		WeaponActor = ResolveOwnedWeaponActor(WeaponActorClass);
	}
	if (!AttachMesh || !WeaponActor || TargetSocket == NAME_None)
	{
		UE_LOG(LogSpellRiseEquipmentTrace, Verbose,
			TEXT("RefreshOffHandVisual_Local aguardando actor/mesh. Owner=%s Item=%s Equipped=%s AttachMesh=%s WeaponActor=%s ReplicatedOffHand=%s Socket=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Item),
			bEquipped ? TEXT("true") : TEXT("false"),
			*GetNameSafe(AttachMesh),
			*GetNameSafe(WeaponActor),
			*GetNameSafe(EquippedOffHandWeapon),
			*TargetSocket.ToString());
		if (UWorld* World = GetWorld())
		{
			const TWeakObjectPtr<USpellRiseEquipmentManagerComponent> WeakThis(this);
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakThis]()
			{
				if (USpellRiseEquipmentManagerComponent* Component = WeakThis.Get())
				{
					Component->RefreshOffHandVisual_Local(Component->IsOffHandGameplayActive());
				}
			}));
		}
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
	FName EquippedSocket = NAME_None;
	FName HolsterSocket = NAME_None;
	if (!WeaponActor || !ResolveWeaponSocketsForItem(ActiveOffHandItem, true, EquippedSocket, HolsterSocket))
	{
		return;
	}

	const FName TargetSocket = bEquipped ? EquippedSocket : HolsterSocket;
	USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh(TargetSocket);
	if (!AttachMesh)
	{
		return;
	}

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
	if (QuickWeaponActors.IsValidIndex(QuickSlotIndex))
	{
		QuickWeaponActors[QuickSlotIndex] = WeaponActor;
	}
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

	if (USkeletalMeshComponent* AttachMesh = ResolveEquipmentAttachMesh(TargetSocket))
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
		WeaponActor->ForceNetUpdate();
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
	if (QuickWeaponActors.Num() != QuickWeaponSlots.Num())
	{
		QuickWeaponActors.SetNum(QuickWeaponSlots.Num());
	}
	ReconcileReplicatedQuickSlotVisuals();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
}

void USpellRiseEquipmentManagerComponent::OnRep_QuickWeaponActors()
{
	ReconcileReplicatedQuickSlotVisuals();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
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
	BroadcastHUDEquipmentSlotsChanged();
}

void USpellRiseEquipmentManagerComponent::OnRep_EquippedWeapon()
{
	ReconcileReplicatedQuickSlotVisuals();
	BroadcastWeaponChangedIfNeeded();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
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
	BroadcastHUDEquipmentSlotsChanged();
}

void USpellRiseEquipmentManagerComponent::OnRep_OffHandSuppressedByTwoHandedWeapon()
{
	RefreshOffHandVisual_Local(IsOffHandGameplayActive());
	BroadcastOffHandWeaponChangedIfNeeded();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
}

void USpellRiseEquipmentManagerComponent::OnRep_EquippedOffHandWeapon()
{
	RefreshOffHandVisual_Local(IsOffHandGameplayActive());
	BroadcastOffHandWeaponChangedIfNeeded();
	OnQuickWeaponSlotsChanged.Broadcast();
	BroadcastHUDEquipmentSlotsChanged();
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
