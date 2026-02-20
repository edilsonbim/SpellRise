#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

UCLASS()
class SPELLRISE_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	// ✅ PRECISA SER PUBLIC porque o .cpp usa UExecCalc_Damage::FCaptureDefs
	struct FCaptureDefs
	{
		FCaptureDefs();

		// --- Source (Derived) ---
		FGameplayEffectAttributeCaptureDefinition MeleeDamageMultiplierDef;
		FGameplayEffectAttributeCaptureDefinition BowDamageMultiplierDef;
		FGameplayEffectAttributeCaptureDefinition SpellDamageMultiplierDef;

		FGameplayEffectAttributeCaptureDefinition CritChanceDef;
		FGameplayEffectAttributeCaptureDefinition CritDamageDef;

		FGameplayEffectAttributeCaptureDefinition ArmorPenetrationDef;

		// --- Target (Resistances) ---
		FGameplayEffectAttributeCaptureDefinition SlashingResDef;
		FGameplayEffectAttributeCaptureDefinition BashingResDef;
		FGameplayEffectAttributeCaptureDefinition PiercingResDef;
		FGameplayEffectAttributeCaptureDefinition ImpactResDef;

		FGameplayEffectAttributeCaptureDefinition FireResDef;
		FGameplayEffectAttributeCaptureDefinition ColdResDef;
		FGameplayEffectAttributeCaptureDefinition AcidResDef;
		FGameplayEffectAttributeCaptureDefinition ShockResDef;

		FGameplayEffectAttributeCaptureDefinition DivineResDef;
		FGameplayEffectAttributeCaptureDefinition CurseResDef;
		FGameplayEffectAttributeCaptureDefinition AlmightyResDef;

		FGameplayEffectAttributeCaptureDefinition GenericResDef;
		FGameplayEffectAttributeCaptureDefinition BleedResDef;
		FGameplayEffectAttributeCaptureDefinition PoisonResDef;
	};

	static const FCaptureDefs& Captures();

protected:
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;
};
