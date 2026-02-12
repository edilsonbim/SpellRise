#include "BasicAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace SpellRiseBasic
{
	constexpr float CARRY_MIN = 0.f;
	constexpr float CARRY_MAX = 300.f;

	constexpr float RES_MIN = 0.f;
	constexpr float RES_MAX = 75.f;

	constexpr float ARMOR_MIN = 0.f;
	constexpr float ARMOR_MAX = 1000.f;
}

UBasicAttributeSet::UBasicAttributeSet()
{
	CarryWeight = 30.f;

	PhysicalResistance = 0.f;
	MagicResistance = 0.f;
	ElementalResistance = 0.f;
	FireResistance = 0.f;
	FrostResistance = 0.f;
	LightningResistance = 0.f;
	Armor = 0.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, CarryWeight, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, ElementalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, FrostResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCarryWeightAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::CARRY_MIN, SpellRiseBasic::CARRY_MAX);
	}
	else if (Attribute == GetPhysicalResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX);
	}
	else if (Attribute == GetMagicResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX);
	}
	else if (Attribute == GetElementalResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX);
	}

	else if (Attribute == GetFireResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX);
	}
	else if (Attribute == GetFrostResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX);
	}
	else if (Attribute == GetLightningResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX);
	}
	else if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::ARMOR_MIN, SpellRiseBasic::ARMOR_MAX);
	}
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	if (Attr == GetCarryWeightAttribute())
	{
		SetCarryWeight(FMath::Clamp(GetCarryWeight(), SpellRiseBasic::CARRY_MIN, SpellRiseBasic::CARRY_MAX));
	}
	else if (Attr == GetPhysicalResistanceAttribute())
	{
		SetPhysicalResistance(FMath::Clamp(GetPhysicalResistance(), SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX));
	}
	else if (Attr == GetMagicResistanceAttribute())
	{
		SetMagicResistance(FMath::Clamp(GetMagicResistance(), SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX));
	}
	else if (Attr == GetElementalResistanceAttribute())
	{
		SetElementalResistance(FMath::Clamp(GetElementalResistance(), SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX));
	}

	else if (Attr == GetFireResistanceAttribute())
	{
		SetFireResistance(FMath::Clamp(GetFireResistance(), SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX));
	}
	else if (Attr == GetFrostResistanceAttribute())
	{
		SetFrostResistance(FMath::Clamp(GetFrostResistance(), SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX));
	}
	else if (Attr == GetLightningResistanceAttribute())
	{
		SetLightningResistance(FMath::Clamp(GetLightningResistance(), SpellRiseBasic::RES_MIN, SpellRiseBasic::RES_MAX));
	}
	else if (Attr == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), SpellRiseBasic::ARMOR_MIN, SpellRiseBasic::ARMOR_MAX));
	}
}

void UBasicAttributeSet::OnRep_CarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, CarryWeight, OldValue);
}

void UBasicAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, PhysicalResistance, OldValue);
}

void UBasicAttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MagicResistance, OldValue);
}

void UBasicAttributeSet::OnRep_ElementalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, ElementalResistance, OldValue);
}


void UBasicAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, FireResistance, OldValue);
}
void UBasicAttributeSet::OnRep_FrostResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, FrostResistance, OldValue);
}
void UBasicAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, LightningResistance, OldValue);
}

void UBasicAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Armor, OldValue);
}
