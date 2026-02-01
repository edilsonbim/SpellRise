// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BasicAttributeSet.generated.h"

#define SPELLRISE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(BlueprintType)
class SPELLRISE_API UBasicAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBasicAttributeSet();

	/* =======================
	   INVENTORY / WEIGHT
	   ======================= */

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Inventory", ReplicatedUsing=OnRep_CarryWeight)
	FGameplayAttributeData CarryWeight;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, CarryWeight);

	/* =======================
	   DEFENSE
	   ======================= */
	// Resistances stored as PERCENT values (0..75). Convert to fraction (÷100) inside ExecCalcs.
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_PhysicalResistance)
	FGameplayAttributeData PhysicalResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, PhysicalResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_MagicResistance)
	FGameplayAttributeData MagicResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, MagicResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Armor);

	/* =======================
	   Overrides
	   ======================= */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/* =======================
	   Replication
	   ======================= */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/* =======================
	   OnRep
	   ======================= */
	UFUNCTION() void OnRep_CarryWeight(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MagicResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldValue);
};
