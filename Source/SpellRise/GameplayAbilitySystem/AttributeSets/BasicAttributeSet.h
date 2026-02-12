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

	// Resistências em % (0..75)
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_PhysicalResistance)
	FGameplayAttributeData PhysicalResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, PhysicalResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_MagicResistance)
	FGameplayAttributeData MagicResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, MagicResistance);

	// ✅ NEW: Resistência elemental em % (0..75) para DamageType.Spell.Fire/Ice/Lightning
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_ElementalResistance)
	FGameplayAttributeData ElementalResistance;
	UFUNCTION() void OnRep_FireResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_FrostResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_LightningResistance(const FGameplayAttributeData& OldValue);

	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, ElementalResistance);


	// Resistências elementais específicas em % (0..75). Somam com ElementalResistance.
	// Usadas quando o spell tiver tags DamageElement.Fire/Frost/Lightning (veja ExecCalc_Damage).
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_FireResistance)
	FGameplayAttributeData FireResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, FireResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_FrostResistance)
	FGameplayAttributeData FrostResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, FrostResistance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Defense", ReplicatedUsing=OnRep_LightningResistance)
	FGameplayAttributeData LightningResistance;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, LightningResistance);


	// Armor bruto (diminishing returns no ExecCalc)
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
	UFUNCTION() void OnRep_ElementalResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& OldValue);
};
