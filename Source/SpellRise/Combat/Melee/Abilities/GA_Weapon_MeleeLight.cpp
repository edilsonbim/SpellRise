#include "GA_Weapon_MeleeLight.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Combat/Melee/Components/SpellRiseMeleeComponent.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Math/UnrealMathUtility.h"

UGA_Weapon_MeleeLight::UGA_Weapon_MeleeLight()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    bReplicateInputDirectly = true;

    StopBlendOutTime = 0.15f;
}

// ============================================================================
// WORLD CONTEXT
// ============================================================================
UWorld* UGA_Weapon_MeleeLight::GetWorldContext() const
{
    if (CurrentActorInfo)
    {
        if (AActor* Avatar = CurrentActorInfo->AvatarActor.Get())
            return Avatar->GetWorld();
        if (AActor* Owner = CurrentActorInfo->OwnerActor.Get())
            return Owner->GetWorld();
    }
    
    if (UAbilitySystemComponent* ASC = GetASC())
    {
        if (AActor* Owner = ASC->GetOwner())
            return Owner->GetWorld();
    }
    
    return GetWorld();
}

bool UGA_Weapon_MeleeLight::IsServer() const
{
    return CurrentActorInfo && CurrentActorInfo->IsNetAuthority();
}

bool UGA_Weapon_MeleeLight::IsLocallyControlled() const
{
    if (!CurrentActorInfo) return false;
    
    if (AActor* Avatar = GetAvatar())
    {
        if (APawn* Pawn = Cast<APawn>(Avatar))
        {
            return Pawn->IsLocallyControlled();
        }
    }
    
    return false;
}

AActor* UGA_Weapon_MeleeLight::GetAvatar() const
{
    return (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
        ? CurrentActorInfo->AvatarActor.Get()
        : nullptr;
}

UAbilitySystemComponent* UGA_Weapon_MeleeLight::GetASC() const
{
    return CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
}

UAnimInstance* UGA_Weapon_MeleeLight::GetAvatarAnimInstance() const
{
    AActor* Avatar = GetAvatar();
    if (!Avatar) return nullptr;

    if (ACharacter* Char = Cast<ACharacter>(Avatar))
    {
        if (USkeletalMeshComponent* MeshComp = Char->GetMesh())
        {
            return MeshComp->GetAnimInstance();
        }
    }

    if (USkeletalMeshComponent* MeshComp = Avatar->FindComponentByClass<USkeletalMeshComponent>())
    {
        return MeshComp->GetAnimInstance();
    }

    return nullptr;
}

// ============================================================================
// CACHE DOS DADOS DA ARMA – FONTE ÚNICA: DA_MeleeWeaponData
// ============================================================================
void UGA_Weapon_MeleeLight::CacheWeaponData()
{
    CachedMeleeComponent = nullptr;
    CachedAttackMontage = nullptr;
    CachedComboSections.Empty();
    CachedMaxComboHits = 3;
    CachedComboWindowBeginTag = FGameplayTag();
    CachedComboWindowEndTag = FGameplayTag();

    AActor* Avatar = GetAvatar();
    if (!Avatar) return;

    USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
    if (!WeaponComp) return;

    CachedMeleeComponent = WeaponComp->GetMeleeComponent();

    // ✅ Agora GetEquippedWeapon() retorna const UDA_MeleeWeaponData*
    if (CachedMeleeComponent && WeaponComp->GetEquippedWeapon())
    {
        const UDA_MeleeWeaponData* Data = WeaponComp->GetEquippedWeapon();
        CachedAttackMontage = Data->AttackMontage;
        CachedComboSections = Data->ComboConfig.ComboSections;
        CachedMaxComboHits = Data->ComboConfig.MaxComboHits;
        CachedComboWindowBeginTag = Data->ComboConfig.ComboWindowBeginTag;
        CachedComboWindowEndTag = Data->ComboConfig.ComboWindowEndTag;
    }

    // Fallback legacy
    if (!CachedAttackMontage)
    {
        CachedAttackMontage = AttackMontage_Legacy;
    }

    if (CachedComboSections.Num() == 0)
    {
        CachedComboSections = { FName("Hit1"), FName("Hit2"), FName("Hit3") };
    }

    if (!CachedComboWindowBeginTag.IsValid())
    {
        CachedComboWindowBeginTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), false);
    }
    if (!CachedComboWindowEndTag.IsValid())
    {
        CachedComboWindowEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"), false);
    }
}

void UGA_Weapon_MeleeLight::InitializeMeleeComponent()
{
    AActor* Avatar = GetAvatar();
    if (!Avatar) return;

    USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
    if (!WeaponComp) return;

    WeaponComp->InitializeMeleeComponent();
    CachedMeleeComponent = WeaponComp->GetMeleeComponent();
}

