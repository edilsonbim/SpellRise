#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "SpellRisePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UGameplayAbility;
class USpellRiseNumberPopComponent_NiagaraText;

USTRUCT(BlueprintType)
struct FSpellRiseAimTraceResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	bool bHit = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FHitResult HitResult;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector TraceStart = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector TraceEnd = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector AimDirection = FVector::ForwardVector;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector TargetPoint = FVector::ZeroVector;
};

UCLASS()
class SPELLRISE_API ASpellRisePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpellRisePlayerController();

	void ShowDamageNumber(
		float Damage,
		const FVector& WorldLocation,
		const FGameplayTagContainer& SourceTags,
		const FGameplayTagContainer& TargetTags,
		bool bIsCritical
	);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Aim")
	bool GetCurrentAimTraceResult(FSpellRiseAimTraceResult& OutResult) const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced", meta=(ClampMin="0"))
	int32 DefaultMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Attack = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TSubclassOf<UGameplayAbility> AttackAbilityClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Feedback", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseNumberPopComponent_NiagaraText> NumberPopComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim", meta=(ClampMin="1.0"))
	float DefaultAimTraceDistance = 10000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim", meta=(ClampMin="0.0"))
	float DefaultAimTraceRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim")
	TEnumAsByte<ECollisionChannel> DefaultAimTraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim")
	bool bDefaultAimTraceComplex = false;

	void SetupEnhancedInput();
	void OnAttackPressed();
	void OnAttackReleased();

private:
	bool BuildAimTraceResult(
		FSpellRiseAimTraceResult& OutResult,
		float TraceDistance,
		float TraceRadius,
		ECollisionChannel TraceChannel,
		bool bTraceComplex) const;

	void CollectAimIgnoredActors(TArray<AActor*>& OutActorsToIgnore) const;
};