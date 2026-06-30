#include "SpellRise/Equipment/SpellRiseEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"
#include "SpellRise/Inventory/SpellRiseItemDefinition.h"
#include "SpellRise/Inventory/SpellRiseItemDefinitionResolver.h"

DEFINE_LOG_CATEGORY(LogSpellRiseEquipment);

void FSpellRiseEquipmentPrivateList::PostReplicatedAdd(const TArrayView<int32>, int32)
{
	if (Owner) Owner->HandlePrivateReplicationChanged();
}

void FSpellRiseEquipmentPrivateList::PostReplicatedChange(const TArrayView<int32>, int32)
{
	if (Owner) Owner->HandlePrivateReplicationChanged();
}

void FSpellRiseEquipmentPrivateList::PreReplicatedRemove(const TArrayView<int32>, int32)
{
	if (Owner) Owner->HandlePrivateReplicationChanged();
}

void FSpellRiseEquipmentVisualList::PostReplicatedAdd(const TArrayView<int32>, int32)
{
	if (Owner) Owner->HandleVisualReplicationChanged();
}

void FSpellRiseEquipmentVisualList::PostReplicatedChange(const TArrayView<int32>, int32)
{
	if (Owner) Owner->HandleVisualReplicationChanged();
}

void FSpellRiseEquipmentVisualList::PreReplicatedRemove(const TArrayView<int32>, int32)
{
	if (Owner) Owner->HandleVisualReplicationChanged();
}

USpellRiseEquipmentComponent::USpellRiseEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
	PrivateEquipment.Owner = this;
	PublicVisualEquipment.Owner = this;
}

void USpellRiseEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	PrivateEquipment.Owner = this;
	PublicVisualEquipment.Owner = this;
}

void USpellRiseEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(USpellRiseEquipmentComponent, PrivateEquipment, COND_OwnerOnly);
}

void USpellRiseEquipmentComponent::RequestEquipItem(const FGuid& ItemInstanceId, const ESpellRiseEquipmentSlot RequestedSlot, const int32 ClientRequestId)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ServerRequestEquipItem_Implementation(ItemInstanceId, RequestedSlot, ClientRequestId);
		return;
	}
	ServerRequestEquipItem(ItemInstanceId, RequestedSlot, ClientRequestId);
}

void USpellRiseEquipmentComponent::RequestUnequipItem(const ESpellRiseEquipmentSlot Slot, const int32 PreferredInventorySlot, const int32 ClientRequestId)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ServerRequestUnequipItem_Implementation(Slot, PreferredInventorySlot, ClientRequestId);
		return;
	}
	ServerRequestUnequipItem(Slot, PreferredInventorySlot, ClientRequestId);
}

void USpellRiseEquipmentComponent::RequestSwapEquipmentSlots(
	const ESpellRiseEquipmentSlot FromSlot,
	const ESpellRiseEquipmentSlot ToSlot,
	const int32 ClientRequestId)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ServerRequestSwapEquipmentSlots_Implementation(FromSlot, ToSlot, ClientRequestId);
		return;
	}
	ServerRequestSwapEquipmentSlots(FromSlot, ToSlot, ClientRequestId);
}

void USpellRiseEquipmentComponent::ServerRequestEquipItem_Implementation(
	const FGuid ItemInstanceId, const ESpellRiseEquipmentSlot RequestedSlot, const int32 ClientRequestId)
{
	if (IsDuplicateRequest(ClientRequestId))
	{
		RejectRequest(TEXT("equip"), ESpellRiseEquipmentRejectReason::DuplicateRequest, ClientRequestId);
		return;
	}
	if (!ConsumeRateLimit(true))
	{
		RejectRequest(TEXT("equip"), ESpellRiseEquipmentRejectReason::RateLimited, ClientRequestId);
		return;
	}
	RecordAcceptedRequest(ClientRequestId);

	ESpellRiseEquipmentRejectReason Reason = ESpellRiseEquipmentRejectReason::None;
	if (!EquipItem_Server(ItemInstanceId, RequestedSlot, Reason))
	{
		RejectRequest(TEXT("equip"), Reason, ClientRequestId);
		return;
	}
}

void USpellRiseEquipmentComponent::ServerRequestUnequipItem_Implementation(
	const ESpellRiseEquipmentSlot Slot, const int32 PreferredInventorySlot, const int32 ClientRequestId)
{
	if (IsDuplicateRequest(ClientRequestId))
	{
		RejectRequest(TEXT("unequip"), ESpellRiseEquipmentRejectReason::DuplicateRequest, ClientRequestId);
		return;
	}
	if (!ConsumeRateLimit(false))
	{
		RejectRequest(TEXT("unequip"), ESpellRiseEquipmentRejectReason::RateLimited, ClientRequestId);
		return;
	}
	RecordAcceptedRequest(ClientRequestId);

	ESpellRiseEquipmentRejectReason Reason = ESpellRiseEquipmentRejectReason::None;
	if (!UnequipItem_Server(Slot, PreferredInventorySlot, Reason))
	{
		RejectRequest(TEXT("unequip"), Reason, ClientRequestId);
		return;
	}
}

