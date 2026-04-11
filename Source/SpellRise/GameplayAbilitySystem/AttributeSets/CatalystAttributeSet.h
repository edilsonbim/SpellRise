#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "CatalystAttributeSet.generated.h"

#ifndef ATTRIBUTE_ACCESSORS
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

UCLASS(BlueprintType)
class SPELLRISE_API UCatalystAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCatalystAttributeSet();


	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst", ReplicatedUsing=OnRep_CatalystCharge)
	FGameplayAttributeData CatalystCharge;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystCharge);


	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst", ReplicatedUsing=OnRep_CatalystXP)
	FGameplayAttributeData CatalystXP;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystXP);


	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst", ReplicatedUsing=OnRep_CatalystLevel)
	FGameplayAttributeData CatalystLevel;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystLevel);


	UPROPERTY(BlueprintReadOnly, Category="Attributes|Catalyst|Meta")
	FGameplayAttributeData CatalystChargeDelta;
	ATTRIBUTE_ACCESSORS(UCatalystAttributeSet, CatalystChargeDelta);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_CatalystCharge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CatalystXP(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CatalystLevel(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
