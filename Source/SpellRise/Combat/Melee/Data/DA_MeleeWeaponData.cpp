#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"

UDA_MeleeWeaponData::UDA_MeleeWeaponData()
{
	WeaponId = NAME_None;
	AttachSocketName = "WeaponSocket";
	AttachOffset = FTransform::Identity;
}
