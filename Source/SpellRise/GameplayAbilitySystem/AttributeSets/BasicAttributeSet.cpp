#include "BasicAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace SpellRiseBasic
{
	constexpr float PRIMARY_MIN = 20.f;
	constexpr float PRIMARY_MAX = 100.f;
}

UBasicAttributeSet::UBasicAttributeSet()
{
	InitStrength(SpellRiseBasic::PRIMARY_MIN);
	InitAgility(SpellRiseBasic::PRIMARY_MIN);
	InitIntelligence(SpellRiseBasic::PRIMARY_MIN);
	InitWisdom(SpellRiseBasic::PRIMARY_MIN);
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Wisdom, COND_None, REPNOTIFY_Always);
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetStrengthAttribute()
		|| Attribute == GetAgilityAttribute()
		|| Attribute == GetIntelligenceAttribute()
		|| Attribute == GetWisdomAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseBasic::PRIMARY_MIN, SpellRiseBasic::PRIMARY_MAX);
	}
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	if (Attr == GetStrengthAttribute())
	{
		SetStrength(FMath::Clamp(GetStrength(), SpellRiseBasic::PRIMARY_MIN, SpellRiseBasic::PRIMARY_MAX));
	}
	else if (Attr == GetAgilityAttribute())
	{
		SetAgility(FMath::Clamp(GetAgility(), SpellRiseBasic::PRIMARY_MIN, SpellRiseBasic::PRIMARY_MAX));
	}
	else if (Attr == GetIntelligenceAttribute())
	{
		SetIntelligence(FMath::Clamp(GetIntelligence(), SpellRiseBasic::PRIMARY_MIN, SpellRiseBasic::PRIMARY_MAX));
	}
	else if (Attr == GetWisdomAttribute())
	{
		SetWisdom(FMath::Clamp(GetWisdom(), SpellRiseBasic::PRIMARY_MIN, SpellRiseBasic::PRIMARY_MAX));
	}
}

void UBasicAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Strength, OldValue);
}

void UBasicAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Agility, OldValue);
}

void UBasicAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Intelligence, OldValue);
}

void UBasicAttributeSet::OnRep_Wisdom(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Wisdom, OldValue);
}
