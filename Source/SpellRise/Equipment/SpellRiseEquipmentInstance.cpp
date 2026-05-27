// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Equipment/SpellRiseEquipmentInstance.h"

#include "Net/UnrealNetwork.h"
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"

void USpellRiseEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellRiseEquipmentInstance, SourceItem);
}

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
