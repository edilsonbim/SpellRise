#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BasicAttributeSet.generated.h"

// Define attribute accessor helpers only once to avoid duplicate macro definitions across multiple header files.
#ifndef SPELLRISE_ATTRIBUTE_ACCESSORS
#define SPELLRISE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

UCLASS(BlueprintType)
class SPELLRISE_API UBasicAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBasicAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Strength)
	FGameplayAttributeData Strength;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Agility)
	FGameplayAttributeData Agility;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Agility);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Intelligence)
	FGameplayAttributeData Intelligence;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Intelligence);

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Primary", ReplicatedUsing=OnRep_Wisdom)
	FGameplayAttributeData Wisdom;
	SPELLRISE_ATTRIBUTE_ACCESSORS(UBasicAttributeSet, Wisdom);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION() void OnRep_Strength(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Agility(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Intelligence(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_Wisdom(const FGameplayAttributeData& OldValue);
};
