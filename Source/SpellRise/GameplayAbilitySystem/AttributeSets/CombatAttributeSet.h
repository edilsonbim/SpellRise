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
 * - Primary stats are clamped 10..60.
 * - CastSpeed is percent.
 * - CastStability and BreakPower are BONUS stats (additive) to allow gear/buffs to scale the system.
 *
 * Stability formula (computed at runtime, uses Carga from BasicAttributeSet):
 *   Stability = (Focus*1.0) + (Willpower*0.6) + (Carga*0.4) + CastStability(BONUS)
 *
 * Break formula (computed at runtime):
 *   Break = (Agility*0.4) + (Vigor*0.3) + WeaponImpact + BreakPower(BONUS)
 */
UCLASS()
class SPELLRISE_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCombatAttributeSet();

	/** Primary Stats (persistent, clamped to 10..60) */
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

	/** Cast speed in percent. Example: 0 = normal, 25 = 25% faster, -20 = 20% slower. */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Casting", ReplicatedUsing=OnRep_CastSpeed)
	FGameplayAttributeData CastSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, CastSpeed);

	/** BONUS stability (added to computed stability). */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Casting", ReplicatedUsing=OnRep_CastStability)
	FGameplayAttributeData CastStability;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, CastStability);

	/** BONUS break power (added to computed break). */
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Casting", ReplicatedUsing=OnRep_BreakPower)
	FGameplayAttributeData BreakPower;
	ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, BreakPower);

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

	// GAS
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
