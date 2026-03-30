#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SpellRiseGameplayAbility.generated.h"

class AController;
class ASpellRiseCharacterBase;
class UAbilitySystemComponent;
class USpellRiseAbilitySystemComponent;

UENUM(BlueprintType)
enum class ESpellRiseAbilityActivationPolicy : uint8
{
	OnInputTriggered = 0 UMETA(DisplayName="On Input Triggered"),
	WhileInputActive = 1 UMETA(DisplayName="While Input Active"),
	OnSpawn          = 2 UMETA(DisplayName="On Spawn")
};

UENUM(BlueprintType)
enum class ESpellRiseAbilityActivationGroup : uint8
{
	Independent           = 0 UMETA(DisplayName="Independent"),
	Exclusive_Replaceable = 1 UMETA(DisplayName="Exclusive Replaceable"),
	Exclusive_Blocking    = 2 UMETA(DisplayName="Exclusive Blocking")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool bActivateAbilityOnGranted = false;

	/** Compat alias para assets antigos. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool AutoActivateWhenGranted = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Activation")
	ESpellRiseAbilityActivationPolicy ActivationPolicy = ESpellRiseAbilityActivationPolicy::OnInputTriggered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Activation")
	ESpellRiseAbilityActivationGroup ActivationGroup = ESpellRiseAbilityActivationGroup::Independent;

	/** Mantido por compatibilidade com assets antigos. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Activation")
	bool bActivateOnSelection = false;

	/**
	 * Regra ideal:
	 * - conceder a ability com este mesmo tag em Spec.GetDynamicSpecSourceTags()
	 * - o ASC procura por esse tag ao receber input
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(Categories="InputTag"))
	FGameplayTag AbilityInputTag;

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
	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

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

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	/** Hook nativo para subclasses C++. Não executa gameplay sensível na base. */
	virtual void NativeOnAbilityInputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);

	/** Hook nativo para subclasses C++. Não executa gameplay sensível na base. */
	virtual void NativeOnAbilityInputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo);

	/** Hook nativo para UX local. */
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailureTags);

protected:
	/** Base limpa: decide apenas se deve tentar auto-activate; não implementa cast/channel/charge. */
	bool ShouldAutoActivateOnGrantOrSpawn() const;

	/** Filtra auto-activate por papel de rede e NetExecutionPolicy. */
	void TryActivateAbilityOnGrantOrSpawn(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec) const;

	/** Eventos Blueprint de input podem rodar no owner local e no servidor autoritativo quando necessário. */
	bool ShouldBroadcastAbilityInputEvents(const FGameplayAbilityActorInfo* ActorInfo) const;

	/** Failure UX só no lado local relevante. */
	bool ShouldBroadcastAbilityFailure(const FGameplayAbilityActorInfo* ActorInfo) const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnAbilityInputPressed")
	void K2_OnAbilityInputPressed();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnAbilityInputReleased")
	void K2_OnAbilityInputReleased();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnAbilityFailedToActivate")
	void K2_OnAbilityFailedToActivate(const FGameplayTagContainer& FailureTags);

protected:
	/** Estado local/transiente de input da spec ativa. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Ability|State")
	bool bIsAbilityInputPressed = false;
};
