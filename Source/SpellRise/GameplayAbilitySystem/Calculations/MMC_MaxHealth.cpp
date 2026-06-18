// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "MMC_MaxHealth.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	StrengthDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		true
	);
	WisdomDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetWisdomAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		true
	);

	RelevantAttributesToCapture.Add(StrengthDef);
	RelevantAttributesToCapture.Add(WisdomDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Strength = 0.f;
	float Wisdom = 0.f;
	if (!GetCapturedAttributeMagnitude(StrengthDef, Spec, Params, Strength))
	{
		Strength = 20.f;
	}
	if (!GetCapturedAttributeMagnitude(WisdomDef, Spec, Params, Wisdom))
	{
		Wisdom = 20.f;
	}
	const float ClampedStrength = FMath::Clamp(Strength, 0.f, 140.f);
	return FMath::Max(1.f, 100.f + (ClampedStrength * 3.f));
}
