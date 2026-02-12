#include "MMC_MaxMana.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	FocusDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetFocusAttribute(),
		EGameplayEffectAttributeCaptureSource::Target, // <- recomendado
		true
	);

	RelevantAttributesToCapture.Add(FocusDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Focus = 0.f;
	if (!GetCapturedAttributeMagnitude(FocusDef, Spec, Params, Focus))
	{
		Focus = 10.f; // fallback seguro
	}

	Focus = FMath::Clamp(Focus, 10.f, 60.f);

	// MaxMana = 80 + (Focus * 14)
	return 80.f + (Focus * 14.f);
}