void USpellRiseEquipmentComponent::ServerRequestSwapEquipmentSlots_Implementation(
	const ESpellRiseEquipmentSlot FromSlot,
	const ESpellRiseEquipmentSlot ToSlot,
	const int32 ClientRequestId)
{
	if (IsDuplicateRequest(ClientRequestId))
	{
		RejectRequest(TEXT("swap_equipment"), ESpellRiseEquipmentRejectReason::DuplicateRequest, ClientRequestId);
		return;
	}
	if (!ConsumeRateLimit(true))
	{
		RejectRequest(TEXT("swap_equipment"), ESpellRiseEquipmentRejectReason::RateLimited, ClientRequestId);
		return;
	}
	RecordAcceptedRequest(ClientRequestId);

	ESpellRiseEquipmentRejectReason Reason = ESpellRiseEquipmentRejectReason::None;
	if (!SwapEquipmentSlots_Server(FromSlot, ToSlot, Reason))
	{
		RejectRequest(TEXT("swap_equipment"), Reason, ClientRequestId);
	}
}

bool USpellRiseEquipmentComponent::EquipItem_Server(
	const FGuid& ItemInstanceId,
	const ESpellRiseEquipmentSlot RequestedSlot,
	ESpellRiseEquipmentRejectReason& OutReason)
{
	if (!ValidateEquipmentContext(OutReason) || !ItemInstanceId.IsValid())
	{
		if (!ItemInstanceId.IsValid()) OutReason = ESpellRiseEquipmentRejectReason::InvalidItem;
		return false;
	}

	ISpellRiseEquipmentInventoryBridge* Inventory = ResolveInventoryBridge();
	if (!Inventory)
	{
		OutReason = ESpellRiseEquipmentRejectReason::InventoryUnavailable;
		return false;
	}

	FSpellRiseEquipmentItemData Item;
	FString InventoryReason;
	if (!Inventory->Equipment_TakeItem(ItemInstanceId, Item, InventoryReason))
	{
		OutReason = ESpellRiseEquipmentRejectReason::InventoryRejected;
		return false;
	}

	if (!Item.SupportsSlot(RequestedSlot) || !ValidateSlotConflict(Item, RequestedSlot, OutReason, true))
	{
		FString RollbackReason;
		if (!Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason))
		{
			UE_LOG(LogSpellRiseEquipment, Error,
				TEXT("Equip rollback failed item=%s reason=%s"), *ItemInstanceId.ToString(), *RollbackReason);
			OutReason = ESpellRiseEquipmentRejectReason::TransactionFailed;
		}
		else if (!Item.SupportsSlot(RequestedSlot))
		{
			OutReason = ESpellRiseEquipmentRejectReason::SlotIncompatible;
		}
		return false;
	}

	const int32 OccupiedIndex = FindPrivateIndexBySlot(RequestedSlot);
	FSpellRiseEquipmentItemData SwappedOutItem;
	const bool bSwapOccupiedSlot = PrivateEquipment.Entries.IsValidIndex(OccupiedIndex);
	if (bSwapOccupiedSlot)
	{
		SwappedOutItem = PrivateEquipment.Entries[OccupiedIndex].ServerItemData;
		if (!Inventory->Equipment_CanReturnItem(SwappedOutItem, INDEX_NONE, InventoryReason))
		{
			FString RollbackReason;
			Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason);
			OutReason = ESpellRiseEquipmentRejectReason::InventoryRejected;
			return false;
		}

		RemoveGrants(SwappedOutItem.ItemInstanceId);
		RemoveReplicatedEntry(OccupiedIndex);
		if (!Inventory->Equipment_ReturnItem(SwappedOutItem, INDEX_NONE, InventoryReason))
		{
			FString RollbackReason;
			Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason);
			OutReason = ESpellRiseEquipmentRejectReason::TransactionFailed;
			return false;
		}
	}

	AddReplicatedEntry(Item, RequestedSlot);
	if (!ApplyGrants(Item))
	{
		RemoveReplicatedEntry(FindPrivateIndexBySlot(RequestedSlot));
		FString RollbackReason;
		Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason);
		OutReason = ESpellRiseEquipmentRejectReason::TransactionFailed;
		return false;
	}
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
	return true;
}

bool USpellRiseEquipmentComponent::SwapEquipmentSlots_Server(
	const ESpellRiseEquipmentSlot FromSlot,
	const ESpellRiseEquipmentSlot ToSlot,
	ESpellRiseEquipmentRejectReason& OutReason)
{
	if (!ValidateEquipmentContext(OutReason)) return false;
	if (FromSlot == ToSlot) return true;

	const int32 FromIndex = FindPrivateIndexBySlot(FromSlot);
	const int32 ToIndex = FindPrivateIndexBySlot(ToSlot);
	if (!PrivateEquipment.Entries.IsValidIndex(FromIndex) || !PrivateEquipment.Entries.IsValidIndex(ToIndex))
	{
		OutReason = ESpellRiseEquipmentRejectReason::InvalidSlot;
		return false;
	}

	FSpellRiseEquippedItemEntry& FromEntry = PrivateEquipment.Entries[FromIndex];
	FSpellRiseEquippedItemEntry& ToEntry = PrivateEquipment.Entries[ToIndex];
	if (!FromEntry.ServerItemData.SupportsSlot(ToSlot) || !ToEntry.ServerItemData.SupportsSlot(FromSlot))
	{
		OutReason = ESpellRiseEquipmentRejectReason::SlotIncompatible;
		return false;
	}

	FromEntry.Slot = ToSlot;
	ToEntry.Slot = FromSlot;
	FromEntry.Revision = ++EquipmentRevision;
	ToEntry.Revision = ++EquipmentRevision;
	PrivateEquipment.MarkItemDirty(FromEntry);
	PrivateEquipment.MarkItemDirty(ToEntry);
	RebuildVisualEntry(FromEntry);
	RebuildVisualEntry(ToEntry);
	OnEquipmentChanged.Broadcast();
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
	return true;
}

