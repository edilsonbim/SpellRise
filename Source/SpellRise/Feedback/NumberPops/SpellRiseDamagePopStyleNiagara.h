#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpellRiseDamagePopStyleNiagara.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseDamagePopStyleNiagara : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	FName NiagaraArrayName = NAME_None;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> TextNiagara = nullptr;
};
