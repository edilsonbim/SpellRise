#include "SpellRise/Equipment/SpellRiseEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Inventory/SpellRiseInventoryComponent.h"

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

	if (!Item.SupportsSlot(RequestedSlot) || !ValidateSlotConflict(Item, RequestedSlot, OutReason))
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
	ESpellRiseEquipmentRejectReason& OutReason) const
{
	if (FindPrivateIndexBySlot(Slot) != INDEX_NONE)
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
