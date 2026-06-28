#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRise/Equipment/SpellRiseEquipmentInventoryBridge.h"
#include "SpellRiseItemTypes.h"
#include "SpellRiseInventoryComponent.generated.h"

class USpellRiseItemDefinition;
class USpellRiseStorageComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellRiseInventoryChanged, ESpellRiseInventoryChangeType, ChangeType, FSpellRiseItemInstance, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellRiseInventoryRequestResolved, int32, ClientRequestId, ESpellRiseInventoryRequestResult, Result);

USTRUCT(BlueprintType)
struct FSpellRiseStarterInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Inventory")
	TSoftObjectPtr<USpellRiseItemDefinition> ItemDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Inventory", meta=(ClampMin="1", ClampMax="1000"))
	int32 Quantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Inventory", meta=(ClampMin="-1"))
	int32 PreferredSlot = INDEX_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Inventory")
	bool bNoDrop = true;
};

USTRUCT()
struct FSpellRiseInventoryRateLimitState
{
	GENERATED_BODY()

	double WindowStartSeconds = 0.0;
	int32 RequestsInWindow = 0;
};

UCLASS(ClassGroup=(SpellRise), BlueprintType, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseInventoryComponent : public UActorComponent, public ISpellRiseEquipmentInventoryBridge
{
	GENERATED_BODY()

public:
	USpellRiseInventoryComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory")
	const TArray<FSpellRiseItemInstance>& GetItems() const { return Inventory.Entries; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory")
	bool FindItem(const FGuid& ItemInstanceId, FSpellRiseItemInstance& OutItem) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory")
	int32 GetMaxSlots() const { return MaxSlots; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Inventory")
	float GetMaxWeight() const { return MaxWeight; }
	void SetMaxWeight_Server(float NewMaxWeight);
	bool IsNativeInventoryEnabled() const { return bNativeInventoryEnabled; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory")
	void RequestMoveItem(FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory")
	void RequestDropItem(FGuid ItemInstanceId, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory")
	void RequestUseItem(FGuid ItemInstanceId, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory")
	void RequestDestroyItem(FGuid ItemInstanceId, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|Storage")
	void RequestDepositToStorage(USpellRiseStorageComponent* Storage, FGuid ItemInstanceId, int32 PreferredStorageSlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|Storage")
	void RequestWithdrawFromStorage(USpellRiseStorageComponent* Storage, FGuid StorageItemInstanceId, int32 PreferredInventorySlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Inventory|Storage")
	void RequestMoveStorageItem(USpellRiseStorageComponent* Storage, FGuid StorageItemInstanceId, int32 DestinationSlot, int32 Quantity, int32 ClientRequestId);

	bool AddItem_Server(const FPrimaryAssetId& DefinitionId, int32 Quantity, int32 PreferredSlot, FGuid& OutItemInstanceId, FString& OutRejectReason);
	bool RemoveItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FString& OutRejectReason);
	bool MoveItem_Server(const FGuid& ItemInstanceId, int32 DestinationSlot, int32 Quantity, FString& OutRejectReason);
	bool UseItem_Server(const FGuid& ItemInstanceId, FString& OutRejectReason);
	bool DestroyItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FString& OutRejectReason);
	bool ExtractItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FSpellRiseItemInstance& OutExtractedItem, FString& OutRejectReason);
	bool InsertItem_Server(const FSpellRiseItemInstance& Item, int32 PreferredSlot, FString& OutRejectReason);
	bool EnsureStarterItems_Server(const TCHAR* ContextTag);
	void ResetInventory_Server();
	const USpellRiseItemDefinition* ResolveDefinition(const FPrimaryAssetId& DefinitionId) const;

	virtual bool Equipment_TakeItem(const FGuid& ItemInstanceId, FSpellRiseEquipmentItemData& OutItem, FString& OutRejectReason) override;
	virtual bool Equipment_ReturnItem(const FSpellRiseEquipmentItemData& Item, int32 PreferredInventorySlot, FString& OutRejectReason) override;
	virtual bool Equipment_CanReturnItem(const FSpellRiseEquipmentItemData& Item, int32 PreferredInventorySlot, FString& OutRejectReason) const override;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|Events")
	FSpellRiseInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Inventory|Events")
	FSpellRiseInventoryRequestResolved OnRequestResolved;

	void NotifyReplicatedChange(ESpellRiseInventoryChangeType ChangeType, const FSpellRiseItemInstance& Item);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestMoveItem(FGuid ItemInstanceId, int32 DestinationSlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestDropItem(FGuid ItemInstanceId, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestUseItem(FGuid ItemInstanceId, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestDestroyItem(FGuid ItemInstanceId, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestDepositToStorage(USpellRiseStorageComponent* Storage, FGuid ItemInstanceId, int32 PreferredStorageSlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestWithdrawFromStorage(USpellRiseStorageComponent* Storage, FGuid StorageItemInstanceId, int32 PreferredInventorySlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestMoveStorageItem(USpellRiseStorageComponent* Storage, FGuid StorageItemInstanceId, int32 DestinationSlot, int32 Quantity, int32 ClientRequestId);

	UFUNCTION(Client, Reliable)
	void ClientResolveRequest(int32 ClientRequestId, ESpellRiseInventoryRequestResult Result);

	UFUNCTION(BlueprintNativeEvent, Category="SpellRise|Inventory")
	bool CommitDrop_Server(const FSpellRiseItemInstance& DroppedItem);
	virtual bool CommitDrop_Server_Implementation(const FSpellRiseItemInstance& DroppedItem);

private:
	bool HasServerAuthority() const;
	bool IsRuntimeReady(FString& OutRejectReason) const;
	bool RestoreExtractedItem_Server(const FSpellRiseItemInstance& Item, FString& OutRejectReason);
	bool ValidateRequestId(int32 ClientRequestId, FString& OutRejectReason);
	bool CheckRateLimit(FSpellRiseInventoryRateLimitState& State, int32 MaxRequests, FString& OutRejectReason);
	bool ValidateStorageRequest(USpellRiseStorageComponent* Storage, const FGuid& ItemInstanceId, int32 Slot, int32 Quantity, bool bAllowAutoSlot, FString& OutRejectReason) const;
	bool ValidateSlot(int32 SlotIndex) const;
	int32 CountFreeSlots() const;
	int32 FindFreeSlot(int32 PreferredSlot = INDEX_NONE) const;
	int32 FindEntryIndexById(const FGuid& ItemInstanceId) const;
	int32 FindEntryIndexBySlot(int32 SlotIndex) const;
	bool CanAddWeight(const USpellRiseItemDefinition* Definition, int32 Quantity) const;
	bool MergeStackIntoSlot_Server(int32 SlotIndex, const USpellRiseItemDefinition* Definition, int32& InOutRemaining);
	bool PlaceInEmptySlot_Server(const FSpellRiseItemInstance& Template, int32 PreferredSlot, int32 MaxStackSize, int32& InOutRemaining);
	bool MoveItemOntoStack_Server(int32 SourceIndex, int32 DestinationSlot, int32 Quantity, FString& OutRejectReason);
	bool PlaceItemWithStacking_Server(const FSpellRiseItemInstance& Item, int32 PreferredSlot, FString& OutRejectReason);
	bool AddItemInternal_Server(const FPrimaryAssetId& DefinitionId, int32 Quantity, int32 PreferredSlot, uint8 Flags, FGuid& OutItemInstanceId, FString& OutRejectReason);
	bool AddResolvedItemInternal_Server(const USpellRiseItemDefinition* Definition, const FPrimaryAssetId& DefinitionId, int32 Quantity, int32 PreferredSlot, uint8 Flags, FGuid& OutItemInstanceId, FString& OutRejectReason);
	void MarkEntryChanged(FSpellRiseItemInstance& Entry);
	void ResolveRequest(int32 ClientRequestId, ESpellRiseInventoryRequestResult Result, const TCHAR* RpcName, const FString& Reason);
	ESpellRiseInventoryRequestResult MapRejectReason(const FString& Reason) const;
	void ForceOwnerNetUpdate() const;

	UPROPERTY(Replicated)
	FSpellRiseInventoryList Inventory;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Rollout")
	bool bNativeInventoryEnabled = false;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="SpellRise|Inventory", meta=(ClampMin="1", ClampMax="500"))
	int32 MaxSlots = 40;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="SpellRise|Inventory", meta=(ClampMin="0.0"))
	float MaxWeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="0.05"))
	float RateLimitWindowSeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="1", ClampMax="20"))
	int32 MoveRequestsPerWindow = 6;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="1", ClampMax="10"))
	int32 DropRequestsPerWindow = 2;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="1", ClampMax="20"))
	int32 UseRequestsPerWindow = 6;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="1", ClampMax="10"))
	int32 DestroyRequestsPerWindow = 2;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="1", ClampMax="20"))
	int32 StorageRequestsPerWindow = 6;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="100.0"))
	float MaxStorageInteractionDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Security", meta=(ClampMin="1", ClampMax="1000"))
	int32 MaxQuantityPerRequest = 1000;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Inventory|Starter")
	TArray<FSpellRiseStarterInventoryItem> StarterItems;

	FSpellRiseInventoryRateLimitState MoveRateLimit;
	FSpellRiseInventoryRateLimitState DropRateLimit;
	FSpellRiseInventoryRateLimitState UseRateLimit;
	FSpellRiseInventoryRateLimitState DestroyRateLimit;
	FSpellRiseInventoryRateLimitState StorageRateLimit;
	TSet<int32> RecentRequestIds;
	TArray<int32> RecentRequestOrder;
	int32 NextRevision = 1;
};
