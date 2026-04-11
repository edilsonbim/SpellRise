// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_EquipWeapon.h"

#include "EquippableItem.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseGAEquipWeapon, Log, All);

USpellRiseGA_EquipWeapon::USpellRiseGA_EquipWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	CastType = ESpellRiseAbilityCastType::Instant;
	bCommitAbilityOnActivate = false;
	bEndAbilityAfterExecution = true;
}

void USpellRiseGA_EquipWeapon::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USpellRiseEquipmentManagerComponent* EquipmentManager = Character->GetSpellRiseEquipmentManager();
	if (!EquipmentManager)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (QuickSlotIndexToActivate != INDEX_NONE)
	{
		const bool bActivated = EquipmentManager->RequestActivateQuickWeaponSlot(QuickSlotIndexToActivate);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, !bActivated);
		return;
	}

	const UEquippableItem* ItemToHandleConst = ResolveEquippableItemFromEventData(TriggerEventData);
	UEquippableItem* ItemToHandle = const_cast<UEquippableItem*>(ItemToHandleConst);
	if (!ItemToHandle)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const bool bRequested = bUnequipInstead
		? EquipmentManager->RequestUnequipItem(ItemToHandle)
		: EquipmentManager->RequestEquipItem(ItemToHandle);

	if (!bRequested)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

const UEquippableItem* USpellRiseGA_EquipWeapon::ResolveEquippableItemFromEventData(const FGameplayEventData* TriggerEventData) const
{
	if (!TriggerEventData)
	{
		return nullptr;
	}

	if (const UEquippableItem* ItemFromOptional = Cast<UEquippableItem>(TriggerEventData->OptionalObject.Get()))
	{
		return ItemFromOptional;
	}

	return nullptr;
}
