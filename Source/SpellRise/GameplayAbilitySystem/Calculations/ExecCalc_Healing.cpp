// Cabeçalho de implementação: calcula cura no servidor preservando o contrato GAS do SpellRise.
#include "ExecCalc_Healing.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseHealing, Log, All);

namespace SpellRiseHealingTags
{
	inline const FGameplayTag& Data_BaseHeal()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseHeal"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_Heal()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.Heal"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_HealingScaling()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.HealingScaling"), false);
		return Tag;
	}

	inline float GetSBC_WithFallback(
		const FGameplayEffectSpec& Spec,
		const FGameplayTag& PrimaryTag,
		const FGameplayTag& FallbackTag,
		float DefaultValue)
	{
		if (PrimaryTag.IsValid() && Spec.SetByCallerTagMagnitudes.Contains(PrimaryTag))
		{
			return Spec.GetSetByCallerMagnitude(PrimaryTag, false, DefaultValue);
		}

		if (FallbackTag.IsValid() && Spec.SetByCallerTagMagnitudes.Contains(FallbackTag))
		{
			return Spec.GetSetByCallerMagnitude(FallbackTag, false, DefaultValue);
		}

		return DefaultValue;
	}
}

namespace SpellRiseHealingProgression
{
	static float ApplyWisdomContribution(float Healing, float Wisdom)
	{
		const float ClampedHealing = FMath::Max(0.f, Healing);
		const float WisdomAlpha = FMath::Clamp(Wisdom, 0.f, 100.f) / 100.f;
		return (ClampedHealing * 0.50f) + (ClampedHealing * 0.50f * WisdomAlpha);
	}
}

UExecCalc_Healing::FCaptureDefs::FCaptureDefs()
{
	WisdomDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetWisdomAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);
}

const UExecCalc_Healing::FCaptureDefs& UExecCalc_Healing::Captures()
{
	static FCaptureDefs CaptureDefs;
	return CaptureDefs;
}

UExecCalc_Healing::UExecCalc_Healing()
{
	RelevantAttributesToCapture.Add(Captures().WisdomDef);
}

void UExecCalc_Healing::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FCaptureDefs& C = Captures();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!SourceASC || !TargetASC)
	{
		return;
	}

	const float BaseHeal = FMath::Max(
		0.f,
		SpellRiseHealingTags::GetSBC_WithFallback(
			Spec,
			SpellRiseHealingTags::Data_BaseHeal(),
			SpellRiseHealingTags::Data_Heal(),
			0.f));

	if (BaseHeal <= 0.f)
	{
		UE_LOG(LogSpellRiseHealing, Verbose, TEXT("Healing rejeitado: BaseHeal <= 0. Source=%s Target=%s"),
			*GetNameSafe(SourceASC->GetAvatarActor()),
			*GetNameSafe(TargetASC->GetAvatarActor()));
		return;
	}

	const float HealingScaling = FMath::Clamp(
		SpellRiseHealingTags::Data_HealingScaling().IsValid()
			? Spec.GetSetByCallerMagnitude(SpellRiseHealingTags::Data_HealingScaling(), false, 1.f)
			: 1.f,
		0.f,
		10.f);

	float FinalHealing = BaseHeal * HealingScaling;
	float Wisdom = 20.f;
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.WisdomDef, Params, Wisdom);
	FinalHealing = SpellRiseHealingProgression::ApplyWisdomContribution(FinalHealing, Wisdom);
	if (!FMath::IsFinite(FinalHealing) || FinalHealing <= 0.f)
	{
		return;
	}

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UResourceAttributeSet::GetHealingAttribute(),
			EGameplayModOp::Additive,
			FinalHealing));
}
