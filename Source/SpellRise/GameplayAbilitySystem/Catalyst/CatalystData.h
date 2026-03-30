#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CatalystData.generated.h"

UENUM(BlueprintType)
enum class ECatalystType : uint8
{
	Physical,
	Archer,
	Mage,
	Support
};

UCLASS(BlueprintType)
class SPELLRISE_API UCatalystData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName CatalystId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECatalystType Type = ECatalystType::Physical;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> GE_Active_L1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> GE_Active_L2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> GE_Active_L3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> GE_PassiveBonus_L1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> GE_PassiveBonus_L2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> GE_PassiveBonus_L3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CatalystTypeTag;
};
