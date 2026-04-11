#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SR_ProjectileBase.generated.h"

class UArrowComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class SPELLRISE_API ASR_ProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	ASR_ProjectileBase();

	UFUNCTION(BlueprintCallable, Category="Projectile")
	void InitializeProjectile(
		const FVector& InTargetLocation,
		float InSpeed,
		const FGameplayEffectSpecHandle& InEffectSpecHandle);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	void ApplyDamageToActor(AActor* OtherActor);
	void TriggerSpawnCue();
	void TriggerImpactCue();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> SphereCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> Arrow = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Settings", meta=(ClampMin="0.0"))
	float Speed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Settings")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Effect")
	FGameplayEffectSpecHandle EffectSpecHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Cue")
	FGameplayTag GameplayCueSpawnTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay Cue")
	FGameplayTag GameplayCueImpactTag;

private:
	bool bImpactCueTriggered = false;
};
