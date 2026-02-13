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
#include "Animation/AnimMontage.h"      // ESSENCIAL para UAnimMontage completo
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
    if (!Avatar) 
    {
        UE_LOG(LogTemp, Error, TEXT("[Cache] Avatar is NULL!"));
        return;
    }

    USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
    if (!WeaponComp) 
    {
        UE_LOG(LogTemp, Error, TEXT("[Cache] WeaponComponent not found on %s"), *GetNameSafe(Avatar));
        return;
    }

    CachedMeleeComponent = WeaponComp->GetMeleeComponent();

    if (CachedMeleeComponent && WeaponComp->GetEquippedWeapon())
    {
        const UDA_MeleeWeaponData* Data = WeaponComp->GetEquippedWeapon();
        CachedAttackMontage = Data->AttackMontage;
        CachedComboSections = Data->ComboConfig.ComboSections;
        CachedMaxComboHits = Data->ComboConfig.MaxComboHits;
        CachedComboWindowBeginTag = Data->ComboConfig.ComboWindowBeginTag;
        CachedComboWindowEndTag = Data->ComboConfig.ComboWindowEndTag;
        
        UE_LOG(LogTemp, Warning, TEXT("[Cache] Loaded from DataAsset: Montage=%s, Sections=%d, MaxHits=%d"),
            CachedAttackMontage ? *CachedAttackMontage->GetName() : TEXT("NULL"),
            CachedComboSections.Num(), CachedMaxComboHits);
    }

    // Fallback legacy
    if (!CachedAttackMontage)
    {
        CachedAttackMontage = AttackMontage_Legacy;
        UE_LOG(LogTemp, Warning, TEXT("[Cache] Using Legacy fallback: %s"),
            CachedAttackMontage ? *CachedAttackMontage->GetName() : TEXT("NULL"));
    }

    if (CachedComboSections.Num() == 0)
    {
        CachedComboSections = { FName("Hit1"), FName("Hit2"), FName("Hit3") };
        UE_LOG(LogTemp, Warning, TEXT("[Cache] Using default combo sections"));
    }

    if (!CachedComboWindowBeginTag.IsValid())
    {
        CachedComboWindowBeginTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), false);
        UE_LOG(LogTemp, Verbose, TEXT("[Cache] Using default Begin tag"));
    }
    if (!CachedComboWindowEndTag.IsValid())
    {
        CachedComboWindowEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"), false);
        UE_LOG(LogTemp, Verbose, TEXT("[Cache] Using default End tag"));
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
    
    UE_LOG(LogTemp, Warning, TEXT("[Init] MeleeComponent initialized: %s"), 
        CachedMeleeComponent ? *CachedMeleeComponent->GetName() : TEXT("NULL"));
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
        if (CachedAttackMontage && Anim->Montage_IsPlaying(CachedAttackMontage.Get()))
        {
            Anim->Montage_SetPlayRate(CachedAttackMontage.Get(), NewRate);
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
    UE_LOG(LogTemp, Warning, TEXT("[StartCombo] Attempting index %d/%d, Sections=%d"), 
        NewIndex + 1, CachedMaxComboHits, CachedComboSections.Num());

    if (!CachedComboSections.IsValidIndex(NewIndex) || NewIndex >= CachedMaxComboHits)
    {
        UE_LOG(LogTemp, Error, TEXT("[StartCombo] Invalid index %d - Finishing"), NewIndex);
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
    if (!Anim)
    {
        UE_LOG(LogTemp, Error, TEXT("[StartCombo] AnimInstance is NULL!"));
        FinishCombo(true);
        return;
    }

    if (!CachedAttackMontage)
    {
        UE_LOG(LogTemp, Error, TEXT("[StartCombo] CachedAttackMontage is NULL!"));
        FinishCombo(true);
        return;
    }

    const float AttackSpeedMult = GetAttackSpeedMultiplierFromASC();
    const float EffectiveRate = FMath::Clamp(MontagePlayRate * AttackSpeedMult, 0.05f, 5.0f);

    UE_LOG(LogTemp, Verbose, TEXT("[StartCombo] Rate: base=%.2f, mult=%.2f, effective=%.2f"),
        MontagePlayRate, AttackSpeedMult, EffectiveRate);

    if (Anim->Montage_IsPlaying(CachedAttackMontage.Get()))
    {
        UE_LOG(LogTemp, Verbose, TEXT("[StartCombo] Montage already playing - jumping to section %s"),
            *CachedComboSections[ComboIndex].ToString());

        if (UAbilitySystemComponent* ASC = GetASC())
        {
            ASC->CurrentMontageJumpToSection(CachedComboSections[ComboIndex]);
        }
        else
        {
            Anim->Montage_JumpToSection(CachedComboSections[ComboIndex], CachedAttackMontage.Get());
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
        CachedAttackMontage.Get(),
        EffectiveRate,
        CachedComboSections[ComboIndex],
        false,
        1.0f
    );

    if (!MontageTask)
    {
        UE_LOG(LogTemp, Error, TEXT("[StartCombo] Failed to create MontageTask!"));
        FinishCombo(true);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[StartCombo] MontageTask created successfully"));

    MontageTask->OnCompleted.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageCompleted);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageInterrupted);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageCancelled);
    MontageTask->ReadyForActivation();
    
    // Verificação adicional: ver se a montagem realmente começou
    if (UAnimInstance* CheckAnim = GetAvatarAnimInstance())
    {
        bool bIsPlaying = CheckAnim->Montage_IsPlaying(CachedAttackMontage.Get());
        UE_LOG(LogTemp, Warning, TEXT("[StartCombo] After activation - Montage playing: %s"), 
            bIsPlaying ? TEXT("YES") : TEXT("NO"));
    }
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
        UE_LOG(LogTemp, Warning, TEXT("[Stop] CachedAttackMontage is NULL!"));
        return;
    }

    UE_LOG(LogTemp, Verbose, TEXT("[Stop] Stopping montage with blend out %.2f"), StopBlendOutTime);

    if (UAbilitySystemComponent* ASC = GetASC())
    {
        ASC->CurrentMontageStop(StopBlendOutTime);
    }
    else
    {
        if (UAnimInstance* Anim = GetAvatarAnimInstance())
        {
            Anim->Montage_Stop(StopBlendOutTime, CachedAttackMontage.Get());
        }
    }
}

