// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BasicAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS_BASIC(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SPELLRISE_API UBasicAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBasicAttributeSet();

	/* =======================
	   RESOURCES
	   ======================= */

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxMana);

	// Meta attribute (não replica)
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resources", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxShield);

	/* =======================
	   INVENTORY / WEIGHT
	   ======================= */

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Inventory", ReplicatedUsing = OnRep_Carga)
	FGameplayAttributeData Carga;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Carga);

	/* =======================
	   DEFENSE
	   ======================= */
	// Resistances stored as PERCENT values (0..75). Convert to fraction (÷100) inside ExecCalcs.
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_PhysicalResistance)
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, PhysicalResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_MagicResistance)
	FGameplayAttributeData MagicResistance;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MagicResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Armor);

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

	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Shield(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxShield(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Carga(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MagicResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldValue);
};
