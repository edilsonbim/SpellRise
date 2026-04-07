#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

class AController;
class ASpellRiseCharacterBase;
class UAbilitySystemComponent;
class USpellRiseAbilitySystemComponent;

UENUM(BlueprintType)
enum class ESpellRiseAbilityCastType : uint8
{
	Instant = 0 UMETA(DisplayName="Instant"),
	Cast = 1 UMETA(DisplayName="Cast"),
	Channel = 2 UMETA(DisplayName="Channel")
};

UENUM(BlueprintType)
enum class ESpellRiseCastCompletionPolicy : uint8
{
	AutoFireOnCastComplete = 0 UMETA(DisplayName="Auto Fire On Cast Complete"),
	WaitReleaseAfterCastComplete = 1 UMETA(DisplayName="Wait Release After Cast Complete")
};

UENUM(BlueprintType)
enum class ESpellRiseCueTriggerMode : uint8
{
	Execute = 0 UMETA(DisplayName="Execute (Instant)"),
	Add = 1 UMETA(DisplayName="Add (Persistent)"),
	Remove = 2 UMETA(DisplayName="Remove (Persistent)")
};

UCLASS(Blueprintable, Abstract)
class SPELLRISE_API USpellRiseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	bool ShouldShowInAbilityBar = false;

	/**
	 * Regra ideal:
	 * - conceder a ability com este mesmo tag em Spec.GetDynamicSpecSourceTags()
	 * - o ASC procura por esse tag ao receber input
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(Categories="InputTag"))
	FGameplayTag AbilityInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	ESpellRiseAbilityCastType CastType = ESpellRiseAbilityCastType::Instant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	bool bFireOnAbilityInput = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(ClampMin="0.0", EditCondition="CastType == ESpellRiseAbilityCastType::Cast", EditConditionHides))
	float CastTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(EditCondition="CastType == ESpellRiseAbilityCastType::Cast", EditConditionHides))
	ESpellRiseCastCompletionPolicy CastCompletionPolicy = ESpellRiseCastCompletionPolicy::AutoFireOnCastComplete;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(ClampMin="0.01", EditCondition="CastType == ESpellRiseAbilityCastType::Channel", EditConditionHides))
	float ChannelInterval = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	bool bCommitAbilityOnActivate = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(EditCondition="CastType == ESpellRiseAbilityCastType::Channel", EditConditionHides))
	bool bCommitAbilityEveryChannelTick = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell")
	bool bEndAbilityAfterExecution = true;

	/** If true, K2_OnSpellExecuted only runs on server authority. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell|Networking")
	bool bExecuteSpellLogicOnAuthorityOnly = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(EditCondition="CastType == ESpellRiseAbilityCastType::Channel", EditConditionHides))
	bool bEndChannelOnInputRelease = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Selection")
	bool bKeepSelectedAfterAbilityEnds = false;

public:
	/** Server authoritative: altera o nível apenas no servidor e marca o spec dirty para replicação. */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability")
	void SetAbilityLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability|GameplayCue")
	bool SR_TriggerGameplayCueReliable(
		FGameplayTag CueTag,
		ESpellRiseCueTriggerMode TriggerMode = ESpellRiseCueTriggerMode::Execute);

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|State")
	bool IsAbilityInputPressed() const
	{
		return bIsAbilityInputPressed;
	}

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|State")
	bool IsCastInProgress() const
	{
		return bIsCasting;
	}

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|State")
	bool IsChannelInProgress() const
	{
		return bIsChanneling;
	}

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|Input")
	bool FiresFromOwnInputTag() const
	{
		return bFireOnAbilityInput;
	}

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|Networking")
	bool HasServerAuthority() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Ability|Networking")
	bool IsLocallyControlledAbility() const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability")
	USpellRiseAbilitySystemComponent* GetSpellRiseAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability")
	ASpellRiseCharacterBase* GetSpellRiseCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability")
	AController* GetControllerFromActorInfo() const;

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	bool TryCommitSpellAbility();
	void StartCastFlow();
	void FinishCastFlow();
	void StartChannelFlow();
	void TickChannelFlow();
	void StopChannelFlow(bool bWasCancelled);
	void ExecuteSpellFromCurrentMode();
	void ResetSpellRuntimeState();
	float ResolveElapsedCastTime() const;
	void ApplyCastingEffect();
	void ApplyCastingBarEffect();
	void RemoveCastingEffect();
	void RemoveCastingBarEffect();
	void NotifyHUDCastStarted(float Duration) const;
	void NotifyHUDCastStopped() const;

	UFUNCTION()
	void HandleCastFinished();

	UFUNCTION()
	void HandleChannelTick();

	virtual void NativeOnSpellExecuted();
	virtual void NativeOnAbilityInputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);
	virtual void NativeOnAbilityInputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);
	virtual void NativeOnCastStarted();
	virtual void NativeOnChannelStarted();
	virtual void NativeOnChannelStopped(bool bWasCancelled);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnSpellExecuted")
	void K2_OnSpellExecuted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnCastStarted")
	void K2_OnCastStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnChannelStarted")
	void K2_OnChannelStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnChannelStopped")
	void K2_OnChannelStopped(bool bWasCancelled);

protected:
	/** Estado local/transiente de input da spec ativa. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	bool bIsAbilityInputPressed = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	bool bIsCasting = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	bool bIsChanneling = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	bool bHasExecutedSpell = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	float CastElapsedTime = 0.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	bool bAwaitingReleaseAfterCastComplete = false;

	FTimerHandle CastTimerHandle;
	FTimerHandle ChannelTimerHandle;
	FActiveGameplayEffectHandle CastingGameplayEffectHandle;
	FActiveGameplayEffectHandle CastingBarGameplayEffectHandle;
	double CastStartTimeSeconds = 0.0;

private:
	UPROPERTY()
	TSubclassOf<UGameplayEffect> CastingGameplayEffectClass;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> CastingBarGameplayEffectClass;

	UPROPERTY()
	FGameplayTag CastDurationMagnitudeTag;
};
