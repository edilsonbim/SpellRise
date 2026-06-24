#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Equipment/SpellRiseWeaponDefinition.h"
#include "SpellRiseWeaponComponent.generated.h"

class ASpellRiseWeaponBase;
class UEquippableItem;
class UGameplayEffect;
class USkeletalMeshComponent;
class USpellRiseAbilitySystemComponent;
class USpellRiseWeaponDefinition;
class USpellRiseEquipmentComponent;

USTRUCT(BlueprintType)
struct FSpellRiseWeaponSlotState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<UEquippableItem> Item = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<USpellRiseWeaponDefinition> WeaponDefinition = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<AActor> WeaponActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	TSubclassOf<AActor> WeaponActorClass = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	bool bIsOffHand = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	bool bIsTwoHanded = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Weapon")
	bool bIsSuppressed = false;
};

USTRUCT()
struct FSpellRiseWeaponRpcRateLimitState
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	double WindowStartServerTimeSeconds = 0.0;

	UPROPERTY(Transient)
	int32 RequestsInWindow = 0;

	UPROPERTY(Transient)
	int32 RejectCount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellRiseWeaponComponentWeaponChanged, AActor*, EquippedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellRiseWeaponComponentDrawStateChanged, bool, bWeaponDrawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpellRiseWeaponComponentQuickSlotsChanged);

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseWeaponComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool EquipWeapon(UEquippableItem* Item);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool EquipWeaponClass(TSubclassOf<AActor> WeaponActorClass, int32 SlotIndex = 0, bool bDrawImmediately = true);
	void SyncFromEquipmentComponent(const USpellRiseEquipmentComponent* EquipmentComponent);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool UnequipWeaponItem(UEquippableItem* Item);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool ActivateQuickSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool AssignQuickSlot(UEquippableItem* Item, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	void HandleWeaponAnimNotify(FName NotifyName);

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	bool IsWeaponItem(UEquippableItem* Item) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	bool IsOffHandWeaponItem(UEquippableItem* Item) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	bool IsTwoHandedWeaponItem(UEquippableItem* Item) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	AActor* GetEquippedWeaponActor() const { return EquippedWeapon; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	ASpellRiseWeaponBase* GetEquippedWeaponBase() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	void GetActiveWeaponTags(FGameplayTagContainer& OutWeaponTags) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	FGameplayTag GetActiveWeaponProgressionTag() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	bool GetActiveEquippedWeaponSpawnPointTransform(FTransform& OutTransform) const;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon, BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<AActor> EquippedWeapon = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveWeaponItem, BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<UEquippableItem> ActiveWeaponItem = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveQuickSlotIndex, BlueprintReadOnly, Category="SpellRise|Weapon")
	int32 ActiveQuickSlotIndex = INDEX_NONE;

	UPROPERTY(ReplicatedUsing=OnRep_QuickWeaponSlots, BlueprintReadOnly, Category="SpellRise|Weapon")
	TArray<FSpellRiseWeaponSlotState> QuickWeaponSlots;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponDrawn, BlueprintReadOnly, Category="SpellRise|Weapon")
	bool bWeaponDrawn = false;

	UPROPERTY(ReplicatedUsing=OnRep_OffHandState, BlueprintReadOnly, Category="SpellRise|Weapon")
	FSpellRiseWeaponSlotState OffHandWeapon;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Weapon|Events")
	FSpellRiseWeaponComponentWeaponChanged OnWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Weapon|Events")
	FSpellRiseWeaponComponentDrawStateChanged OnWeaponDrawStateChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Weapon|Events")
	FSpellRiseWeaponComponentQuickSlotsChanged OnQuickSlotsChanged;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Weapon")
	void BP_ApplyWeaponOverlayState(uint8 OverlayStateValue, bool bHasWeaponOverlay);

