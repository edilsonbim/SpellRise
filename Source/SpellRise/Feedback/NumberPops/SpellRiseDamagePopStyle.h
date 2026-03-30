#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SpellRiseDamagePopStyle.generated.h"

class UStaticMesh;

UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseDamagePopStyle : public UDataAsset
{
	GENERATED_BODY()

public:
	USpellRiseDamagePopStyle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	FString DisplayText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	FGameplayTagQuery MatchPattern;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop", meta=(EditCondition="bOverrideColor"))
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop", meta=(EditCondition="bOverrideColor"))
	FLinearColor CriticalColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop", meta=(EditCondition="bOverrideMesh"))
	TObjectPtr<UStaticMesh> TextMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	bool bOverrideColor = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DamagePop")
	bool bOverrideMesh = false;
};