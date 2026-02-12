#include "DerivedStatsAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace SpellRiseDerived
{
	constexpr float MULT_MIN = 0.f;
	constexpr float MULT_MAX = 10.f;

	constexpr float SPEED_MIN = 0.2f;
	constexpr float SPEED_MAX = 10.0f;

	constexpr float CRITCHANCE_MIN = 0.f;
	constexpr float CRITCHANCE_MAX = 0.6f;

	constexpr float CRITDMG_MIN = 1.0f;
	constexpr float CRITDMG_MAX = 3.0f;

	constexpr float MANACOST_MIN = 0.5f;
	constexpr float MANACOST_MAX = 2.0f;

	constexpr float BREAK_MIN = 0.f;
	constexpr float BREAK_MAX = 1000.f;

	constexpr float STABILITY_MIN = 0.f;
	constexpr float STABILITY_MAX = 1000.f;

	constexpr float ARMORPEN_MIN = 0.f;
	constexpr float ARMORPEN_MAX = 75.f;

	constexpr float ELEMPEN_MIN = 0.f;
	constexpr float ELEMPEN_MAX = 75.f;
}

UDerivedStatsAttributeSet::UDerivedStatsAttributeSet()
{
	// Baselines consistent with your "primaries start at 10 and give no bonus"
	InitWeaponDamageMultiplier(1.f);
	InitAttackSpeedMultiplier(1.f);
	InitCritChance(0.f);
	InitCritDamage(1.5f);
	InitSpellPowerMultiplier(1.f);
	InitCastSpeedMultiplier(1.f);
	InitManaCostMultiplier(1.f);

	InitBreakPower(0.f);
	InitCastStability(0.f);

	InitArmorPenetration(0.f);
	InitElementPenetration(0.f);
}

void UDerivedStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, WeaponDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, AttackSpeedMultiplier,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritChance,            COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritDamage,            COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, SpellPowerMultiplier,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CastSpeedMultiplier,   COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ManaCostMultiplier,    COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, BreakPower,            COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CastStability,         COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ArmorPenetration,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ElementPenetration,    COND_None, REPNOTIFY_Always);
}

void UDerivedStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetWeaponDamageMultiplierAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::MULT_MIN, SpellRiseDerived::MULT_MAX);
	else if (Attribute == GetAttackSpeedMultiplierAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::SPEED_MIN, SpellRiseDerived::SPEED_MAX);
	else if (Attribute == GetCritChanceAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::CRITCHANCE_MIN, SpellRiseDerived::CRITCHANCE_MAX);
	else if (Attribute == GetCritDamageAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::CRITDMG_MIN, SpellRiseDerived::CRITDMG_MAX);
	else if (Attribute == GetSpellPowerMultiplierAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::MULT_MIN, SpellRiseDerived::MULT_MAX);
	else if (Attribute == GetCastSpeedMultiplierAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::SPEED_MIN, SpellRiseDerived::SPEED_MAX);
	else if (Attribute == GetManaCostMultiplierAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::MANACOST_MIN, SpellRiseDerived::MANACOST_MAX);
	else if (Attribute == GetBreakPowerAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::BREAK_MIN, SpellRiseDerived::BREAK_MAX);
	else if (Attribute == GetCastStabilityAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::STABILITY_MIN, SpellRiseDerived::STABILITY_MAX);
	else if (Attribute == GetArmorPenetrationAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::ARMORPEN_MIN, SpellRiseDerived::ARMORPEN_MAX);
	else if (Attribute == GetElementPenetrationAttribute())
		NewValue = FMath::Clamp(NewValue, SpellRiseDerived::ELEMPEN_MIN, SpellRiseDerived::ELEMPEN_MAX);
}

void UDerivedStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& A = Data.EvaluatedData.Attribute;

	if (A == GetWeaponDamageMultiplierAttribute())
		SetWeaponDamageMultiplier(FMath::Clamp(GetWeaponDamageMultiplier(), SpellRiseDerived::MULT_MIN, SpellRiseDerived::MULT_MAX));
	else if (A == GetAttackSpeedMultiplierAttribute())
		SetAttackSpeedMultiplier(FMath::Clamp(GetAttackSpeedMultiplier(), SpellRiseDerived::SPEED_MIN, SpellRiseDerived::SPEED_MAX));
	else if (A == GetCritChanceAttribute())
		SetCritChance(FMath::Clamp(GetCritChance(), SpellRiseDerived::CRITCHANCE_MIN, SpellRiseDerived::CRITCHANCE_MAX));
	else if (A == GetCritDamageAttribute())
		SetCritDamage(FMath::Clamp(GetCritDamage(), SpellRiseDerived::CRITDMG_MIN, SpellRiseDerived::CRITDMG_MAX));
	else if (A == GetSpellPowerMultiplierAttribute())
		SetSpellPowerMultiplier(FMath::Clamp(GetSpellPowerMultiplier(), SpellRiseDerived::MULT_MIN, SpellRiseDerived::MULT_MAX));
	else if (A == GetCastSpeedMultiplierAttribute())
		SetCastSpeedMultiplier(FMath::Clamp(GetCastSpeedMultiplier(), SpellRiseDerived::SPEED_MIN, SpellRiseDerived::SPEED_MAX));
	else if (A == GetManaCostMultiplierAttribute())
		SetManaCostMultiplier(FMath::Clamp(GetManaCostMultiplier(), SpellRiseDerived::MANACOST_MIN, SpellRiseDerived::MANACOST_MAX));
	else if (A == GetBreakPowerAttribute())
		SetBreakPower(FMath::Clamp(GetBreakPower(), SpellRiseDerived::BREAK_MIN, SpellRiseDerived::BREAK_MAX));
	else if (A == GetCastStabilityAttribute())
		SetCastStability(FMath::Clamp(GetCastStability(), SpellRiseDerived::STABILITY_MIN, SpellRiseDerived::STABILITY_MAX));
	else if (A == GetArmorPenetrationAttribute())
		SetArmorPenetration(FMath::Clamp(GetArmorPenetration(), SpellRiseDerived::ARMORPEN_MIN, SpellRiseDerived::ARMORPEN_MAX));
	else if (A == GetElementPenetrationAttribute())
		SetElementPenetration(FMath::Clamp(GetElementPenetration(), SpellRiseDerived::ELEMPEN_MIN, SpellRiseDerived::ELEMPEN_MAX));
}

// RepNotifies
void UDerivedStatsAttributeSet::OnRep_WeaponDamageMultiplier(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, WeaponDamageMultiplier, OldValue); }
void UDerivedStatsAttributeSet::OnRep_AttackSpeedMultiplier(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, AttackSpeedMultiplier, OldValue); }
void UDerivedStatsAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CritChance, OldValue); }
void UDerivedStatsAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CritDamage, OldValue); }
void UDerivedStatsAttributeSet::OnRep_SpellPowerMultiplier(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, SpellPowerMultiplier, OldValue); }
void UDerivedStatsAttributeSet::OnRep_CastSpeedMultiplier(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CastSpeedMultiplier, OldValue); }
void UDerivedStatsAttributeSet::OnRep_ManaCostMultiplier(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, ManaCostMultiplier, OldValue); }

void UDerivedStatsAttributeSet::OnRep_BreakPower(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, BreakPower, OldValue); }
void UDerivedStatsAttributeSet::OnRep_CastStability(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CastStability, OldValue); }

void UDerivedStatsAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, ArmorPenetration, OldValue); }
void UDerivedStatsAttributeSet::OnRep_ElementPenetration(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, ElementPenetration, OldValue); }
