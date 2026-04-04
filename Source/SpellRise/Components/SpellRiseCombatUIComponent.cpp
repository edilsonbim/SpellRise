// SpellRiseCombatUIComponent.cpp
#include "SpellRiseCombatUIComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Pawn.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

USpellRiseCombatUIComponent::USpellRiseCombatUIComponent()
{
    SetIsReplicatedByDefault(false);
}

void USpellRiseCombatUIComponent::BeginPlay()
{
    Super::BeginPlay();

    // Tenta obter o AbilitySystemComponent do ator proprietário
    AActor* Owner = GetOwner();
    if (Owner)
    {
        CachedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
        if (CachedASC)
        {
            BindAttributeChangeDelegates();
        }
    }
}

void USpellRiseCombatUIComponent::BindAttributeChangeDelegates()
{
    using AttrSet = UResourceAttributeSet;
    // Assina delegates para Health, Mana e Stamina
    CachedASC->GetGameplayAttributeValueChangeDelegate(AttrSet::GetHealthAttribute())
        .AddUObject(this, &USpellRiseCombatUIComponent::OnHealthChanged);
    CachedASC->GetGameplayAttributeValueChangeDelegate(AttrSet::GetManaAttribute())
        .AddUObject(this, &USpellRiseCombatUIComponent::OnManaChanged);
    CachedASC->GetGameplayAttributeValueChangeDelegate(AttrSet::GetStaminaAttribute())
        .AddUObject(this, &USpellRiseCombatUIComponent::OnStaminaChanged);
}

void USpellRiseCombatUIComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (!CachedASC) { return; }

    float NewHealth   = Data.NewValue;
    float MaxHealth   = CachedASC->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
    float NewMana     = CachedASC->GetNumericAttribute(UResourceAttributeSet::GetManaAttribute());
    float MaxMana     = CachedASC->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute());
    float NewStamina  = CachedASC->GetNumericAttribute(UResourceAttributeSet::GetStaminaAttribute());
    float MaxStamina  = CachedASC->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());

    const AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    if (OwnerActor->HasAuthority())
    {
        Client_UpdateResourceBars(NewHealth, MaxHealth, NewMana, MaxMana, NewStamina, MaxStamina);
        return;
    }

    const APawn* OwnerPawn = Cast<APawn>(OwnerActor);
    if (OwnerPawn && !OwnerPawn->IsLocallyControlled())
    {
        return;
    }

    BP_UpdateResourceBars(NewHealth, MaxHealth, NewMana, MaxMana, NewStamina, MaxStamina);
}

void USpellRiseCombatUIComponent::OnManaChanged(const FOnAttributeChangeData& Data)
{
    // Reutiliza a lógica de atualização
    OnHealthChanged(Data);
}

void USpellRiseCombatUIComponent::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
    OnHealthChanged(Data);
}

void USpellRiseCombatUIComponent::ServerShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        Multi_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);
    }
}

void USpellRiseCombatUIComponent::Multi_ShowDamagePop_Implementation(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
    BP_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);
}

void USpellRiseCombatUIComponent::Client_UpdateResourceBars_Implementation(
    float NewHealth,  float MaxHealth,
    float NewMana,    float MaxMana,
    float NewStamina, float MaxStamina)
{
    if (GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    BP_UpdateResourceBars(NewHealth, MaxHealth, NewMana, MaxMana, NewStamina, MaxStamina);
}
