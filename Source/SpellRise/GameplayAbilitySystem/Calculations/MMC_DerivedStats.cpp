// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "MMC_DerivedStats.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "GameplayEffectExtension.h"

namespace SR_Primary
{
	constexpr float MIN = 0.f;
	constexpr float BASE = 20.f;
	constexpr float EFFECTIVE_MAX = 140.f;
	constexpr float BONUS_RANGE = 100.f;

	static float ClampPrimary(float V)
	{
		return FMath::Clamp(V, MIN, EFFECTIVE_MAX);
	}

	static float Normalize01_FromBaseline(float V)
	{
		const float C = ClampPrimary(V);
		const float Bonus = FMath::Clamp(C - BASE, 0.f, BONUS_RANGE);
		return Bonus / BONUS_RANGE;
	}
}

UMMC_PrimaryBase::UMMC_PrimaryBase()
	: StrengthDef(UCombatAttributeSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, AgilityDef(UCombatAttributeSet::GetAgilityAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, IntelligenceDef(UCombatAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, WisdomDef(UCombatAttributeSet::GetWisdomAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	RelevantAttributesToCapture.Add(StrengthDef);
	RelevantAttributesToCapture.Add(AgilityDef);
	RelevantAttributesToCapture.Add(IntelligenceDef);
	RelevantAttributesToCapture.Add(WisdomDef);
}

static void SR_BuildParams(const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters& OutParams)
{
	OutParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	OutParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
}

float UMMC_PrimaryBase::GetStrength(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	SR_BuildParams(Spec, Params);

	float Out = SR_Primary::BASE;
	GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Out);
	return Out;
}

float UMMC_PrimaryBase::GetAgility(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	SR_BuildParams(Spec, Params);

	float Out = SR_Primary::BASE;
	GetCapturedAttributeMagnitude(AgilityDef, Spec, Params, Out);
	return Out;
}

float UMMC_PrimaryBase::GetIntelligence(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	SR_BuildParams(Spec, Params);

	float Out = SR_Primary::BASE;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, Params, Out);
	return Out;
}

float UMMC_PrimaryBase::GetWisdom(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	SR_BuildParams(Spec, Params);

	float Out = SR_Primary::BASE;
	GetCapturedAttributeMagnitude(WisdomDef, Spec, Params, Out);
	return Out;
}

float UMMC_PrimaryBase::Normalize01_FromBaseline(float PrimaryValue) const
{
	return SR_Primary::Normalize01_FromBaseline(PrimaryValue);
}

UMMC_CritChance::UMMC_CritChance() = default;
float UMMC_CritChance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return 0.05f;
}

UMMC_CritDamage::UMMC_CritDamage() = default;
float UMMC_CritDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return 1.5f;
}

UMMC_ArmorPenetration::UMMC_ArmorPenetration() = default;
float UMMC_ArmorPenetration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return 0.f;
}

UMMC_MaxHealthFromPrimaries::UMMC_MaxHealthFromPrimaries() = default;
float UMMC_MaxHealthFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Str = SR_Primary::ClampPrimary(GetStrength(Spec));
	return FMath::Max(1.f, 100.f + (Str * 3.f));
}

UMMC_MaxManaFromPrimaries::UMMC_MaxManaFromPrimaries() = default;
float UMMC_MaxManaFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Int = SR_Primary::ClampPrimary(GetIntelligence(Spec));
	const float Wis = SR_Primary::ClampPrimary(GetWisdom(Spec));
	return FMath::Max(1.f, 100.f + (Int * 2.f) + Wis);
}

UMMC_MaxStaminaFromPrimaries::UMMC_MaxStaminaFromPrimaries() = default;
float UMMC_MaxStaminaFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Agi = SR_Primary::ClampPrimary(GetAgility(Spec));
	return FMath::Max(1.f, 100.f + (Agi * 3.f));
}

UMMC_CarryWeightFromPrimaries::UMMC_CarryWeightFromPrimaries() = default;
float UMMC_CarryWeightFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Str = SR_Primary::ClampPrimary(GetStrength(Spec));
	return Str * 2.f;
}
