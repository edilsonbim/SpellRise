#include "MMC_DerivedStats.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

// --------------------------------------------------------------------
// Base Class Implementation
// --------------------------------------------------------------------
USpellRiseDerivedStatMMC::USpellRiseDerivedStatMMC()
{
    InitializeCaptureDefinitions();
}

void USpellRiseDerivedStatMMC::InitializeCaptureDefinitions()
{
    // IMPORTANT: ALWAYS capture from TARGET (the character owning the stats)
    // bSnapshot = false to use LIVE values, not snapshot at effect creation
    
    VigorDef = FGameplayEffectAttributeCaptureDefinition(
        UCombatAttributeSet::GetVigorAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false);  // Live values!

    AgilityDef = FGameplayEffectAttributeCaptureDefinition(
        UCombatAttributeSet::GetAgilityAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false);

    FocusDef = FGameplayEffectAttributeCaptureDefinition(
        UCombatAttributeSet::GetFocusAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false);

    WillpowerDef = FGameplayEffectAttributeCaptureDefinition(
        UCombatAttributeSet::GetWillpowerAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false);

    AttunementDef = FGameplayEffectAttributeCaptureDefinition(
        UCombatAttributeSet::GetAttunementAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false);

    // Add all to capture list
    RelevantAttributesToCapture.Add(VigorDef);
    RelevantAttributesToCapture.Add(AgilityDef);
    RelevantAttributesToCapture.Add(FocusDef);
    RelevantAttributesToCapture.Add(WillpowerDef);
    RelevantAttributesToCapture.Add(AttunementDef);
}

float USpellRiseDerivedStatMMC::ClampPrimary(float Value)
{
    return FMath::Clamp(Value, 10.f, 60.f);
}

float USpellRiseDerivedStatMMC::Clamp10To60(float Value)
{
    return FMath::Clamp(Value, 10.f, 60.f);
}

float USpellRiseDerivedStatMMC::GetPrimaryClamped(
    const FGameplayEffectSpec& Spec,
    const FGameplayEffectAttributeCaptureDefinition& Def) const
{
    FAggregatorEvaluateParameters EvalParams;
    EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    float Value = 0.f;
    GetCapturedAttributeMagnitude(Def, Spec, EvalParams, Value);
    
    // Policy: baseline is 10. If capture fails, default to 10.
    return Clamp10To60(Value);
}

float USpellRiseDerivedStatMMC::GetDeltaFrom10(
    const FGameplayEffectSpec& Spec,
    const FGameplayEffectAttributeCaptureDefinition& Def) const
{
    const float Clamped = GetPrimaryClamped(Spec, Def);
    return FMath::Max(0.f, Clamped - 10.f);
}

// --------------------------------------------------------------------
// Individual MMC Implementations
// --------------------------------------------------------------------

float UMMC_WeaponDamageMultiplier::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaVigor = GetDeltaFrom10(Spec, VigorDef);
    // 1.0 base + 3.5% per point over 10
    return 1.f + (DeltaVigor * 0.035f);
}

float UMMC_AttackSpeedMultiplier::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaAgility = GetDeltaFrom10(Spec, AgilityDef);
    // 1.0 base + 2.0% per point over 10
    return 1.f + (DeltaAgility * 0.020f);
}

float UMMC_CritChance::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaFocus = GetDeltaFrom10(Spec, FocusDef);
    // 0.75% per point, capped at 60%
    return FMath::Min(DeltaFocus * 0.0075f, 0.60f);
}

float UMMC_CritDamage::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaFocus = GetDeltaFrom10(Spec, FocusDef);
    // 1.5 base + 1.5% per point, capped at 3.0
    return FMath::Min(1.5f + (DeltaFocus * 0.015f), 3.0f);
}

float UMMC_SpellPowerMultiplier::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaAttunement = GetDeltaFrom10(Spec, AttunementDef);
    // 1.0 base + 8.0% per point over 10
    return 1.f + (DeltaAttunement * 0.080f);
}

float UMMC_CastSpeedMultiplier::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaFocus = GetDeltaFrom10(Spec, FocusDef);
    // 1.0 base + 0.5% per point over 10
    return 1.f + (DeltaFocus * 0.005f);
}

float UMMC_ManaCostMultiplier::CalculateBaseMagnitude_Implementation(
    const FGameplayEffectSpec& Spec) const
{
    const float DeltaWillpower = GetDeltaFrom10(Spec, WillpowerDef);
    // 1.0 base - 0.6% per point, floor at 0.5
    return FMath::Max(1.f - (DeltaWillpower * 0.006f), 0.5f);
}