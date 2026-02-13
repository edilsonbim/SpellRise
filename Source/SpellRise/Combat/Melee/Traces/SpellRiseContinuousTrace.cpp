#include "SpellRise/Combat/Melee/Traces/SpellRiseContinuousTrace.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"

USpellRiseContinuousTrace::USpellRiseContinuousTrace()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    bAutoActivate = false;
    
    bIsTracing = false;
    bDebugVisual = false;
    bHasValidPrevious = false;
}

void USpellRiseContinuousTrace::SetDebugVisual(bool bEnable)
{
    bDebugVisual = bEnable;
    
    if (bDebugVisual)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[ContinuousTrace] 🔵 Debug visual ON"));
    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("[ContinuousTrace] ⚫ Debug visual OFF"));
    }
}

void USpellRiseContinuousTrace::StartTrace(const FMeleeTraceConfig& Config, USceneComponent* WeaponMesh)
{
    CurrentConfig = Config;
    WeaponMeshComponent = WeaponMesh;
    bIsTracing = true;
    bHasValidPrevious = false;
    
    SetComponentTickEnabled(true);
}

void USpellRiseContinuousTrace::EndTrace()
{
    bIsTracing = false;
    bHasValidPrevious = false;
    bDebugVisual = false;  // ← DESLIGA DEBUG VISUAL!
    
    LastBaseLocation = FVector::ZeroVector;
    LastTipLocation = FVector::ZeroVector;
    
    SetComponentTickEnabled(false);
}

void USpellRiseContinuousTrace::ResetHitHistory()
{
    LastHitTimeByActor.Empty();
}

bool USpellRiseContinuousTrace::GetSocketLocations(FVector& OutBase, FVector& OutTip) const
{
    if (!WeaponMeshComponent.IsValid())
        return false;

    UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(WeaponMeshComponent.Get());
    if (!Primitive)
        return false;

    if (!Primitive->DoesSocketExist(CurrentConfig.SocketBase) || 
        !Primitive->DoesSocketExist(CurrentConfig.SocketTip))
        return false;

    OutBase = Primitive->GetSocketLocation(CurrentConfig.SocketBase);
    OutTip = Primitive->GetSocketLocation(CurrentConfig.SocketTip);
    return true;
}

int32 USpellRiseContinuousTrace::CalculateTraceSteps(const FVector& FromBase, const FVector& ToBase,
                                                     const FVector& FromTip, const FVector& ToTip) const
{
    const float BaseDistance = FVector::Dist(FromBase, ToBase);
    const float TipDistance = FVector::Dist(FromTip, ToTip);
    const float MaxDistance = FMath::Max(BaseDistance, TipDistance);
    
    if (MaxDistance < CurrentConfig.MinStepDistance)
        return 1;
    
    int32 Steps = FMath::CeilToInt(MaxDistance / CurrentConfig.MinStepDistance);
    
    if (CurrentConfig.MaxTraceSteps > 0)
        Steps = FMath::Min(Steps, CurrentConfig.MaxTraceSteps);
    
    return FMath::Max(1, Steps);
}

void USpellRiseContinuousTrace::PerformSweepSegment(const FVector& StartBase, const FVector& EndBase,
                                                    const FVector& StartTip, const FVector& EndTip,
                                                    int32 StepIndex, int32 TotalSteps)
{
    UWorld* World = GetWorld();
    if (!World) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    const float Alpha = (StepIndex + 1.0f) / TotalSteps;
    const FVector CurrentBase = FMath::Lerp(StartBase, EndBase, Alpha);
    const FVector CurrentTip = FMath::Lerp(StartTip, EndTip, Alpha);

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(Owner);
    
    if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(WeaponMeshComponent.Get()))
    {
        QueryParams.AddIgnoredComponent(PrimComp);
    }

    FCollisionShape Shape = FCollisionShape::MakeSphere(CurrentConfig.TraceRadius);

    TArray<FHitResult> Hits;
    
    const bool bHit = World->SweepMultiByChannel(
        Hits,
        CurrentBase,
        CurrentTip,
        FQuat::Identity,
        CurrentConfig.TraceChannel,
        Shape,
        QueryParams
    );

    // ============================================
    // 🔴🔴🔴 DEBUG VISUAL - SÓ APARECE QUANDO ATIVADO!
    // ============================================
    if (bDebugVisual)
    {
        DrawDebugLine(World, CurrentBase, CurrentTip, FColor::Cyan, false, 0.1f, 0, 1.0f);
        DrawDebugSphere(World, CurrentTip, CurrentConfig.TraceRadius, 8, FColor::Cyan, false, 0.1f);
        
        if (bHit)
        {
            for (const FHitResult& Hit : Hits)
            {
                DrawDebugPoint(World, Hit.ImpactPoint, 10.0f, FColor::Red, false, 0.1f);
            }
        }
    }

    if (!bHit) return;

    for (const FHitResult& Hit : Hits)
    {
        ProcessHit(Hit);
    }
}

bool USpellRiseContinuousTrace::CanHitActor(AActor* Actor) const
{
    if (!Actor) return false;
    
    if (const float* LastHitTime = LastHitTimeByActor.Find(Actor))
    {
        const float TimeSinceHit = GetWorld()->GetTimeSeconds() - *LastHitTime;
        if (TimeSinceHit < 0.5f)
            return false;
    }
    
    return true;
}

void USpellRiseContinuousTrace::ProcessHit(const FHitResult& Hit)
{
    AActor* HitActor = Hit.GetActor();
    if (!CanHitActor(HitActor)) return;

    LastHitTimeByActor.Add(HitActor, GetWorld()->GetTimeSeconds());
    OnHitDetected.Broadcast(Hit);
}

void USpellRiseContinuousTrace::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsTracing || !WeaponMeshComponent.IsValid())
        return;

    FVector CurrentBase, CurrentTip;
    if (!GetSocketLocations(CurrentBase, CurrentTip))
        return;

    if (!bHasValidPrevious)
    {
        LastBaseLocation = CurrentBase;
        LastTipLocation = CurrentTip;
        bHasValidPrevious = true;
        return;
    }

    int32 Steps = CalculateTraceSteps(LastBaseLocation, CurrentBase, LastTipLocation, CurrentTip);
    
    for (int32 i = 0; i < Steps; i++)
    {
        PerformSweepSegment(LastBaseLocation, CurrentBase, LastTipLocation, CurrentTip, i, Steps);
    }

    LastBaseLocation = CurrentBase;
    LastTipLocation = CurrentTip;
}