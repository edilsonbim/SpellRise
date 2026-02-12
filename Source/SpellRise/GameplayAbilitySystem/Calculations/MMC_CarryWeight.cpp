#include "MMC_CarryWeight.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_CarryWeight::UMMC_CarryWeight()
{
	VigorDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetVigorAttribute(),
		EGameplayEffectAttributeCaptureSource::Target, // <- padrão SpellRise
		true
	);

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_CarryWeight::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Vigor = 0.f;
	if (!GetCapturedAttributeMagnitude(VigorDef, Spec, Params, Vigor))
	{
		Vigor = 10.f; // fallback
	}

	Vigor = FMath::Clamp(Vigor, 10.f, 60.f);

	// CarryWeight = 30 + (Vigor * 1.5)
	return 30.f + (Vigor * 1.5f);
}
