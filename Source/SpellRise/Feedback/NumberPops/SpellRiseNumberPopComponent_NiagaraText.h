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

	/** Niagara component persistente usado para alimentar a lista de hits */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraComp = nullptr;
};