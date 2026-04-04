// FallDamageMoverComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "FallDamageMoverComponent.generated.h"

class APawn;
class UMoverComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class UCurveFloat;
struct FHitResult;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API UFallDamageMoverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFallDamageMoverComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(Transient)
	TObjectPtr<APawn> OwnerPawn = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMoverComponent> OwnerMover = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	TSubclassOf<UGameplayEffect> GE_FallDamage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	TObjectPtr<UCurveFloat> FallDamagePercentCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	float MinFallTime = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	float MinFallHeight = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	float SafeSpeed = 900.f;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	float FatalSpeed = 2200.f;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	float StaminaMitigationAt30 = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category="Fall Damage")
	float StaminaMitigationAt60 = 0.30f;

	UPROPERTY(Transient)
	bool bFallTrackingActive = false;

	UPROPERTY(Transient)
	float FallStartZ = 0.f;

	UPROPERTY(Transient)
	float FallStartTime = 0.f;

	UPROPERTY(Transient)
	float MaxFallSpeedAbs = 0.f;

	UPROPERTY(Transient)
	bool bIgnoreNextFallDamage = false;

	UPROPERTY(Transient)
	FName LastMovementModeName = NAME_None;

	UFUNCTION()
	void HandleMoverMovementModeChanged(const FName& PreviousMovementModeName, const FName& NewMovementModeName);

	void StartFallTracking();
	void StopFallTracking();
	void EvaluateLanding(const FHitResult& Hit);

	bool IsCurrentModeFalling() const;
	bool IsFallingModeName(const FName& ModeName) const;
	bool IsSafeSurface(const FHitResult& Hit) const;
	bool HasFallImmunity() const;

	float ComputeSeverity() const;
	float ComputeDamagePercent(float Severity) const;
	float ApplyMitigations(float Damage, const FHitResult& Hit) const;

	UAbilitySystemComponent* GetASC() const;
	float GetMaxHealth() const;
	float GetCurrentStamina() const;
	float GetMaxStamina() const;

	void ApplyFallDamage(float Damage, float Severity, const FHitResult& Hit);
};
