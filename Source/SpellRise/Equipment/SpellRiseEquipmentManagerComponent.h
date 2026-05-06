#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "HAL/Platform.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquippableItem.h"
#include "SpellRiseEquipmentManagerComponent.generated.h"

class UEquippableItem;
class UEquipmentComponent;
class USpellRiseEquipmentInstance;
class AActor;
class UPrimitiveComponent;
class USceneComponent;
class USkeletalMeshComponent;
class UNarrativeInventoryComponent;
class UNarrativeItem;
class UActorChannel;
class FOutBunch;
struct FReplicationFlags;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickWeaponSlotsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpellRiseHUDEquipmentSlotsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellRiseWeaponChanged, AActor*, EquippedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellRiseOffHandWeaponChanged, AActor*, EquippedOffHandWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellRiseWeaponLoadoutChanged, AActor*, EquippedMainHandWeapon, AActor*, EquippedOffHandWeapon);

UENUM(BlueprintType)
enum class ESpellRiseHUDEquipmentSlot : uint8
{
	WeaponSlot1 UMETA(DisplayName="Weapon Slot 1"),
	WeaponSlot2 UMETA(DisplayName="Weapon Slot 2"),
	OffHand UMETA(DisplayName="Off Hand"),
	QuickSlot1 UMETA(DisplayName="Quick Slot 1"),
	QuickSlot2 UMETA(DisplayName="Quick Slot 2"),
	QuickSlot3 UMETA(DisplayName="Quick Slot 3"),
	QuickSlot4 UMETA(DisplayName="Quick Slot 4"),
	Backpack UMETA(DisplayName="Backpack"),
	Necklace UMETA(DisplayName="Necklace"),
	Head UMETA(DisplayName="Head"),
	Torso UMETA(DisplayName="Torso"),
	Hands UMETA(DisplayName="Hands"),
	Legs UMETA(DisplayName="Legs"),
	Feet UMETA(DisplayName="Feet"),
};

USTRUCT(BlueprintType)
struct FSpellRiseHUDEquipmentSlotView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	ESpellRiseHUDEquipmentSlot Slot = ESpellRiseHUDEquipmentSlot::WeaponSlot1;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	FName SlotName = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	TObjectPtr<UEquippableItem> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	bool bHasItem = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	bool bIsStowed = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	bool bIsBlockedByTwoHandedWeapon = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|HUD")
	bool bIsTwoHandedWeapon = false;
};

