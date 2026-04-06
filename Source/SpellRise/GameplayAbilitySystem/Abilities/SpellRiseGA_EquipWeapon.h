#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRiseGA_EquipWeapon.generated.h"

class UEquippableItem;

UCLASS()
class SPELLRISE_API USpellRiseGA_EquipWeapon : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGA_EquipWeapon();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	const UEquippableItem* ResolveEquippableItemFromEventData(const FGameplayEventData* TriggerEventData) const;

protected:
	/** Se true, esta ability remove o item em vez de equipar. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Equipment")
	bool bUnequipInstead = false;

	/** Se >= 0, ativa diretamente o quick slot informado (0 = tecla 1, 1 = tecla 2). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Equipment", meta=(ClampMin="-1", ClampMax="1"))
	int32 QuickSlotIndexToActivate = INDEX_NONE;
};
