#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/AssetManagerTypes.h"
#include "SpellRise/Inventory/SpellRiseItemTypes.h"
#include "SpellRiseInventoryViewModelComponent.generated.h"

class USpellRiseStorageComponent;

UENUM(BlueprintType)
enum class ESpellRiseInventoryContainerSource : uint8
{
	PlayerInventory,
	Storage
};

UENUM(BlueprintType)
enum class ESpellRiseUIEquipmentSlot : uint8
{
	Head,
	Chest,
	Hands,
	Legs,
	Feet,
	Necklace,
	Backpack,
	MainHand,
	OffHand
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseInventorySlotView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	FGuid ItemInstanceId;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	FPrimaryAssetId DefinitionId;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 Quantity = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 Durability = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 MaxDurability = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	uint8 Flags = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	bool bDropLocked = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	bool bPendingServerConfirmation = false;
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseEquipmentSlotView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	ESpellRiseUIEquipmentSlot Slot = ESpellRiseUIEquipmentSlot::Head;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	FGuid ItemInstanceId;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	FPrimaryAssetId DefinitionId;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	int32 Durability = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	int32 MaxDurability = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	bool bOccupied = false;
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseInventoryViewSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	TArray<FSpellRiseInventorySlotView> Slots;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 MaxSlots = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	float CurrentWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	float MaxWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 Revision = 0;
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseEquipmentViewSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	TArray<FSpellRiseEquipmentSlotView> Slots;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	float EquippedWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment")
	int32 Revision = 0;
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseInventoryContainerViewSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	ESpellRiseInventoryContainerSource Source = ESpellRiseInventoryContainerSource::PlayerInventory;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	TObjectPtr<USpellRiseStorageComponent> Storage = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	TArray<FSpellRiseInventorySlotView> Slots;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 MaxSlots = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	float CurrentWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	float MaxWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 Revision = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellRiseInventoryViewSnapshotChangedSignature,
	const FSpellRiseInventoryViewSnapshot&,
	Snapshot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellRiseEquipmentViewSnapshotChangedSignature,
	const FSpellRiseEquipmentViewSnapshot&,
	Snapshot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FSpellRiseInventoryMoveRequestedSignature,
	FGuid,
	ItemInstanceId,
	int32,
	DestinationSlot,
	int32,
	Quantity);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventoryEquipRequestedSignature,
	FGuid,
	ItemInstanceId,
	ESpellRiseUIEquipmentSlot,
	EquipmentSlot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventoryUnequipRequestedSignature,
	ESpellRiseUIEquipmentSlot,
	EquipmentSlot,
	int32,
	PreferredInventorySlot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventoryDropRequestedSignature,
	FGuid,
	ItemInstanceId,
	int32,
	Quantity);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellRiseInventoryUseRequestedSignature,
	FGuid,
	ItemInstanceId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventoryDestroyRequestedSignature,
	FGuid,
	ItemInstanceId,
	int32,
	Quantity);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseStorageViewSnapshotChangedSignature,
	USpellRiseStorageComponent*,
	Storage,
	const FSpellRiseInventoryContainerViewSnapshot&,
	Snapshot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventoryRequestRejectedSignature,
	FName,
	RequestType,
	FName,
	Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventorySlotRemovedSignature,
	int32,
	SlotIndex,
	FGuid,
	ItemInstanceId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FSpellRiseStorageSlotRemovedSignature,
	USpellRiseStorageComponent*,
	Storage,
	int32,
	SlotIndex,
	FGuid,
	ItemInstanceId);

/**
 * Presentation-only bridge for inventory and equipment widgets.
 * It owns no gameplay state, does not replicate and never mutates inventory directly.
 */
UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseInventoryViewModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseInventoryViewModelComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory|ViewModel")
	const FSpellRiseInventoryViewSnapshot& GetInventorySnapshot() const { return InventorySnapshot; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory|ViewModel")
	const FSpellRiseEquipmentViewSnapshot& GetEquipmentSnapshot() const { return EquipmentSnapshot; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory|ViewModel")
	FSpellRiseInventoryContainerViewSnapshot GetPlayerInventoryContainerSnapshot() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory|ViewModel|Storage")
	FSpellRiseInventoryContainerViewSnapshot GetStorageSnapshot(USpellRiseStorageComponent* Storage) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Presentation")
	void ApplyAuthoritativeInventorySnapshot(const FSpellRiseInventoryViewSnapshot& Snapshot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Presentation")
	void ApplyAuthoritativeEquipmentSnapshot(const FSpellRiseEquipmentViewSnapshot& Snapshot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Presentation")
	void RefreshPresentationFromAuthoritativeState();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestMoveItem(FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestMoveItemFromInventory(USpellRiseInventoryViewModelComponent* SourceInventoryVM, FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity);

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory|ViewModel|Requests")
	bool CanPreviewMoveItemFromInventory(USpellRiseInventoryViewModelComponent* SourceInventoryVM, const FSpellRiseInventorySlotView& SourceSlotView, int32 DestinationSlot, int32 Quantity, FName& OutReason) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	void SetTransferAuthorityInventoryViewModel(USpellRiseInventoryViewModelComponent* InventoryVM);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestEquipItem(FGuid ItemInstanceId, ESpellRiseUIEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestUnequipItem(ESpellRiseUIEquipmentSlot EquipmentSlot, int32 PreferredInventorySlot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestSwapEquipmentSlots(ESpellRiseUIEquipmentSlot FromSlot, ESpellRiseUIEquipmentSlot ToSlot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestDropItem(FGuid ItemInstanceId, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestUseItem(FGuid ItemInstanceId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestDestroyItem(FGuid ItemInstanceId, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Storage")
	bool RequestDepositToStorage(USpellRiseStorageComponent* Storage, FGuid InventoryItemInstanceId, int32 PreferredStorageSlot, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Storage")
	bool RequestWithdrawFromStorage(USpellRiseStorageComponent* Storage, FGuid StorageItemInstanceId, int32 PreferredInventorySlot, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Storage")
	bool RequestMoveStorageItem(USpellRiseStorageComponent* Storage, FGuid StorageItemInstanceId, int32 DestinationSlot, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Storage")
	void WatchStorage(USpellRiseStorageComponent* Storage);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Storage")
	void UnwatchStorage(USpellRiseStorageComponent* Storage);

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Events")
	FSpellRiseInventoryViewSnapshotChangedSignature OnInventorySnapshotChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Events")
	FSpellRiseEquipmentViewSnapshotChangedSignature OnEquipmentSnapshotChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Requests")
	FSpellRiseInventoryMoveRequestedSignature OnMoveItemRequested;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Requests")
	FSpellRiseInventoryEquipRequestedSignature OnEquipItemRequested;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Requests")
	FSpellRiseInventoryUnequipRequestedSignature OnUnequipItemRequested;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Requests")
	FSpellRiseInventoryDropRequestedSignature OnDropItemRequested;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Requests")
	FSpellRiseInventoryUseRequestedSignature OnUseItemRequested;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Requests")
	FSpellRiseInventoryDestroyRequestedSignature OnDestroyItemRequested;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Storage")
	FSpellRiseStorageViewSnapshotChangedSignature OnStorageSnapshotChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Events")
	FSpellRiseInventoryRequestRejectedSignature OnLocalRequestRejected;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Events")
	FSpellRiseInventorySlotRemovedSignature OnInventorySlotRemoved;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Storage")
	FSpellRiseStorageSlotRemovedSignature OnStorageSlotRemoved;

private:
	bool RejectRequest(FName RequestType, FName Reason);
	int32 CountFreeSlotsInSnapshot() const;
	void RefreshFromAuthoritativeComponents();
	void BroadcastStorageSnapshot(USpellRiseStorageComponent* Storage);
	int32 AllocateRequestId();
	bool ValidateStorageRequest_Local(FName RequestType, USpellRiseStorageComponent* Storage, const FGuid& ItemInstanceId, int32 Slot, int32 Quantity, bool bAllowAutoSlot);
	const FSpellRiseInventorySlotView* FindSnapshotSlotByIndex(int32 SlotIndex) const;
	USpellRiseInventoryComponent* GetOwnerInventoryComponent() const;
	USpellRiseStorageComponent* GetOwnerStorageComponent() const;

	UFUNCTION()
	void HandleInventoryChanged(ESpellRiseInventoryChangeType ChangeType, FSpellRiseItemInstance Item);

	UFUNCTION()
	void HandleEquipmentChanged();

	UFUNCTION()
	void HandleStorageChanged(ESpellRiseInventoryChangeType ChangeType, FSpellRiseItemInstance Item);

	UPROPERTY(Transient)
	FSpellRiseInventoryViewSnapshot InventorySnapshot;

	UPROPERTY(Transient)
	FSpellRiseEquipmentViewSnapshot EquipmentSnapshot;

	UPROPERTY(Transient)
	TSet<TObjectPtr<USpellRiseStorageComponent>> WatchedStorageComponents;

	UPROPERTY(Transient)
	TObjectPtr<USpellRiseInventoryViewModelComponent> TransferAuthorityInventoryVM;

	int32 NextClientRequestId = 1;
};
