#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRiseHitHistoryComponent.generated.h"

USTRUCT()
struct FHistoricalTransform
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Location = FVector::ZeroVector;

    UPROPERTY()
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY()
    float Timestamp = 0.0f;

    FHistoricalTransform() {}
    FHistoricalTransform(const FVector& InLoc, const FRotator& InRot, float InTime)
        : Location(InLoc), Rotation(InRot), Timestamp(InTime) {}
};

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseHitHistoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USpellRiseHitHistoryComponent();

    UFUNCTION(BlueprintCallable, Category = "Melee|LagCompensation")
    void RecordCurrentTransform();

    UFUNCTION(BlueprintCallable, Category = "Melee|LagCompensation")
    bool GetTransformAtTime(float Timestamp, FVector& OutLocation, FRotator& OutRotation) const;

    UFUNCTION(BlueprintCallable, Category = "Melee|LagCompensation")
    void ClearHistory();

    UPROPERTY(EditAnywhere, Category = "Melee|LagCompensation", meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float HistoryWindowSeconds = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Melee|LagCompensation", meta = (ClampMin = "0.01", ClampMax = "0.1"))
    float RecordInterval = 0.033f;

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TArray<FHistoricalTransform> TransformHistory;

    float TimeSinceLastRecord = 0.0f;
    int32 CurrentIndex = 0;

    int32 GetMaxHistorySize() const;
};