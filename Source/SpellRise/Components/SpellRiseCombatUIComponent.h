#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "SpellRiseCombatUIComponent.generated.h"

class UAbilitySystemComponent;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseCombatUIComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USpellRiseCombatUIComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="SpellRise|UI")
    void ServerShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

protected:
    UFUNCTION(NetMulticast, Unreliable)
    void Multi_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

    UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI")
    void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

    UFUNCTION(Client, Reliable)
    void Client_UpdateResourceBars(
        float NewHealth, float MaxHealth,
        float NewMana, float MaxMana,
        float NewStamina, float MaxStamina
    );

    UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI")
    void BP_UpdateResourceBars(
        float NewHealth, float MaxHealth,
        float NewMana, float MaxMana,
        float NewStamina, float MaxStamina
    );

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CachedASC = nullptr;

    void BindAttributeChangeDelegates();


    void OnHealthChanged(const FOnAttributeChangeData& Data);
    void OnManaChanged(const FOnAttributeChangeData& Data);
    void OnStaminaChanged(const FOnAttributeChangeData& Data);
};
