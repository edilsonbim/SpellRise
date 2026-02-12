#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "CatalystAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(BlueprintType)
class SPELLRISE_API UCatalystAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCatalystAttributeSet();

	// 0..100
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst", ReplicatedUsing=OnRep_CatalystCharge)
	FGameplayAttributeData CatalystCharge;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystCharge);

	// persistent progression (>=0)
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst", ReplicatedUsing=OnRep_CatalystXP)
	FGameplayAttributeData CatalystXP;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystXP);

	// 1..3
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst", ReplicatedUsing=OnRep_CatalystLevel)
	FGameplayAttributeData CatalystLevel;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystLevel);

	// Meta: somar carga via GE simples (Add no Delta) e consumir aqui
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst|Meta")
	FGameplayAttributeData CatalystChargeDelta;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystChargeDelta);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// OnRep
	UFUNCTION() void OnRep_CatalystCharge(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CatalystXP(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_CatalystLevel(const FGameplayAttributeData& OldValue);

	// clamps + proc trigger
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
