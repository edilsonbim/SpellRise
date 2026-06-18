#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "SpellRiseAbilityHotbarComponent.generated.h"

class UGameplayAbility;
class USpellRiseAbilitySystemComponent;
class UTexture2D;

UENUM(BlueprintType)
enum class ESpellRiseAbilityHotbarGroup : uint8
{
	Weapon UMETA(DisplayName="Weapon"),
	Common UMETA(DisplayName="Common")
};

USTRUCT(BlueprintType)
struct FSpellRiseAbilityHotbarSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Hotbar")
	ESpellRiseAbilityHotbarGroup Group = ESpellRiseAbilityHotbarGroup::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Hotbar", meta=(Categories="InputTag"))
	FGameplayTag AbilityInputTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Hotbar", meta=(AllowedClasses="/Script/GameplayAbilities.GameplayAbility"))
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Hotbar")
	TSoftObjectPtr<UObject> AbilityDefinition;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar")
	bool bLockedByServer = false;
};

USTRUCT(BlueprintType)
struct FSpellRiseAbilityHotbarSlotViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	int32 GroupSlotIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	ESpellRiseAbilityHotbarGroup Group = ESpellRiseAbilityHotbarGroup::Weapon;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI", meta=(Categories="InputTag"))
	FGameplayTag AbilityInputTag;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI", meta=(AllowedClasses="/Script/GameplayAbilities.GameplayAbility"))
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	TObjectPtr<UTexture2D> ActiveIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	bool bHasAbility = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	bool bIsGranted = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	float CooldownTimeRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	float CooldownDuration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	float CooldownPercent = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Hotbar|UI")
	bool bLockedByServer = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpellRiseAbilityHotbarChanged);

UCLASS(ClassGroup=(SpellRise), BlueprintType, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseAbilityHotbarComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseAbilityHotbarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar")
	const TArray<FSpellRiseAbilityHotbarSlot>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar")
	bool GetSlot(int32 SlotIndex, FSpellRiseAbilityHotbarSlot& OutSlot) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar|UI")
	bool GetSlotViewData(int32 SlotIndex, FSpellRiseAbilityHotbarSlotViewData& OutViewData) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar|UI")
	TArray<FSpellRiseAbilityHotbarSlotViewData> GetSlotViewDataForGroup(ESpellRiseAbilityHotbarGroup Group) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar")
	bool RequestSetSlot(int32 SlotIndex, FGameplayTag AbilityInputTag, TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar")
	bool RequestSetSlotFromAbilityDefinition(int32 SlotIndex, UObject* AbilityDefinition);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar")
	bool RequestSetSlotFromAbilityDefinitionSoft(int32 SlotIndex, TSoftObjectPtr<UObject> AbilityDefinition);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar")
	bool RequestClearSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar|Input")
	void AbilitySlotPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar|Input")
	void AbilitySlotReleased(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar")
	static int32 MakeSlotIndex(ESpellRiseAbilityHotbarGroup Group, int32 GroupSlotIndex);

	bool ApplyPersistentSlots_Server(const TArray<FSpellRiseAbilityHotbarSlot>& SavedSlots);

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Hotbar|Events")
	FSpellRiseAbilityHotbarChanged OnHotbarChanged;

private:
	UFUNCTION(Server, Reliable)
	void ServerSetSlot(int32 SlotIndex, FGameplayTag AbilityInputTag, TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(Server, Reliable)
	void ServerSetSlotFromAbilityDefinition(int32 SlotIndex, const TSoftObjectPtr<UObject>& AbilityDefinition);

	UFUNCTION(Server, Reliable)
	void ServerClearSlot(int32 SlotIndex);

	UFUNCTION()
	void OnRep_Slots();

	void InitializeDefaultSlots();
	bool SetSlot_Server(int32 SlotIndex, FGameplayTag AbilityInputTag, TSubclassOf<UGameplayAbility> AbilityClass, FString& OutRejectReason);
	bool SetSlotFromAbilityDefinition_Server(int32 SlotIndex, TSoftObjectPtr<UObject> AbilityDefinition, FString& OutRejectReason);
	bool ClearSlot_Server(int32 SlotIndex, FString& OutRejectReason);
	bool ValidateSlotPayload(int32 SlotIndex, FGameplayTag AbilityInputTag, TSubclassOf<UGameplayAbility> AbilityClass, FString& OutRejectReason) const;
	bool ValidateAbilityDefinitionPayload(int32 SlotIndex, TSoftObjectPtr<UObject> AbilityDefinition, TSubclassOf<UGameplayAbility>& OutAbilityClass, FString& OutRejectReason) const;
	bool CheckServerRateLimit(FString& OutRejectReason);
	void AuditRejectedHotbarRpc(const TCHAR* RpcName, const FString& RejectReason) const;
	USpellRiseAbilitySystemComponent* ResolveASC() const;

	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<FSpellRiseAbilityHotbarSlot> Slots;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Hotbar|Security", meta=(ClampMin="0.1", UIMin="0.1"))
	float HotbarRpcRateLimitWindowSeconds = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Hotbar|Security", meta=(ClampMin="1", UIMin="1"))
	int32 HotbarRpcRateLimitMaxRequestsPerWindow = 8;

	double RateLimitWindowStartServerTimeSeconds = 0.0;
	int32 RateLimitRequestsInWindow = 0;
	mutable TMap<FString, int32> RejectedRpcCountByReason;
};
