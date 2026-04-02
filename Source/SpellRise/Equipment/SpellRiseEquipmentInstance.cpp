#include "SpellRise/Equipment/SpellRiseEquipmentInstance.h"

#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

void USpellRiseEquipmentInstance::Initialize(USpellRiseEquipmentManagerComponent* InOwnerComponent, UEquippableItem* InSourceItem)
{
	OwnerComponent = InOwnerComponent;
	SourceItem = InSourceItem;
}

void USpellRiseEquipmentInstance::OnEquipped()
{
	if (USpellRiseEquipmentManagerComponent* Manager = OwnerComponent.Get())
	{
		Manager->ApplyReplicatedEquipmentVisual(*this, true);
	}
}

void USpellRiseEquipmentInstance::OnUnequipped()
{
	if (USpellRiseEquipmentManagerComponent* Manager = OwnerComponent.Get())
	{
		Manager->ApplyReplicatedEquipmentVisual(*this, false);
	}
}
