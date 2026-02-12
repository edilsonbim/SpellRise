#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DerivedStatsAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS_DERIVED(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Derived stats (calculated from primaries + gear/buffs/catalyst).
 *
 * Conventions:
 * - Multipliers: 1.0 = normal (e.g. 1.10 = +10%)
 * - CritChance:  0..1 (e.g. 0.10 = 10%)
 * - CritDamage:  multiplier total (e.g. 1.5 = 150%)
 * - Penetrations: percent 0..75
 * - BreakPower / CastStability: scalar values
 */
UCLASS(BlueprintType)
class SPELLRISE_API UDerivedStatsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDerivedStatsAttributeSet();

	// -------------------------
	// Multipliers / Crit
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_WeaponDamageMultiplier)
	FGameplayAttributeData WeaponDamageMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, WeaponDamageMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_AttackSpeedMultiplier)
	FGameplayAttributeData AttackSpeedMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, AttackSpeedMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Crit", ReplicatedUsing=OnRep_CritChance)
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CritChance);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Crit", ReplicatedUsing=OnRep_CritDamage)
	FGameplayAttributeData CritDamage;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CritDamage);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_SpellPowerMultiplier)
	FGameplayAttributeData SpellPowerMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, SpellPowerMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_CastSpeedMultiplier)
	FGameplayAttributeData CastSpeedMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CastSpeedMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Multipliers", ReplicatedUsing=OnRep_ManaCostMultiplier)
	FGameplayAttributeData ManaCostMultiplier;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, ManaCostMultiplier);

	// -------------------------
	// Utility (scalar values)
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Utility", ReplicatedUsing=OnRep_BreakPower)
	FGameplayAttributeData BreakPower;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, BreakPower);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Utility", ReplicatedUsing=OnRep_CastStability)
	FGameplayAttributeData CastStability;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, CastStability);

	// -------------------------
	// Penetrations (percent)
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Penetration", ReplicatedUsing=OnRep_ArmorPenetration)
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Derived|Penetration", ReplicatedUsing=OnRep_ElementPenetration)
	FGameplayAttributeData ElementPenetration;
	ATTRIBUTE_ACCESSORS_DERIVED(UDerivedStatsAttributeSet, ElementPenetration);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION() void OnRep_WeaponDamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_AttackSpeedMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CritChance(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CritDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_SpellPowerMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CastSpeedMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ManaCostMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_BreakPower(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CastStability(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ElementPenetration(const FGameplayAttributeData& OldValue);
};
