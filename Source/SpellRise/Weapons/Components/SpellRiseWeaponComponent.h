// ============================================================================
// SpellRiseWeaponComponent.h (COMPLETO - COM GRANT/CLEAR ABILITIES)
// Path: Source/SpellRise/Weapons/Components/SpellRiseWeaponComponent.h
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "SpellRiseWeaponComponent.generated.h"

class ASpellRiseWeaponActor;
class USkeletalMeshComponent;
class USpellRiseMeleeComponent;
class UDA_MeleeWeaponData;
class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS(ClassGroup = (SpellRise), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseWeaponComponent();

	// ============================================
	// EQUIPAMENTO DE ARMA
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
	// REPLICAÇÃO
	// ============================================
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<const UDA_MeleeWeaponData> EquippedWeapon = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AActor> EquippedWeaponVisualActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ASpellRiseWeaponActor> EquippedWeaponActor = nullptr;

	// ============================================
	// COMPONENTES (referência ao componente do Owner)
	// ============================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|Weapon|Melee")
	TObjectPtr<USpellRiseMeleeComponent> MeleeComponent = nullptr;

	// ============================================
	// CONTROLE DE INICIALIZAÇÃO
	// ============================================
	UPROPERTY(Transient)
	bool bMeleeComponentInitialized = false;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// ============================================
	// RPCs
	// ============================================
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(const UDA_MeleeWeaponData* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerUnequipWeapon();

private:
	// ============================================
	// HELPERS
	// ============================================
	void DestroyEquippedWeaponActor();
	void SpawnAndAttachWeaponActor();
	USkeletalMeshComponent* GetOwnerSkeletalMesh() const;

	void ResolveMeleeComponentOnOwner();

	UPROPERTY(EditDefaultsOnly, Category = "SpellRise|Weapon")
	bool bRebuildVisualOnServer = true;

	// ============================================
	// GAS: Grant/Clear abilities (SERVER)
	// ============================================
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Abilities", meta=(ClampMin="0"))
	int32 WeaponPrimaryAttackInputID = 9; // must match BP "Press Input ID"

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	UAbilitySystemComponent* GetOwnerASC() const;

	void GrantWeaponAbilities_Server();
	void ClearWeaponAbilities_Server();
};
