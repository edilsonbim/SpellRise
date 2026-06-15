// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

namespace SpellRiseDerivedStats
{
	constexpr float MULTIPLIER_MIN = 0.f;
	constexpr float MULTIPLIER_MAX = 10.f;

	constexpr float CRIT_CHANCE_MIN = 0.f;
	constexpr float CRIT_CHANCE_MAX = 0.25f;
	constexpr float CRIT_CHANCE_BASE = 0.05f;

	constexpr float CRIT_DAMAGE_MIN = 1.f;
	constexpr float CRIT_DAMAGE_MAX = 2.f;
	constexpr float CRIT_DAMAGE_BASE = 1.5f;

	constexpr float ARMOR_PENETRATION_MIN = 0.f;
	constexpr float ARMOR_PENETRATION_MAX = 0.30f;
	constexpr float ARMOR_PENETRATION_BASE = 0.f;
}

UDerivedStatsAttributeSet::UDerivedStatsAttributeSet()
{
	InitCritChance(SpellRiseDerivedStats::CRIT_CHANCE_BASE);
	InitCritDamage(SpellRiseDerivedStats::CRIT_DAMAGE_BASE);
	InitArmorPenetration(SpellRiseDerivedStats::ARMOR_PENETRATION_BASE);
}

void UDerivedStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritChance,            COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritDamage,            COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ArmorPenetration,      COND_OwnerOnly, REPNOTIFY_Always);
}

void UDerivedStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCritChanceAttribute())
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
}

void UDerivedStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;

	if (Attribute == GetCritChanceAttribute())
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
