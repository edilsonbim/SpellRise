#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "SpellRise/Equipment/SpellRiseWeaponDefinition.h"
#include "SpellRiseEquipmentInventoryBridge.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ESpellRiseEquipmentSlot : uint8
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
struct SPELLRISE_API FSpellRiseEquipmentItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid ItemInstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPrimaryAssetId DefinitionId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Durability = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ESpellRiseEquipmentSlot> AllowedSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTwoHanded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UnitWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USpellRiseWeaponDefinition> WeaponDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSpellRiseGrantedAbility> GrantedAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftClassPtr<UGameplayEffect>> GrantedEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> SetByCallerMagnitudes;

	bool SupportsSlot(const ESpellRiseEquipmentSlot Slot) const
	{
		return AllowedSlots.Contains(Slot);
	}
};

UINTERFACE(MinimalAPI)
class USpellRiseEquipmentInventoryBridge : public UInterface
{
	GENERATED_BODY()
};

/**
 * Server-only transaction boundary implemented by the future SpellRise inventory.
 * Implementations must validate ownership and mutate inventory atomically.
 */
class SPELLRISE_API ISpellRiseEquipmentInventoryBridge
{
	GENERATED_BODY()

public:
	virtual bool Equipment_TakeItem(
		const FGuid& ItemInstanceId,
		FSpellRiseEquipmentItemData& OutItem,
		FString& OutRejectReason) = 0;

	virtual bool Equipment_ReturnItem(
		const FSpellRiseEquipmentItemData& Item,
		int32 PreferredInventorySlot,
		FString& OutRejectReason) = 0;

	virtual bool Equipment_CanReturnItem(
		const FSpellRiseEquipmentItemData& Item,
		int32 PreferredInventorySlot,
		FString& OutRejectReason) const = 0;
};
