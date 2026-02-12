#include "GA_Weapon_MeleeLight_ClientPrediction.h"

// Engine
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// GAS
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

// SpellRise - INCLUDES ADICIONADOS!
#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Weapons/Data/DA_WeaponDefinition.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"

// Framework
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// ============================================================================
// CONSTRUCTOR
// ============================================================================
UGA_Weapon_MeleeLight_ClientPrediction::UGA_Weapon_MeleeLight_ClientPrediction()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    bReplicateInputDirectly = true;

    MaxComboHits = 3;
    ComboSections = { FName("Hit1"), FName("Hit2"), FName("Hit3") };
    StopBlendOutTime = 0.15f;

    ComboWindowBeginTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), false);
    ComboWindowEndTag   = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"), false);
}

// ============================================================================
// HELPERS
// ============================================================================
UWorld* UGA_Weapon_MeleeLight_ClientPrediction::GetGameWorld() const
{
    if (CurrentActorInfo)
    {
        if (AActor* Avatar = CurrentActorInfo->AvatarActor.Get())
            return Avatar->GetWorld();
        if (AActor* Owner = CurrentActorInfo->OwnerActor.Get())
            return Owner->GetWorld();
    }
    if (UAbilitySystemComponent* ASC = GetASC())
        if (AActor* Owner = ASC->GetOwner())
            return Owner->GetWorld();
    return GetWorld();
}

bool UGA_Weapon_MeleeLight_ClientPrediction::IsServerAuthority() const
{
    return CurrentActorInfo && CurrentActorInfo->IsNetAuthority();
}

AActor* UGA_Weapon_MeleeLight_ClientPrediction::GetAvatarActor() const
{
    return CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr;
}

UAbilitySystemComponent* UGA_Weapon_MeleeLight_ClientPrediction::GetASC() const
{
    return CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
}

UAnimInstance* UGA_Weapon_MeleeLight_ClientPrediction::GetAvatarAnimInstance() const
{
    AActor* Avatar = GetAvatarActor();
    if (!Avatar) return nullptr;

    if (ACharacter* Character = Cast<ACharacter>(Avatar))
    {
        if (USkeletalMeshComponent* Mesh = Character->GetMesh())
            return Mesh->GetAnimInstance();
    }

    if (USkeletalMeshComponent* Mesh = Avatar->FindComponentByClass<USkeletalMeshComponent>())
        return Mesh->GetAnimInstance();

    return nullptr;
}

// ============================================================================
// CACHE
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::CacheWeaponData()
{
    CachedWeaponData = nullptr;
    CachedMontage = nullptr;

    AActor* Avatar = GetAvatarActor();
    if (!Avatar) return;

    if (USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>())
    {
        CachedWeaponData = WeaponComp->GetEquippedWeapon();
    }

    if (CachedWeaponData && CachedWeaponData->LightAttack.AttackMontage)
    {
        CachedMontage = CachedWeaponData->LightAttack.AttackMontage;
    }
    else
    {
        CachedMontage = AttackMontage;
    }
}

float UGA_Weapon_MeleeLight_ClientPrediction::GetDamageForCurrentHit() const
{
    if (CachedWeaponData)
    {
        return FMath::Max(0.0f, CachedWeaponData->GetLightAttackFinalBaseDamage());
    }
    return FMath::Max(0.0f, BaseDamage);
}

float UGA_Weapon_MeleeLight_ClientPrediction::GetComboScalingForCurrentHit() const
{
    if (!CachedWeaponData) return 1.0f;
    
    const TArray<float>& ScalingArray = CachedWeaponData->LightAttack.DamageScalingPerHit;
    if (ScalingArray.IsValidIndex(CurrentComboIndex))
    {
        return FMath::Max(0.0f, ScalingArray[CurrentComboIndex]);
    }
    return 1.0f;
}

float UGA_Weapon_MeleeLight_ClientPrediction::GetAttackSpeedMultiplier() const
{
    UAbilitySystemComponent* ASC = GetASC();
    if (!ASC) return 1.0f;

    const float AttackSpeed = ASC->GetNumericAttribute(
        UDerivedStatsAttributeSet::GetAttackSpeedMultiplierAttribute()
    );
    return FMath::Clamp(AttackSpeed, 0.25f, 3.0f);
}

