#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpellRiseProgressionBalanceData.generated.h"

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseLevelProgressionRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="1", UIMin="1"))
	int32 Level = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="0", UIMin="0"))
	int32 RequiredTotalExperience = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="0", UIMin="0"))
	int32 TalentPointsOnLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="0", UIMin="0"))
	int32 CraftPointsOnLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="0", UIMin="0"))
	int32 AttributePointsOnLevel = 0;
};

UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseProgressionBalanceData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression")
	TArray<FSpellRiseLevelProgressionRow> Levels;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	bool GetLevelRow(int32 Level, FSpellRiseLevelProgressionRow& OutRow) const;
};
