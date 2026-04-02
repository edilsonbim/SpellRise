#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SpellRiseEquipmentManagerComponent.generated.h"

class UEquippableItem;
class UEquipmentComponent;
class USpellRiseEquipmentInstance;
class UActorChannel;
class FOutBunch;
struct FReplicationFlags;

USTRUCT()
struct FSpellRiseAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UEquippableItem> SourceItem = nullptr;

	UPROPERTY()
	TObjectPtr<USpellRiseEquipmentInstance> Instance = nullptr;

	UPROPERTY()
	uint8 SlotValue = 255;

	UPROPERTY()
	TSubclassOf<UEquippableItem> ItemClass = nullptr;
};

USTRUCT()
struct FSpellRiseEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FSpellRiseEquipmentList() = default;
	explicit FSpellRiseEquipmentList(class USpellRiseEquipmentManagerComponent* InOwner)
		: OwnerComponent(InOwner)
	{
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FSpellRiseAppliedEquipmentEntry, FSpellRiseEquipmentList>(Entries, DeltaParams, *this);
	}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	FSpellRiseAppliedEquipmentEntry* FindBySlot(uint8 SlotValue);
	const FSpellRiseAppliedEquipmentEntry* FindBySlot(uint8 SlotValue) const;

	UPROPERTY()
	TArray<FSpellRiseAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<class USpellRiseEquipmentManagerComponent> OwnerComponent = nullptr;
};

template<>
struct TStructOpsTypeTraits<FSpellRiseEquipmentList> : public TStructOpsTypeTraitsBase2<FSpellRiseEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	bool EquipItem(UEquippableItem* Item);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	bool UnequipItem(UEquippableItem* Item);

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	USpellRiseEquipmentInstance* GetEquippedInstanceBySlot(uint8 SlotValue) const;

	void ApplyReplicatedEquipmentVisual(USpellRiseEquipmentInstance& EquipmentInstance, bool bEquipped);
	void HandleEntryAdded(const FSpellRiseAppliedEquipmentEntry& Entry);
	void HandleEntryRemoved(const FSpellRiseAppliedEquipmentEntry& Entry);

private:
	bool ValidateItemOwnership(UEquippableItem* Item, FString& OutReason) const;
	uint8 ResolveItemSlot(UEquippableItem* Item) const;
	UEquipmentComponent* ResolveEquipmentComponent() const;
	void ApplyVisualForItem(UEquippableItem* Item, bool bEquip);

	FSpellRiseAppliedEquipmentEntry* AddEntry(UEquippableItem* Item, uint8 SlotValue);
	bool RemoveEntryBySlot(uint8 SlotValue);

private:
	UPROPERTY(Replicated)
	FSpellRiseEquipmentList EquipmentList;
};
