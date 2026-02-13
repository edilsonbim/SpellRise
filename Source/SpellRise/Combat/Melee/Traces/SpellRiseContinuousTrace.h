#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/Engine.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"
#include "SpellRiseContinuousTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitDetected, const FHitResult&, Hit);

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseContinuousTrace : public UActorComponent
{
    GENERATED_BODY()

public:
    USpellRiseContinuousTrace();

    UFUNCTION(BlueprintCallable, Category = "Melee|Trace")
    void StartTrace(const FMeleeTraceConfig& Config, USceneComponent* WeaponMesh);

    UFUNCTION(BlueprintCallable, Category = "Melee|Trace")
    void EndTrace();

    UFUNCTION(BlueprintCallable, Category = "Melee|Trace")
    void ResetHitHistory();

    UPROPERTY(BlueprintAssignable, Category = "Melee|Trace")
    FOnHitDetected OnHitDetected;

    // ============================================
    // DEBUG VISUAL - CONTROLADO PELO HIT WINDOW!
    // ============================================
    /** Ativar/desativar debug visual (linhas azuis) */
    void SetDebugVisual(bool bEnable);

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FMeleeTraceConfig CurrentConfig;
    TWeakObjectPtr<USceneComponent> WeaponMeshComponent;

    bool bIsTracing = false;
    bool bDebugVisual = false;  // ← NOVO: controle de debug visual
    FVector LastBaseLocation = FVector::ZeroVector;
    FVector LastTipLocation = FVector::ZeroVector;
    bool bHasValidPrevious = false;

    UPROPERTY()
    TMap<TWeakObjectPtr<AActor>, float> LastHitTimeByActor;

    bool GetSocketLocations(FVector& OutBase, FVector& OutTip) const;
    int32 CalculateTraceSteps(const FVector& FromBase, const FVector& ToBase, 
                              const FVector& FromTip, const FVector& ToTip) const;
    void PerformSweepSegment(const FVector& StartBase, const FVector& EndBase,
                             const FVector& StartTip, const FVector& EndTip,
                             int32 StepIndex, int32 TotalSteps);
    bool CanHitActor(AActor* Actor) const;
    void ProcessHit(const FHitResult& Hit);
};