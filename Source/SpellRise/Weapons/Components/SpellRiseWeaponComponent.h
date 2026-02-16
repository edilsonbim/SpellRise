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
class UAbilitySystemComponent;
class UGameplayAbility;
class UStaticMeshComponent;

// Weapon DAs
class UPrimaryDataAsset;
class UDA_MeleeWeaponData;
class UDA_StaffWeaponData;

UCLASS(ClassGroup = (SpellRise), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseWeaponComponent();

	// ============================================
	// EQUIPAMENTO DE ARMA (GENÉRICO)
	// ============================================
	UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
	void EquipWeapon(const UPrimaryDataAsset* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
	void UnequipWeapon();

	// Wrappers tipados (Blueprint-friendly)
	UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
	void EquipMeleeWeapon(const UDA_MeleeWeaponData* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
	void EquipStaffWeapon(const UDA_StaffWeaponData* NewWeapon);

	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	const UPrimaryDataAsset* GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	const UDA_MeleeWeaponData* GetEquippedMeleeWeapon() const;

	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	const UDA_StaffWeaponData* GetEquippedStaffWeapon() const;

	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	ASpellRiseWeaponActor* GetEquippedWeaponActor() const { return EquippedWeaponActor; }

	// For spell/projectile systems: get the StaticMesh used by the equipped weapon actor (if any).
	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	UStaticMeshComponent* GetEquippedWeaponMesh() const;

	// Get a socket transform on the weapon mesh (used by staff muzzle, etc).
	// Returns false if weapon/mesh/socket invalid.
	UFUNCTION(BlueprintCallable, Category = "SpellRise|Weapon")
	bool GetWeaponSocketTransform(const FName SocketName, FTransform& OutTransform) const;

	// Default staff muzzle socket name (visual-only WeaponActor mesh).
	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	FName GetDefaultSpellMuzzleSocketName() const { return FName(TEXT("Spell_Muzzle")); }

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
	TObjectPtr<const UPrimaryDataAsset> EquippedWeapon = nullptr;

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
	void ServerEquipWeapon(const UPrimaryDataAsset* NewWeapon);

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

	// Extract common fields from either melee or staff DA
	bool GetWeaponVisualData(
		TSubclassOf<AActor>& OutWeaponActorClass,
		FName& OutAttachSocketName,
		FTransform& OutAttachOffset
	) const;

	bool GetWeaponGrantedAbilities(TArray<TSubclassOf<UGameplayAbility>>& OutAbilities) const;

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
