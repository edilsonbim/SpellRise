// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
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
		return;
	}

	if (!Spec.SourceObject->IsA(USpellRiseEquipmentInstance::StaticClass()))
	{
	}
}
