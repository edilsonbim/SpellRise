// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "MMC_MaxStamina.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_MaxStamina::UMMC_MaxStamina()
{
	AgilityDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetAgilityAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		true
	);

	StrengthDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		true
	);

	RelevantAttributesToCapture.Add(AgilityDef);
	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_MaxStamina::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Agility = 0.f;
	float Strength = 0.f;

	if (!GetCapturedAttributeMagnitude(AgilityDef, Spec, Params, Agility))
	{
		Agility = 20.f;
	}
	if (!GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Strength))
	{
		Strength = 20.f;
	}

	const float ClampedAgility = FMath::Clamp(Agility, 20.f, 120.f);
	const float ClampedStrength = FMath::Clamp(Strength, 20.f, 120.f);
	const float AgilityBonus = FMath::Clamp(ClampedAgility - 20.f, 0.f, 100.f);
	const float StrengthBonus = FMath::Clamp(ClampedStrength - 20.f, 0.f, 100.f);


	return FMath::Max(1.f, 180.f + (StrengthBonus * 1.f) + (AgilityBonus * 2.f));
}
