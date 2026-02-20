#include "CombatAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
// Include basic attribute set to forward primary attribute requests
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"

namespace SR_Combat
{
	constexpr float PRIMARY_MIN = 20.f;
	constexpr float PRIMARY_MAX_EFFECTIVE = 120.f;

	constexpr float RES_MIN = 0.f;
	constexpr float RES_MAX = 75.f;

	constexpr float MOVESPEED_MIN = 0.f;
	constexpr float MOVESPEED_MAX = 600.f;

	constexpr float MOVESPEEDMULT_MIN = 0.1f;
	constexpr float MOVESPEEDMULT_MAX = 3.0f;
}

UCombatAttributeSet::UCombatAttributeSet()
{
    // Primary attributes are defined on UBasicAttributeSet, so avoid initializing duplicates here.

	MoveSpeed = 0.f;
	MoveSpeedMultiplier = 1.f;

	SlashingRes = 0.f;
	BashingRes = 0.f;
	PiercingRes = 0.f;
	FireRes = 0.f;
	ColdRes = 0.f;
	AcidRes = 0.f;
	ShockRes = 0.f;
	DivineRes = 0.f;
	CurseRes = 0.f;
	AlmightyRes = 0.f;
	GenericRes = 0.f;
	BleedRes = 0.f;
	PoisonRes = 0.f;
	ImpactRes = 0.f;
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Primary attributes are replicated on UBasicAttributeSet

	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MoveSpeedMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, SlashingRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BashingRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PiercingRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, FireRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ColdRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, AcidRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ShockRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, DivineRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CurseRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, AlmightyRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, GenericRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BleedRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PoisonRes, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ImpactRes, COND_None, REPNOTIFY_Always);
}

static float ClampRes(float V)
{
	return FMath::Clamp(V, SR_Combat::RES_MIN, SR_Combat::RES_MAX);
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

    // No clamping for primary attributes here since they live on UBasicAttributeSet

	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SR_Combat::MOVESPEED_MIN, SR_Combat::MOVESPEED_MAX);
		return;
	}

	if (Attribute == GetMoveSpeedMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SR_Combat::MOVESPEEDMULT_MIN, SR_Combat::MOVESPEEDMULT_MAX);
		return;
	}

	if (Attribute == GetSlashingResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetBashingResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetPiercingResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetFireResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetColdResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetAcidResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetShockResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetDivineResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetCurseResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetAlmightyResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetGenericResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetBleedResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetPoisonResAttribute()) NewValue = ClampRes(NewValue);
	else if (Attribute == GetImpactResAttribute()) NewValue = ClampRes(NewValue);
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& A = Data.EvaluatedData.Attribute;

    if (A == GetMoveSpeedAttribute()) SetMoveSpeed(FMath::Clamp(GetMoveSpeed(), SR_Combat::MOVESPEED_MIN, SR_Combat::MOVESPEED_MAX));
    else if (A == GetMoveSpeedMultiplierAttribute()) SetMoveSpeedMultiplier(FMath::Clamp(GetMoveSpeedMultiplier(), SR_Combat::MOVESPEEDMULT_MIN, SR_Combat::MOVESPEEDMULT_MAX));

	else if (A == GetSlashingResAttribute()) SetSlashingRes(ClampRes(GetSlashingRes()));
	else if (A == GetBashingResAttribute()) SetBashingRes(ClampRes(GetBashingRes()));
	else if (A == GetPiercingResAttribute()) SetPiercingRes(ClampRes(GetPiercingRes()));
	else if (A == GetFireResAttribute()) SetFireRes(ClampRes(GetFireRes()));
	else if (A == GetColdResAttribute()) SetColdRes(ClampRes(GetColdRes()));
	else if (A == GetAcidResAttribute()) SetAcidRes(ClampRes(GetAcidRes()));
	else if (A == GetShockResAttribute()) SetShockRes(ClampRes(GetShockRes()));
	else if (A == GetDivineResAttribute()) SetDivineRes(ClampRes(GetDivineRes()));
	else if (A == GetCurseResAttribute()) SetCurseRes(ClampRes(GetCurseRes()));
	else if (A == GetAlmightyResAttribute()) SetAlmightyRes(ClampRes(GetAlmightyRes()));
	else if (A == GetGenericResAttribute()) SetGenericRes(ClampRes(GetGenericRes()));
	else if (A == GetBleedResAttribute()) SetBleedRes(ClampRes(GetBleedRes()));
	else if (A == GetPoisonResAttribute()) SetPoisonRes(ClampRes(GetPoisonRes()));
	else if (A == GetImpactResAttribute()) SetImpactRes(ClampRes(GetImpactRes()));
}

// Primary attributes (Strength, Agility, Intelligence, Wisdom) reside on UBasicAttributeSet,
// so there are no replication handlers needed for them on UCombatAttributeSet.

void UCombatAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MoveSpeed, OldValue); }
void UCombatAttributeSet::OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MoveSpeedMultiplier, OldValue); }

void UCombatAttributeSet::OnRep_SlashingRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, SlashingRes, OldValue); }
void UCombatAttributeSet::OnRep_BashingRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BashingRes, OldValue); }
void UCombatAttributeSet::OnRep_PiercingRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PiercingRes, OldValue); }
void UCombatAttributeSet::OnRep_FireRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, FireRes, OldValue); }
void UCombatAttributeSet::OnRep_ColdRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ColdRes, OldValue); }
void UCombatAttributeSet::OnRep_AcidRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, AcidRes, OldValue); }
void UCombatAttributeSet::OnRep_ShockRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ShockRes, OldValue); }
void UCombatAttributeSet::OnRep_DivineRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, DivineRes, OldValue); }
void UCombatAttributeSet::OnRep_CurseRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CurseRes, OldValue); }
void UCombatAttributeSet::OnRep_AlmightyRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, AlmightyRes, OldValue); }
void UCombatAttributeSet::OnRep_GenericRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, GenericRes, OldValue); }
void UCombatAttributeSet::OnRep_BleedRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BleedRes, OldValue); }
void UCombatAttributeSet::OnRep_PoisonRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PoisonRes, OldValue); }
void UCombatAttributeSet::OnRep_ImpactRes(const FGameplayAttributeData& OldValue){ GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ImpactRes, OldValue); }

// Primary attribute forwarding helpers
FGameplayAttribute UCombatAttributeSet::GetStrengthAttribute()
{
    return UBasicAttributeSet::GetStrengthAttribute();
}

FGameplayAttribute UCombatAttributeSet::GetAgilityAttribute()
{
    return UBasicAttributeSet::GetAgilityAttribute();
}

FGameplayAttribute UCombatAttributeSet::GetIntelligenceAttribute()
{
    return UBasicAttributeSet::GetIntelligenceAttribute();
}

FGameplayAttribute UCombatAttributeSet::GetWisdomAttribute()
{
    return UBasicAttributeSet::GetWisdomAttribute();
}
