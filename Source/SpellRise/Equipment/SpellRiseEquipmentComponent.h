#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SpellRise/Equipment/SpellRiseEquipmentInventoryBridge.h"
#include "SpellRiseEquipmentComponent.generated.h"

class UAbilitySystemComponent;
class USpellRiseEquipmentComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogSpellRiseEquipment, Log, All);

UENUM(BlueprintType)
enum class ESpellRiseEquipmentRejectReason : uint8
{
	None,
	NotAuthority,
	InvalidOwner,
	InvalidItem,
	InvalidSlot,
	SlotIncompatible,
	SlotOccupied,
	TwoHandConflict,
	InventoryUnavailable,
	InventoryRejected,
	RateLimited,
	DuplicateRequest,
	AbilitySystemUnavailable,
	TransactionFailed
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseEquippedItemEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid ItemInstanceId;

	UPROPERTY(BlueprintReadOnly)
	FPrimaryAssetId DefinitionId;

	UPROPERTY(BlueprintReadOnly)
	ESpellRiseEquipmentSlot Slot = ESpellRiseEquipmentSlot::Head;

	UPROPERTY(BlueprintReadOnly)
	int32 Durability = 0;

	UPROPERTY(BlueprintReadOnly)
	uint8 Flags = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Revision = 0;

	UPROPERTY(NotReplicated)
	FSpellRiseEquipmentItemData ServerItemData;
};

USTRUCT()
struct SPELLRISE_API FSpellRiseEquipmentPrivateList : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpellRiseEquippedItemEntry> Entries;

	USpellRiseEquipmentComponent* Owner = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSpellRiseEquippedItemEntry, FSpellRiseEquipmentPrivateList>(
			Entries, DeltaParms, *this);
	}

	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
};

template<>
struct TStructOpsTypeTraits<FSpellRiseEquipmentPrivateList> : TStructOpsTypeTraitsBase2<FSpellRiseEquipmentPrivateList>
{
	enum { WithNetDeltaSerializer = true };
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseEquipmentVisualEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FPrimaryAssetId DefinitionId;

	UPROPERTY(BlueprintReadOnly)
	ESpellRiseEquipmentSlot Slot = ESpellRiseEquipmentSlot::Head;

	UPROPERTY(BlueprintReadOnly)
	bool bDrawn = false;

	UPROPERTY(BlueprintReadOnly)
	bool bTwoHanded = false;

	UPROPERTY(BlueprintReadOnly)
	bool bSuppressed = false;
};

USTRUCT()
struct SPELLRISE_API FSpellRiseEquipmentVisualList : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpellRiseEquipmentVisualEntry> Entries;

	USpellRiseEquipmentComponent* Owner = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSpellRiseEquipmentVisualEntry, FSpellRiseEquipmentVisualList>(
			Entries, DeltaParms, *this);
	}

	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
};

