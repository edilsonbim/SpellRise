#include "SpellRiseItemDefinition.h"

FPrimaryAssetId USpellRiseItemDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("SpellRiseItem"), GetFName());
}

USpellRiseWeaponItemDefinition::USpellRiseWeaponItemDefinition()
{
	ItemKind = ESpellRiseItemKind::Weapon;
	MaxStackSize = 1;
}

USpellRiseArmorItemDefinition::USpellRiseArmorItemDefinition()
{
	ItemKind = ESpellRiseItemKind::Armor;
	MaxStackSize = 1;
}

USpellRiseConsumableItemDefinition::USpellRiseConsumableItemDefinition()
{
	ItemKind = ESpellRiseItemKind::Consumable;
}

USpellRiseContainerItemDefinition::USpellRiseContainerItemDefinition()
{
	ItemKind = ESpellRiseItemKind::Container;
	MaxStackSize = 1;
}
