#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SpellRiseEquipmentManagerComponent.generated.h"

class UEquippableItem;
class UEquipmentComponent;
class USpellRiseEquipmentInstance;
class AActor;
class USkeletalMeshComponent;
class UNarrativeInventoryComponent;
class UNarrativeItem;
class UActorChannel;
class FOutBunch;
struct FReplicationFlags;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickWeaponSlotsChanged);

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
	bool RequestEquipItem(UEquippableItem* Item);


	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	bool RequestUnequipItem(UEquippableItem* Item);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	bool RequestActivateQuickWeaponSlot(int32 QuickSlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	bool RequestAssignQuickWeaponSlot(UEquippableItem* Item, int32 QuickSlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	bool RequestDropItem(UNarrativeItem* Item, int32 QuantityToDrop);

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	UEquippableItem* GetQuickWeaponItemByIndex(int32 QuickSlotIndex) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	int32 GetActiveQuickWeaponSlotIndex() const { return ActiveQuickWeaponSlotIndex; }

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Equipment")
	FOnQuickWeaponSlotsChanged OnQuickWeaponSlotsChanged;

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
	UFUNCTION(Server, Reliable)
	void ServerRequestEquipItem(UEquippableItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerRequestUnequipItem(UEquippableItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerRequestActivateQuickWeaponSlot(int32 QuickSlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRequestAssignQuickWeaponSlot(UEquippableItem* Item, int32 QuickSlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRequestDropItem(UNarrativeItem* Item, int32 QuantityToDrop);

	bool ValidateItemOwnership(UEquippableItem* Item, FString& OutReason) const;
	uint8 ResolveItemSlot(UEquippableItem* Item) const;
	UEquipmentComponent* ResolveEquipmentComponent() const;
	void ApplyVisualForItem(UEquippableItem* Item, bool bEquip);
	bool ExtractAbilitiesToGrantFromItem(UEquippableItem* Item, TArray<struct FSpellRiseGrantedAbility>& OutAbilities) const;
	bool ResolveWeaponActorClassFromItem(UEquippableItem* Item, UClass*& OutWeaponActorClass, const void*& OutWeaponConfigPtr, const UStruct*& OutWeaponConfigStruct) const;
	bool ResolveWeaponSocketsFromConfig(const void* WeaponConfigPtr, const UStruct* WeaponConfigStruct, FName& OutEquippedSocket, FName& OutStowedSocket) const;
	AActor* ResolveOwnedWeaponActor(UClass* WeaponActorClass) const;
	USkeletalMeshComponent* ResolveEquipmentAttachMesh() const;
	void SnapItemWeaponToSocket(UEquippableItem* Item, bool bEquip);
	void ApplyGrantedAbilitiesForSlot(UEquippableItem* Item, uint8 SlotValue);
	void RemoveGrantedAbilitiesForSlot(uint8 SlotValue);
	bool IsWeaponItem(UEquippableItem* Item) const;
	int32 FindQuickSlotByItem(UEquippableItem* Item) const;
	int32 FindFirstFreeQuickSlot() const;
	void HandleWeaponEquipIntent(UEquippableItem* Item);
	bool ActivateQuickWeaponSlot_Server(int32 QuickSlotIndex);
	bool AssignQuickWeaponSlot_Server(UEquippableItem* Item, int32 QuickSlotIndex);
	void RemoveQuickWeaponSlot_Server(int32 QuickSlotIndex, bool bDestroyWeaponActor);
	bool DropItem_Server(UNarrativeItem* Item, int32 QuantityToDrop);
	bool SpawnPickupActorForDroppedItem_Server(TSubclassOf<UNarrativeItem> ItemClass, int32 QuantityToDrop, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	AActor* GetOrSpawnWeaponActorForItem(UEquippableItem* Item);
	void DestroyWeaponActorForItem(UEquippableItem* Item);
	void RefreshQuickSlotVisual_Server(int32 QuickSlotIndex, bool bEquipped);
	void SetNarrativeItemActiveState(UEquippableItem* Item, bool bShouldBeActive);
	void CleanupOrphanedWeaponActors_Server();
	void BindInventoryRemovalHook();
	UFUNCTION()
	void HandleInventoryItemRemoved(UNarrativeItem* RemovedItem, int32 Amount);
	UFUNCTION()
	void OnRep_QuickWeaponSlots();
	UFUNCTION()
	void OnRep_ActiveQuickSlotIndex();

	FSpellRiseAppliedEquipmentEntry* AddEntry(UEquippableItem* Item, uint8 SlotValue);
	bool RemoveEntryBySlot(uint8 SlotValue);

private:
	UPROPERTY(Replicated)
	FSpellRiseEquipmentList EquipmentList;

	TMap<uint8, TArray<FGameplayAbilitySpecHandle>> GrantedAbilityHandlesBySlot;
	TMap<TObjectPtr<UEquippableItem>, TObjectPtr<AActor>> SpawnedWeaponActorsByItem;

	UPROPERTY(ReplicatedUsing=OnRep_QuickWeaponSlots)
	TArray<TObjectPtr<UEquippableItem>> QuickWeaponSlots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveQuickSlotIndex)
	int32 ActiveQuickWeaponSlotIndex = INDEX_NONE;

	UPROPERTY(Transient)
	TObjectPtr<UNarrativeInventoryComponent> CachedInventoryComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Equipment|Drop")
	TSoftClassPtr<AActor> DropPickupActorClass;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Equipment|Drop", meta=(ClampMin="50.0", UIMin="50.0"))
	float DropSpawnForwardDistance = 150.0f;
};
