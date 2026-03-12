#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "SpellRisePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UGameplayAbility;
class USpellRiseNumberPopComponent_NiagaraText;

/**
 * PlayerController (Enhanced Input + Combat Feedback):
 * - Applies DefaultMappingContext on local controller
 * - Binds IA_Attack and forwards to GAS
 * - Owns the NumberPopComponent (Lyra-style) for local damage feedback
 */
UCLASS()
class SPELLRISE_API ASpellRisePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpellRisePlayerController();

	/** Local-only visual feedback for combat damage numbers */
	void ShowDamageNumber(
		float Damage,
		const FVector& WorldLocation,
		const FGameplayTagContainer& SourceTags,
		const FGameplayTagContainer& TargetTags,
		bool bIsCritical
	);

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

	// -------------------------
	// Combat Feedback
	// -------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Feedback", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseNumberPopComponent_NiagaraText> NumberPopComponent = nullptr;

	void SetupEnhancedInput();

	// -------------------------
	// Input handlers
	// -------------------------
	void OnAttackPressed();
	void OnAttackReleased();
};