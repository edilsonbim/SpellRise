#include "MMC_MaxHealth.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetVigorAttribute(),
		EGameplayEffectAttributeCaptureSource::Target, // <- recomendado
		true
	);

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Vigor = 0.f;
	if (!GetCapturedAttributeMagnitude(VigorDef, Spec, Params, Vigor))
	{
		Vigor = 10.f;
	}

	Vigor = FMath::Clamp(Vigor, 10.f, 60.f);

	// MaxHealth = 100 + (Vigor * 12)
	return 100.f + (Vigor * 12.f);
}
