#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UDerivedStatsAttributeSet::UDerivedStatsAttributeSet()
{
}

void UDerivedStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, MeleeDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, BowDamageMultiplier,   COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, SpellDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, HealingMultiplier,     COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CastTimeReduction,     COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritChance,            COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, CritDamage,            COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ArmorPenetration,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDerivedStatsAttributeSet, ManaCostReduction,     COND_None, REPNOTIFY_Always);
}

void UDerivedStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// se quiser clamp global aqui, faça por Attribute == GetXAttribute()
}

void UDerivedStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

// -------------------- OnRep --------------------

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
