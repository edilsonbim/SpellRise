#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "Engine/Engine.h"

// Forward declarations
class UDA_MeleeWeaponData;
class USpellRiseContinuousTrace;
class USpellRiseHitHistoryComponent;
class UGameplayEffect;
class UAbilitySystemComponent;

#include "SpellRiseMeleeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHit, const FHitResult&, Hit);

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseMeleeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USpellRiseMeleeComponent();

    // ============================================
    // INICIALIZAÇÃO
    // ============================================
    UFUNCTION(BlueprintCallable, Category = "Melee")
    void InitializeWithWeaponData(const UDA_MeleeWeaponData* InWeaponData, USceneComponent* InWeaponMesh);

    UFUNCTION(BlueprintCallable, Category = "Melee")
    void BeginHitWindow(int32 ComboIndex);

    UFUNCTION(BlueprintCallable, Category = "Melee")
    void EndHitWindow();

    // ============================================
    // DANO - PROPRIEDADE EXPOSTA!
    // ============================================
    
    /** Gameplay Effect de dano - configure no Blueprint! */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee|Damage", meta = (
        DisplayName = "Damage Effect Class",
        ToolTip = "Gameplay Effect que aplica dano ao alvo"
    ))
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    // ============================================
    // DEBUG - PROPRIEDADES EXPOSTAS!
    // ============================================
    
    /** Ativar logs de debug */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee|Debug")
    bool bDebug = false;

    /** Ativar/desativar debug visual do trace */
    UFUNCTION(BlueprintCallable, Category = "Melee|Debug")
    void SetDebugVisual(bool bEnable);
    
    // ============================================
    // FEEDBACK
    // ============================================
    UFUNCTION(BlueprintCallable, Category = "Melee|Feedback")
    void PlayHitFeedback(const FHitResult& Hit);

    UFUNCTION(BlueprintCallable, Category = "Melee|Feedback")
    void PlayWhooshFeedback();

    // ============================================
    // EVENTOS
    // ============================================
    UPROPERTY(BlueprintAssignable, Category = "Melee|Events")
    FOnWeaponHit OnWeaponHit;

protected:
    virtual void BeginPlay() override;

private:
    // ============================================
    // COMPONENTES
    // ============================================
    
    UPROPERTY()
    TObjectPtr<USpellRiseContinuousTrace> TraceComponent = nullptr;

    UPROPERTY()
    TObjectPtr<USpellRiseHitHistoryComponent> HistoryComponent = nullptr;

    // ============================================
    // DADOS DA ARMA
    // ============================================
    
    UPROPERTY(Transient)
    TObjectPtr<const UDA_MeleeWeaponData> WeaponData = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<USceneComponent> CachedWeaponMesh = nullptr;

    // ============================================
    // ESTADO DO COMPONENTE
    // ============================================
    
    int32 CurrentComboIndex = 0;
    bool bIsActive = false;
    bool bDidHitThisWindow = false;
    bool bDebugVisual = false;

    // ============================================
    // FUNÇÕES PRIVADAS
    // ============================================
    
    UFUNCTION()
    void OnTraceHitDetected(const FHitResult& Hit);
    
    void ApplyDamageToTarget(const FHitResult& Hit);
    
    bool ValidateHit(const FHitResult& Hit, AActor*& OutTargetActor, UAbilitySystemComponent*& OutTargetASC) const;
    float CalculateFinalDamage(const FHitResult& Hit) const;
    void ApplyGameplayEffectDamage(AActor* TargetActor, UAbilitySystemComponent* TargetASC, float Damage);
};