float UGA_Weapon_MeleeLight::GetAttackSpeedMultiplierFromASC() const
{
    UAbilitySystemComponent* ASC = GetASC();
    if (!ASC) return 1.f;

    const float ASM = ASC->GetNumericAttribute(
        UDerivedStatsAttributeSet::GetAttackSpeedMultiplierAttribute()
    );
    
    return FMath::Clamp(ASM, AttackSpeedMin, AttackSpeedMax);
}

void UGA_Weapon_MeleeLight::ApplyCurrentMontagePlayRate(float NewRate) const
{
    NewRate = FMath::Clamp(NewRate, 0.05f, 5.0f);

    if (UAbilitySystemComponent* ASC = GetASC())
    {
        ASC->CurrentMontageSetPlayRate(NewRate);
        return;
    }

    if (UAnimInstance* Anim = GetAvatarAnimInstance())
    {
        if (CachedAttackMontage && Anim->Montage_IsPlaying(CachedAttackMontage))
        {
            Anim->Montage_SetPlayRate(CachedAttackMontage, NewRate);
        }
    }
}

void UGA_Weapon_MeleeLight::TryApplyHitOnceForCurrentWindow()
{
    if (bDidHitThisWindow) return;
    if (!IsServer()) return;

    bDidHitThisWindow = true;
    
    if (bDebug)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 💥 Damage applied for hit %d"), ComboIndex + 1);
    }
}

// ============================================================================
// COMBO CONTROL – USANDO CACHE
// ============================================================================
void UGA_Weapon_MeleeLight::StartComboAt(int32 NewIndex)
{
    if (!CachedComboSections.IsValidIndex(NewIndex) || NewIndex >= CachedMaxComboHits)
    {
        FinishCombo(false);
        return;
    }

    ComboIndex = NewIndex;
    bDidHitThisWindow = false;
    bShouldCancelOnWindowEnd = true;
    
    if (IsServer())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 🔵 Starting hit %d"), ComboIndex + 1);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][CLIENT] 🔵 Starting hit %d"), ComboIndex + 1);
    }

    UAnimInstance* Anim = GetAvatarAnimInstance();
    if (!CachedAttackMontage || !Anim)
    {
        FinishCombo(true);
        return;
    }

    const float AttackSpeedMult = GetAttackSpeedMultiplierFromASC();
    const float EffectiveRate = FMath::Clamp(MontagePlayRate * AttackSpeedMult, 0.05f, 5.0f);

    if (Anim->Montage_IsPlaying(CachedAttackMontage))
    {
        if (UAbilitySystemComponent* ASC = GetASC())
        {
            ASC->CurrentMontageJumpToSection(CachedComboSections[ComboIndex]);
        }
        else
        {
            Anim->Montage_JumpToSection(CachedComboSections[ComboIndex], CachedAttackMontage);
        }

        ApplyCurrentMontagePlayRate(EffectiveRate);
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
        CachedAttackMontage,
        EffectiveRate,
        CachedComboSections[ComboIndex],
        false,
        1.0f
    );

    if (!MontageTask)
    {
        FinishCombo(true);
        return;
    }

    MontageTask->OnCompleted.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageCancelled);
    MontageTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::CancelCombo()
{
    if (IsServer())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 🛑 Cancelling combo at hit %d"), ComboIndex + 1);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][CLIENT] 🛑 Cancelling combo at hit %d"), ComboIndex + 1);
    }
    
    StopAttackMontage();
    FinishCombo(true);
}

void UGA_Weapon_MeleeLight::StopAttackMontage()
{
    if (!CachedAttackMontage) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo] ❌ CachedAttackMontage is NULL!"));
        return;
    }

    if (UAbilitySystemComponent* ASC = GetASC())
    {
        ASC->CurrentMontageStop(StopBlendOutTime);
    }
    else
    {
        if (UAnimInstance* Anim = GetAvatarAnimInstance())
        {
            Anim->Montage_Stop(StopBlendOutTime, CachedAttackMontage);
        }
    }
}

// ============================================================================
// WAITING FOR EVENTS – AGORA USA CACHED TAGS
// ============================================================================
void UGA_Weapon_MeleeLight::StartWaitingForComboWindowBegin()
{
    if (!CachedComboWindowBeginTag.IsValid()) return;

    WaitComboBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, CachedComboWindowBeginTag, nullptr, true, false
    );

    if (!WaitComboBeginTask) return;

    WaitComboBeginTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowBegin);
    WaitComboBeginTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::StartWaitingForComboWindowEnd()
{
    if (!CachedComboWindowEndTag.IsValid()) return;

    WaitComboEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, CachedComboWindowEndTag, nullptr, true, false
    );

    if (!WaitComboEndTask) return;

    WaitComboEndTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowEnd);
    WaitComboEndTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::ActivateAbility(
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

    if (!CachedAttackMontage || CachedComboSections.Num() == 0)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    InitializeMeleeComponent();

    ComboIndex = 0;
    bAcceptingComboInput = false;
    bComboInputQueued = false;
    bDidHitThisWindow = false;
    bShouldCancelOnWindowEnd = false;

    StartWaitingForComboWindowBegin();
    StartWaitingForComboWindowEnd();

    StartComboAt(0);
}

