#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRiseWeaponComponent.generated.h"

class ASpellRiseWeaponActor;
class USkeletalMeshComponent;
class AActor;
class USpellRiseMeleeComponent;
class UDA_MeleeWeaponData;

UCLASS(ClassGroup = (SpellRise), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USpellRiseWeaponComponent();

    // ============================================
    // EQUIPAMENTO DE ARMA – CONST-CORRETO
    // ============================================
    UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
    void EquipWeapon(const UDA_MeleeWeaponData* NewWeapon);

    UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
    void UnequipWeapon();

    UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
    const UDA_MeleeWeaponData* GetEquippedWeapon() const { return EquippedWeapon; }

    UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
    ASpellRiseWeaponActor* GetEquippedWeaponActor() const { return EquippedWeaponActor; }

    UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
    void HandleOwnerDeath();

    // ============================================
    // SISTEMA MELEE
    // ============================================
    UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon|Melee")
    USpellRiseMeleeComponent* GetMeleeComponent() const { return MeleeComponent; }

    UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon|Melee")
    void InitializeMeleeComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ============================================
    // REPLICADO – CONST CORRETO
    // ============================================
    UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
    TObjectPtr<const UDA_MeleeWeaponData> EquippedWeapon = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<AActor> EquippedWeaponVisualActor = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ASpellRiseWeaponActor> EquippedWeaponActor = nullptr;

    // ============================================
    // COMPONENTES
    // ============================================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|Weapon|Melee")
    TObjectPtr<USpellRiseMeleeComponent> MeleeComponent;

    // ============================================
    // CONTROLE DE INICIALIZAÇÃO
    // ============================================
    UPROPERTY(Transient)
    bool bMeleeComponentInitialized = false;

    UFUNCTION()
    void OnRep_EquippedWeapon();

    // ============================================
    // RPCs – ASSINATURAS CORRETAS (CONST)
    // ============================================
    UFUNCTION(Server, Reliable)
    void ServerEquipWeapon(const UDA_MeleeWeaponData* NewWeapon);

    UFUNCTION(Server, Reliable)
    void ServerUnequipWeapon();

    // ============================================
    // HELPERS
    // ============================================
    void DestroyEquippedWeaponActor();
    void SpawnAndAttachWeaponActor();
    USkeletalMeshComponent* GetOwnerSkeletalMesh() const;

    UPROPERTY(EditDefaultsOnly, Category = "SpellRise|Weapon")
    bool bRebuildVisualOnServer = true;
};