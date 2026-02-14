#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ResourceAttributeSet.generated.h"

class UGameplayEffect;

#define SPELLRISE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(BlueprintType)
class SPELLRISE_API UResourceAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UResourceAttributeSet();

	// -------------------------
	// Resources (replicated)
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_Mana)
	FGameplayAttributeData Mana;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData Stamina;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, MaxStamina);

	// Shield opcional
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_Shield)
	FGameplayAttributeData Shield;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Resources", ReplicatedUsing=OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, MaxShield);

	// -------------------------
	// Regen (replicated)
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Regen", ReplicatedUsing=OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, HealthRegen);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Regen", ReplicatedUsing=OnRep_ManaRegen)
	FGameplayAttributeData ManaRegen;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, ManaRegen);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Regen", ReplicatedUsing=OnRep_StaminaRegen)
	FGameplayAttributeData StaminaRegen;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, StaminaRegen);

	// -------------------------
	// Meta (NOT replicated)
	// -------------------------
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Meta")
	FGameplayAttributeData Damage;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UResourceAttributeSet, Damage);

	// -------------------------
	// Catalyst (aplica charge via GE instant)
	// -------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	TSubclassOf<UGameplayEffect> GE_Catalyst_AddCharge;

	// GAS overrides
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	// Rep notifies
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_Shield(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_MaxShield(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_HealthRegen(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_ManaRegen(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_StaminaRegen(const FGameplayAttributeData& OldValue);
};
