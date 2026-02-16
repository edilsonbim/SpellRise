// ============================================================================
// ASpellRiseMagicProjectileBase.h
// Path: Source/SpellRise/Combat/Spells/Projectiles/ASpellRiseMagicProjectileBase.h
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ASpellRiseMagicProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

/**
 * Server-authoritative projectile:
 * - Server validates hit, applies damage GE.
 * - Server fires a GameplayCue (ImpactCueTag) with GameplayCueParameters (location/normal/hit/etc).
 * - Clients render VFX/SFX through GameplayCue system (GCN/GCE); dedicated server does nothing visual.
 */
UCLASS(BlueprintType, Blueprintable)
class SPELLRISE_API ASpellRiseMagicProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	ASpellRiseMagicProjectileBase();

	/**
	 * Server-only init called right after spawn.
	 * ImpactCueTag is used to spawn impact FX/SFX on clients via GameplayCue (GCN/GCE).
	 *
	 * @param InSourceActor        Instigator/owner source (ideally has ASC)
	 * @param InDamageEffectClass  GameplayEffect applied on server
	 * @param InBaseSpellDamage    SetByCaller magnitude
	 * @param InDynamicAssetTags   Tags appended to Spec as DynamicAssetTags
	 * @param InImpactCueTag       GameplayCue tag for impact VFX/SFX (e.g., GameplayCue.Spell.Impact)
	 * @param InSourceObject       Optional: DA_StaffWeaponData (or other) passed into cue params SourceObject
	 */
	void InitProjectile(
		AActor* InSourceActor,
		TSubclassOf<UGameplayEffect> InDamageEffectClass,
		float InBaseSpellDamage,
		const FGameplayTagContainer& InDynamicAssetTags,
		FGameplayTag InImpactCueTag,
		UObject* InSourceObject = nullptr
	);

protected:
	virtual void BeginPlay() override;

	// --------------------------------------------
	// Collision callbacks (server-authoritative)
	// --------------------------------------------
	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	void HandleImpact_Server(AActor* OtherActor, const FHitResult& Hit);
	void ApplySpellDamage_Server(AActor* TargetActor, const FHitResult& Hit);

	// Called only on server after impact is confirmed
	void FinishImpact_Server(AActor* TargetActor, const FHitResult& Hit);

	// ------------------------------------------------
	// Validation
	// ------------------------------------------------
	bool IsValidTarget_Server(AActor* OtherActor) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	TObjectPtr<USphereComponent> Collision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement = nullptr;

	// --------------------------------------------
	// Config (tunable per BP child)
	// --------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Config", meta=(ClampMin="1.0"))
	float CollisionRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Config", meta=(ClampMin="0.0"))
	float InitialSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Config", meta=(ClampMin="0.0"))
	float MaxSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Config", meta=(ClampMin="0.0"))
	float LifeSeconds = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Config")
	bool bDestroyOnImpact = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Config")
	bool bIgnoreOwner = true;

	// Optional debug
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Debug")
	bool bDebugCollision = false;

private:
	// --------------------------------------------
	// Runtime (server authoritative)
	// --------------------------------------------
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> SourceActor;

	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(Transient)
	float BaseSpellDamage = 0.f;

	UPROPERTY(Transient)
	FGameplayTagContainer DynamicAssetTags;

	UPROPERTY(Transient)
	FGameplayTag ImpactCueTag;

	UPROPERTY(Transient)
	TWeakObjectPtr<UObject> SourceObject;

	UPROPERTY(Transient)
	bool bImpacted = false;

	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AActor>> HitActors;
};
