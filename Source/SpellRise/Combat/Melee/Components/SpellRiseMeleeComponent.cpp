#include "SpellRise/Combat/Melee/Components/SpellRiseMeleeComponent.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"
#include "SpellRise/Combat/Melee/Traces/SpellRiseContinuousTrace.h"
#include "SpellRise/Combat/Melee/Components/SpellRiseHitHistoryComponent.h"
#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// ============================================================================
// CONSTRUTOR
// ============================================================================
USpellRiseMeleeComponent::USpellRiseMeleeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    bIsActive = false;
    bDidHitThisWindow = false;
    bDebugVisual = false;
    CurrentComboIndex = 0;
    DamageEffectClass = nullptr;
    WeaponData = nullptr;
    TraceComponent = nullptr;
    HistoryComponent = nullptr;
    CachedWeaponMesh = nullptr;
    
    // Fallback: tenta carregar GE_MeleeDamage
    static ConstructorHelpers::FClassFinder<UGameplayEffect> DamageEffectFinder(
        TEXT("/Game/GameplayAbilities/Effects/GE_MeleeDamage.GE_MeleeDamage_C")
    );
    
    if (DamageEffectFinder.Succeeded())
    {
        DamageEffectClass = DamageEffectFinder.Class;
        UE_LOG(LogTemp, Verbose, TEXT("[Melee] Default DamageEffectClass loaded: GE_MeleeDamage"));
    }
}

// ============================================================================
// BEGIN PLAY
// ============================================================================
void USpellRiseMeleeComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cria TraceComponent se não existir
    if (!TraceComponent)
    {
        TraceComponent = NewObject<USpellRiseContinuousTrace>(this, TEXT("MeleeTraceComponent"));
        TraceComponent->RegisterComponent();
        UE_LOG(LogTemp, Verbose, TEXT("[Melee] TraceComponent created"));
    }

    // Cria HistoryComponent apenas no servidor
    if (!HistoryComponent && GetOwner() && GetOwner()->HasAuthority())
    {
        HistoryComponent = NewObject<USpellRiseHitHistoryComponent>(this, TEXT("MeleeHistoryComponent"));
        HistoryComponent->RegisterComponent();
        UE_LOG(LogTemp, Verbose, TEXT("[Melee] HistoryComponent created (server only)"));
    }
}

// ============================================================================
// SET DEBUG VISUAL
// ============================================================================
void USpellRiseMeleeComponent::SetDebugVisual(bool bEnable)
{
    bDebugVisual = bEnable;
    
    if (TraceComponent)
    {
        TraceComponent->SetDebugVisual(bEnable);
    }
    
    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Melee] Debug Visual: %s"), bEnable ? TEXT("ON") : TEXT("OFF"));
    }
}

// ============================================================================
// INITIALIZE WITH WEAPON DATA - CORRIGIDO!
// ============================================================================
void USpellRiseMeleeComponent::InitializeWithWeaponData(const UDA_MeleeWeaponData* InWeaponData, USceneComponent* InWeaponMesh)
{
    WeaponData = InWeaponData;
    CachedWeaponMesh = InWeaponMesh;
    
    if (!WeaponData)
    {
        UE_LOG(LogTemp, Error, TEXT("[Melee] ❌ InitializeWithWeaponData failed - WeaponData is NULL!"));
        return;
    }

    if (!CachedWeaponMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("[Melee] ❌ InitializeWithWeaponData failed - InWeaponMesh is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Melee] 🔵 Initializing with WeaponData: %s"), 
        *GetNameSafe(WeaponData));

    if (TraceComponent)
    {
        // 🔴 REMOVER DELEGATE ANTIGO ANTES DE ADICIONAR NOVO!
        TraceComponent->OnHitDetected.RemoveDynamic(this, &USpellRiseMeleeComponent::OnTraceHitDetected);
        
        TraceComponent->StartTrace(WeaponData->TraceConfig, CachedWeaponMesh);
        TraceComponent->SetDebugVisual(bDebugVisual);
        TraceComponent->OnHitDetected.AddDynamic(this, &USpellRiseMeleeComponent::OnTraceHitDetected);
        
        UE_LOG(LogTemp, Warning, TEXT("[Melee] ✅ ContinuousTrace STARTED! SocketBase: %s, SocketTip: %s"), 
            *WeaponData->TraceConfig.SocketBase.ToString(),
            *WeaponData->TraceConfig.SocketTip.ToString());
    }
}

// ============================================================================
// HIT WINDOW CONTROL
// ============================================================================
void USpellRiseMeleeComponent::BeginHitWindow(int32 ComboIndex)
{
    CurrentComboIndex = ComboIndex;
    bIsActive = true;
    bDidHitThisWindow = false;

    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Melee] 🟢 Hit Window BEGIN - ComboIndex: %d"), ComboIndex);
    }

    if (TraceComponent && WeaponData && CachedWeaponMesh)
    {
        TraceComponent->StartTrace(WeaponData->TraceConfig, CachedWeaponMesh);
        TraceComponent->SetDebugVisual(bDebugVisual);
        
        if (WeaponData->DamageConfig.bResetCooldownPerCombo)
        {
            TraceComponent->ResetHitHistory();
            if (bDebug) UE_LOG(LogTemp, Verbose, TEXT("[Melee] Hit history reset"));
        }
    }

    PlayWhooshFeedback();
}

