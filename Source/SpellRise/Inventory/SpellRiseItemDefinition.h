#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SpellRiseItemTypes.h"
#include "SpellRiseItemDefinition.generated.h"

class UGameplayEffect;
class USpellRiseWeaponDefinition;
class UTexture2D;

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(ClampMin="0.0"))
	float UnitWeight = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(ClampMin="1", ClampMax="1000"))
	int32 MaxStackSize = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(ClampMin="0"))
	int32 MaxDurability = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FGameplayTagContainer ItemTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	ESpellRiseItemKind ItemKind = ESpellRiseItemKind::Generic;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseWeaponItemDefinition : public USpellRiseItemDefinition
{
	GENERATED_BODY()

public:
	USpellRiseWeaponItemDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TSoftObjectPtr<USpellRiseWeaponDefinition> WeaponDefinition;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseArmorItemDefinition : public USpellRiseItemDefinition
{
	GENERATED_BODY()

public:
	USpellRiseArmorItemDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Armor")
	FGameplayTagContainer AllowedEquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Armor")
	TArray<TSoftClassPtr<UGameplayEffect>> EquippedEffects;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseConsumableItemDefinition : public USpellRiseItemDefinition
{
	GENERATED_BODY()

public:
	USpellRiseConsumableItemDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	TSoftClassPtr<UGameplayEffect> ConsumeEffect;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API USpellRiseContainerItemDefinition : public USpellRiseItemDefinition
{
	GENERATED_BODY()

public:
	USpellRiseContainerItemDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Container", meta=(ClampMin="0"))
	int32 GrantedSlotCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Container", meta=(ClampMin="0.0"))
	float GrantedWeightCapacity = 0.0f;
};
