#pragma once

// Cabeçalho de interface: declara o cálculo autoritativo de cura usado por GameplayEffects.

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Healing.generated.h"

UCLASS()
class SPELLRISE_API UExecCalc_Healing : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Healing();

	struct FCaptureDefs
	{
		FCaptureDefs();

		FGameplayEffectAttributeCaptureDefinition WisdomDef;
	};

	static const FCaptureDefs& Captures();

protected:
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;
};
