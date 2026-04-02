#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_FromEquipment.h"

#include "EquippableItem.h"
#include "SpellRise/Equipment/SpellRiseEquipmentInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipmentAbility, Log, All);

USpellRiseGameplayAbility_FromEquipment::USpellRiseGameplayAbility_FromEquipment()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

USpellRiseEquipmentInstance* USpellRiseGameplayAbility_FromEquipment::GetAssociatedEquipmentInstance() const
{
	if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Cast<USpellRiseEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UEquippableItem* USpellRiseGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (USpellRiseEquipmentInstance* EquipmentInstance = GetAssociatedEquipmentInstance())
	{
		return EquipmentInstance->GetSourceItem();
	}

	return nullptr;
}

void USpellRiseGameplayAbility_FromEquipment::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (!Spec.SourceObject.IsValid())
	{
		UE_LOG(LogSpellRiseEquipmentAbility, Warning, TEXT("[GAS][EquipmentAbility] Ability without SourceObject: %s"), *GetNameSafe(this));
		return;
	}

	if (!Spec.SourceObject->IsA(USpellRiseEquipmentInstance::StaticClass()))
	{
		UE_LOG(LogSpellRiseEquipmentAbility, Warning, TEXT("[GAS][EquipmentAbility] SourceObject is not equipment instance Ability=%s Source=%s"), *GetNameSafe(this), *GetNameSafe(Spec.SourceObject.Get()));
	}
}
