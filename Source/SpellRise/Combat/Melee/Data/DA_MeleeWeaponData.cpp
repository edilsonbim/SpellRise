#include "DA_MeleeWeaponData.h"

UDA_MeleeWeaponData::UDA_MeleeWeaponData()
	: WeaponId(NAME_None)
	, AttachSocketName("Socket_Weapon_R")
	, AttackMontage(nullptr)
{
	DisplayName = FText::GetEmpty();
}