// ============================================================================
// CLIENT PREDICTION
// ============================================================================
FCombatHitResult UGA_Weapon_MeleeLight_ClientPrediction::PerformClientTraceAndPredictDamage()
{
    FCombatHitResult Result;
    Result.HitIndex = CurrentComboIndex;
    
    AActor* Avatar = GetAvatarActor();
    if (!Avatar || !CachedWeaponData) return Result;

    USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
    if (!WeaponComp) return Result;

    FHitResult Hit;
    const bool bHit = WeaponComp->PerformLightAttackTrace(
        Avatar,
        TraceChannel.GetValue(),
        bDebug,
        Hit
    );

    if (!bHit || !Hit.GetActor()) return Result;

    Result.TargetActor = Hit.GetActor();
    Result.HitLocation = Hit.ImpactPoint;
    
    const float BaseDamageValue = GetDamageForCurrentHit();
    const float ComboScaling = GetComboScalingForCurrentHit();
    Result.DamageAmount = BaseDamageValue * ComboScaling;
    
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Result.TargetActor);
    if (TargetASC && GetASC())
    {
        const float CritChance = GetASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritChanceAttribute());
        Result.bWasCrit = FMath::FRand() <= FMath::Clamp(CritChance, 0.0f, 1.0f);
        if (Result.bWasCrit)
        {
            const float CritDamage = GetASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritDamageAttribute());
            Result.DamageAmount *= CritDamage;
        }
    }

    return Result;
}

void UGA_Weapon_MeleeLight_ClientPrediction::ApplyClientVisualDamage(const FCombatHitResult& HitResult)
{
    if (!HitResult.TargetActor) return;

    if (ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(GetAvatarActor()))
    {
        FGameplayTag DamageTypeTag = CachedWeaponData ? CachedWeaponData->LightAttack.DamageTypeTag : FGameplayTag();
        Character->MultiShowDamagePop(
            HitResult.DamageAmount,
            GetAvatarActor(),
            DamageTypeTag,
            HitResult.bWasCrit
        );
    }

    if (UWorld* World = GetGameWorld())
    {
        UParticleSystem* BloodEffect = LoadObject<UParticleSystem>(nullptr, TEXT("/Engine/EngineMaterials/DefaultParticle.DefaultParticle"));
        if (BloodEffect)
        {
            UGameplayStatics::SpawnEmitterAtLocation(World, BloodEffect, HitResult.HitLocation);
        }
    }

    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Visual Damage: %.1f to %s (Crit: %d)"),
            HitResult.DamageAmount,
            *GetNameSafe(HitResult.TargetActor),
            HitResult.bWasCrit ? 1 : 0);
    }
}

// ============================================================================
// RPCs
// ============================================================================
bool UGA_Weapon_MeleeLight_ClientPrediction::Server_ReportHitResult_Validate(const FCombatHitResult& HitResult)
{
    if (!HitResult.TargetActor) return false;
    if (HitResult.DamageAmount <= 0.0f || HitResult.DamageAmount > 1000.0f) return false;
    if (HitResult.HitIndex < 0 || HitResult.HitIndex >= MaxComboHits) return false;
    
    AActor* Avatar = GetAvatarActor();
    if (Avatar && HitResult.TargetActor)
    {
        const float Distance = Avatar->GetDistanceTo(HitResult.TargetActor);
        if (Distance > 500.0f) return false;
    }
    
    return true;
}

void UGA_Weapon_MeleeLight_ClientPrediction::Server_ReportHitResult_Implementation(const FCombatHitResult& HitResult)
{
    if (!IsServerAuthority()) return;
    if (!ValidateHitResult(HitResult)) return;
    
    ApplyServerDamage(HitResult);
    Client_ConfirmDamage(HitResult);
}

void UGA_Weapon_MeleeLight_ClientPrediction::Client_ConfirmDamage_Implementation(const FCombatHitResult& HitResult)
{
    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Damage CONFIRMED by server: %.1f to %s"),
            HitResult.DamageAmount,
            *GetNameSafe(HitResult.TargetActor));
    }
}

