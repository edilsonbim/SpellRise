#include "BasicAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace SpellRiseBasic
{
	// CarryWeight (capacidade). Você pode ajustar depois:
	// - Base 30 (já estava)
	// - Gear pesado deve aumentar isso
	constexpr float CARRY_MIN = 0.f;
	constexpr float CARRY_MAX = 300.f; // cap seguro (ajuste quando tiver itens reais)

	// Resistências em % (0..75)
	constexpr float RES_MIN = 0.f;
	constexpr float RES_MAX = 75.f;

	// Armor bruto (diminishing no ExecCalc).
	// Cap aqui é só safety pra não explodir numericamente.
	constexpr float ARMOR_MIN = 0.f;
	constexpr float ARMOR_MAX = 1000.f;
}

UBasicAttributeSet::UBasicAttributeSet()
{
	// Inventory / Weight
	CarryWeight = 30.f;

	// Defense
	PhysicalResistance = 0.f;
	MagicResistance = 0.f;
	Armor = 0.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, CarryWeight, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);
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
	else if (Attr == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), SpellRiseBasic::ARMOR_MIN, SpellRiseBasic::ARMOR_MAX));
	}
}

/* =======================
   OnRep
   ======================= */

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

void UBasicAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Armor, OldValue);
}
