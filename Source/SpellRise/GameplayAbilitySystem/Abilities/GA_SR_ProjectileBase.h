#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "GA_SR_ProjectileBase.generated.h"

class UAbilityTask_WaitInputRelease;
class UAnimMontage;
class UGameplayEffect;
class UProjectileMovementComponent;
class AActor;
class APawn;

UCLASS()
class SPELLRISE_API UGA_SR_ProjectileBase : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SR_ProjectileBase();

protected:
	UPROPERTY(Transient)
	bool bHasCachedClientTargetData = false;

	UPROPERTY(Transient)
	FVector CachedClientTargetLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	FHitResult CachedClientAimHitResult;

	UFUNCTION()
	void OnReplicatedTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	UFUNCTION()
	void OnReplicatedTargetDataCancelled();

	void SendCurrentAimTargetDataToServer();
	void ConsumeClientTargetData();
	bool BuildClientTargetDataHandle(FGameplayAbilityTargetDataHandle& OutTargetDataHandle) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Animation")
	TObjectPtr<UAnimMontage> CastMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Animation")
	TObjectPtr<UAnimMontage> FireMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Animation", meta=(ClampMin="0.01"))
	float CastMontagePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Animation", meta=(ClampMin="0.01"))
	float FireMontagePlayRate = 1.0f;

	/* ---------------- CAST FLOW ---------------- */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Casting", meta=(ClampMin="0.0"))
	float CastTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Casting")
	bool bCancelIfReleasedEarly = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Casting")
	bool bCommitOnCastStart = false;

	/* ---------------- PROJECTILE ---------------- */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile", meta=(ClampMin="0.0"))
	float ProjectileSpeed = 2500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	FName ProjectileSpawnSocketName = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile", meta=(ClampMin="100.0"))
	float TargetTraceRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TEnumAsByte<ECollisionChannel> TargetTraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	bool bTraceComplex = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	bool bUseControllerViewForTargeting = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Security")
	bool bValidateReplicatedTargetDataOnServer = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Security", meta=(ClampMin="100.0"))
	float MaxReplicatedTargetDataDistance = 7000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Security", meta=(ClampMin="1.0", ClampMax="89.0"))
	float MaxReplicatedTargetDataConeHalfAngleDegrees = 55.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Security", meta=(ClampMin="0.0"))
	float MaxReplicatedTargetDataStartOffset = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandling =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	/* ---------------- DAMAGE ---------------- */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Damage")
	FGameplayTag DamageMagnitudeSetByCallerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Damage")
	FGameplayTagContainer DamageTypeTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|Damage", meta=(ClampMin="0.0"))
	float DamageMagnitude = 50.0f;

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

	virtual void NativeOnAbilityInputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintNativeEvent, Category="Projectile")
	void OnProjectileCastStarted();
	virtual void OnProjectileCastStarted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Projectile")
	void OnProjectileCastCompleted();
	virtual void OnProjectileCastCompleted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Projectile")
	void OnProjectileCastCancelled();
	virtual void OnProjectileCastCancelled_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="Projectile")
	void OnProjectileFired(float ChargeAlpha);
	virtual void OnProjectileFired_Implementation(float ChargeAlpha);

	UFUNCTION(BlueprintNativeEvent, Category="Projectile")
	bool CanFireProjectile() const;
	virtual bool CanFireProjectile_Implementation() const;

	UFUNCTION(BlueprintCallable, Category="Projectile")
	bool GetProjectileTargetLocation(FVector& OutTargetLocation) const;

	UFUNCTION(BlueprintCallable, Category="Projectile")
	FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintCallable, Category="Projectile")
	FRotator GetProjectileSpawnRotation(const FVector& TargetLocation) const;

	UFUNCTION(BlueprintCallable, Category="Projectile")
	FGameplayEffectSpecHandle MakeProjectileDamageSpec() const;

	UFUNCTION(BlueprintNativeEvent, Category="Projectile")
	void OnProjectileSpawned(AActor* SpawnedProjectile, const FGameplayEffectSpecHandle& DamageSpec, const FVector& TargetLocation);
	virtual void OnProjectileSpawned_Implementation(AActor* SpawnedProjectile, const FGameplayEffectSpecHandle& DamageSpec, const FVector& TargetLocation);

private:
	void StartCast();
	void FinishCast();
	void FireProjectile();
	void ClearRuntimeTasks();

	UFUNCTION()
	void OnReleaseBeforeCastFinished(float HeldTime);

	float GetChargeAlpha() const;
	void PlayMontageIfValid(UAnimMontage* MontageToPlay, float PlayRate) const;
	void StopMontageIfValid(UAnimMontage* MontageToStop) const;
	APawn* GetAvatarPawn() const;
	bool ValidateReplicatedTargetDataOnServer(const FHitResult& ClientHitResult, FVector& OutTargetLocation, FString& OutRejectReason) const;

private:
	FTimerHandle CastTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitInputRelease> WaitInputReleaseTask = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Projectile|Casting", meta=(AllowPrivateAccess="true"))
	bool bCastFinished = false;

	UPROPERTY(BlueprintReadOnly, Category="Projectile|Casting", meta=(AllowPrivateAccess="true"))
	bool bInputReleasedBeforeFinish = false;

	UPROPERTY(BlueprintReadOnly, Category="Projectile|Casting", meta=(AllowPrivateAccess="true"))
	float InputHeldTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Projectile|Casting", meta=(AllowPrivateAccess="true"))
	bool bProjectileFired = false;
};
