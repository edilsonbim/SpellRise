#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/AssetManagerTypes.h"
#include "SpellRise/Inventory/SpellRiseItemTypes.h"
#include "SpellRiseInventoryViewModelComponent.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseInventoryRequestRejectedSignature,
	FName,
	RequestType,
	FName,
	Reason);

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

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Presentation")
	void ApplyAuthoritativeInventorySnapshot(const FSpellRiseInventoryViewSnapshot& Snapshot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Presentation")
	void ApplyAuthoritativeEquipmentSnapshot(const FSpellRiseEquipmentViewSnapshot& Snapshot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestMoveItem(FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestEquipItem(FGuid ItemInstanceId, ESpellRiseUIEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestUnequipItem(ESpellRiseUIEquipmentSlot EquipmentSlot, int32 PreferredInventorySlot);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|ViewModel|Requests")
	bool RequestDropItem(FGuid ItemInstanceId, int32 Quantity);

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

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|ViewModel|Events")
	FSpellRiseInventoryRequestRejectedSignature OnLocalRequestRejected;

private:
	bool RejectRequest(FName RequestType, FName Reason);
	void RefreshFromAuthoritativeComponents();
	int32 AllocateRequestId();

	UFUNCTION()
	void HandleInventoryChanged(ESpellRiseInventoryChangeType ChangeType, FSpellRiseItemInstance Item);

	UFUNCTION()
	void HandleEquipmentChanged();

	UPROPERTY(Transient)
	FSpellRiseInventoryViewSnapshot InventorySnapshot;

	UPROPERTY(Transient)
	FSpellRiseEquipmentViewSnapshot EquipmentSnapshot;

	int32 NextClientRequestId = 1;
};
