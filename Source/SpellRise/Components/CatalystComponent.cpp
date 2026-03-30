// CatalystComponent.cpp

#include "SpellRise/Components/CatalystComponent.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseCatalystRuntime, Log, All);

namespace
{
    struct FCatalystRuntimeCounters
    {
        int64 ChargeOnHitApplied = 0;
        int64 ChargeOnDamageTakenApplied = 0;
        int64 ProcAttempts = 0;
        int64 ProcActivated = 0;
    };

    FCatalystRuntimeCounters GCatalystRuntimeCounters;
}

UCatalystComponent::UCatalystComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCatalystComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache references to the owner character and ability system component
    OwnerCharacter = Cast<ASpellRiseCharacterBase>(GetOwner());
    ResolveAbilitySystemComponent();

    // If catalyst is enabled on the server, bind to attribute changes
    if (bCatalystEnabled && AbilitySystemComponent && AbilitySystemComponent->IsOwnerActorAuthoritative())
    {
        EnsureCatalystChargeListenerBound_Server();
    }
}

bool UCatalystComponent::ResolveAbilitySystemComponent()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ASpellRiseCharacterBase>(GetOwner());
    }

    if (!OwnerCharacter)
    {
        return false;
    }

    USpellRiseAbilitySystemComponent* ResolvedASC =
        Cast<USpellRiseAbilitySystemComponent>(OwnerCharacter->GetAbilitySystemComponent());

    if (AbilitySystemComponent != ResolvedASC)
    {
        if (AbilitySystemComponent && bCatalystListenerBound)
        {
            AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
                UCatalystAttributeSet::GetCatalystChargeAttribute()
            ).RemoveAll(this);
        }

        AbilitySystemComponent = ResolvedASC;
        bCatalystListenerBound = false;
    }

    return AbilitySystemComponent != nullptr;
}

bool UCatalystComponent::HasServerAuthority() const
{
    const AActor* OwnerActor = GetOwner();
    return OwnerActor && OwnerActor->HasAuthority();
}

void UCatalystComponent::EnsureCatalystChargeListenerBound_Server()
{
    if (!ResolveAbilitySystemComponent())
    {
        return;
    }

    if (!AbilitySystemComponent || !HasServerAuthority())
    {
        return;
    }

    if (bCatalystListenerBound || !bCatalystEnabled)
    {
        return;
    }

    BindCatalystChargeListener_Server();
    bCatalystListenerBound = true;
}

void UCatalystComponent::BindCatalystChargeListener_Server()
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    // Bind to changes of the CatalystCharge attribute on the ability system component
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UCatalystAttributeSet::GetCatalystChargeAttribute()
    ).AddUObject(this, &UCatalystComponent::OnCatalystChargeChanged);
}

void UCatalystComponent::OnCatalystChargeChanged(const FOnAttributeChangeData& Data)
{
    if (!AbilitySystemComponent || !HasServerAuthority())
    {
        return;
    }

    if (!bCatalystEnabled)
    {
        return;
    }

    // Crossed 100% threshold?  Trigger proc if so.
    const bool bCrossedToReady = (Data.OldValue < 100.f && Data.NewValue >= 100.f);
    if (bCrossedToReady)
    {
        TryProcCatalystIfReady();
    }
}

bool UCatalystComponent::TryAddCatalystCharge_OnValidHit(AActor* TargetActor)
{
    if (!ResolveAbilitySystemComponent())
    {
        return false;
    }

    if (!bCatalystEnabled || !AbilitySystemComponent)
    {
        return false;
    }

    // Only the server modifies attributes
    if (!HasServerAuthority())
    {
        return false;
    }

    EnsureCatalystChargeListenerBound_Server();

    AActor* SourceActor = AbilitySystemComponent->GetAvatarActor();
    if (!SourceActor)
    {
        SourceActor = OwnerCharacter ? Cast<AActor>(OwnerCharacter) : GetOwner();
    }

    if (!SourceActor || !TargetActor || TargetActor == SourceActor)
    {
        return false;
    }

    // Do not gain charges while dead or when catalyst active
    static FGameplayTag TAG_State_Dead = FGameplayTag::RequestGameplayTag(FName("State.Dead"), false);
    static FGameplayTag TAG_State_Catalyst_Active = FGameplayTag::RequestGameplayTag(FName("State.Catalyst.Active"), false);
    if (AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Dead) || AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Catalyst_Active))
    {
        return false;
    }

    if (!GE_CatalystAddCharge)
    {
        UE_LOG(LogSpellRiseCatalystRuntime, Warning, TEXT("[CatalystComponent] GE_CatalystAddCharge is NULL on %s (Owner=%s Avatar=%s). Set it on the CatalystComponent defaults."),
            *GetNameSafe(this), *GetNameSafe(GetOwner()), *GetNameSafe(SourceActor));
        return false;
    }

    // Prepare effect context with the source actor for stat tracking
    FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
    Ctx.AddSourceObject(SourceActor);

    // Create an outgoing spec for the charge effect
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
    {
        UE_LOG(LogSpellRiseCatalystRuntime, Error, TEXT("[CatalystComponent] MakeOutgoingSpec failed for GE_CatalystAddCharge on %s"), *GetNameSafe(SourceActor));
        return false;
    }

    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    ++GCatalystRuntimeCounters.ChargeOnHitApplied;
    UE_LOG(LogSpellRiseCatalystRuntime, Verbose, TEXT("[CATALYST][CHARGE_ON_HIT] Source=%s Target=%s AppliedCount=%lld"),
        *GetNameSafe(SourceActor),
        *GetNameSafe(TargetActor),
        GCatalystRuntimeCounters.ChargeOnHitApplied);
    return true;
}

