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

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComp = nullptr;
};
