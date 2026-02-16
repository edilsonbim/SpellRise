// ============================================================================
// DA_StaffWeaponData.cpp
// Path: Source/SpellRise/Combat/Spells/Data/DA_StaffWeaponData.cpp
// ============================================================================

#include "SpellRise/Combat/Spells/Data/DA_StaffWeaponData.h"

UDA_StaffWeaponData::UDA_StaffWeaponData()
{
	WeaponId = NAME_None;
	AttachSocketName = "WeaponSocket";
	AttachOffset = FTransform::Identity;
}