void USpellRiseMeleeComponent::EndHitWindow()
{
    bIsActive = false;
    
    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Melee] 🔴 Hit Window END - ComboIndex: %d"), CurrentComboIndex);
    }

    if (TraceComponent)
    {
        TraceComponent->SetDebugVisual(false);
        TraceComponent->EndTrace();
    }
}

// ============================================================================
// HIT DETECTION
// ============================================================================
void USpellRiseMeleeComponent::OnTraceHitDetected(const FHitResult& Hit)
{
    if (bDidHitThisWindow) return;
    
    AActor* HitActor = Hit.GetActor();
    if (!bIsActive || !WeaponData || !HitActor) return;

    bDidHitThisWindow = true;
    
    if (bDebug)
    {
        float CalculatedDamage = CalculateFinalDamage(Hit);
        UE_LOG(LogTemp, Warning, TEXT("[Melee] 💥 HIT! Target: %s, Damage: %.1f, ComboIndex: %d"), 
            *GetNameSafe(HitActor), CalculatedDamage, CurrentComboIndex);
    }
    
    ApplyDamageToTarget(Hit);
    OnWeaponHit.Broadcast(Hit);
}

// ============================================================================
// VALIDAÇÃO DO HIT
// ============================================================================
bool USpellRiseMeleeComponent::ValidateHit(const FHitResult& Hit, AActor*& OutTargetActor, UAbilitySystemComponent*& OutTargetASC) const
{
    OutTargetActor = Hit.GetActor();
    if (!OutTargetActor) return false;
    if (OutTargetActor == GetOwner()) return false;

    if (WeaponData)
    {
        const float Distance = GetOwner()->GetDistanceTo(OutTargetActor);
        const float MaxDistance = WeaponData->TraceConfig.MinStepDistance * 10.0f;
        if (Distance > MaxDistance) return false;
    }

    OutTargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OutTargetActor);
    return OutTargetASC != nullptr;
}

// ============================================================================
// CÁLCULO DE DANO
// ============================================================================
float USpellRiseMeleeComponent::CalculateFinalDamage(const FHitResult& Hit) const
{
    if (!WeaponData) return 0.0f;

    float Damage = WeaponData->GetFinalBaseDamage();
    
    if (WeaponData->DamageConfig.ComboScaling.IsValidIndex(CurrentComboIndex))
    {
        Damage *= WeaponData->DamageConfig.ComboScaling[CurrentComboIndex];
    }

    return Damage;
}

// ============================================================================
// APLICAÇÃO DE DANO
// ============================================================================
void USpellRiseMeleeComponent::ApplyDamageToTarget(const FHitResult& Hit)
{
    AActor* TargetActor = nullptr;
    UAbilitySystemComponent* TargetASC = nullptr;

    if (!ValidateHit(Hit, TargetActor, TargetASC)) return;

    const float Damage = CalculateFinalDamage(Hit);
    ApplyGameplayEffectDamage(TargetActor, TargetASC, Damage);
    PlayHitFeedback(Hit);
}

void USpellRiseMeleeComponent::ApplyGameplayEffectDamage(AActor* TargetActor, UAbilitySystemComponent* TargetASC, float Damage)
{
    UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
    
    if (!SourceASC || !TargetASC || !DamageEffectClass) return;
    if (!SourceASC->IsOwnerActorAuthoritative()) return;

    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.AddSourceObject(GetOwner());

    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1, EffectContext);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid()) return;

    FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.BaseWeaponDamage"), false);
    if (DamageTag.IsValid())
    {
        SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, Damage);
        
        if (bDebug)
        {
            UE_LOG(LogTemp, Verbose, TEXT("[Melee] SetByCaller: %s = %.1f"), 
                *DamageTag.ToString(), Damage);
        }
    }

    SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
    
    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Melee] ✅ Damage Applied: %.1f to %s"), 
            Damage, *GetNameSafe(TargetActor));
    }
}

// ============================================================================
// FEEDBACK
// ============================================================================
void USpellRiseMeleeComponent::PlayHitFeedback(const FHitResult& Hit)
{
    if (!WeaponData) return;

    if (WeaponData->FeedbackConfig.HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, WeaponData->FeedbackConfig.HitSound, Hit.ImpactPoint);
    }

    if (WeaponData->FeedbackConfig.BloodEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            this,
            WeaponData->FeedbackConfig.BloodEffect,
            Hit.ImpactPoint,
            Hit.ImpactNormal.Rotation(),
            true
        );
    }
}

void USpellRiseMeleeComponent::PlayWhooshFeedback()
{
    if (!WeaponData || !WeaponData->FeedbackConfig.WhooshSound) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UGameplayStatics::PlaySoundAtLocation(this, WeaponData->FeedbackConfig.WhooshSound, Owner->GetActorLocation());
}