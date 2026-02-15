// SpellRisePlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpellRisePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UGameplayAbility;

/**
 * PlayerController (Enhanced Input):
 * - Applies DefaultMappingContext on local controller
 * - Binds IA_Attack (Pressed) and forwards to GAS:
 *   - ASC->TryActivateAbilityByClass(AttackAbilityClass)
 *
 * This fixes: "sem input para atacar" even if you are not using InputID binding yet.
 */
UCLASS()
class SPELLRISE_API ASpellRisePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpellRisePlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// -------------------------
	// Enhanced Input
	// -------------------------
	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced", meta=(ClampMin="0"))
	int32 DefaultMappingPriority = 0;

	// Attack input action (set this in BP: IA_Attack)
	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Attack = nullptr;

	// Which ability should be activated on attack pressed (set in BP: GA_Weapon_MeleeLight)
	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TSubclassOf<UGameplayAbility> AttackAbilityClass;

	void SetupEnhancedInput();

	// -------------------------
	// Input handlers
	// -------------------------
	void OnAttackPressed();
	void OnAttackReleased();
};
