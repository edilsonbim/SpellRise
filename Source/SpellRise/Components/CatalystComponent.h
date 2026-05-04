#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "CatalystComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class USpellRiseAbilitySystemComponent;
class UCatalystAttributeSet;
class ASpellRiseCharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API UCatalystComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCatalystComponent();

protected:
    virtual void BeginPlay() override;

public:


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
    TSubclassOf<UGameplayEffect> GE_CatalystAddCharge;



    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
    TSubclassOf<UGameplayAbility> CatalystProcAbilityClass;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Catalyst")
    bool bCatalystEnabled = true;



    UFUNCTION(BlueprintCallable, Category="Catalyst")
    bool TryAddCatalystCharge_OnValidHit(AActor* TargetActor);



    UFUNCTION(BlueprintCallable, Category="Catalyst")
    bool TryAddCatalystCharge_OnDamageTaken(AActor* InstigatorActor);



    UFUNCTION(BlueprintCallable, Category="Catalyst")
    void TryProcCatalystIfReady();

private:
    bool ResolveAbilitySystemComponent();
    bool HasServerAuthority() const;

    void EnsureCatalystChargeListenerBound_Server();

    void BindCatalystChargeListener_Server();

    void OnCatalystChargeChanged(const struct FOnAttributeChangeData& Data);


    bool bCatalystListenerBound = false;


    UPROPERTY(Transient)
    TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent;


    UPROPERTY(Transient)
    TObjectPtr<ASpellRiseCharacterBase> OwnerCharacter;

    UPROPERTY(Transient)
    TObjectPtr<AActor> OwnerAbilityActor;
};
