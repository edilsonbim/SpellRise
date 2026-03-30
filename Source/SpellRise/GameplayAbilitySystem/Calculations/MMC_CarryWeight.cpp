#include "MMC_CarryWeight.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_CarryWeight::UMMC_CarryWeight()
{
	StrengthDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target, // <- padrão SpellRise
		true
	);

	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_CarryWeight::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Strength = 0.f;
	if (!GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Strength))
	{
		Strength = 20.f; // fallback
	}

	const float ClampedStrength = FMath::Clamp(Strength, 20.f, 120.f);
	const float LegacyStrengthScale = 10.f + (((ClampedStrength - 20.f) / 100.f) * 50.f);
	const float StrengthEquivalent = FMath::Clamp(LegacyStrengthScale, 10.f, 60.f);

	// Legacy output scale preserved: CarryWeight = 30 + (StrengthEquivalent * 1.5)
	return 30.f + (StrengthEquivalent * 1.5f);
}
