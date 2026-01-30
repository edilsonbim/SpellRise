#include "CombatAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

namespace SpellRiseAttr
{
	constexpr float PRIMARY_MIN = 10.f;
	constexpr float PRIMARY_MAX = 60.f;

	// CastSpeed is percent (can be negative for slow).
	constexpr float CASTSPEED_MIN = -80.f;
	constexpr float CASTSPEED_MAX = 200.f;

	// Bonus stats: keep sane
	constexpr float CASTSTAB_MIN = 0.f;
	constexpr float CASTSTAB_MAX = 500.f;

	constexpr float BREAK_MIN = 0.f;
	constexpr float BREAK_MAX = 500.f;
}

UCombatAttributeSet::UCombatAttributeSet()
{
	InitVigor(SpellRiseAttr::PRIMARY_MIN);
	InitFocus(SpellRiseAttr::PRIMARY_MIN);
	InitAgility(SpellRiseAttr::PRIMARY_MIN);
	InitWillpower(SpellRiseAttr::PRIMARY_MIN);
	InitAttunement(SpellRiseAttr::PRIMARY_MIN);

	InitCastSpeed(0.f);
	InitCastStability(0.f);
	InitBreakPower(0.f);
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Focus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Willpower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Attunement, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CastSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CastStability, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BreakPower, COND_None, REPNOTIFY_Always);
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Primary clamps
	if (Attribute == GetVigorAttribute()
		|| Attribute == GetFocusAttribute()
		|| Attribute == GetAgilityAttribute()
		|| Attribute == GetWillpowerAttribute()
		|| Attribute == GetAttunementAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseAttr::PRIMARY_MIN, SpellRiseAttr::PRIMARY_MAX);
	}
	else if (Attribute == GetCastSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseAttr::CASTSPEED_MIN, SpellRiseAttr::CASTSPEED_MAX);
	}
	else if (Attribute == GetCastStabilityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseAttr::CASTSTAB_MIN, SpellRiseAttr::CASTSTAB_MAX);
	}
	else if (Attribute == GetBreakPowerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseAttr::BREAK_MIN, SpellRiseAttr::BREAK_MAX);
	}
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& A = Data.EvaluatedData.Attribute;

	if (A == GetVigorAttribute())
	{
		SetVigor(FMath::Clamp(GetVigor(), SpellRiseAttr::PRIMARY_MIN, SpellRiseAttr::PRIMARY_MAX));
	}
	else if (A == GetFocusAttribute())
	{
		SetFocus(FMath::Clamp(GetFocus(), SpellRiseAttr::PRIMARY_MIN, SpellRiseAttr::PRIMARY_MAX));
	}
	else if (A == GetAgilityAttribute())
	{
		SetAgility(FMath::Clamp(GetAgility(), SpellRiseAttr::PRIMARY_MIN, SpellRiseAttr::PRIMARY_MAX));
	}
	else if (A == GetWillpowerAttribute())
	{
		SetWillpower(FMath::Clamp(GetWillpower(), SpellRiseAttr::PRIMARY_MIN, SpellRiseAttr::PRIMARY_MAX));
	}
	else if (A == GetAttunementAttribute())
	{
		SetAttunement(FMath::Clamp(GetAttunement(), SpellRiseAttr::PRIMARY_MIN, SpellRiseAttr::PRIMARY_MAX));
	}
	else if (A == GetCastSpeedAttribute())
	{
		SetCastSpeed(FMath::Clamp(GetCastSpeed(), SpellRiseAttr::CASTSPEED_MIN, SpellRiseAttr::CASTSPEED_MAX));
	}
	else if (A == GetCastStabilityAttribute())
	{
		SetCastStability(FMath::Clamp(GetCastStability(), SpellRiseAttr::CASTSTAB_MIN, SpellRiseAttr::CASTSTAB_MAX));
	}
	else if (A == GetBreakPowerAttribute())
	{
		SetBreakPower(FMath::Clamp(GetBreakPower(), SpellRiseAttr::BREAK_MIN, SpellRiseAttr::BREAK_MAX));
	}
}

/* =======================
   OnRep
   ======================= */

void UCombatAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Vigor, OldValue);
}
void UCombatAttributeSet::OnRep_Focus(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Focus, OldValue);
}
void UCombatAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Agility, OldValue);
}
void UCombatAttributeSet::OnRep_Willpower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Willpower, OldValue);
}
void UCombatAttributeSet::OnRep_Attunement(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Attunement, OldValue);
}
void UCombatAttributeSet::OnRep_CastSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CastSpeed, OldValue);
}
void UCombatAttributeSet::OnRep_CastStability(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CastStability, OldValue);
}
void UCombatAttributeSet::OnRep_BreakPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BreakPower, OldValue);
}
