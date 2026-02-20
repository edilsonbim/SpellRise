#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CombatAttributeSet.generated.h"

// Define attribute accessor helpers only once to avoid duplicate macro definitions across multiple header files.
#ifndef SPELLRISE_ATTRIBUTE_ACCESSORS
#define SPELLRISE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

UCLASS(BlueprintType)
class SPELLRISE_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCombatAttributeSet();

    // Primary attributes (Strength, Agility, Intelligence and Wisdom) are defined on UBasicAttributeSet.
    // Provide static helper functions here so existing code can continue to access them via UCombatAttributeSet,
    // but forward the calls to the corresponding UBasicAttributeSet attributes. This removes duplicate
    // attribute definitions on both sets.
    static FGameplayAttribute GetStrengthAttribute();
    static FGameplayAttribute GetAgilityAttribute();
    static FGameplayAttribute GetIntelligenceAttribute();
    static FGameplayAttribute GetWisdomAttribute();

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Utility", ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Utility", ReplicatedUsing=OnRep_MoveSpeedMultiplier)
	FGameplayAttributeData MoveSpeedMultiplier;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MoveSpeedMultiplier);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_SlashingRes)
	FGameplayAttributeData SlashingRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, SlashingRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_BashingRes)
	FGameplayAttributeData BashingRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BashingRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_PiercingRes)
	FGameplayAttributeData PiercingRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PiercingRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_FireRes)
	FGameplayAttributeData FireRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, FireRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_ColdRes)
	FGameplayAttributeData ColdRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ColdRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_AcidRes)
	FGameplayAttributeData AcidRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AcidRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_ShockRes)
	FGameplayAttributeData ShockRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ShockRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_DivineRes)
	FGameplayAttributeData DivineRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DivineRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_CurseRes)
	FGameplayAttributeData CurseRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CurseRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_AlmightyRes)
	FGameplayAttributeData AlmightyRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AlmightyRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_GenericRes)
	FGameplayAttributeData GenericRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, GenericRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_BleedRes)
	FGameplayAttributeData BleedRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BleedRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_PoisonRes)
	FGameplayAttributeData PoisonRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PoisonRes);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat|Resistance", ReplicatedUsing=OnRep_ImpactRes)
	FGameplayAttributeData ImpactRes;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ImpactRes);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Compat: evita quebrar arquivos antigos enquanto você migra
	static FGameplayAttribute GetVigorAttribute() { return GetStrengthAttribute(); }
	static FGameplayAttribute GetFocusAttribute() { return GetIntelligenceAttribute(); }

protected:
    // Primary attributes live on UBasicAttributeSet so there are no OnRep functions for them here.

	UFUNCTION() void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_SlashingRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BashingRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_PiercingRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_FireRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ColdRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_AcidRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ShockRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_DivineRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CurseRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_AlmightyRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_GenericRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BleedRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_PoisonRes(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ImpactRes(const FGameplayAttributeData& OldValue);
};
