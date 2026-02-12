#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_MeleeWeaponData.generated.h"

class UAnimMontage;
class USoundBase;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FMeleeTraceConfig
{
    GENERATED_BODY()

    // ============================================
    // SWEEP CONTINUO (Anti-Teleport)
    // ============================================
    
    /** Raio do sweep sphere */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "1.0"))
    float TraceRadius = 12.0f;

    /** Número máximo de steps interpolados entre frames (0 = automático baseado na velocidade) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "0", ClampMax = "10"))
    int32 MaxTraceSteps = 3;

    /** Distância mínima entre steps para forçar trace adicional (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (ClampMin = "10.0"))
    float MinStepDistance = 30.0f;

    /** Socket base da arma */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
    FName SocketBase = "Melee_Base";

    /** Socket tip da arma */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
    FName SocketTip = "Melee_Tip";

    /** Canal de colisão */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

    // ============================================
    // BAKED TRACING (Low-FPS Fallback)
    // ============================================
    
    /** Se deve pré-calcular posições de trace para clientes fracos */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace|Baked")
    bool bEnableBakedTracing = true;

    /** Número de posições pré-calculadas por animação */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace|Baked", meta = (ClampMin = "5", ClampMax = "30"))
    int32 BakedTracePoints = 15;
};

USTRUCT(BlueprintType)
struct FMeleeDamageConfig
{
    GENERATED_BODY()

    // ============================================
    // DANO BASE
    // ============================================
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
    float BaseDamage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
    float DamageScalar = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    TArray<float> ComboScaling = { 1.00f, 1.10f, 1.25f };

    // ============================================
    // COOLDOWN POR ALVO (Anti-Double-Hit)
    // ============================================
    
    /** Tempo mínimo entre hits no mesmo alvo (segundos) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage|Cooldown", meta = (ClampMin = "0.1"))
    float TargetCooldown = 0.5f;

    /** Se deve resetar cooldown por alvo em cada nova janela de combo */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage|Cooldown")
    bool bResetCooldownPerCombo = true;

    // ============================================
    // TAGS
    // ============================================
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage|Tags")
    FGameplayTag DamageTypeTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage|Tags")
    FGameplayTagContainer HitReactTags;
};

USTRUCT(BlueprintType)
struct FMeleeComboConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    TArray<FName> ComboSections = { "Hit1", "Hit2", "Hit3" };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo", meta = (ClampMin = "1", ClampMax = "5"))
    int32 MaxComboHits = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    float StopBlendOutTime = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    FGameplayTag ComboWindowBeginTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
    FGameplayTag ComboWindowEndTag;
};

USTRUCT(BlueprintType)
struct FMeleeFeedbackConfig
{
    GENERATED_BODY()

    // ============================================
    // TELLS VISUAIS (Glint)
    // ============================================
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback|Visual")
    TObjectPtr<UNiagaraSystem> HitGlintEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback|Visual")
    TObjectPtr<UNiagaraSystem> BlockGlintEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback|Visual")
    TObjectPtr<UNiagaraSystem> BloodEffect = nullptr;

    // ============================================
    // SONS (Tells)
    // ============================================
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback|Audio")
    TObjectPtr<USoundBase> WhooshSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback|Audio")
    TObjectPtr<USoundBase> HitSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback|Audio")
    TObjectPtr<USoundBase> BlockSound = nullptr;
};

UCLASS(BlueprintType, Const)
class SPELLRISE_API UDA_MeleeWeaponData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    FName WeaponId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Config")
    FMeleeTraceConfig TraceConfig;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Config")
    FMeleeDamageConfig DamageConfig;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Config")
    FMeleeComboConfig ComboConfig;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Config")
    FMeleeFeedbackConfig FeedbackConfig;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Visual")
    TSubclassOf<AActor> WeaponActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Visual")
    FName AttachSocketName = "Socket_Weapon_R";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Visual")
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    FORCEINLINE float GetFinalBaseDamage() const
    {
        return DamageConfig.BaseDamage * DamageConfig.DamageScalar;
    }
};