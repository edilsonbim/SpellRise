#include "MMC_MaxStamina.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_MaxStamina::UMMC_MaxStamina()
{
	AgilityDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetAgilityAttribute(),
		EGameplayEffectAttributeCaptureSource::Target, // <- recomendado
		true
	);

	VigorDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetVigorAttribute(),
		EGameplayEffectAttributeCaptureSource::Target, // <- recomendado
		true
	);

	RelevantAttributesToCapture.Add(AgilityDef);
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxStamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Agility = 0.f;
	float Vigor = 0.f;

	if (!GetCapturedAttributeMagnitude(AgilityDef, Spec, Params, Agility))
	{
		Agility = 10.f;
	}
	if (!GetCapturedAttributeMagnitude(VigorDef, Spec, Params, Vigor))
	{
		Vigor = 10.f;
	}

	Agility = FMath::Clamp(Agility, 10.f, 60.f);
	Vigor   = FMath::Clamp(Vigor, 10.f, 60.f);

	// MaxStamina = 100 + (Agility * 10) + (Vigor * 4)
	return 100.f + (Agility * 10.f) + (Vigor * 4.f);
}