USTRUCT(BlueprintType)
struct FSpellRiseEquipmentAbilityPreview
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	TSubclassOf<class UGameplayAbility> Ability = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	int32 AbilityLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	bool bAutoActivateIfNoInputTag = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	FGameplayAbilitySpecHandle GrantedSpecHandle;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	bool bIsCurrentlyGranted = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	float CooldownRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	float CooldownDuration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	double CooldownCapturedWorldTimeSeconds = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	double CooldownStartWorldTimeSeconds = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	double CooldownEndWorldTimeSeconds = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Equipment|Abilities")
	bool bIsOnCooldown = false;
};

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

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
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
	UEquippableItem* GetActiveOffHandItem() const { return ActiveOffHandItem; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	bool IsOffHandSuppressedByTwoHandedWeapon() const { return bOffHandSuppressedByTwoHandedWeapon; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	int32 GetActiveQuickWeaponSlotIndex() const { return ActiveQuickWeaponSlotIndex; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment|HUD")
	FSpellRiseHUDEquipmentSlotView GetHUDEquipmentSlotView(ESpellRiseHUDEquipmentSlot Slot) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment|HUD")
	TArray<FSpellRiseHUDEquipmentSlotView> GetHUDEquipmentSlotViews() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment|Abilities")
	TArray<FSpellRiseEquipmentAbilityPreview> GetAbilitiesToGrantPreviewForItem(UEquippableItem* Item) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment|Abilities", meta=(WorldContext="WorldContextObject"))
	static float GetAbilityPreviewCooldownRemainingAtCurrentTime(const UObject* WorldContextObject, const FSpellRiseEquipmentAbilityPreview& Preview);

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment|Abilities", meta=(WorldContext="WorldContextObject"))
	static float GetAbilityPreviewCooldownPercentAtCurrentTime(const UObject* WorldContextObject, const FSpellRiseEquipmentAbilityPreview& Preview, bool bReadyProgress = false);

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	AActor* GetActiveEquippedWeaponActor() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	AActor* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	AActor* GetEquippedOffHandWeapon() const { return EquippedOffHandWeapon; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment", meta=(DeterminesOutputType="ExpectedClass"))
	AActor* GetEquippedWeaponTyped(TSubclassOf<AActor> ExpectedClass) const;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon, BlueprintReadOnly, Category="SpellRise|Equipment")
	TObjectPtr<AActor> EquippedWeapon = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedOffHandWeapon, BlueprintReadOnly, Category="SpellRise|Equipment")
	TObjectPtr<AActor> EquippedOffHandWeapon = nullptr;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	bool GetActiveEquippedWeaponSpawnPointLocation(FVector& OutLocation) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	bool GetActiveEquippedWeaponSpawnPointTransform(FTransform& OutTransform) const;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable, Category="SpellRise|Equipment|Events", meta=(DisplayName="On Quick Weapon Slots Changed"))
	FOnQuickWeaponSlotsChanged OnQuickWeaponSlotsChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable, Category="SpellRise|Equipment|Events", meta=(DisplayName="On HUD Equipment Slots Changed"))
	FOnSpellRiseHUDEquipmentSlotsChanged OnHUDEquipmentSlotsChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable, Category="SpellRise|Equipment|Events", meta=(DisplayName="On Weapon Changed"))
	FOnSpellRiseWeaponChanged OnWeaponChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable, Category="SpellRise|Equipment|Events", meta=(DisplayName="On Off Hand Weapon Changed"))
	FOnSpellRiseOffHandWeaponChanged OnOffHandWeaponChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable, Category="SpellRise|Equipment|Events", meta=(DisplayName="On Weapon Loadout Changed"))
	FOnSpellRiseWeaponLoadoutChanged OnWeaponLoadoutChanged;

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
	void SyncNarrativeEquipmentComponentState(UEquippableItem* Item, bool bEquipped);
	void ApplyVisualForItem(UEquippableItem* Item, bool bEquip);
	bool ExtractAbilitiesToGrantFromItem(UEquippableItem* Item, TArray<struct FSpellRiseGrantedAbility>& OutAbilities) const;
	bool ResolveWeaponActorClassFromItem(UEquippableItem* Item, UClass*& OutWeaponActorClass, const void*& OutWeaponConfigPtr, const UStruct*& OutWeaponConfigStruct) const;
	bool ResolveWeaponSocketsFromConfig(const void* WeaponConfigPtr, const UStruct* WeaponConfigStruct, FName& OutEquippedSocket, FName& OutStowedSocket) const;
	bool ResolveWeaponSocketsForItem(UEquippableItem* Item, bool bOffHand, FName& OutEquippedSocket, FName& OutHolsterSocket) const;
	void RefreshEquippedWeaponReference();
	void RefreshEquippedOffHandWeaponReference();
	void BroadcastWeaponChangedIfNeeded();
	void BroadcastOffHandWeaponChangedIfNeeded();
	void BroadcastWeaponLoadoutChangedIfNeeded();
	AActor* ResolveOwnedWeaponActor(UClass* WeaponActorClass) const;
	USceneComponent* ResolveWeaponSpawnPointComponent(AActor* WeaponActor) const;
	USkeletalMeshComponent* ResolveEquipmentAttachMesh() const;
	void PrepareWeaponActorForAttachment(AActor* WeaponActor) const;
	bool AttachWeaponActorToSocket(AActor* WeaponActor, USkeletalMeshComponent* AttachMesh, FName TargetSocket) const;
	void SnapItemWeaponToSocket(UEquippableItem* Item, bool bEquip);
	void RefreshQuickSlotVisual_Local(int32 QuickSlotIndex, bool bEquipped);
	void ReconcileReplicatedQuickSlotVisuals();
	void ApplyGrantedAbilitiesForSlot(UEquippableItem* Item, uint8 SlotValue);
	void RemoveGrantedAbilitiesForSlot(uint8 SlotValue);
	bool ExtractGrantedEffectsFromItem(UEquippableItem* Item, TArray<TSubclassOf<class UGameplayEffect>>& OutEffects) const;
	bool ExtractSetByCallerMagnitudesFromItem(UEquippableItem* Item, TMap<FGameplayTag, float>& OutMagnitudes) const;
	bool IsWeaponItem(UEquippableItem* Item) const;
	bool IsOffHandWeaponItem(UEquippableItem* Item) const;
	bool IsTwoHandedWeaponItem(UEquippableItem* Item) const;
	bool IsOffHandGameplayActive() const;
	void RefreshOffHandSuppression_Server();
	int32 FindQuickSlotByItem(UEquippableItem* Item) const;
	int32 FindFirstFreeQuickSlot() const;
	int32 GetPreferredQuickWeaponSlotForItem(UEquippableItem* Item) const;
	bool HandleWeaponEquipIntent(UEquippableItem* Item);
	bool HandleOffHandEquipIntent(UEquippableItem* Item);
	bool ActivateQuickWeaponSlot_Server(int32 QuickSlotIndex);
	bool AssignQuickWeaponSlot_Server(UEquippableItem* Item, int32 QuickSlotIndex);
	void RemoveQuickWeaponSlot_Server(int32 QuickSlotIndex, bool bDestroyWeaponActor);
	void RemoveOffHandWeapon_Server(bool bDestroyWeaponActor);
	bool DropItem_Server(UNarrativeItem* Item, int32 QuantityToDrop);
	bool SpawnPickupActorForDroppedItem_Server(TSubclassOf<UNarrativeItem> ItemClass, int32 QuantityToDrop, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	AActor* GetOrSpawnWeaponActorForItem(UEquippableItem* Item);
	void DestroyWeaponActorForItem(UEquippableItem* Item);
	void RefreshQuickSlotVisual_Server(int32 QuickSlotIndex, bool bEquipped);
	void RefreshOffHandVisual_Server(bool bEquipped);
	void RefreshOffHandVisual_Local(bool bEquipped);
	void SetNarrativeItemActiveState(UEquippableItem* Item, bool bShouldBeActive);
	void CleanupOrphanedWeaponActors_Server();
	void BindInventoryRemovalHook();
	UFUNCTION()
	void HandleInventoryItemRemoved(UNarrativeItem* RemovedItem, int32 Amount);
	UFUNCTION()
	void OnRep_QuickWeaponSlots();
	UFUNCTION()
	void OnRep_ActiveQuickSlotIndex();
	UFUNCTION()
	void OnRep_EquippedWeapon();
	UFUNCTION()
	void OnRep_ActiveOffHandItem();
	UFUNCTION()
	void OnRep_OffHandSuppressedByTwoHandedWeapon();
	UFUNCTION()
	void OnRep_EquippedOffHandWeapon();

	FSpellRiseAppliedEquipmentEntry* AddEntry(UEquippableItem* Item, uint8 SlotValue);
	bool RemoveEntryBySlot(uint8 SlotValue);
	UEquippableItem* GetEquippedItemByNarrativeSlot(EEquippableSlot Slot) const;
	static FName GetHUDEquipmentSlotName(ESpellRiseHUDEquipmentSlot Slot);

private:
	UPROPERTY(Replicated)
	FSpellRiseEquipmentList EquipmentList;

	TMap<uint8, TArray<FGameplayAbilitySpecHandle>> GrantedAbilityHandlesBySlot;
	TMap<uint8, TArray<FActiveGameplayEffectHandle>> GrantedEffectHandlesBySlot;
	TMap<TObjectPtr<UEquippableItem>, TObjectPtr<AActor>> SpawnedWeaponActorsByItem;

	UPROPERTY(ReplicatedUsing=OnRep_QuickWeaponSlots)
	TArray<TObjectPtr<UEquippableItem>> QuickWeaponSlots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveQuickSlotIndex)
	int32 ActiveQuickWeaponSlotIndex = INDEX_NONE;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveOffHandItem)
	TObjectPtr<UEquippableItem> ActiveOffHandItem = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_OffHandSuppressedByTwoHandedWeapon)
	bool bOffHandSuppressedByTwoHandedWeapon = false;

	UPROPERTY(Transient)
	TObjectPtr<UNarrativeInventoryComponent> CachedInventoryComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AActor> LastBroadcastEquippedWeapon = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AActor> LastBroadcastEquippedOffHandWeapon = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AActor> LastBroadcastLoadoutMainHandWeapon = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AActor> LastBroadcastLoadoutOffHandWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Equipment|Drop")
	TSoftClassPtr<AActor> DropPickupActorClass;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Equipment|Drop", meta=(ClampMin="50.0", UIMin="50.0"))
	float DropSpawnForwardDistance = 150.0f;

	int32 VisualSyncDepth = 0;
};
