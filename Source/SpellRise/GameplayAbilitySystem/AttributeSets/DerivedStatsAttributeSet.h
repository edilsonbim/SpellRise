#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DerivedStatsAttributeSet.generated.h"

// Define derived attribute accessor helpers only once to avoid duplicate macro definitions.
#ifndef ATTRIBUTE_ACCESSORS_DERIVED
#define ATTRIBUTE_ACCESSORS_DERIVED(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

UCLASS(BlueprintType)
class SPELLRISE_API UDerivedStatsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDerivedStatsAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_MeleeDamageMultiplier)
	FGameplayAttributeData MeleeDamageMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, MeleeDamageMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_BowDamageMultiplier)
	FGameplayAttributeData BowDamageMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, BowDamageMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_SpellDamageMultiplier)
	FGameplayAttributeData SpellDamageMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, SpellDamageMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_HealingMultiplier)
	FGameplayAttributeData HealingMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, HealingMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Utility", ReplicatedUsing=OnRep_CastTimeReduction)
	FGameplayAttributeData CastTimeReduction;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CastTimeReduction);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Crit", ReplicatedUsing=OnRep_CritChance)
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CritChance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Crit", ReplicatedUsing=OnRep_CritDamage)
	FGameplayAttributeData CritDamage;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CritDamage);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Penetration", ReplicatedUsing=OnRep_ArmorPenetration)
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Utility", ReplicatedUsing=OnRep_ManaCostReduction)
	FGameplayAttributeData ManaCostReduction;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, ManaCostReduction);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION() void OnRep_MeleeDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BowDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_SpellDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_HealingMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_CastTimeReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_CritChance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CritDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ManaCostReduction(const FGameplayAttributeData& OldValue);
};
