#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SpellRise/Inventory/SpellRiseItemTypes.h"
#include "SpellRiseStorageComponent.generated.h"

class USpellRiseInventoryComponent;
class USpellRiseItemDefinition;
class USpellRiseStorageComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellRiseStorageChanged, ESpellRiseInventoryChangeType, ChangeType, FSpellRiseItemInstance, Item);

USTRUCT()
struct SPELLRISE_API FSpellRiseStorageList : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpellRiseItemInstance> Entries;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSpellRiseItemInstance, FSpellRiseStorageList>(Entries, DeltaParms, *this);
	}

	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void SetOwner(USpellRiseStorageComponent* InOwner) { Owner = InOwner; }

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<USpellRiseStorageComponent> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FSpellRiseStorageList> : public TStructOpsTypeTraitsBase2<FSpellRiseStorageList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseStorageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseStorageComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="SpellRise|Storage")
	const TArray<FSpellRiseItemInstance>& GetItems() const { return Storage.Entries; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Storage")
	bool FindItem(const FGuid& ItemInstanceId, FSpellRiseItemInstance& OutItem) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Storage")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Storage")
	int32 GetMaxSlots() const { return MaxSlots; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Storage")
	float GetMaxWeight() const { return MaxWeight; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Storage")
	bool AddItem_Server(const FPrimaryAssetId& DefinitionId, int32 Quantity, int32 PreferredSlot, FGuid& OutItemInstanceId, FString& OutRejectReason);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Storage")
	bool RemoveItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FString& OutRejectReason);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Storage")
	bool MoveItem_Server(const FGuid& ItemInstanceId, int32 DestinationSlot, int32 Quantity, FString& OutRejectReason);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Storage")
	bool DepositFromInventory_Server(USpellRiseInventoryComponent* Inventory, const FGuid& InventoryItemInstanceId, int32 PreferredStorageSlot, int32 Quantity, FString& OutRejectReason);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Storage")
	bool WithdrawToInventory_Server(USpellRiseInventoryComponent* Inventory, const FGuid& StorageItemInstanceId, int32 PreferredInventorySlot, int32 Quantity, FString& OutRejectReason);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Storage")
	void ResetStorage_Server();

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Storage|Events")
	FSpellRiseStorageChanged OnStorageChanged;

	void NotifyReplicatedChange(ESpellRiseInventoryChangeType ChangeType, const FSpellRiseItemInstance& Item);

private:
	bool HasServerAuthority() const;
	bool ValidateSlot(int32 SlotIndex) const;
	int32 FindFreeSlot(int32 PreferredSlot = INDEX_NONE) const;
	int32 FindEntryIndexById(const FGuid& ItemInstanceId) const;
	int32 FindEntryIndexBySlot(int32 SlotIndex) const;
	bool CanAddWeight(const USpellRiseItemDefinition* Definition, int32 Quantity) const;
	bool ExtractItem_Server(const FGuid& ItemInstanceId, int32 Quantity, FSpellRiseItemInstance& OutExtractedItem, FString& OutRejectReason);
	bool InsertItem_Server(const FSpellRiseItemInstance& Item, int32 PreferredSlot, FString& OutRejectReason);
	bool RestoreExtractedItem_Server(const FSpellRiseItemInstance& Item, FString& OutRejectReason);
	const USpellRiseItemDefinition* ResolveDefinition(const FPrimaryAssetId& DefinitionId) const;
	void MarkEntryChanged(FSpellRiseItemInstance& Entry);
	void ForceNetUpdate() const;

	UPROPERTY(Replicated)
	FSpellRiseStorageList Storage;

	UPROPERTY(EditAnywhere, Category="SpellRise|Storage", meta=(ClampMin="1", ClampMax="500"))
	int32 MaxSlots = 40;

	UPROPERTY(EditAnywhere, Category="SpellRise|Storage", meta=(ClampMin="0.0"))
	float MaxWeight = 0.0f;

	UPROPERTY(EditAnywhere, Category="SpellRise|Storage|Security", meta=(ClampMin="1", ClampMax="1000"))
	int32 MaxQuantityPerOperation = 1000;

	int32 NextRevision = 1;
};
