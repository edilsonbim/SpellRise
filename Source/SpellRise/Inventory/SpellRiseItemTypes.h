#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManagerTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SpellRiseItemTypes.generated.h"

class USpellRiseInventoryComponent;

UENUM(BlueprintType)
enum class ESpellRiseItemKind : uint8
{
	Generic,
	Weapon,
	Armor,
	Consumable,
	Container
};

UENUM(BlueprintType)
enum class ESpellRiseInventoryChangeType : uint8
{
	Added,
	Changed,
	Removed,
	Reset
};

UENUM(BlueprintType)
enum class ESpellRiseInventoryRequestResult : uint8
{
	Success,
	InvalidContext,
	InvalidItem,
	InvalidDefinition,
	InvalidSlot,
	InvalidQuantity,
	StackMismatch,
	CapacityExceeded,
	WeightExceeded,
	ItemLocked,
	RateLimited,
	DuplicateRequest,
	DropUnavailable,
	TransactionFailed
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseItemInstance : public FFastArraySerializerItem
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
	uint8 Flags = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Inventory")
	int32 Revision = 0;
};

USTRUCT()
struct SPELLRISE_API FSpellRiseInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpellRiseItemInstance> Entries;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSpellRiseItemInstance, FSpellRiseInventoryList>(Entries, DeltaParms, *this);
	}

	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void SetOwner(USpellRiseInventoryComponent* InOwner) { Owner = InOwner; }

private:
	UPROPERTY(NotReplicated)
	TObjectPtr<USpellRiseInventoryComponent> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FSpellRiseInventoryList> : public TStructOpsTypeTraitsBase2<FSpellRiseInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
