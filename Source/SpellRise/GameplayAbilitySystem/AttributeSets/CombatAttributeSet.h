// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS_BASIC(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Primary / combat attributes for SpellRise.
 *
 * Primaries (clamped 10..60):
 *   - Vigor, Focus, Agility, Willpower, Attunement
 *
 * Secondary bonuses (gear/buffs/catalyst):
 *   - CastSpeed      (% faster/slower cast)
 *   - CastStability  (bonus stability; reduces interrupt pressure)
 *   - BreakPower     (bonus break; increases interrupt pressure)
 *   - SpellPowerBonusPct     (bonus spell damage scaling - additive)
 *   - MoveSpeed      (bonus walk speed in uu/s; applied by character code)
 *   - MoveSpeedMultiplier (multiplier for final walk speed; 1.0 = normal)
 *
 * DPS/Derived stats were moved to UDerivedStatsAttributeSet.
 */
UCLASS(BlueprintType)
class SPELLRISE_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCombatAttributeSet();

	// -------------------------
	// Primaries (replicated)
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Vigor)
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Vigor);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Focus)
	FGameplayAttributeData Focus;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Focus);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Agility)
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Agility);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Willpower)
	FGameplayAttributeData Willpower;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Willpower);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Attunement)
	FGameplayAttributeData Attunement;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Attunement);

	// -------------------------
	// Casting bonuses (replicated)
	// -------------------------

	/** Percent. Example: 0 = normal, 25 = 25% faster, -20 = 20% slower. */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Casting", ReplicatedUsing=OnRep_CastSpeed)
	FGameplayAttributeData CastSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, CastSpeed);

	/** BONUS stability (additive). Used by abilities to reduce interrupt/cast break. */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Casting", ReplicatedUsing=OnRep_CastStability)
	FGameplayAttributeData CastStability;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, CastStability);

	// -------------------------
	// Pressure / break bonuses (replicated)
	// -------------------------

	/** BONUS break power (additive). Used by damage system to increase interrupt pressure. */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat", ReplicatedUsing=OnRep_BreakPower)
	FGameplayAttributeData BreakPower;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, BreakPower);

	// -------------------------
	// Spell & Utility bonuses (replicated)
	// -------------------------

	/** BONUS spell power (additive). Used by spell damage calc. */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat", ReplicatedUsing=OnRep_SpellPowerBonusPct)
	FGameplayAttributeData SpellPowerBonusPct;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, SpellPowerBonusPct);

	/**
	 * BONUS move speed in uu/s (additive to MaxWalkSpeed).
	 * Example: +50 means +50 uu/s.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Utility", ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, MoveSpeed);

	/**
	 * Multiplier for final movement speed.
	 * 1.00 = normal, 1.15 = +15%, 0.90 = -10%.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Utility", ReplicatedUsing=OnRep_MoveSpeedMultiplier)
	FGameplayAttributeData MoveSpeedMultiplier;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, MoveSpeedMultiplier);

protected:
	// Replication callbacks
	UFUNCTION() void OnRep_Vigor(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Focus(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Agility(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Willpower(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Attunement(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_CastSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CastStability(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BreakPower(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_SpellPowerBonusPct(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldValue);

	// GAS
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