bool USpellRiseEquipmentComponent::UnequipItem_Server(
	const ESpellRiseEquipmentSlot Slot,
	const int32 PreferredInventorySlot,
	ESpellRiseEquipmentRejectReason& OutReason)
{
	if (!ValidateEquipmentContext(OutReason)) return false;

	const int32 EntryIndex = FindPrivateIndexBySlot(Slot);
	if (!PrivateEquipment.Entries.IsValidIndex(EntryIndex))
	{
		OutReason = ESpellRiseEquipmentRejectReason::InvalidSlot;
		return false;
	}

	ISpellRiseEquipmentInventoryBridge* Inventory = ResolveInventoryBridge();
	if (!Inventory)
	{
		OutReason = ESpellRiseEquipmentRejectReason::InventoryUnavailable;
		return false;
	}

	const FSpellRiseEquipmentItemData Item = PrivateEquipment.Entries[EntryIndex].ServerItemData;
	FString InventoryReason;
	if (!Inventory->Equipment_CanReturnItem(Item, PreferredInventorySlot, InventoryReason)
		|| !Inventory->Equipment_ReturnItem(Item, PreferredInventorySlot, InventoryReason))
	{
		OutReason = ESpellRiseEquipmentRejectReason::InventoryRejected;
		return false;
	}

	RemoveDamagedPenalty_Server(Item.ItemInstanceId, Slot);
	RemoveGrants(Item.ItemInstanceId);
	RemoveReplicatedEntry(EntryIndex);
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
	return true;
}

bool USpellRiseEquipmentComponent::ValidateEquipmentContext(ESpellRiseEquipmentRejectReason& OutReason) const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		OutReason = ESpellRiseEquipmentRejectReason::NotAuthority;
		return false;
	}
	if (!Cast<APlayerState>(OwnerActor))
	{
		OutReason = ESpellRiseEquipmentRejectReason::InvalidOwner;
		return false;
	}
	const ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(OwnerActor);
	const USpellRiseInventoryComponent* Inventory = PlayerState ? PlayerState->GetInventoryComponent() : nullptr;
	if (!PlayerState || !PlayerState->IsPersistenceProfileApplied() || !Inventory || !Inventory->IsNativeInventoryEnabled())
	{
		OutReason = ESpellRiseEquipmentRejectReason::InventoryUnavailable;
		return false;
	}
	if (!ResolveAbilitySystem())
	{
		OutReason = ESpellRiseEquipmentRejectReason::AbilitySystemUnavailable;
		return false;
	}
	return true;
}

bool USpellRiseEquipmentComponent::ValidateSlotConflict(
	const FSpellRiseEquipmentItemData& Item,
	const ESpellRiseEquipmentSlot Slot,
	ESpellRiseEquipmentRejectReason& OutReason,
	const bool bAllowOccupiedTarget) const
{
	if (!bAllowOccupiedTarget && FindPrivateIndexBySlot(Slot) != INDEX_NONE)
	{
		OutReason = ESpellRiseEquipmentRejectReason::SlotOccupied;
		return false;
	}

	if (Slot == ESpellRiseEquipmentSlot::OffHand)
	{
		const int32 MainIndex = FindPrivateIndexBySlot(ESpellRiseEquipmentSlot::MainHand);
		if (PrivateEquipment.Entries.IsValidIndex(MainIndex) && PrivateEquipment.Entries[MainIndex].ServerItemData.bTwoHanded)
		{
			OutReason = ESpellRiseEquipmentRejectReason::TwoHandConflict;
			return false;
		}
	}
	if (Slot == ESpellRiseEquipmentSlot::MainHand && Item.bTwoHanded
		&& FindPrivateIndexBySlot(ESpellRiseEquipmentSlot::OffHand) != INDEX_NONE)
	{
		OutReason = ESpellRiseEquipmentRejectReason::TwoHandConflict;
		return false;
	}
	return true;
}

ESpellRiseEquipmentSlot USpellRiseEquipmentComponent::ChooseBestSlotForItem(
	const FSpellRiseEquipmentItemData& Item,
	bool& bOutFound) const
{
	bOutFound = false;
	for (const ESpellRiseEquipmentSlot Slot : Item.AllowedSlots)
	{
		ESpellRiseEquipmentRejectReason Reason = ESpellRiseEquipmentRejectReason::None;
		if (FindPrivateIndexBySlot(Slot) == INDEX_NONE && ValidateSlotConflict(Item, Slot, Reason, false))
		{
			bOutFound = true;
			return Slot;
		}
	}
	for (const ESpellRiseEquipmentSlot Slot : Item.AllowedSlots)
	{
		ESpellRiseEquipmentRejectReason Reason = ESpellRiseEquipmentRejectReason::None;
		if (ValidateSlotConflict(Item, Slot, Reason, true))
		{
			bOutFound = true;
			return Slot;
		}
	}
	return ESpellRiseEquipmentSlot::MainHand;
}

bool USpellRiseEquipmentComponent::ConsumeRateLimit(const bool)
{
	const UWorld* World = GetWorld();
	if (!World) return false;
	const double Now = World->GetTimeSeconds();
	if (Now < BlockedUntilSeconds) return false;

	if (Now - RateWindowStartSeconds >= RateWindowSeconds)
	{
		ConsecutiveExceededWindows = RequestsInRateWindow > MaxRequestsPerWindow ? ConsecutiveExceededWindows + 1 : 0;
		RateWindowStartSeconds = Now;
		RequestsInRateWindow = 0;
	}
	++RequestsInRateWindow;
	if (RequestsInRateWindow <= MaxRequestsPerWindow) return true;

	++ConsecutiveExceededWindows;
	if (ConsecutiveExceededWindows >= 3)
	{
		BlockedUntilSeconds = Now + 1.0;
		ConsecutiveExceededWindows = 0;
	}
	return false;
}

