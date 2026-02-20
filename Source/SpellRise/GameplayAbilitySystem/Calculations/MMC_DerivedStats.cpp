#include "MMC_DerivedStats.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "GameplayEffectExtension.h"

namespace SR_Primary
{
	constexpr float BASE = 20.f;
	constexpr float EFFECTIVE_MAX = 120.f;
	constexpr float BONUS_RANGE = 100.f;

	static float ClampPrimary(float V)
	{
		return FMath::Clamp(V, BASE, EFFECTIVE_MAX);
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

UMMC_MeleeDamageMultiplier::UMMC_MeleeDamageMultiplier() = default;
float UMMC_MeleeDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetStrength(Spec));
	return FMath::Clamp(1.f + (0.50f * T), 0.f, 10.f);
}

UMMC_BowDamageMultiplier::UMMC_BowDamageMultiplier() = default;
float UMMC_BowDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetAgility(Spec));
	return FMath::Clamp(1.f + (0.50f * T), 0.f, 10.f);
}

UMMC_SpellDamageMultiplier::UMMC_SpellDamageMultiplier() = default;
float UMMC_SpellDamageMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetIntelligence(Spec));
	return FMath::Clamp(1.f + (0.50f * T), 0.f, 10.f);
}

UMMC_HealingMultiplier::UMMC_HealingMultiplier() = default;
float UMMC_HealingMultiplier::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetWisdom(Spec));
	return FMath::Clamp(1.f + (0.40f * T), 0.f, 10.f);
}

UMMC_CastTimeReduction::UMMC_CastTimeReduction() = default;
float UMMC_CastTimeReduction::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetIntelligence(Spec));
	return FMath::Clamp(0.f + (0.096f * T), 0.f, 0.096f);
}

UMMC_CritChance::UMMC_CritChance() = default;
float UMMC_CritChance::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetAgility(Spec));
	return FMath::Clamp(0.05f + (0.20f * T), 0.f, 0.25f);
}

UMMC_CritDamage::UMMC_CritDamage() = default;
float UMMC_CritDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetWisdom(Spec));
	return FMath::Clamp(1.5f + (0.50f * T), 1.f, 2.0f);
}

UMMC_ArmorPenetration::UMMC_ArmorPenetration() = default;
float UMMC_ArmorPenetration::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetStrength(Spec));
	return FMath::Clamp(0.f + (0.30f * T), 0.f, 0.30f);
}

UMMC_ManaCostReduction::UMMC_ManaCostReduction() = default;
float UMMC_ManaCostReduction::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float T = Normalize01_FromBaseline(GetWisdom(Spec));
	return FMath::Clamp(0.f + (0.20f * T), 0.f, 0.20f);
}

UMMC_MaxHealthFromPrimaries::UMMC_MaxHealthFromPrimaries() = default;
float UMMC_MaxHealthFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Str = SR_Primary::ClampPrimary(GetStrength(Spec));
	const float Wis = SR_Primary::ClampPrimary(GetWisdom(Spec));

	const float StrBonus = FMath::Clamp(Str - SR_Primary::BASE, 0.f, 100.f);
	const float WisBonus = FMath::Clamp(Wis - SR_Primary::BASE, 0.f, 100.f);

	return FMath::Max(1.f, 180.f + (StrBonus * 2.f) + (WisBonus * 1.f));
}

UMMC_MaxManaFromPrimaries::UMMC_MaxManaFromPrimaries() = default;
float UMMC_MaxManaFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Int = SR_Primary::ClampPrimary(GetIntelligence(Spec));
	const float Wis = SR_Primary::ClampPrimary(GetWisdom(Spec));

	const float IntBonus = FMath::Clamp(Int - SR_Primary::BASE, 0.f, 100.f);
	const float WisBonus = FMath::Clamp(Wis - SR_Primary::BASE, 0.f, 100.f);

	return FMath::Max(1.f, 180.f + (IntBonus * 2.f) + (WisBonus * 1.f));
}

UMMC_MaxStaminaFromPrimaries::UMMC_MaxStaminaFromPrimaries() = default;
float UMMC_MaxStaminaFromPrimaries::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Str = SR_Primary::ClampPrimary(GetStrength(Spec));
	const float Agi = SR_Primary::ClampPrimary(GetAgility(Spec));

	const float StrBonus = FMath::Clamp(Str - SR_Primary::BASE, 0.f, 100.f);
	const float AgiBonus = FMath::Clamp(Agi - SR_Primary::BASE, 0.f, 100.f);

	return FMath::Max(1.f, 180.f + (StrBonus * 1.f) + (AgiBonus * 2.f));
}
