#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "SpellRiseWeaponComponent.generated.h"

class UDA_WeaponDefinition;
class ASpellRiseWeaponActor;
class USkeletalMeshComponent;
class AActor;
class UGameplayEffect;

/**
 * Server-authoritative weapon equip component.
 * Replicates the equipped weapon definition (DataAsset).
 * Spawns/attaches a WeaponActor for visuals on both server and clients.
 *
 * AAA Damage Tagging:
 * - NEW (preferred):
 *   - Data.BaseWeaponDamage  (weapon/melee/ranged physical)
 *   - Data.DamageScaling     (generic multiplier)
 * - LEGACY (fallback support):
 *   - Data.BaseDamage
 *   - Data.DamageMultiplier
 *
 * We set BOTH where possible to keep older assets working during migration.
 */
UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseWeaponComponent();

	/** Performs a melee trace using the currently equipped weapon tuning (LightAttack). Pure trace only. */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool PerformLightAttackTrace(
		AActor* InstigatorActor,
		ECollisionChannel TraceChannel,
		bool bDebug,
		FHitResult& OutHit
	) const;

	/**
	 * Server-authoritative: Trace + Apply damage via GAS.
	 * Safe to call on client (routes to server).
	 *
	 * IMPORTANT:
	 * - InstigatorActor should be the attacking Character (Avatar).
	 * - If a PlayerState/Controller is passed, we will resolve it to the Pawn.
	 * - If null, we will use the component Owner as instigator.
	 *
	 * @param BaseDamage      Base damage to set (weapon base)
	 * @param DamageScaling   Multiplier (combo/buffs). 1.0 = normal.
	 */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	bool PerformLightAttack_TraceAndApplyDamage(
		AActor* InstigatorActor,
		TSubclassOf<UGameplayEffect> DamageGE,
		float BaseDamage,
		float DamageScaling,
		ECollisionChannel TraceChannel,
		bool bDebug
	);

	/** Equip weapon (safe to call on client; will route to server). */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	void EquipWeapon(UDA_WeaponDefinition* NewWeapon);

	/** Unequip weapon (safe to call on client; will route to server). */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	void UnequipWeapon();

	/** Returns the equipped weapon definition (may be null). */
	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	UDA_WeaponDefinition* GetEquippedWeapon() const { return EquippedWeapon; }

	/** Returns the currently spawned weapon actor if it derives from ASpellRiseWeaponActor (may be null). */
	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	ASpellRiseWeaponActor* GetEquippedWeaponActor() const { return EquippedWeaponActor; }

	/** Called by owning character on death (visual cleanup). */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Weapon")
	void HandleOwnerDeath();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Replicated: data definition of equipped weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_EquippedWeapon, Category="SpellRise|Weapon")
	TObjectPtr<UDA_WeaponDefinition> EquippedWeapon = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AActor> EquippedWeaponVisualActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ASpellRiseWeaponActor> EquippedWeaponActor = nullptr;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// ---------------------------
	// RPCs
	// ---------------------------

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(UDA_WeaponDefinition* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerUnequipWeapon();

	/** IMPORTANT: RPC must use ECollisionChannel (not TEnumAsByte) */
	UFUNCTION(Server, Reliable)
	void ServerPerformLightAttack_TraceAndApplyDamage(
		AActor* InstigatorActor,
		TSubclassOf<UGameplayEffect> DamageGE,
		float BaseDamage,
		float DamageScaling,
		ECollisionChannel TraceChannel,
		bool bDebug
	);

	// ---------------------------
	// Helpers
	// ---------------------------
	void DestroyEquippedWeaponActor();
	void SpawnAndAttachWeaponActor();
	USkeletalMeshComponent* GetOwnerSkeletalMesh() const;

	/** If a PlayerState/Controller is passed, resolves to its Pawn. Falls back to component Owner. */
	AActor* ResolveInstigatorActor(AActor* InInstigator) const;

	/**
	 * Applies DamageGE to Target via GAS using SetByCaller.
	 * Server-only.
	 *
	 * We set NEW AAA tags and LEGACY tags (during migration).
	 */
	bool ApplyDamageToTarget_Server(
		AActor* InstigatorActor,
		AActor* TargetActor,
		TSubclassOf<UGameplayEffect> DamageGE,
		float BaseDamage,
		float DamageScaling
	) const;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon")
	bool bRebuildVisualOnServer = true;

	// ---------------------------
	// Damage Tags (AAA + Legacy)
	// ---------------------------

	/** NEW (AAA): Base weapon damage tag used by ExecCalc_Damage. */
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Damage")
	FGameplayTag SetByCallerBaseWeaponDamageTag;

	/** NEW (AAA): Generic scaling multiplier tag (combo/buffs). */
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Damage")
	FGameplayTag SetByCallerDamageScalingTag;

	/** LEGACY: Old base damage tag kept for backward compatibility with older GEs. */
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Damage")
	FGameplayTag SetByCallerBaseDamageTag_Legacy;

	/** LEGACY: Old multiplier tag kept for backward compatibility with older GEs. */
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Weapon|Damage")
	FGameplayTag SetByCallerDamageMultiplierTag_Legacy;
};