bool USpellRiseEquipmentComponent::IsDuplicateRequest(const int32 ClientRequestId)
{
	return ClientRequestId <= 0 || RecentRequestIds.Contains(ClientRequestId);
}

void USpellRiseEquipmentComponent::RecordAcceptedRequest(const int32 ClientRequestId)
{
	RecentRequestIds.Add(ClientRequestId);
	if (RecentRequestIds.Num() > MaxRememberedRequestIds)
	{
		RecentRequestIds.RemoveAt(0, RecentRequestIds.Num() - MaxRememberedRequestIds, EAllowShrinking::No);
	}
}

bool USpellRiseEquipmentComponent::ApplyGrants(const FSpellRiseEquipmentItemData& Item)
{
	if (!Item.ItemInstanceId.IsValid()) return false;
	if (ActiveGrantHandles.Contains(Item.ItemInstanceId)) return true;
	UAbilitySystemComponent* ASC = ResolveAbilitySystem();
	if (!ASC) return false;

	USpellRiseEquipmentGrantSource* Source = NewObject<USpellRiseEquipmentGrantSource>(GetOwner());
	Source->ItemInstanceId = Item.ItemInstanceId;
	GrantSources.Add(Item.ItemInstanceId, Source);

	FGrantHandles Handles;
	const auto CleanupPendingHandles = [ASC, &Handles]()
	{
		for (const FGameplayAbilitySpecHandle Handle : Handles.AbilityHandles)
		{
			if (Handle.IsValid()) ASC->ClearAbility(Handle);
		}
		for (const FActiveGameplayEffectHandle Handle : Handles.EffectHandles)
		{
			if (Handle.IsValid()) ASC->RemoveActiveGameplayEffect(Handle);
		}
	};
	for (const FSpellRiseGrantedAbility& Grant : Item.GrantedAbilities)
	{
		TSubclassOf<UGameplayAbility> AbilityClass = Grant.AbilityClass.LoadSynchronous();
		if (!AbilityClass) { CleanupPendingHandles(); GrantSources.Remove(Item.ItemInstanceId); return false; }
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, Source);
		if (Grant.InputTag.IsValid()) Spec.GetDynamicSpecSourceTags().AddTag(Grant.InputTag);
		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		if (!Handle.IsValid()) { CleanupPendingHandles(); GrantSources.Remove(Item.ItemInstanceId); return false; }
		Handles.AbilityHandles.Add(Handle);
		if (Grant.bAutoActivateIfNoInputTag && !Grant.InputTag.IsValid())
		{
			ASC->TryActivateAbility(Handle);
		}
	}

	for (const TSoftClassPtr<UGameplayEffect>& EffectRef : Item.GrantedEffects)
	{
		TSubclassOf<UGameplayEffect> EffectClass = EffectRef.LoadSynchronous();
		if (!EffectClass) { CleanupPendingHandles(); GrantSources.Remove(Item.ItemInstanceId); return false; }
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(Source);
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
		if (!Spec.IsValid()) { CleanupPendingHandles(); GrantSources.Remove(Item.ItemInstanceId); return false; }
		for (const TPair<FGameplayTag, float>& Pair : Item.SetByCallerMagnitudes)
		{
			Spec.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
		}
		const FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
		if (!Handle.IsValid()) { CleanupPendingHandles(); GrantSources.Remove(Item.ItemInstanceId); return false; }
		Handles.EffectHandles.Add(Handle);
	}
	ActiveGrantHandles.Add(Item.ItemInstanceId, MoveTemp(Handles));
	return true;
}

void USpellRiseEquipmentComponent::RemoveGrants(const FGuid& ItemInstanceId)
{
	UAbilitySystemComponent* ASC = ResolveAbilitySystem();
	FGrantHandles* Handles = ActiveGrantHandles.Find(ItemInstanceId);
	if (ASC && Handles)
	{
		for (const FGameplayAbilitySpecHandle Handle : Handles->AbilityHandles) ASC->ClearAbility(Handle);
		for (const FActiveGameplayEffectHandle Handle : Handles->EffectHandles) ASC->RemoveActiveGameplayEffect(Handle);
	}
	ActiveGrantHandles.Remove(ItemInstanceId);
	GrantSources.Remove(ItemInstanceId);
}

void USpellRiseEquipmentComponent::AddReplicatedEntry(
	const FSpellRiseEquipmentItemData& Item,
	const ESpellRiseEquipmentSlot Slot)
{
	FSpellRiseEquippedItemEntry& Entry = PrivateEquipment.Entries.AddDefaulted_GetRef();
	Entry.ItemInstanceId = Item.ItemInstanceId;
	Entry.DefinitionId = Item.DefinitionId;
	Entry.Slot = Slot;
	Entry.Durability = Item.Durability;
	Entry.Flags = Item.Flags;
	Entry.Revision = ++EquipmentRevision;
	Entry.ServerItemData = Item;
	PrivateEquipment.MarkItemDirty(Entry);
	RebuildVisualEntry(Entry);
	OnEquipmentChanged.Broadcast();
}

