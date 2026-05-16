// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

namespace SpellRiseDerivedStats
{
	constexpr float MULTIPLIER_MIN = 0.f;
	constexpr float MULTIPLIER_MAX = 10.f;

	constexpr float MELEE_DAMAGE_MULTIPLIER_BASE = 1.f;
	constexpr float BOW_DAMAGE_MULTIPLIER_BASE = 1.f;
	constexpr float SPELL_DAMAGE_MULTIPLIER_BASE = 1.f;
	constexpr float HEALING_MULTIPLIER_BASE = 1.f;

	constexpr float CAST_TIME_REDUCTION_MIN = 0.f;
	constexpr float CAST_TIME_REDUCTION_MAX = 0.096f;
	constexpr float CAST_TIME_REDUCTION_BASE = 0.f;

	constexpr float CRIT_CHANCE_MIN = 0.f;
	constexpr float CRIT_CHANCE_MAX = 0.25f;
	constexpr float CRIT_CHANCE_BASE = 0.05f;

	constexpr float CRIT_DAMAGE_MIN = 1.f;
	constexpr float CRIT_DAMAGE_MAX = 2.f;
	constexpr float CRIT_DAMAGE_BASE = 1.5f;

	constexpr float ARMOR_PENETRATION_MIN = 0.f;
	constexpr float ARMOR_PENETRATION_MAX = 0.30f;
	constexpr float ARMOR_PENETRATION_BASE = 0.f;

	constexpr float MANA_COST_REDUCTION_MIN = 0.f;
	constexpr float MANA_COST_REDUCTION_MAX = 0.20f;
	constexpr float MANA_COST_REDUCTION_BASE = 0.f;
}

UDerivedStatsAttributeSet::UDerivedStatsAttributeSet()
{
	InitMeleeDamageMultiplier(SpellRiseDerivedStats::MELEE_DAMAGE_MULTIPLIER_BASE);
	InitBowDamageMultiplier(SpellRiseDerivedStats::BOW_DAMAGE_MULTIPLIER_BASE);
	InitSpellDamageMultiplier(SpellRiseDerivedStats::SPELL_DAMAGE_MULTIPLIER_BASE);
	InitHealingMultiplier(SpellRiseDerivedStats::HEALING_MULTIPLIER_BASE);
	InitCastTimeReduction(SpellRiseDerivedStats::CAST_TIME_REDUCTION_BASE);
	InitCritChance(SpellRiseDerivedStats::CRIT_CHANCE_BASE);
	InitCritDamage(SpellRiseDerivedStats::CRIT_DAMAGE_BASE);
	InitArmorPenetration(SpellRiseDerivedStats::ARMOR_PENETRATION_BASE);
	InitManaCostReduction(SpellRiseDerivedStats::MANA_COST_REDUCTION_BASE);
}

void UDerivedStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, MeleeDamageMultiplier, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, BowDamageMultiplier,   COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, SpellDamageMultiplier, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, HealingMultiplier,     COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CastTimeReduction,     COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritChance,            COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritDamage,            COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ArmorPenetration,      COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ManaCostReduction,     COND_OwnerOnly, REPNOTIFY_Always);
}

void UDerivedStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMeleeDamageMultiplierAttribute()
		|| Attribute == GetBowDamageMultiplierAttribute()
		|| Attribute == GetSpellDamageMultiplierAttribute()
		|| Attribute == GetHealingMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseDerivedStats::MULTIPLIER_MIN, SpellRiseDerivedStats::MULTIPLIER_MAX);
	}
	else if (Attribute == GetCastTimeReductionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseDerivedStats::CAST_TIME_REDUCTION_MIN, SpellRiseDerivedStats::CAST_TIME_REDUCTION_MAX);
	}
	else if (Attribute == GetCritChanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseDerivedStats::CRIT_CHANCE_MIN, SpellRiseDerivedStats::CRIT_CHANCE_MAX);
	}
	else if (Attribute == GetCritDamageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseDerivedStats::CRIT_DAMAGE_MIN, SpellRiseDerivedStats::CRIT_DAMAGE_MAX);
	}
	else if (Attribute == GetArmorPenetrationAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseDerivedStats::ARMOR_PENETRATION_MIN, SpellRiseDerivedStats::ARMOR_PENETRATION_MAX);
	}
	else if (Attribute == GetManaCostReductionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseDerivedStats::MANA_COST_REDUCTION_MIN, SpellRiseDerivedStats::MANA_COST_REDUCTION_MAX);
	}
}

void UDerivedStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	if (Attribute == GetMeleeDamageMultiplierAttribute())
	{
		SetMeleeDamageMultiplier(FMath::Clamp(GetMeleeDamageMultiplier(), SpellRiseDerivedStats::MULTIPLIER_MIN, SpellRiseDerivedStats::MULTIPLIER_MAX));
	}
	else if (Attribute == GetBowDamageMultiplierAttribute())
	{
		SetBowDamageMultiplier(FMath::Clamp(GetBowDamageMultiplier(), SpellRiseDerivedStats::MULTIPLIER_MIN, SpellRiseDerivedStats::MULTIPLIER_MAX));
	}
	else if (Attribute == GetSpellDamageMultiplierAttribute())
	{
		SetSpellDamageMultiplier(FMath::Clamp(GetSpellDamageMultiplier(), SpellRiseDerivedStats::MULTIPLIER_MIN, SpellRiseDerivedStats::MULTIPLIER_MAX));
	}
	else if (Attribute == GetHealingMultiplierAttribute())
	{
		SetHealingMultiplier(FMath::Clamp(GetHealingMultiplier(), SpellRiseDerivedStats::MULTIPLIER_MIN, SpellRiseDerivedStats::MULTIPLIER_MAX));
	}
	else if (Attribute == GetCastTimeReductionAttribute())
	{
		SetCastTimeReduction(FMath::Clamp(GetCastTimeReduction(), SpellRiseDerivedStats::CAST_TIME_REDUCTION_MIN, SpellRiseDerivedStats::CAST_TIME_REDUCTION_MAX));
	}
	else if (Attribute == GetCritChanceAttribute())
	{
		SetCritChance(FMath::Clamp(GetCritChance(), SpellRiseDerivedStats::CRIT_CHANCE_MIN, SpellRiseDerivedStats::CRIT_CHANCE_MAX));
	}
	else if (Attribute == GetCritDamageAttribute())
	{
		SetCritDamage(FMath::Clamp(GetCritDamage(), SpellRiseDerivedStats::CRIT_DAMAGE_MIN, SpellRiseDerivedStats::CRIT_DAMAGE_MAX));
	}
	else if (Attribute == GetArmorPenetrationAttribute())
	{
		SetArmorPenetration(FMath::Clamp(GetArmorPenetration(), SpellRiseDerivedStats::ARMOR_PENETRATION_MIN, SpellRiseDerivedStats::ARMOR_PENETRATION_MAX));
	}
	else if (Attribute == GetManaCostReductionAttribute())
	{
		SetManaCostReduction(FMath::Clamp(GetManaCostReduction(), SpellRiseDerivedStats::MANA_COST_REDUCTION_MIN, SpellRiseDerivedStats::MANA_COST_REDUCTION_MAX));
	}
}

void UDerivedStatsAttributeSet::OnRep_MeleeDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, MeleeDamageMultiplier, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_BowDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, BowDamageMultiplier, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_SpellDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, SpellDamageMultiplier, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_HealingMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, HealingMultiplier, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_CastTimeReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CastTimeReduction, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CritChance, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, CritDamage, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, ArmorPenetration, OldValue);
}

void UDerivedStatsAttributeSet::OnRep_ManaCostReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDerivedStatsAttributeSet, ManaCostReduction, OldValue);
}
