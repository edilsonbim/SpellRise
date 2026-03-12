#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpellRiseDamagePopStyleNiagara.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseDamagePopStyleNiagara : public UDataAsset
{
	GENERATED_BODY()

public:
	// Nome do array Vector4 dentro do Niagara que receberá os hits
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	FName NiagaraArrayName = NAME_None;

	// Niagara System usado para renderizar os números
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> TextNiagara = nullptr;
};