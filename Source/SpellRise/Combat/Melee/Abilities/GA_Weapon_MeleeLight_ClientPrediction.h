#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"

// FORWARD DECLARATIONS - ADICIONADOS!
class UDA_WeaponDefinition;
class UAnimMontage;
class UGameplayEffect;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

#include "GA_Weapon_MeleeLight_ClientPrediction.generated.h"

USTRUCT(BlueprintType)
struct FCombatHitResult
{
    GENERATED_BODY()

    UPROPERTY()
    AActor* TargetActor = nullptr;

    UPROPERTY()
    float DamageAmount = 0.0f;

    UPROPERTY()
    bool bWasCrit = false;

    UPROPERTY()
    FVector_NetQuantize HitLocation = FVector_NetQuantize::ZeroVector;

    UPROPERTY()
    uint8 HitIndex = 0;
};

UCLASS()
class SPELLRISE_API UGA_Weapon_MeleeLight_ClientPrediction : public USpellRiseGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Weapon_MeleeLight_ClientPrediction();

    // ============================================
    // GAS Overrides
    // ============================================
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    virtual void InputPressed(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
    // ============================================
    // CONFIGURAÇÃO
    // ============================================
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Damage")
    TSubclassOf<UGameplayEffect> DamageGE;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Animation")
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Combo")
    TArray<FName> ComboSections = { FName("Hit1"), FName("Hit2"), FName("Hit3") };

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Combo")
    int32 MaxComboHits = 3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Combo")
    FGameplayTag ComboWindowBeginTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Combo")
    FGameplayTag ComboWindowEndTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Trace")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Animation")
    float MontagePlayRate = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Animation")
    float StopBlendOutTime = 0.15f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Damage")
    float BaseDamage = 25.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Debug")
    bool bDebug = false;

    // ============================================
    // RPCs
    // ============================================
    
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ReportHitResult(const FCombatHitResult& HitResult);

    UFUNCTION(Client, Reliable)
    void Client_ConfirmDamage(const FCombatHitResult& HitResult);

private:
    // ============================================
    // ESTADO
    // ============================================
    
    int32 CurrentComboIndex = 0;
    bool bIsAcceptingComboInput = false;
    bool bWasInputReceivedThisWindow = false;
    bool bHasAppliedDamageThisWindow = false;

    // Cache - CORRIGIDO: Usando raw pointer em vez de TObjectPtr
    UPROPERTY(Transient)
    UDA_WeaponDefinition* CachedWeaponData = nullptr;

    UPROPERTY(Transient)
    UAnimMontage* CachedMontage = nullptr;

    // Tasks
    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask = nullptr;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitBeginTask = nullptr;

    UPROPERTY()
    TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEndTask = nullptr;

    // ============================================
    // MÉTODOS PRIVADOS
    // ============================================
    
    void CacheWeaponData();
    
    float GetDamageForCurrentHit() const;
    float GetComboScalingForCurrentHit() const;
    float GetAttackSpeedMultiplier() const;
    
    FCombatHitResult PerformClientTraceAndPredictDamage();
    void ApplyClientVisualDamage(const FCombatHitResult& HitResult);
    
    bool ValidateHitResult(const FCombatHitResult& HitResult) const;
    void ApplyServerDamage(const FCombatHitResult& HitResult);
    
    void StartComboAt(int32 Index);
    void CancelCombo();
    void StopCurrentMontage();
    void KillAllTasks();
    void EndCombo(bool bWasCancelled);
    
    void BindComboWindowBegin();
    void BindComboWindowEnd();
    
    UAnimInstance* GetAvatarAnimInstance() const;
    AActor* GetAvatarActor() const;
    UAbilitySystemComponent* GetASC() const;
    UWorld* GetGameWorld() const;
    bool IsServerAuthority() const;

    // ============================================
    // HANDLERS
    // ============================================
    
    UFUNCTION()
    void HandleComboWindowBegin(FGameplayEventData Payload);

    UFUNCTION()
    void HandleComboWindowEnd(FGameplayEventData Payload);

    UFUNCTION()
    void HandleMontageCompleted();

    UFUNCTION()
    void HandleMontageInterrupted();

    UFUNCTION()
    void HandleMontageCancelled();
};