private:
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(UEquippableItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerUnequipWeapon();

	UFUNCTION(Server, Reliable)
	void ServerUnequipWeaponItem(UEquippableItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerActivateQuickSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerAssignQuickSlot(UEquippableItem* Item, int32 SlotIndex);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_ActiveWeaponItem();

	UFUNCTION()
	void OnRep_ActiveQuickSlotIndex();

	UFUNCTION()
	void OnRep_QuickWeaponSlots();

	UFUNCTION()
	void OnRep_WeaponDrawn();

	UFUNCTION()
	void OnRep_OffHandState();

	bool EquipWeapon_Server(UEquippableItem* Item);
	bool EquipWeaponClass_Server(TSubclassOf<AActor> WeaponActorClass, int32 SlotIndex, bool bDrawImmediately);
	bool UnequipWeapon_Server(UEquippableItem* Item);
	bool AssignQuickSlot_Server(UEquippableItem* Item, int32 SlotIndex);
	bool ActivateQuickSlot_Server(int32 SlotIndex);
	bool SetWeaponDrawn_Server(bool bDraw);
	void RemoveActiveWeaponGrants_Server();
	void ApplyActiveWeaponGrants_Server();
	void RemoveOffHandWeaponGrants_Server();
	void ApplyOffHandWeaponGrants_Server();
	void RefreshEquippedWeaponReference_Server();
	void RefreshOffHandSuppression_Server();
	void AttachCurrentWeaponVisual();
	void AttachWeaponVisual(AActor* WeaponActor, UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, bool bDraw, bool bOffHand);
	void BroadcastWeaponState();
	void ApplyOverlayState();
	void ForceOwnerNetUpdate() const;

	bool ValidateItemOwnership(UEquippableItem* Item, FString& OutReason) const;
	bool CheckServerWeaponRpcRateLimit(const TCHAR* RpcName, FSpellRiseWeaponRpcRateLimitState& RateState, FString& OutRejectReason);
	void AuditRejectedWeaponRpc(const TCHAR* RpcName, const FString& RejectReason) const;
	int32 FindQuickSlotByItem(UEquippableItem* Item) const;
	int32 FindFirstFreeQuickSlot() const;
	int32 GetPreferredQuickSlotForItem(UEquippableItem* Item) const;
	FSpellRiseWeaponSlotState* GetActiveSlotState();
	const FSpellRiseWeaponSlotState* GetActiveSlotState() const;

	bool BuildWeaponSlotState(UEquippableItem* Item, FSpellRiseWeaponSlotState& OutState);
	bool BuildWeaponSlotStateFromClass(TSubclassOf<AActor> WeaponActorClass, FSpellRiseWeaponSlotState& OutState);
	AActor* GetOrSpawnWeaponActorForSlot(FSpellRiseWeaponSlotState& SlotState);
	void DestroyWeaponActor(AActor* WeaponActor);
	bool ResolveWeaponActorClassFromItem(UEquippableItem* Item, UClass*& OutWeaponActorClass) const;
	bool ExtractWeaponDefinitionFromItem(UEquippableItem* Item, USpellRiseWeaponDefinition*& OutWeaponDefinition) const;
	bool ExtractWeaponDefinitionFromObject(const UObject* SourceObject, USpellRiseWeaponDefinition*& OutWeaponDefinition) const;
	bool ExtractWeaponDefinitionFromSlot(const FSpellRiseWeaponSlotState& SlotState, USpellRiseWeaponDefinition*& OutWeaponDefinition) const;
	bool ExtractGrantedEffects(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, TArray<TSubclassOf<UGameplayEffect>>& OutEffects) const;
	bool ExtractGrantedAbilities(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, TArray<FSpellRiseGrantedAbility>& OutAbilities) const;
	void ExtractSetByCallerMagnitudes(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, TMap<FGameplayTag, float>& OutMagnitudes) const;
	bool ResolveWeaponSocket(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition, bool bOffHand, FName& OutEquippedSocket) const;
	bool ResolveWeaponSocketForSlot(const FSpellRiseWeaponSlotState& SlotState, bool bOffHand, FName& OutEquippedSocket) const;
	uint8 ResolveOverlayStateValue(UEquippableItem* Item, USpellRiseWeaponDefinition* WeaponDefinition) const;
	uint8 ResolveOverlayStateValueForSlot(const FSpellRiseWeaponSlotState& SlotState) const;
	USkeletalMeshComponent* ResolveVisualOverrideMesh() const;
	USkeletalMeshComponent* ResolveAnimationMesh() const;
	USkeletalMeshComponent* ResolveAttachMesh(FName TargetSocket) const;
	USceneComponent* ResolveWeaponSpawnPoint(AActor* WeaponActor) const;
	USpellRiseAbilitySystemComponent* ResolveSpellRiseASC() const;

private:
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon")
	int32 QuickSlotCount = 2;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon")
	TSoftClassPtr<AActor> DefaultWeaponActorClass;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Security")
	bool bAllowWeaponClassEquipOnPlayerControlledOwner = false;

	UPROPERTY(Transient)
	bool bExternalEquipmentAuthorityActive = false;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Attach")
	FName DefaultEquippedSocket = TEXT("weaponSocket");

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Security", meta=(ClampMin="0.05", UIMin="0.05"))
	float WeaponRpcRateLimitWindowSeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Security", meta=(ClampMin="1", UIMin="1"))
	int32 WeaponRpcRateLimitMaxRequestsPerWindow = 6;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UEquippableItem>, TObjectPtr<AActor>> SpawnedWeaponActorsByItem;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> ActiveWeaponAbilityHandles;

	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> ActiveWeaponEffectHandles;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> OffHandWeaponAbilityHandles;

	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> OffHandWeaponEffectHandles;

	UPROPERTY(Transient)
	FSpellRiseWeaponRpcRateLimitState EquipRpcRateState;

	UPROPERTY(Transient)
	FSpellRiseWeaponRpcRateLimitState UnequipRpcRateState;

	UPROPERTY(Transient)
	FSpellRiseWeaponRpcRateLimitState ActivateQuickSlotRpcRateState;

	UPROPERTY(Transient)
	FSpellRiseWeaponRpcRateLimitState AssignQuickSlotRpcRateState;
};
