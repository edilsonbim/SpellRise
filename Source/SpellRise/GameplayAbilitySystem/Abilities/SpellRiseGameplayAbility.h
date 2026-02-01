#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

class UAbilityTask_WaitInputRelease;

/**
 * Input slots for ability bar / activation binding.
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None     UMETA(DisplayName = "None"),
	Ability1 UMETA(DisplayName = "Ability1"),
	Ability2 UMETA(DisplayName = "Ability2"),
	Ability3 UMETA(DisplayName = "Ability3"),
	Ability4 UMETA(DisplayName = "Ability4"),
	Ability5 UMETA(DisplayName = "Ability5"),
	Ability6 UMETA(DisplayName = "Ability6"),
	Ability7 UMETA(DisplayName = "Ability7"),
	Ability8 UMETA(DisplayName = "Ability8"),

	// (Opcional) se quiser reservar um slot para cancel:
	// CancelCast UMETA(DisplayName="CancelCast")
};

UENUM(BlueprintType)
enum class ESpellRiseCommitPolicy : uint8
{
	CommitOnStart UMETA(DisplayName="Commit On Start"),
	CommitOnFire  UMETA(DisplayName="Commit On Fire"),
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility();

	// ===== UI / Input =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInAbilityBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation")
	bool AutoActivateWhenGranted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetAbilityLevel(int32 NewLevel);

	// =====================================================================
	// Casting (Opt-in)
	// =====================================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bUseCasting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting", ClampMin="0.0"))
	float CastTime = 1.0f;

	/**
	 * IMPORTANTE (seu design):
	 * - FALSE: soltou antes de completar -> continua castando -> dispara quando completar.
	 * - TRUE : soltou antes de completar -> cancela imediatamente.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	bool bCancelIfReleasedEarly = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	ESpellRiseCommitPolicy CommitPolicy = ESpellRiseCommitPolicy::CommitOnFire;

	/** Tag aplicada durante o cast. Default: State.Casting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	FGameplayTag CastingStateTag;

	// ===== State (replicação não necessária, é só controle local do GA) =====
	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	bool bCastCompleted = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	bool bInputReleased = false;

	UPROPERTY(BlueprintReadOnly, Category="Casting|State")
	float TimeHeld = 0.f;

	UFUNCTION(BlueprintCallable, Category="Casting")
	void StartCastingFlow();

	UFUNCTION(BlueprintCallable, Category="Casting")
	void FireNow();

	UFUNCTION(BlueprintCallable, Category="Casting")
	void ActivateSpellFlow();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastStart();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCancelled();

	/** Dispara o feitiço. ChargeAlpha no seu design tende a 1.0 quando completa. */
	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastFired(float ChargeAlpha);

	UFUNCTION(BlueprintNativeEvent, Category="Casting|BP")
	bool BP_CanFire() const;
	virtual bool BP_CanFire_Implementation() const { return true; }

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	FTimerHandle CastTimerHandle;

	UPROPERTY()
	UAbilityTask_WaitInputRelease* WaitReleaseTask = nullptr;

	UFUNCTION()
	void OnInputReleased(float HeldTime);

	void OnCastComplete();
	void FireInternal();

	/** Seu design: se cast completou, ChargeAlpha = 1.0 */
	float GetChargeAlpha() const;

	void ClearCastTimer();
	void ClearReleaseTask();
	void AddCastingTag();
	void RemoveCastingTag();

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	bool HasPC() const;
};
