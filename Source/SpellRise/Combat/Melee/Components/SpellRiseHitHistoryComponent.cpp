#include "SpellRise/Combat/Melee/Components/SpellRiseHitHistoryComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

// ============================================================================
// Construtor
// ============================================================================
USpellRiseHitHistoryComponent::USpellRiseHitHistoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    
    // Valores padrão
    HistoryWindowSeconds = 0.3f;
    RecordInterval = 0.033f;
    TimeSinceLastRecord = 0.0f;
    CurrentIndex = 0;
}

// ============================================================================
// BeginPlay
// ============================================================================
void USpellRiseHitHistoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Pré-aloca o buffer
    const int32 MaxSize = GetMaxHistorySize();
    TransformHistory.SetNum(MaxSize);
}

// ============================================================================
// Tick
// ============================================================================
void USpellRiseHitHistoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Apenas o servidor registra histórico
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
        return;

    TimeSinceLastRecord += DeltaTime;
    
    if (TimeSinceLastRecord >= RecordInterval)
    {
        RecordCurrentTransform();
        TimeSinceLastRecord = 0.0f;
    }
}

// ============================================================================
// Utilitários
// ============================================================================
int32 USpellRiseHitHistoryComponent::GetMaxHistorySize() const
{
    // Garante pelo menos 1 frame
    return FMath::Max(1, FMath::CeilToInt(HistoryWindowSeconds / RecordInterval) + 1);
}

// ============================================================================
// API Pública
// ============================================================================
void USpellRiseHitHistoryComponent::RecordCurrentTransform()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    const int32 MaxSize = GetMaxHistorySize();
    
    // Garante que o buffer tem o tamanho correto
    if (TransformHistory.Num() != MaxSize)
    {
        TransformHistory.SetNum(MaxSize);
    }

    // Adiciona registro atual
    TransformHistory[CurrentIndex] = FHistoricalTransform(
        Owner->GetActorLocation(),
        Owner->GetActorRotation(),
        GetWorld()->GetTimeSeconds()
    );

    // Avança índice circular
    CurrentIndex = (CurrentIndex + 1) % MaxSize;
}

bool USpellRiseHitHistoryComponent::GetTransformAtTime(float Timestamp, FVector& OutLocation, FRotator& OutRotation) const
{
    if (TransformHistory.Num() == 0)
        return false;

    int32 BestIndex = -1;
    float BestDelta = TNumericLimits<float>::Max();

    // Busca linear (buffer pequeno, performance ok)
    for (int32 i = 0; i < TransformHistory.Num(); i++)
    {
        const FHistoricalTransform& Entry = TransformHistory[i];
        if (Entry.Timestamp == 0.0f)
            continue;

        const float Delta = FMath::Abs(Entry.Timestamp - Timestamp);
        if (Delta < BestDelta)
        {
            BestDelta = Delta;
            BestIndex = i;
        }
    }

    if (BestIndex == -1)
        return false;

    OutLocation = TransformHistory[BestIndex].Location;
    OutRotation = TransformHistory[BestIndex].Rotation;
    return true;
}

void USpellRiseHitHistoryComponent::ClearHistory()
{
    const int32 MaxSize = GetMaxHistorySize();
    TransformHistory.Empty();
    TransformHistory.SetNum(MaxSize);
    CurrentIndex = 0;
    TimeSinceLastRecord = 0.0f;
}