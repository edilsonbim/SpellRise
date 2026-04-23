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
		UE_LOG(LogSpellRiseGAEquipWeapon, Warning, TEXT("ActivateAbility abortada: ActorInfo nulo."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		UE_LOG(LogSpellRiseGAEquipWeapon, Warning, TEXT("ActivateAbility abortada: Avatar nao e SpellRiseCharacterBase."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USpellRiseEquipmentManagerComponent* EquipmentManager = Character->GetSpellRiseEquipmentManager();
	if (!EquipmentManager)
	{
		UE_LOG(LogSpellRiseGAEquipWeapon, Warning, TEXT("ActivateAbility abortada: EquipmentManager ausente. Character=%s"), *GetNameSafe(Character));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (QuickSlotIndexToActivate != INDEX_NONE)
	{
		UE_LOG(LogSpellRiseGAEquipWeapon, Log,
			TEXT("EquipWeapon GA ativando quick slot. Character=%s Slot=%d LocalControlled=%s"),
			*GetNameSafe(Character),
			QuickSlotIndexToActivate,
			ActorInfo->IsLocallyControlled() ? TEXT("true") : TEXT("false"));
		const bool bActivated = EquipmentManager->RequestActivateQuickWeaponSlot(QuickSlotIndexToActivate);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, !bActivated);
		return;
	}

	const UEquippableItem* ItemToHandleConst = ResolveEquippableItemFromEventData(TriggerEventData);
	UEquippableItem* ItemToHandle = const_cast<UEquippableItem*>(ItemToHandleConst);
	if (!ItemToHandle)
	{
		UE_LOG(LogSpellRiseGAEquipWeapon, Warning, TEXT("ActivateAbility abortada: EventData sem UEquippableItem em OptionalObject."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogSpellRiseGAEquipWeapon, Log,
		TEXT("EquipWeapon GA. Character=%s Item=%s Unequip=%s LocalControlled=%s HasAuthority=%s"),
		*GetNameSafe(Character),
		*GetNameSafe(ItemToHandle),
		bUnequipInstead ? TEXT("true") : TEXT("false"),
		ActorInfo->IsLocallyControlled() ? TEXT("true") : TEXT("false"),
		Character->HasAuthority() ? TEXT("true") : TEXT("false"));

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