void USpellRiseEquipmentComponent::RemoveReplicatedEntry(const int32 PrivateIndex)
{
	if (!PrivateEquipment.Entries.IsValidIndex(PrivateIndex)) return;
	const ESpellRiseEquipmentSlot Slot = PrivateEquipment.Entries[PrivateIndex].Slot;
	PrivateEquipment.Entries.RemoveAt(PrivateIndex);
	PrivateEquipment.MarkArrayDirty();
	const int32 VisualIndex = FindVisualIndexBySlot(Slot);
	if (PublicVisualEquipment.Entries.IsValidIndex(VisualIndex))
	{
		PublicVisualEquipment.Entries.RemoveAt(VisualIndex);
		PublicVisualEquipment.MarkArrayDirty();
	}
	OnEquipmentChanged.Broadcast();
}

void USpellRiseEquipmentComponent::RebuildVisualEntry(const FSpellRiseEquippedItemEntry& PrivateEntry)
{
	const int32 ExistingIndex = FindVisualIndexBySlot(PrivateEntry.Slot);
	FSpellRiseEquipmentVisualEntry& Visual = ExistingIndex != INDEX_NONE
		? PublicVisualEquipment.Entries[ExistingIndex]
		: PublicVisualEquipment.Entries.AddDefaulted_GetRef();
	Visual.DefinitionId = PrivateEntry.DefinitionId;
	Visual.Slot = PrivateEntry.Slot;
	Visual.bTwoHanded = PrivateEntry.ServerItemData.bTwoHanded;
	Visual.bSuppressed = PrivateEntry.Slot == ESpellRiseEquipmentSlot::OffHand
		&& FindPrivateIndexBySlot(ESpellRiseEquipmentSlot::MainHand) != INDEX_NONE
		&& PrivateEquipment.Entries[FindPrivateIndexBySlot(ESpellRiseEquipmentSlot::MainHand)].ServerItemData.bTwoHanded;
	PublicVisualEquipment.MarkItemDirty(Visual);
}

ISpellRiseEquipmentInventoryBridge* USpellRiseEquipmentComponent::ResolveInventoryBridge() const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return nullptr;
	TArray<UActorComponent*> Components;
	OwnerActor->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (Component && Component->GetClass()->ImplementsInterface(USpellRiseEquipmentInventoryBridge::StaticClass()))
		{
			return Cast<ISpellRiseEquipmentInventoryBridge>(Component);
		}
	}
	return nullptr;
}

UAbilitySystemComponent* USpellRiseEquipmentComponent::ResolveAbilitySystem() const
{
	if (const IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		return Interface->GetAbilitySystemComponent();
	}
	return nullptr;
}

void USpellRiseEquipmentComponent::RejectRequest(
	const TCHAR* Operation,
	const ESpellRiseEquipmentRejectReason Reason,
	const int32 ClientRequestId) const
{
	UE_LOG(LogSpellRiseEquipment, Warning,
		TEXT("Equipment RPC rejected operation=%s reason=%d request=%d owner=%s"),
		Operation, static_cast<int32>(Reason), ClientRequestId, *GetNameSafe(GetOwner()));
}

int32 USpellRiseEquipmentComponent::FindPrivateIndexBySlot(const ESpellRiseEquipmentSlot Slot) const
{
	return PrivateEquipment.Entries.IndexOfByPredicate(
		[Slot](const FSpellRiseEquippedItemEntry& Entry) { return Entry.Slot == Slot; });
}

int32 USpellRiseEquipmentComponent::FindVisualIndexBySlot(const ESpellRiseEquipmentSlot Slot) const
{
	return PublicVisualEquipment.Entries.IndexOfByPredicate(
		[Slot](const FSpellRiseEquipmentVisualEntry& Entry) { return Entry.Slot == Slot; });
}

bool USpellRiseEquipmentComponent::GetEquippedItem(
	const ESpellRiseEquipmentSlot Slot,
	FSpellRiseEquippedItemEntry& OutEntry) const
{
	const int32 Index = FindPrivateIndexBySlot(Slot);
	if (!PrivateEquipment.Entries.IsValidIndex(Index)) return false;
	OutEntry = PrivateEquipment.Entries[Index];
	return true;
}

float USpellRiseEquipmentComponent::GetEquippedWeight() const
{
	float Weight = 0.0f;
	for (const FSpellRiseEquippedItemEntry& Entry : PrivateEquipment.Entries)
	{
		Weight += FMath::Max(0.0f, Entry.ServerItemData.UnitWeight);
	}
	return Weight;
}

bool USpellRiseEquipmentComponent::EquipItemToBestSlot_Server(
	const FGuid& ItemInstanceId,
	FString& OutRejectReason)
{
	ESpellRiseEquipmentRejectReason RejectReason = ESpellRiseEquipmentRejectReason::None;
	if (!ValidateEquipmentContext(RejectReason))
	{
		OutRejectReason = FString::Printf(TEXT("equip_rejected:%d"), static_cast<int32>(RejectReason));
		return false;
	}

	ISpellRiseEquipmentInventoryBridge* Inventory = ResolveInventoryBridge();
	if (!Inventory)
	{
		OutRejectReason = TEXT("inventory_unavailable");
		return false;
	}

	FSpellRiseEquipmentItemData Item;
	FString InventoryReason;
	if (!Inventory->Equipment_TakeItem(ItemInstanceId, Item, InventoryReason))
	{
		OutRejectReason = InventoryReason.IsEmpty() ? TEXT("invalid_item") : InventoryReason;
		return false;
	}

	bool bFoundSlot = false;
	const ESpellRiseEquipmentSlot Slot = ChooseBestSlotForItem(Item, bFoundSlot);
	if (!bFoundSlot)
	{
		FString RollbackReason;
		Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason);
		OutRejectReason = TEXT("slot_incompatible");
		return false;
	}

	FString RollbackReason;
	if (!Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason))
	{
		Inventory->Equipment_ReturnItem(Item, INDEX_NONE, RollbackReason);
		OutRejectReason = TEXT("transaction_failed");
		return false;
	}

	if (!EquipItem_Server(ItemInstanceId, Slot, RejectReason))
	{
		OutRejectReason = FString::Printf(TEXT("equip_rejected:%d"), static_cast<int32>(RejectReason));
		return false;
	}

	return true;
}

