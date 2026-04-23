#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.h"
#include "SpellRiseGameplayAbility_AuthoritativeProjectile.generated.h"

class ASR_ProjectileBase;
class UGameplayEffect;

UCLASS(Blueprintable, Abstract)
class SPELLRISE_API USpellRiseGameplayAbility_AuthoritativeProjectile : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility_AuthoritativeProjectile();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability|Projectile")
	bool SR_ConfirmProjectileTargetAndSpawn(const FVector& TargetLocation);

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|Projectile")
	bool IsAwaitingProjectileTargetConfirmation() const
	{
		return bAwaitingProjectileTargetConfirmation;
	}

protected:
	virtual void NativeOnSpellExecuted() override;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnProjectileExecutionStarted")
	void K2_OnProjectileExecutionStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnProjectileSpawned")
	void K2_OnProjectileSpawned();

protected:
	bool SpawnAuthoritativeProjectile(const FVector& TargetLocation);
	FGameplayEffectSpecHandle BuildProjectileEffectSpec() const;
	FTransform ResolveProjectileSpawnTransform() const;
	FRotator ResolveProjectileSpawnRotation(const FVector& SpawnLocation, const FVector& TargetLocation) const;
	void ResetProjectileRuntimeState();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile")
	TSubclassOf<ASR_ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile", meta=(ClampMin="0.0"))
	float ProjectileSpeed = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile", meta=(ClampMin="0.0"))
	float DamageMagnitude = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile", meta=(Categories="Damage"))
	FGameplayTagContainer DamageTypeTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile")
	FName ProjectileSpawnSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile", meta=(ClampMin="0.0"))
	float MaxProjectileRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Projectile")
	bool bRequireServerAuthorityForSpawn = true;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|Projectile")
	bool bAwaitingProjectileTargetConfirmation = false;
};
