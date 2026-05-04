// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Components/CatalystComponent.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
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


    OwnerCharacter = Cast<ASpellRiseCharacterBase>(GetOwner());
    ResolveAbilitySystemComponent();


    if (bCatalystEnabled && AbilitySystemComponent && AbilitySystemComponent->IsOwnerActorAuthoritative())
    {
        EnsureCatalystChargeListenerBound_Server();
    }
}

bool UCatalystComponent::ResolveAbilitySystemComponent()
{
    AActor* OwnerActor = GetOwner();
    OwnerAbilityActor = OwnerActor;
    OwnerCharacter = Cast<ASpellRiseCharacterBase>(OwnerActor);

    UAbilitySystemComponent* GenericASC = nullptr;
    if (IAbilitySystemInterface* AbilityOwner = Cast<IAbilitySystemInterface>(OwnerActor))
    {
        GenericASC = AbilityOwner->GetAbilitySystemComponent();
    }

    USpellRiseAbilitySystemComponent* ResolvedASC = Cast<USpellRiseAbilitySystemComponent>(GenericASC);

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


    if (!HasServerAuthority())
    {
        return false;
    }

    EnsureCatalystChargeListenerBound_Server();

    AActor* SourceActor = AbilitySystemComponent->GetAvatarActor();
    if (!SourceActor)
    {
        SourceActor = OwnerAbilityActor ? OwnerAbilityActor.Get() : GetOwner();
    }

    if (!SourceActor || !TargetActor || TargetActor == SourceActor)
    {
        return false;
    }


    static FGameplayTag TAG_State_Dead = FGameplayTag::RequestGameplayTag(FName("State.Dead"), false);
    static FGameplayTag TAG_State_Catalyst_Active = FGameplayTag::RequestGameplayTag(FName("State.Catalyst.Active"), false);
    if (AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Dead) || AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Catalyst_Active))
    {
        return false;
    }

    if (!GE_CatalystAddCharge)
    {
        return false;
    }


    FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
    Ctx.AddSourceObject(SourceActor);


    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
    {
        return false;
    }

    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    ++GCatalystRuntimeCounters.ChargeOnHitApplied;
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


    if (!HasServerAuthority())
    {
        return false;
    }

    EnsureCatalystChargeListenerBound_Server();

    AActor* VictimActor = AbilitySystemComponent->GetAvatarActor();
    if (!VictimActor)
    {
        VictimActor = OwnerAbilityActor ? OwnerAbilityActor.Get() : GetOwner();
    }

    if (!VictimActor)
    {
        return false;
    }


    static FGameplayTag TAG_State_Dead = FGameplayTag::RequestGameplayTag(FName("State.Dead"), false);
    static FGameplayTag TAG_State_Catalyst_Active = FGameplayTag::RequestGameplayTag(FName("State.Catalyst.Active"), false);
    if (AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Dead) || AbilitySystemComponent->HasMatchingGameplayTag(TAG_State_Catalyst_Active))
    {
        return false;
    }

    if (!GE_CatalystAddCharge)
    {
        return false;
    }


    FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
    Ctx.AddSourceObject(InstigatorActor ? InstigatorActor : VictimActor);


    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
    if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
    {
        return false;
    }

    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    ++GCatalystRuntimeCounters.ChargeOnDamageTakenApplied;
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
        return;
    }


    const float CurrentCharge = AbilitySystemComponent->GetNumericAttribute(UCatalystAttributeSet::GetCatalystChargeAttribute());
    if (CurrentCharge < 100.f)
    {
        return;
    }


    FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(CatalystProcAbilityClass);
    if (!Spec)
    {
        return;
    }

    ++GCatalystRuntimeCounters.ProcAttempts;
    const bool bAllowRemoteActivation = true;
    const bool bActivated = AbilitySystemComponent->TryActivateAbility(Spec->Handle, bAllowRemoteActivation);
    if (bActivated)
    {
        ++GCatalystRuntimeCounters.ProcActivated;
    }
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
