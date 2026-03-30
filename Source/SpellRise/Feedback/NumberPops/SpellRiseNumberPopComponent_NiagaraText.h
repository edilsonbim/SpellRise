#pragma once

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

	//~USpellRiseNumberPopComponent interface
	virtual void AddNumberPop(const FSpellRiseNumberPopRequest& NewRequest) override;
	//~End of USpellRiseNumberPopComponent interface

protected:
	/** Style asset com o Niagara System e o nome do array Vector4 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Style")
	TObjectPtr<USpellRiseDamagePopStyleNiagara> Style = nullptr;

	/** Compensa escala no mundo para manter tamanho aparente mais estável em tela conforme distância. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability")
	bool bCompensateScaleByDistance = true;

	/** Distância base onde escala=1.0 para o pop. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="1.0"))
	float ReferenceDistance = 1200.0f;

	/** Compensa variações de FOV para manter o tamanho aparente consistente em zoom in/out. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability")
	bool bCompensateScaleByFOV = true;

	/** FOV base onde a compensação de FOV aplica escala=1.0. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="1.0", ClampMax="170.0"))
	float ReferenceFOV = 90.0f;

	/** Se true, aplica compensação exata (escala proporcional à distância) para manter tamanho aparente constante. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability")
	bool bExactScreenSizeCompensation = true;

	/** Limites de escala aplicados após compensação por distância. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="0.1"))
	float MinWorldScale = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Number Pop|Readability", meta=(ClampMin="0.1"))
	float MaxWorldScale = 3.5f;

	/** Niagara component persistente usado para alimentar a lista de hits */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComp = nullptr;
};
