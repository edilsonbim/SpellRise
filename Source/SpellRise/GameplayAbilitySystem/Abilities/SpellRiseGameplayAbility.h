#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

class UAbilityTask_WaitInputRelease;

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None     = 0 UMETA(DisplayName="None"),

	// Slots (mantidos)
	Ability1 = 1 UMETA(DisplayName="Ability1"),
	Ability2 = 2 UMETA(DisplayName="Ability2"),
	Ability3 = 3 UMETA(DisplayName="Ability3"),
	Ability4 = 4 UMETA(DisplayName="Ability4"),
	Ability5 = 5 UMETA(DisplayName="Ability5"),
	Ability6 = 6 UMETA(DisplayName="Ability6"),
	Ability7 = 7 UMETA(DisplayName="Ability7"),
	Ability8 = 8 UMETA(DisplayName="Ability8"),

	// Universal
	Primary   = 9  UMETA(DisplayName="Primary"),
	Secondary = 10 UMETA(DisplayName="Secondary"),
	Interact  = 11 UMETA(DisplayName="Interact"),
};

UENUM(BlueprintType)
enum class ESpellRiseCommitPolicy : uint8
{
	CommitOnStart = 0 UMETA(DisplayName="Commit On Start"),
	CommitOnFire  = 1 UMETA(DisplayName="Commit On Fire"),
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	bool ShouldShowInAbilityBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool AutoActivateWhenGranted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	UFUNCTION(BlueprintCallable, Category="Ability")
	void SetAbilityLevel(int32 NewLevel);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting")
	bool bUseCasting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting", ClampMin="0.0"))
	float CastTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	bool bCancelIfReleasedEarly = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	ESpellRiseCommitPolicy CommitPolicy = ESpellRiseCommitPolicy::CommitOnFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casting", meta=(EditCondition="bUseCasting"))
	FGameplayTag CastingStateTag;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Activation")
	bool bActivateOnSelection = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Activation")
	bool bChannelAbility = false;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) override;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastStart();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category="Casting|BP")
	void BP_OnCastCancelled();

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

	UFUNCTION(BlueprintPure, Category="Input")
	int32 GetInputID_Int() const { return static_cast<int32>(AbilityInputID); }

private:
	bool bHasFired = false;

	FTimerHandle CastTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_WaitInputRelease> WaitReleaseTask = nullptr;

	UWorld* GetWorldSafe() const;

	void ResetRuntimeState();
	void ClearCastTimer();
	void ClearReleaseTask();

	void AddCastingTag();
	void RemoveCastingTag();

	UFUNCTION()
	void OnInputReleased(float HeldTime);

	void OnCastComplete();
	void FireInternal();
	float GetChargeAlpha() const;

	UFUNCTION(BlueprintCallable, Category="Helpers")
	bool HasPC() const;
};
