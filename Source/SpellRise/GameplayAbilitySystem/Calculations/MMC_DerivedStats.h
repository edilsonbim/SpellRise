#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_DerivedStats.generated.h"

/**
 * DERIVED STATS - UNIFIED MMC SYSTEM
 * 
 * BASELINE RULE (AAA):
 * - Primaries start at 10 (minimum)
 * - Only points ABOVE 10 contribute to derived stats
 * - Delta = max(0, Primary - 10)
 * 
 * CAPTURE POLICY:
 * - ALWAYS capture from TARGET (the character who owns the stats)
 * - bSnapshot = false (use live values, not snapshot)
 * 
 * FORMULAS (balanced for 10-60 range):
 * 
 * WeaponDamageMultiplier = 1 + (VIG-10) * 0.035
 * AttackSpeedMultiplier  = 1 + (AGI-10) * 0.020
 * CritChance             = min((FOC-10) * 0.0075, 0.60)
 * CritDamage             = min(1.5 + (FOC-10) * 0.0150, 3.0)
 * SpellPowerMultiplier   = 1 + (ATT-10) * 0.080
 * CastSpeedMultiplier    = 1 + (FOC-10) * 0.005
 * ManaCostMultiplier     = max(1 - (WIL-10) * 0.006, 0.5)
 */

class UCombatAttributeSet;

// --------------------------------------------------------------------
// Base class with common capture logic
// --------------------------------------------------------------------
UCLASS(Abstract)
class SPELLRISE_API USpellRiseDerivedStatMMC : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()

public:
    USpellRiseDerivedStatMMC();

protected:
    // Capture definitions - ALL from TARGET
    FGameplayEffectAttributeCaptureDefinition VigorDef;
    FGameplayEffectAttributeCaptureDefinition AgilityDef;
    FGameplayEffectAttributeCaptureDefinition FocusDef;
    FGameplayEffectAttributeCaptureDefinition WillpowerDef;
    FGameplayEffectAttributeCaptureDefinition AttunementDef;

    // Initialize captures (called from constructor)
    void InitializeCaptureDefinitions();

    // Helper methods
    float GetPrimaryClamped(const FGameplayEffectSpec& Spec, 
                            const FGameplayEffectAttributeCaptureDefinition& Def) const;
    
    float GetDeltaFrom10(const FGameplayEffectSpec& Spec, 
                         const FGameplayEffectAttributeCaptureDefinition& Def) const;
    
    static float ClampPrimary(float Value);
    static float Clamp10To60(float Value);
};

// --------------------------------------------------------------------
// Individual MMCs - each handles ONE derived stat
// --------------------------------------------------------------------

UCLASS()
class SPELLRISE_API UMMC_WeaponDamageMultiplier : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_AttackSpeedMultiplier : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_CritChance : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_CritDamage : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_SpellPowerMultiplier : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_CastSpeedMultiplier : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_ManaCostMultiplier : public USpellRiseDerivedStatMMC
{
    GENERATED_BODY()
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};