// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "BasicAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace SpellRiseBasic
{
	constexpr float PRIMARY_MIN = 0.f;
	constexpr float PRIMARY_MAX = 140.f;
	constexpr float PRIMARY_INITIAL = 20.f;
}

UBasicAttributeSet::UBasicAttributeSet()
{
	InitStrength(SpellRiseBasic::PRIMARY_INITIAL);
	InitAgility(SpellRiseBasic::PRIMARY_INITIAL);
	InitIntelligence(SpellRiseBasic::PRIMARY_INITIAL);
	InitWisdom(SpellRiseBasic::PRIMARY_INITIAL);
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Strength, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Agility, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Intelligence, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Wisdom, COND_OwnerOnly, REPNOTIFY_Always);
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
