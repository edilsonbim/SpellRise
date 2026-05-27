#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRiseGameplayAbility_FromEquipment.generated.h"

class UEquippableItem;
class USpellRiseEquipmentInstance;

UCLASS(Abstract)
class SPELLRISE_API USpellRiseGameplayAbility_FromEquipment : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility_FromEquipment();

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|Equipment")
	USpellRiseEquipmentInstance* GetAssociatedEquipmentInstance() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|Equipment")
	UEquippableItem* GetAssociatedItem() const;

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
