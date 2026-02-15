#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * SpellRise damage execution:
 * - Base damage via SetByCaller:
 *   - Data.BaseWeaponDamage (melee/weapon)
 *   - Data.BaseSpellDamage  (spell)
 * - Optional multipliers:
 *   - Data.DamageScaling (e.g. combo scaling)
 * - Power multipliers (DerivedStatsAttributeSet):
 *   - WeaponDamageMultiplier / SpellPowerMultiplier
 * - Crit/Penetration (DerivedStatsAttributeSet):
 *   - CritChance (0..1), CritDamage (multiplier e.g. 1.5..2.0), ArmorPen/ElementPen (%)
 * - Target defenses (BasicAttributeSet):
 *   - Armor, PhysicalResist/MagicResist/ElementalResist + Fire/Frost/Lightning
 * - Output:
 *   - ResourceAttributeSet::Damage (meta)
 */
UCLASS()
class SPELLRISE_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;

protected:
	struct FCaptureDefs
	{
		FCaptureDefs();

		// Source: multipliers
		FGameplayEffectAttributeCaptureDefinition WeaponDamageMultiplierDef;
		FGameplayEffectAttributeCaptureDefinition SpellPowerMultiplierDef;

		// Source: crit/pen
		FGameplayEffectAttributeCaptureDefinition CritChanceDef;        // expected 0..1
		FGameplayEffectAttributeCaptureDefinition CritDamageDef;        // expected multiplier (e.g. 1.5..2.0)
		FGameplayEffectAttributeCaptureDefinition ArmorPenetrationDef;  // expected percent 0..100
		FGameplayEffectAttributeCaptureDefinition ElementPenetrationDef; // expected percent 0..100

		// Target: defenses
		FGameplayEffectAttributeCaptureDefinition PhysicalResistDef;   // percent 0..100
		FGameplayEffectAttributeCaptureDefinition MagicResistDef;      // percent 0..100
		FGameplayEffectAttributeCaptureDefinition ElementalResistDef;  // percent 0..100
		FGameplayEffectAttributeCaptureDefinition FireResistDef;       // percent 0..100
		FGameplayEffectAttributeCaptureDefinition FrostResistDef;      // percent 0..100
		FGameplayEffectAttributeCaptureDefinition LightningResistDef;  // percent 0..100
		FGameplayEffectAttributeCaptureDefinition ArmorDef;            // raw armor points
	};

	static const FCaptureDefs& Captures();
};