bool UCatalystComponent::TryAddCatalystCharge_OnDamageTaken(AActor* InstigatorActor)
{
    if (!ResolveAbilitySystemComponent())
    {
        return false;
    }

    if (!bCatalystEnabled || !AbilitySystemComponent)
    {
        return false;
    }

    // Only the server modifies attributes
    if (!HasServerAuthority())
    {
        return false;
    }

    EnsureCatalystChargeListenerBound_Server();

    AActor* VictimActor = AbilitySystemComponent->GetAvatarActor();
    if (!VictimActor)
    {
        VictimActor = OwnerCharacter ? Cast<AActor>(OwnerCharacter) : GetOwner();
    }

    if (!VictimActor)
    {
        return false;
    }

    // Do not gain charges while dead or when catalyst active
    static FGameplayTag TAG_State_Dead = FGameplayTag::RequestGameplayTag(FName("State.Dead"), false);
    static FGameplayTag TAG_State_Catalyst_Active = FGameplayTag::RequestGameplayTag(FName("State.Catalyst.Active"), false);
    if (AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Dead) || AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Catalyst_Active))
    {
        return false;
    }

    if (!GE_CatalystAddCharge)
    {
        UE_LOG(LogSpellRiseCatalystRuntime, Warning, TEXT("[CatalystComponent] GE_CatalystAddCharge is NULL on %s (Owner=%s Avatar=%s). Set it on the CatalystComponent defaults."),
            *GetNameSafe(this), *GetNameSafe(GetOwner()), *GetNameSafe(VictimActor));
        return false;
    }

    // Prepare effect context with the instigator (the one who damaged us) for stat tracking
    FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
    Ctx.AddSourceObject(InstigatorActor ? InstigatorActor : VictimActor);

    // Create an outgoing spec for the charge effect
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
    {
        UE_LOG(LogSpellRiseCatalystRuntime, Error, TEXT("[CatalystComponent] MakeOutgoingSpec failed for GE_CatalystAddCharge on victim %s"), *GetNameSafe(VictimActor));
        return false;
    }

    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    ++GCatalystRuntimeCounters.ChargeOnDamageTakenApplied;
    UE_LOG(LogSpellRiseCatalystRuntime, Verbose, TEXT("[CATALYST][CHARGE_ON_DAMAGE_TAKEN] Victim=%s Instigator=%s AppliedCount=%lld"),
        *GetNameSafe(VictimActor),
        *GetNameSafe(InstigatorActor),
        GCatalystRuntimeCounters.ChargeOnDamageTakenApplied);
    return true;
}

void UCatalystComponent::TryProcCatalystIfReady()
{
    if (!ResolveAbilitySystemComponent())
    {
        return;
    }

    if (!bCatalystEnabled || !AbilitySystemComponent)
    {
        return;
    }

    // Only the server should activate abilities
    if (!HasServerAuthority())
    {
        return;
    }

    EnsureCatalystChargeListenerBound_Server();

    static FGameplayTag TAG_State_Catalyst_Active = FGameplayTag::RequestGameplayTag(FName("State.Catalyst.Active"), false);
    if (AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Catalyst_Active))
    {
        return;
    }

    if (!CatalystProcAbilityClass)
    {
        UE_LOG(LogSpellRiseCatalystRuntime, Warning, TEXT("[CatalystComponent] CatalystProcAbilityClass not set."));
        return;
    }

    // Check if we have enough charge
    const float CurrentCharge = AbilitySystemComponent->GetNumericAttribute(UCatalystAttributeSet::GetCatalystChargeAttribute());
    if (CurrentCharge < 100.f)
    {
        return;
    }

    // Find the spec for the proc ability on the ability system component
    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(CatalystProcAbilityClass);
    if (!Spec)
    {
        UE_LOG(LogSpellRiseCatalystRuntime, Warning, TEXT("[CatalystComponent] Proc ability not granted on ASC."));
        return;
    }

    ++GCatalystRuntimeCounters.ProcAttempts;
    const bool bAllowRemoteActivation = true;
    const bool bActivated = AbilitySystemComponent->TryActivateAbility(Spec->Handle, bAllowRemoteActivation);
    if (bActivated)
    {
        ++GCatalystRuntimeCounters.ProcActivated;
    }
    UE_LOG(LogSpellRiseCatalystRuntime, Verbose, TEXT("[CATALYST][PROC] Owner=%s Activated=%d Attempts=%lld ActivatedCount=%lld Charge=%.2f"),
        *GetNameSafe(GetOwner()),
        bActivated ? 1 : 0,
        GCatalystRuntimeCounters.ProcAttempts,
        GCatalystRuntimeCounters.ProcActivated,
        CurrentCharge);

    if (bActivated && OwnerCharacter)
    {
        int32 Tier = 1;
        if (const UCatalystAttributeSet* CatAS = OwnerCharacter->GetCatalystAttributeSet())
        {
            Tier = FMath::Clamp(FMath::RoundToInt(CatAS->GetCatalystLevel()), 1, 3);
        }
        OwnerCharacter->MultiOnCatalystProc(Tier);
    }
}