// ============================================================================
// WAITING FOR EVENTS – AGORA USA CACHED TAGS
// ============================================================================
void UGA_Weapon_MeleeLight::StartWaitingForComboWindowBegin()
{
    if (!CachedComboWindowBeginTag.IsValid()) 
    {
        UE_LOG(LogTemp, Verbose, TEXT("[Wait] Begin tag invalid, skipping"));
        return;
    }

    WaitComboBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, CachedComboWindowBeginTag, nullptr, true, false
    );

    if (!WaitComboBeginTask) 
    {
        UE_LOG(LogTemp, Error, TEXT("[Wait] Failed to create WaitComboBeginTask"));
        return;
    }

    WaitComboBeginTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowBegin);
    WaitComboBeginTask->ReadyForActivation();
    UE_LOG(LogTemp, Verbose, TEXT("[Wait] Waiting for Begin tag: %s"), *CachedComboWindowBeginTag.ToString());
}

void UGA_Weapon_MeleeLight::StartWaitingForComboWindowEnd()
{
    if (!CachedComboWindowEndTag.IsValid()) 
    {
        UE_LOG(LogTemp, Verbose, TEXT("[Wait] End tag invalid, skipping"));
        return;
    }

    WaitComboEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this, CachedComboWindowEndTag, nullptr, true, false
    );

    if (!WaitComboEndTask) 
    {
        UE_LOG(LogTemp, Error, TEXT("[Wait] Failed to create WaitComboEndTask"));
        return;
    }

    WaitComboEndTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowEnd);
    WaitComboEndTask->ReadyForActivation();
    UE_LOG(LogTemp, Verbose, TEXT("[Wait] Waiting for End tag: %s"), *CachedComboWindowEndTag.ToString());
}

void UGA_Weapon_MeleeLight::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UE_LOG(LogTemp, Warning, TEXT("[Activate] Ability activated"));
    UE_LOG(LogTemp, Warning, TEXT("[Ability] ActivateAbility on %s"), IsServer() ? TEXT("SERVER") : TEXT("CLIENT"));

    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !DamageGE)
    {
        UE_LOG(LogTemp, Error, TEXT("[Activate] Failed: ActorInfo=%s, ASC=%s, DamageGE=%s"),
            ActorInfo ? TEXT("OK") : TEXT("NULL"),
            (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid()) ? TEXT("OK") : TEXT("NULL"),
            *GetNameSafe(DamageGE));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    CacheWeaponData();

    if (!CachedAttackMontage || CachedComboSections.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[Activate] Failed: Montage=%s, Sections=%d"),
            CachedAttackMontage ? *CachedAttackMontage->GetName() : TEXT("NULL"),
            CachedComboSections.Num());
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Activate] Commit failed"));
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
    // NÃO recriar a task aqui – ela já terminou.
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
// LÓGICA PRINCIPAL - SERVER AUTHORITATIVE
// ============================================================================
void UGA_Weapon_MeleeLight::OnComboWindowEnd(FGameplayEventData Payload)
{
    // NÃO recriar a task aqui – ela já terminou.
    bAcceptingComboInput = false;

    if (IsServer())
    {
        if (bComboInputQueued)
        {
            // ✅ AVANÇAR
            bComboInputQueued = false;
            UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] ✅ DECISION: Advance %d -> %d"), 
                ComboIndex + 1, ComboIndex + 2);
            StartComboAt(ComboIndex + 1);
        }
        else
        {
            // ❌ CANCELAR
            UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] ❌ DECISION: Cancel at hit %d"), ComboIndex + 1);
            StopAttackMontage();
            FinishCombo(true);
        }
    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("[Combo][CLIENT] ⏳ Waiting..."));
    }
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
    UE_LOG(LogTemp, Verbose, TEXT("[Finish] Combo finished, cancelled=%d"), bCancelled);
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