bool UGA_Weapon_MeleeLight_ClientPrediction::ValidateHitResult(const FCombatHitResult& HitResult) const
{
    if (!HitResult.TargetActor) return false;
    
    AActor* Avatar = GetAvatarActor();
    if (!Avatar) return false;
    
    const float Distance = Avatar->GetDistanceTo(HitResult.TargetActor);
    const float MaxRange = CachedWeaponData ? CachedWeaponData->LightAttack.Range : 180.0f;
    if (Distance > MaxRange + 50.0f) return false;
    
    if (HitResult.HitIndex != CurrentComboIndex) return false;
    
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.TargetActor);
    if (!TargetASC) return false;
    
    return true;
}

void UGA_Weapon_MeleeLight_ClientPrediction::ApplyServerDamage(const FCombatHitResult& HitResult)
{
    AActor* Avatar = GetAvatarActor();
    if (!Avatar || !DamageGE) return;

    UAbilitySystemComponent* SourceASC = GetASC();
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.TargetActor);
    
    if (!SourceASC || !TargetASC) return;

    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.AddSourceObject(Avatar);
    
    APawn* Pawn = Cast<APawn>(Avatar);
    if (Pawn && Pawn->GetController())
    {
        EffectContext.AddInstigator(Avatar, Pawn->GetController());
    }
    else
    {
        EffectContext.AddInstigator(Avatar, nullptr);
    }

    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageGE, 1, EffectContext);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid()) return;

    FGameplayTag BaseWeaponDamageTag = FGameplayTag::RequestGameplayTag(FName("Data.BaseWeaponDamage"), false);
    if (BaseWeaponDamageTag.IsValid())
    {
        SpecHandle.Data->SetSetByCallerMagnitude(
            BaseWeaponDamageTag,
            HitResult.DamageAmount
        );
    }

    SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
    
    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SERVER] Applied REAL damage: %.1f to %s"),
            HitResult.DamageAmount,
            *GetNameSafe(HitResult.TargetActor));
    }
}

// ============================================================================
// COMBO CONTROL
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::StartComboAt(int32 Index)
{
    if (!ComboSections.IsValidIndex(Index) || Index >= MaxComboHits)
    {
        EndCombo(false);
        return;
    }

    CurrentComboIndex = Index;
    bHasAppliedDamageThisWindow = false;
    bWasInputReceivedThisWindow = false;

    UAnimInstance* AnimInstance = GetAvatarAnimInstance();
    if (!CachedMontage || !AnimInstance)
    {
        EndCombo(true);
        return;
    }

    const float SpeedMult = GetAttackSpeedMultiplier();
    const float FinalRate = FMath::Clamp(MontagePlayRate * SpeedMult, 0.05f, 5.0f);

    if (AnimInstance->Montage_IsPlaying(CachedMontage))
    {
        if (UAbilitySystemComponent* ASC = GetASC())
        {
            ASC->CurrentMontageJumpToSection(ComboSections[CurrentComboIndex]);
        }
        else
        {
            AnimInstance->Montage_JumpToSection(ComboSections[CurrentComboIndex], CachedMontage);
        }
        return;
    }

    if (MontageTask)
    {
        MontageTask->EndTask();
        MontageTask = nullptr;
    }

    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        CachedMontage,
        FinalRate,
        ComboSections[CurrentComboIndex],
        false,
        1.0f
    );

    if (!MontageTask)
    {
        EndCombo(true);
        return;
    }

    MontageTask->OnCompleted.AddDynamic(this, &UGA_Weapon_MeleeLight_ClientPrediction::HandleMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_Weapon_MeleeLight_ClientPrediction::HandleMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_Weapon_MeleeLight_ClientPrediction::HandleMontageCancelled);
    MontageTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight_ClientPrediction::CancelCombo()
{
    StopCurrentMontage();
    EndCombo(true);
}

void UGA_Weapon_MeleeLight_ClientPrediction::StopCurrentMontage()
{
    if (!CachedMontage) return;

    if (UAbilitySystemComponent* ASC = GetASC())
    {
        ASC->CurrentMontageStop(StopBlendOutTime);
        return;
    }

    if (UAnimInstance* Anim = GetAvatarAnimInstance())
    {
        Anim->Montage_Stop(StopBlendOutTime, CachedMontage);
    }
}

