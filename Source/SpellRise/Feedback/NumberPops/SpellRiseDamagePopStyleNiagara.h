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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop", meta=(ClampMin="0.01"))
	float WorldScale = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop|Color")
	FName NumberColorParameterName = TEXT("User.NumberColor");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop|Color")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop|Color")
	FLinearColor CriticalColor = FLinearColor(1.f, 0.08f, 0.02f, 1.f);
};
