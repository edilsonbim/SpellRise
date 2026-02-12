#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * SpellRise damage execution (AAA):
 * - Power multipliers: DerivedStatsAttributeSet (WeaponDamageMultiplier / SpellPowerMultiplier)
 * - Crit/Penetration: DerivedStatsAttributeSet (CritChance/CritDamage/ArmorPen/ElementPen)
 * - Target defenses: BasicAttributeSet (Armor/PhysicalResist/MagicResist)
 * - Output: ResourceAttributeSet::Damage (meta)
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

		// Source: Power multipliers (DerivedStatsAttributeSet)
		FGameplayEffectAttributeCaptureDefinition WeaponDamageMultiplierDef;
		FGameplayEffectAttributeCaptureDefinition SpellPowerMultiplierDef;

		// Source: DPS / Derived (DerivedStatsAttributeSet)
		FGameplayEffectAttributeCaptureDefinition CritChanceDef;
		FGameplayEffectAttributeCaptureDefinition CritDamageDef;
		FGameplayEffectAttributeCaptureDefinition ArmorPenetrationDef;
		FGameplayEffectAttributeCaptureDefinition ElementPenetrationDef;

		// Target: Defenses (BasicAttributeSet)
		FGameplayEffectAttributeCaptureDefinition PhysicalResistDef;
		FGameplayEffectAttributeCaptureDefinition MagicResistDef;
		FGameplayEffectAttributeCaptureDefinition ElementalResistDef;
		FGameplayEffectAttributeCaptureDefinition FireResistDef;
		FGameplayEffectAttributeCaptureDefinition FrostResistDef;
		FGameplayEffectAttributeCaptureDefinition LightningResistDef;
		FGameplayEffectAttributeCaptureDefinition ArmorDef;
	};

	static const FCaptureDefs& Captures();
};