bool USpellRiseEquipmentComponent::UnequipItemById_Server(
	const FGuid& ItemInstanceId,
	const int32 PreferredInventorySlot,
	FString& OutRejectReason)
{
	ESpellRiseEquipmentSlot Slot = ESpellRiseEquipmentSlot::Head;
	if (!IsItemEquipped(ItemInstanceId, Slot))
	{
		OutRejectReason = TEXT("invalid_item");
		return false;
	}

	ESpellRiseEquipmentRejectReason RejectReason = ESpellRiseEquipmentRejectReason::None;
	if (!UnequipItem_Server(Slot, PreferredInventorySlot, RejectReason))
	{
		OutRejectReason = FString::Printf(TEXT("unequip_rejected:%d"), static_cast<int32>(RejectReason));
		return false;
	}
	return true;
}

bool USpellRiseEquipmentComponent::IsItemEquipped(
	const FGuid& ItemInstanceId,
	ESpellRiseEquipmentSlot& OutSlot) const
{
	const int32 Index = PrivateEquipment.Entries.IndexOfByPredicate(
		[&ItemInstanceId](const FSpellRiseEquippedItemEntry& Entry)
		{
			return Entry.ItemInstanceId == ItemInstanceId;
		});
	if (!PrivateEquipment.Entries.IsValidIndex(Index))
	{
		return false;
	}

	OutSlot = PrivateEquipment.Entries[Index].Slot;
	return true;
}

bool USpellRiseEquipmentComponent::RestoreEquippedItem_Server(
	const FSpellRiseEquipmentItemData& Item,
	const ESpellRiseEquipmentSlot Slot,
	FString& OutRejectReason)
{
	ESpellRiseEquipmentRejectReason RejectReason = ESpellRiseEquipmentRejectReason::None;
	if (!ValidateEquipmentContext(RejectReason)
		|| !Item.ItemInstanceId.IsValid()
		|| !Item.SupportsSlot(Slot)
		|| !ValidateSlotConflict(Item, Slot, RejectReason))
	{
		OutRejectReason = FString::Printf(TEXT("restore_rejected:%d"), static_cast<int32>(RejectReason));
		return false;
	}
	AddReplicatedEntry(Item, Slot);
	if (!ApplyGrants(Item))
	{
		RemoveReplicatedEntry(FindPrivateIndexBySlot(Slot));
		OutRejectReason = TEXT("restore_grants_failed");
		return false;
	}
	CheckAndUpdateDamagedState_Server(FindPrivateIndexBySlot(Slot));
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	return true;
}

void USpellRiseEquipmentComponent::ResetEquipment_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	for (const FSpellRiseEquippedItemEntry& Entry : PrivateEquipment.Entries)
	{
		RemoveDamagedPenalty_Server(Entry.ItemInstanceId, Entry.Slot);
	}
	TArray<FGuid> GrantedIds;
	ActiveGrantHandles.GetKeys(GrantedIds);
	for (const FGuid& ItemId : GrantedIds)
	{
		RemoveGrants(ItemId);
	}
	PrivateEquipment.Entries.Reset();
	PrivateEquipment.MarkArrayDirty();
	PublicVisualEquipment.Entries.Reset();
	PublicVisualEquipment.MarkArrayDirty();
	OnEquipmentChanged.Broadcast();
}

void USpellRiseEquipmentComponent::HandlePrivateReplicationChanged()
{
	OnEquipmentChanged.Broadcast();
}

void USpellRiseEquipmentComponent::HandleVisualReplicationChanged()
{
	OnEquipmentChanged.Broadcast();
}

void USpellRiseEquipmentComponent::OnHitTakenByOwner_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (int32 i = PrivateEquipment.Entries.Num() - 1; i >= 0; --i)
	{
		const FSpellRiseEquippedItemEntry& Entry = PrivateEquipment.Entries[i];
		const USpellRiseItemDefinition* Def = SpellRiseItemDefinitionResolver::ResolveItemDefinition(Entry.DefinitionId);
		if (!Def || Def->MaxDurability <= 0 || Def->ItemKind != ESpellRiseItemKind::Armor) continue;
		const USpellRiseArmorItemDefinition* ArmorDef = Cast<USpellRiseArmorItemDefinition>(Def);
		if (!ArmorDef || ArmorDef->DurabilityLossWhenHit <= 0) continue;
		ApplyDurabilityLoss_Server(i, ArmorDef->DurabilityLossWhenHit);
	}
}

