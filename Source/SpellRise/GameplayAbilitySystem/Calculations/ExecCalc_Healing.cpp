// Cabeçalho de implementação: calcula cura no servidor preservando o contrato GAS do SpellRise.
#include "ExecCalc_Healing.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

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

UExecCalc_Healing::UExecCalc_Healing()
{
}

void UExecCalc_Healing::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
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
