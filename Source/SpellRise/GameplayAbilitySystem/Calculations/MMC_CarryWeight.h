#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_CarryWeight.generated.h"

UCLASS()
class SPELLRISE_API UMMC_CarryWeight : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_CarryWeight();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