void USpellRiseEquipmentComponent::OnHitGivenByOwner_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	static const ESpellRiseEquipmentSlot WeaponSlots[] = {
		ESpellRiseEquipmentSlot::MainHand, ESpellRiseEquipmentSlot::OffHand
	};
	for (const ESpellRiseEquipmentSlot Slot : WeaponSlots)
	{
		const int32 Index = FindPrivateIndexBySlot(Slot);
		if (!PrivateEquipment.Entries.IsValidIndex(Index)) continue;
		const FSpellRiseEquippedItemEntry& Entry = PrivateEquipment.Entries[Index];
		const USpellRiseItemDefinition* Def = SpellRiseItemDefinitionResolver::ResolveItemDefinition(Entry.DefinitionId);
		if (!Def || Def->MaxDurability <= 0 || Def->ItemKind != ESpellRiseItemKind::Weapon) continue;
		const USpellRiseWeaponItemDefinition* WeaponDef = Cast<USpellRiseWeaponItemDefinition>(Def);
		if (!WeaponDef || WeaponDef->DurabilityLossPerAttack <= 0) continue;
		ApplyDurabilityLoss_Server(Index, WeaponDef->DurabilityLossPerAttack);
	}
}

void USpellRiseEquipmentComponent::ApplyDurabilityLoss_Server(const int32 PrivateIndex, const int32 Loss)
{
	if (!PrivateEquipment.Entries.IsValidIndex(PrivateIndex) || Loss <= 0) return;

	FSpellRiseEquippedItemEntry& Entry = PrivateEquipment.Entries[PrivateIndex];
	const int32 Previous = Entry.Durability;
	Entry.Durability = FMath::Max(0, Entry.Durability - Loss);
	Entry.ServerItemData.Durability = Entry.Durability;

	UE_LOG(LogSpellRiseEquipment, Verbose,
		TEXT("[Durability][Decay] Owner=%s Slot=%d Item=%s Durability=%d->%d"),
		*GetNameSafe(GetOwner()),
		static_cast<int32>(Entry.Slot),
		*Entry.ItemInstanceId.ToString(),
		Previous,
		Entry.Durability);

	if (Entry.Durability <= 0)
	{
		DestroyEquippedItem_Server(PrivateIndex);
		return;
	}

	CheckAndUpdateDamagedState_Server(PrivateIndex);
	Entry.Revision = ++EquipmentRevision;
	PrivateEquipment.MarkItemDirty(Entry);
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
}

void USpellRiseEquipmentComponent::DestroyEquippedItem_Server(const int32 PrivateIndex)
{
	if (!PrivateEquipment.Entries.IsValidIndex(PrivateIndex)) return;

	const FGuid ItemInstanceId = PrivateEquipment.Entries[PrivateIndex].ItemInstanceId;
	const ESpellRiseEquipmentSlot Slot = PrivateEquipment.Entries[PrivateIndex].Slot;
	const FPrimaryAssetId DefinitionId = PrivateEquipment.Entries[PrivateIndex].DefinitionId;

	UE_LOG(LogSpellRiseEquipment, Warning,
		TEXT("[Durability][Destroyed] Owner=%s Slot=%d Item=%s Definition=%s"),
		*GetNameSafe(GetOwner()),
		static_cast<int32>(Slot),
		*ItemInstanceId.ToString(),
		*DefinitionId.ToString());

	RemoveDamagedPenalty_Server(ItemInstanceId, Slot);
	RemoveGrants(ItemInstanceId);
	RemoveReplicatedEntry(PrivateIndex);
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();
}

FGameplayTag USpellRiseEquipmentComponent::GetDamagedTagForSlot(const ESpellRiseEquipmentSlot Slot)
{
	switch (Slot)
	{
	case ESpellRiseEquipmentSlot::Head:     return FGameplayTag::RequestGameplayTag("Item.Damaged.Head", false);
	case ESpellRiseEquipmentSlot::Chest:    return FGameplayTag::RequestGameplayTag("Item.Damaged.Chest", false);
	case ESpellRiseEquipmentSlot::Hands:    return FGameplayTag::RequestGameplayTag("Item.Damaged.Hands", false);
	case ESpellRiseEquipmentSlot::Legs:     return FGameplayTag::RequestGameplayTag("Item.Damaged.Legs", false);
	case ESpellRiseEquipmentSlot::Feet:     return FGameplayTag::RequestGameplayTag("Item.Damaged.Feet", false);
	case ESpellRiseEquipmentSlot::MainHand: return FGameplayTag::RequestGameplayTag("Item.Damaged.MainHand", false);
	case ESpellRiseEquipmentSlot::OffHand:  return FGameplayTag::RequestGameplayTag("Item.Damaged.OffHand", false);
	default:                                return FGameplayTag();
	}
}

void USpellRiseEquipmentComponent::CheckAndUpdateDamagedState_Server(const int32 PrivateIndex)
{
	if (!PrivateEquipment.Entries.IsValidIndex(PrivateIndex)) return;

	const FSpellRiseEquippedItemEntry& Entry = PrivateEquipment.Entries[PrivateIndex];
	const USpellRiseItemDefinition* Def = SpellRiseItemDefinitionResolver::ResolveItemDefinition(Entry.DefinitionId);
	if (!Def || Def->MaxDurability <= 0) return;

	const bool bIsDamaged = Entry.Durability < static_cast<int32>(Def->MaxDurability * 0.2f);
	const bool bTagged = DamagedTaggedItems.Contains(Entry.ItemInstanceId);

	if (bIsDamaged && !bTagged)
	{
		ApplyDamagedPenalty_Server(Entry.ItemInstanceId, Entry.Slot);
	}
	else if (!bIsDamaged && bTagged)
	{
		RemoveDamagedPenalty_Server(Entry.ItemInstanceId, Entry.Slot);
	}
}

