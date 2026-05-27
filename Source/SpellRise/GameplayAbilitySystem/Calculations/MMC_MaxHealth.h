#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"

UCLASS()
class SPELLRISE_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxHealth();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition WisdomDef;
};