void UGA_Weapon_MeleeLight::InputPressed(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);

    if (!bAcceptingComboInput) return;
    if (ComboIndex >= CachedMaxComboHits - 1) return;

    bComboInputQueued = true;
    
    if (bDebug && IsServer())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 🎯 Input received during window %d"), ComboIndex + 1);
    }
}

void UGA_Weapon_MeleeLight::OnComboWindowBegin(FGameplayEventData Payload)
{
    StartWaitingForComboWindowBegin();

    bAcceptingComboInput = true;
    bComboInputQueued = false;
    bDidHitThisWindow = false;
    bShouldCancelOnWindowEnd = true;

    if (IsServer())
    {
        TryApplyHitOnceForCurrentWindow();
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 🟢 Window BEGIN for hit %d"), ComboIndex + 1);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][CLIENT] 🟢 Window BEGIN for hit %d"), ComboIndex + 1);
    }
}

// ============================================================================
// LÓGICA PRINCIPAL - SERVER AUTHORITATIVE COM MULTICAST!
// ============================================================================
void UGA_Weapon_MeleeLight::OnComboWindowEnd(FGameplayEventData Payload)
{
    StartWaitingForComboWindowEnd();
    bAcceptingComboInput = false;

    if (IsServer())
    {
        if (bComboInputQueued)
        {
            // ✅ AVANÇAR
            bComboInputQueued = false;
            const int32 NextHitIndex = ComboIndex + 1;
            
            UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] ✅ DECISION: Advance %d -> %d"), 
                ComboIndex + 1, NextHitIndex + 1);
            
            StartComboAt(NextHitIndex);
        }
        else
        {
            // ❌ CANCELAR
            UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] ❌ DECISION: Cancel at hit %d"), 
                ComboIndex + 1);
            
            // ENVIA MULTICAST PARA PARAR A MONTAGEM EM TODOS
            Multicast_StopMontage();
            
            FinishCombo(true);
        }
    }
    else
    {
        // CLIENTE: apenas aguarda – a montagem será parada pelo multicast ou pela replicação
        UE_LOG(LogTemp, Verbose, TEXT("[Combo][CLIENT] ⏳ Waiting..."));
    }
}

// ============================================================================
// MULTICAST PARA PARAR A MONTAGEM
// ============================================================================
void UGA_Weapon_MeleeLight::Multicast_StopMontage_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("[Combo][Multicast] 🛑 Stopping montage on %s"), 
        IsServer() ? TEXT("SERVER") : TEXT("CLIENT"));
    
    StopAttackMontage();
}

void UGA_Weapon_MeleeLight::OnMontageCompleted()
{
    if (IsServer())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 📌 Montage completed at hit %d"), ComboIndex + 1);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo][CLIENT] 📌 Montage completed at hit %d"), ComboIndex + 1);
    }
    
    if (ComboIndex >= CachedMaxComboHits - 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo] 🏆 Combo FINISHED! %d hits"), ComboIndex + 1);
        FinishCombo(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo] ⚠️ Montage completed early - cancelling"));
        FinishCombo(true);
    }
}

void UGA_Weapon_MeleeLight::OnMontageInterrupted()
{
    UE_LOG(LogTemp, Warning, TEXT("[Combo] ⚠️ Montage INTERRUPTED at hit %d"), ComboIndex + 1);
    FinishCombo(true);
}

void UGA_Weapon_MeleeLight::OnMontageCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("[Combo] ⚠️ Montage CANCELLED at hit %d"), ComboIndex + 1);
    FinishCombo(true);
}

void UGA_Weapon_MeleeLight::StopAllTasks()
{
    if (MontageTask) { MontageTask->EndTask(); MontageTask = nullptr; }
    if (WaitComboBeginTask) { WaitComboBeginTask->EndTask(); WaitComboBeginTask = nullptr; }
    if (WaitComboEndTask) { WaitComboEndTask->EndTask(); WaitComboEndTask = nullptr; }
}

void UGA_Weapon_MeleeLight::FinishCombo(bool bCancelled)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bCancelled);
}

void UGA_Weapon_MeleeLight::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (bWasCancelled)
    {
        StopAttackMontage();
    }

    StopAllTasks();

    bAcceptingComboInput = false;
    bComboInputQueued = false;
    ComboIndex = 0;
    bDidHitThisWindow = false;
    bShouldCancelOnWindowEnd = false;

    CachedMeleeComponent = nullptr;
    CachedAttackMontage = nullptr;
    CachedComboSections.Empty();
    CachedMaxComboHits = 3;
    CachedComboWindowBeginTag = FGameplayTag();
    CachedComboWindowEndTag = FGameplayTag();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}