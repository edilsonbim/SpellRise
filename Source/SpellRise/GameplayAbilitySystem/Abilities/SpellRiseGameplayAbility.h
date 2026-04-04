#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

class AController;
class ASpellRisePawnBase;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(ClampMin="0.01", EditCondition="CastType == ESpellRiseAbilityCastType::Channel", EditConditionHides))
	float ChannelInterval = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell", meta=(EditCondition="CastType == ESpellRiseAbilityCastType::Channel", EditConditionHides))
	bool bCommitAbilityEveryChannelTick = true;

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
	ASpellRisePawnBase* GetSpellRisePawnFromActorInfo() const;

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
	virtual void NativeOnSpellCommitFailed(const FGameplayTagContainer& FailureTags);

protected:
	/**
	 * Dispare o efeito do spell aqui (projétil, dano, VFX de impacto), não no Event ActivateAbility do Blueprint.
	 * O C++ não chama EndAbility após este evento; no Blueprint, chame End Ability quando o spell terminar (Instant/Cast).
	 * Channel: primeiro tick e cada ChannelInterval; com bEndChannelOnInputRelease, o C++ encerra a ability ao soltar o input.
	 * - Instant: após commit, no mesmo fluxo que ActivateAbility.
	 * - Cast: se soltar antes do fim do cast, executa ao terminar; se mantiver pressionado até terminar, executa no release.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnSpellExecuted")
	void K2_OnSpellExecuted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnCastStarted")
	void K2_OnCastStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnChannelStarted")
	void K2_OnChannelStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnChannelStopped")
	void K2_OnChannelStopped(bool bWasCancelled);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnSpellCommitFailed")
	void K2_OnSpellCommitFailed(const FGameplayTagContainer& FailureTags);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell|Casting", meta=(EditCondition="CastType == ESpellRiseAbilityCastType::Cast", EditConditionHides))
	TSubclassOf<UGameplayEffect> CastingGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell|Casting", meta=(EditCondition="CastType == ESpellRiseAbilityCastType::Cast", EditConditionHides))
	TSubclassOf<UGameplayEffect> CastingBarGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spell|Casting", meta=(Categories="Data", EditCondition="CastType == ESpellRiseAbilityCastType::Cast", EditConditionHides))
	FGameplayTag CastDurationMagnitudeTag;

	FTimerHandle CastTimerHandle;
	FTimerHandle ChannelTimerHandle;
	FActiveGameplayEffectHandle CastingGameplayEffectHandle;
	FActiveGameplayEffectHandle CastingBarGameplayEffectHandle;
	double CastStartTimeSeconds = 0.0;
};