template<>
struct TStructOpsTypeTraits<FSpellRiseEquipmentVisualList> : TStructOpsTypeTraitsBase2<FSpellRiseEquipmentVisualList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS()
class SPELLRISE_API USpellRiseEquipmentGrantSource : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid ItemInstanceId;
};

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseEquipmentComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	void RequestEquipItem(const FGuid& ItemInstanceId, ESpellRiseEquipmentSlot RequestedSlot, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	void RequestUnequipItem(ESpellRiseEquipmentSlot Slot, int32 PreferredInventorySlot, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	void RequestSwapEquipmentSlots(ESpellRiseEquipmentSlot FromSlot, ESpellRiseEquipmentSlot ToSlot, int32 ClientRequestId);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	void RequestRepairItem(FGuid ItemInstanceId, int32 ClientRequestId);

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	bool GetEquippedItem(ESpellRiseEquipmentSlot Slot, FSpellRiseEquippedItemEntry& OutEntry) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	float GetEquippedWeight() const;
	bool RestoreEquippedItem_Server(const FSpellRiseEquipmentItemData& Item, ESpellRiseEquipmentSlot Slot, FString& OutRejectReason);
	bool EquipItemToBestSlot_Server(const FGuid& ItemInstanceId, FString& OutRejectReason);
	bool UnequipItemById_Server(const FGuid& ItemInstanceId, int32 PreferredInventorySlot, FString& OutRejectReason);
	bool IsItemEquipped(const FGuid& ItemInstanceId, ESpellRiseEquipmentSlot& OutSlot) const;
	void ResetEquipment_Server();

	void OnHitTakenByOwner_Server();
	void OnHitGivenByOwner_Server();

	const FSpellRiseEquipmentPrivateList& GetPrivateEquipment() const { return PrivateEquipment; }
	const FSpellRiseEquipmentVisualList& GetPublicVisualEquipment() const { return PublicVisualEquipment; }

	void HandlePrivateReplicationChanged();
	void HandleVisualReplicationChanged();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestEquipItem(FGuid ItemInstanceId, ESpellRiseEquipmentSlot RequestedSlot, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestUnequipItem(ESpellRiseEquipmentSlot Slot, int32 PreferredInventorySlot, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestSwapEquipmentSlots(ESpellRiseEquipmentSlot FromSlot, ESpellRiseEquipmentSlot ToSlot, int32 ClientRequestId);

	UFUNCTION(Server, Reliable)
	void ServerRequestRepairItem(FGuid ItemInstanceId, int32 ClientRequestId);

	UPROPERTY(Replicated)
	FSpellRiseEquipmentPrivateList PrivateEquipment;

	UPROPERTY()
	FSpellRiseEquipmentVisualList PublicVisualEquipment;

private:
	struct FGrantHandles
	{
		TArray<FGameplayAbilitySpecHandle> AbilityHandles;
		TArray<FActiveGameplayEffectHandle> EffectHandles;
	};

	bool EquipItem_Server(const FGuid& ItemInstanceId, ESpellRiseEquipmentSlot RequestedSlot, ESpellRiseEquipmentRejectReason& OutReason);
	bool UnequipItem_Server(ESpellRiseEquipmentSlot Slot, int32 PreferredInventorySlot, ESpellRiseEquipmentRejectReason& OutReason);
	bool SwapEquipmentSlots_Server(ESpellRiseEquipmentSlot FromSlot, ESpellRiseEquipmentSlot ToSlot, ESpellRiseEquipmentRejectReason& OutReason);
	bool ValidateEquipmentContext(ESpellRiseEquipmentRejectReason& OutReason) const;
	bool ValidateSlotConflict(const FSpellRiseEquipmentItemData& Item, ESpellRiseEquipmentSlot Slot, ESpellRiseEquipmentRejectReason& OutReason, bool bAllowOccupiedTarget = false) const;
	bool ConsumeRateLimit(bool bEquipFlow);
	ESpellRiseEquipmentSlot ChooseBestSlotForItem(const FSpellRiseEquipmentItemData& Item, bool& bOutFound) const;
	bool IsDuplicateRequest(int32 ClientRequestId);
	void RecordAcceptedRequest(int32 ClientRequestId);
	bool ApplyGrants(const FSpellRiseEquipmentItemData& Item);
	void RemoveGrants(const FGuid& ItemInstanceId);
	void AddReplicatedEntry(const FSpellRiseEquipmentItemData& Item, ESpellRiseEquipmentSlot Slot);
	void RemoveReplicatedEntry(int32 PrivateIndex);
	void RebuildVisualEntry(const FSpellRiseEquippedItemEntry& PrivateEntry);
	ISpellRiseEquipmentInventoryBridge* ResolveInventoryBridge() const;
	UAbilitySystemComponent* ResolveAbilitySystem() const;
	void RejectRequest(const TCHAR* Operation, ESpellRiseEquipmentRejectReason Reason, int32 ClientRequestId) const;

	int32 FindPrivateIndexBySlot(ESpellRiseEquipmentSlot Slot) const;
	int32 FindVisualIndexBySlot(ESpellRiseEquipmentSlot Slot) const;
	void ApplyDurabilityLoss_Server(int32 PrivateIndex, int32 Loss);
	void DestroyEquippedItem_Server(int32 PrivateIndex);
	bool RepairEquippedItem_Server(int32 PrivateIndex);
	bool RepairInventoryItem_Server(const FGuid& ItemInstanceId);
	void CheckAndUpdateDamagedState_Server(int32 PrivateIndex);
	void ApplyDamagedPenalty_Server(const FGuid& ItemInstanceId, ESpellRiseEquipmentSlot Slot);
	void RemoveDamagedPenalty_Server(const FGuid& ItemInstanceId, ESpellRiseEquipmentSlot Slot);
	static FGameplayTag GetDamagedTagForSlot(ESpellRiseEquipmentSlot Slot);

	UPROPERTY(Transient)
	TMap<FGuid, TObjectPtr<USpellRiseEquipmentGrantSource>> GrantSources;

	TMap<FGuid, FGrantHandles> ActiveGrantHandles;
	TSet<FGuid> DamagedTaggedItems;
	TArray<int32> RecentRequestIds;
	double RateWindowStartSeconds = 0.0;
	int32 RequestsInRateWindow = 0;
	int32 ConsecutiveExceededWindows = 0;
	double BlockedUntilSeconds = 0.0;
	int32 EquipmentRevision = 0;

	static constexpr int32 MaxRequestsPerWindow = 6;
	static constexpr double RateWindowSeconds = 0.25;
	static constexpr int32 MaxRememberedRequestIds = 64;
};
