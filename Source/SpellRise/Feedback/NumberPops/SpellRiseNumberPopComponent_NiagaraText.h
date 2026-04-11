#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"
#include "SpellRiseNumberPopComponent_NiagaraText.generated.h"

class USpellRiseDamagePopStyleNiagara;
class UNiagaraComponent;

UCLASS(Blueprintable, ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseNumberPopComponent_NiagaraText : public USpellRiseNumberPopComponent
{
	GENERATED_BODY()

public:
	USpellRiseNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void AddNumberPop(const FSpellRiseNumberPopRequest& NewRequest) override;


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Style")
	TObjectPtr<USpellRiseDamagePopStyleNiagara> Style = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability")
	bool bCompensateScaleByDistance = true;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="1.0"))
	float ReferenceDistance = 1200.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability")
	bool bCompensateScaleByFOV = true;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="1.0", ClampMax="170.0"))
	float ReferenceFOV = 90.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability")
	bool bExactScreenSizeCompensation = true;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="0.1"))
	float MinWorldScale = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="0.1"))
	float MaxWorldScale = 3.5f;


	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComp = nullptr;
};