// ============================================================================
// EVENT BINDING
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::BindComboWindowBegin()
{
    if (!ComboWindowBeginTag.IsValid()) return;
    
    WaitBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboWindowBeginTag, nullptr, true, false);
    if (WaitBeginTask)
    {
        WaitBeginTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight_ClientPrediction::HandleComboWindowBegin);
        WaitBeginTask->ReadyForActivation();
    }
}

void UGA_Weapon_MeleeLight_ClientPrediction::BindComboWindowEnd()
{
    if (!ComboWindowEndTag.IsValid()) return;
    
    WaitEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboWindowEndTag, nullptr, true, false);
    if (WaitEndTask)
    {
        WaitEndTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight_ClientPrediction::HandleComboWindowEnd);
        WaitEndTask->ReadyForActivation();
    }
}

// ============================================================================
// GAS ACTIVATION
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !DamageGE)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    CacheWeaponData();

    if (!CachedMontage || ComboSections.Num() == 0)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    CurrentComboIndex = 0;
    bIsAcceptingComboInput = false;
    bWasInputReceivedThisWindow = false;
    bHasAppliedDamageThisWindow = false;

    BindComboWindowBegin();
    BindComboWindowEnd();
    StartComboAt(0);
}

// ============================================================================
// INPUT
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::InputPressed(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);

    if (!bIsAcceptingComboInput) return;
    if (CurrentComboIndex >= MaxComboHits - 1) return;

    bWasInputReceivedThisWindow = true;
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::HandleComboWindowBegin(FGameplayEventData Payload)
{
    BindComboWindowBegin();

    bIsAcceptingComboInput = true;
    bWasInputReceivedThisWindow = false;
    bHasAppliedDamageThisWindow = false;

    if (!IsServerAuthority())
    {
        FCombatHitResult HitResult = PerformClientTraceAndPredictDamage();
        
        if (HitResult.TargetActor)
        {
            ApplyClientVisualDamage(HitResult);
            Server_ReportHitResult(HitResult);
        }
    }
}

void UGA_Weapon_MeleeLight_ClientPrediction::HandleComboWindowEnd(FGameplayEventData Payload)
{
    BindComboWindowEnd();
    bIsAcceptingComboInput = false;

    if (bWasInputReceivedThisWindow)
    {
        bWasInputReceivedThisWindow = false;
        const int32 NextHitIndex = CurrentComboIndex + 1;
        StartComboAt(NextHitIndex);
    }
    else
    {
        CancelCombo();
    }
}

void UGA_Weapon_MeleeLight_ClientPrediction::HandleMontageCompleted()
{
    if (CurrentComboIndex >= MaxComboHits - 1)
        EndCombo(false);
    else
        EndCombo(true);
}

void UGA_Weapon_MeleeLight_ClientPrediction::HandleMontageInterrupted()
{
    EndCombo(true);
}

void UGA_Weapon_MeleeLight_ClientPrediction::HandleMontageCancelled()
{
    EndCombo(true);
}

// ============================================================================
// CLEANUP
// ============================================================================
void UGA_Weapon_MeleeLight_ClientPrediction::KillAllTasks()
{
    if (MontageTask) { MontageTask->EndTask(); MontageTask = nullptr; }
    if (WaitBeginTask) { WaitBeginTask->EndTask(); WaitBeginTask = nullptr; }
    if (WaitEndTask) { WaitEndTask->EndTask(); WaitEndTask = nullptr; }
}

void UGA_Weapon_MeleeLight_ClientPrediction::EndCombo(bool bWasCancelled)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

void UGA_Weapon_MeleeLight_ClientPrediction::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    KillAllTasks();
    
    if (bWasCancelled)
        StopCurrentMontage();

    CurrentComboIndex = 0;
    bIsAcceptingComboInput = false;
    bWasInputReceivedThisWindow = false;
    bHasAppliedDamageThisWindow = false;
    
    CachedWeaponData = nullptr;
    CachedMontage = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}