void USpellRiseEquipmentComponent::ApplyDamagedPenalty_Server(
	const FGuid& ItemInstanceId,
	const ESpellRiseEquipmentSlot Slot)
{
	UAbilitySystemComponent* ASC = ResolveAbilitySystem();
	if (!ASC) return;

	const FGameplayTag DamagedTag = GetDamagedTagForSlot(Slot);
	if (DamagedTag.IsValid())
	{
		ASC->AddLooseGameplayTag(DamagedTag);
		DamagedTaggedItems.Add(ItemInstanceId);
		UE_LOG(LogSpellRiseEquipment, Log,
			TEXT("[Durability][Damaged] Owner=%s Slot=%d Item=%s Tag=%s"),
			*GetNameSafe(GetOwner()), static_cast<int32>(Slot),
			*ItemInstanceId.ToString(), *DamagedTag.ToString());
	}
}

void USpellRiseEquipmentComponent::RemoveDamagedPenalty_Server(
	const FGuid& ItemInstanceId,
	const ESpellRiseEquipmentSlot Slot)
{
	if (!DamagedTaggedItems.Contains(ItemInstanceId)) return;

	UAbilitySystemComponent* ASC = ResolveAbilitySystem();
	const FGameplayTag DamagedTag = GetDamagedTagForSlot(Slot);
	if (DamagedTag.IsValid() && ASC)
	{
		ASC->RemoveLooseGameplayTag(DamagedTag);
	}
	DamagedTaggedItems.Remove(ItemInstanceId);
}

void USpellRiseEquipmentComponent::RequestRepairItem(FGuid ItemInstanceId, const int32 ClientRequestId)
{
	ServerRequestRepairItem(ItemInstanceId, ClientRequestId);
}

void USpellRiseEquipmentComponent::ServerRequestRepairItem_Implementation(
	const FGuid ItemInstanceId,
	const int32 ClientRequestId)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (!ItemInstanceId.IsValid())
	{
		RejectRequest(TEXT("repair"), ESpellRiseEquipmentRejectReason::InvalidItem, ClientRequestId);
		return;
	}
	if (IsDuplicateRequest(ClientRequestId))
	{
		RejectRequest(TEXT("repair"), ESpellRiseEquipmentRejectReason::DuplicateRequest, ClientRequestId);
		return;
	}
	if (!ConsumeRateLimit(false))
	{
		RejectRequest(TEXT("repair"), ESpellRiseEquipmentRejectReason::RateLimited, ClientRequestId);
		return;
	}

	// TODO: deduct repair cost (economy system)

	const int32 EquippedIndex = PrivateEquipment.Entries.IndexOfByPredicate(
		[&ItemInstanceId](const FSpellRiseEquippedItemEntry& E)
		{
			return E.ItemInstanceId == ItemInstanceId;
		});

	if (PrivateEquipment.Entries.IsValidIndex(EquippedIndex))
	{
		if (!RepairEquippedItem_Server(EquippedIndex))
		{
			RejectRequest(TEXT("repair"), ESpellRiseEquipmentRejectReason::InvalidItem, ClientRequestId);
		}
		else
		{
			RecordAcceptedRequest(ClientRequestId);
		}
		return;
	}

	if (!RepairInventoryItem_Server(ItemInstanceId))
	{
		RejectRequest(TEXT("repair"), ESpellRiseEquipmentRejectReason::InvalidItem, ClientRequestId);
	}
	else
	{
		RecordAcceptedRequest(ClientRequestId);
	}
}

bool USpellRiseEquipmentComponent::RepairEquippedItem_Server(const int32 PrivateIndex)
{
	if (!PrivateEquipment.Entries.IsValidIndex(PrivateIndex)) return false;

	FSpellRiseEquippedItemEntry& Entry = PrivateEquipment.Entries[PrivateIndex];
	const USpellRiseItemDefinition* Def = SpellRiseItemDefinitionResolver::ResolveItemDefinition(Entry.DefinitionId);
	if (!Def || Def->MaxDurability <= 0 || Entry.Durability >= Def->MaxDurability) return false;

	const int32 Previous = Entry.Durability;
	Entry.Durability = Def->MaxDurability;
	Entry.ServerItemData.Durability = Def->MaxDurability;

	CheckAndUpdateDamagedState_Server(PrivateIndex);

	Entry.Revision = ++EquipmentRevision;
	PrivateEquipment.MarkItemDirty(Entry);
	if (AActor* OwnerActor = GetOwner()) OwnerActor->ForceNetUpdate();

	UE_LOG(LogSpellRiseEquipment, Log,
		TEXT("[Repair][Equipped] Owner=%s Slot=%d Item=%s Durability=%d->%d"),
		*GetNameSafe(GetOwner()), static_cast<int32>(Entry.Slot),
		*Entry.ItemInstanceId.ToString(), Previous, Entry.Durability);
	return true;
}

bool USpellRiseEquipmentComponent::RepairInventoryItem_Server(const FGuid& ItemInstanceId)
{
	const ASpellRisePlayerState* PS = Cast<ASpellRisePlayerState>(GetOwner());
	USpellRiseInventoryComponent* Inventory = PS ? PS->GetInventoryComponent() : nullptr;
	if (!Inventory) return false;

	FString RejectReason;
	if (!Inventory->RepairItem_Server(ItemInstanceId, RejectReason))
	{
		UE_LOG(LogSpellRiseEquipment, Warning,
			TEXT("[Repair][Inventory] Rejected Item=%s Reason=%s"),
			*ItemInstanceId.ToString(), *RejectReason);
		return false;
	}
	return true;
}
