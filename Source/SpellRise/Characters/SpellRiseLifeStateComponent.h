#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "SpellRiseLifeStateComponent.generated.h"

class ASpellRiseCharacterBase;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ESpellRiseLifeState : uint8
{
	Alive,
	Downed,
	ReviveRecovery,
	Dead
};

UENUM(BlueprintType)
enum class ESpellRiseDownedExpirationResult : uint8
{
	FinalDeath UMETA(DisplayName="Morte final"),
	AutomaticRevive UMETA(DisplayName="Revive automatico")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseLifeStateChangedSignature,
	ESpellRiseLifeState,
	NewState,
	ESpellRiseLifeState,
	OldState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellRiseShowDownedActionsSignature,
	ASpellRiseCharacterBase*,
	TargetCharacter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpellRiseHideDownedActionsSignature);

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseLifeStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseLifeStateComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="SpellRise|LifeState")
	ESpellRiseLifeState GetLifeState() const { return LifeState; }

	UFUNCTION(BlueprintPure, Category="SpellRise|LifeState")
	bool IsDowned() const { return LifeState == ESpellRiseLifeState::Downed; }

	UFUNCTION(BlueprintPure, Category="SpellRise|LifeState")
	bool IsDead() const { return LifeState == ESpellRiseLifeState::Dead; }

	// Downed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Downed", meta=(ClampMin="1.0", Units="s"))
	float DownedDurationSeconds = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Downed")
	ESpellRiseDownedExpirationResult DownedExpirationResult = ESpellRiseDownedExpirationResult::FinalDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Downed", meta=(ClampMin="10.0", Units="cm/s"))
	float DownedCrawlSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Downed", meta=(ClampMin="50.0", Units="cm"))
	float InteractionMaxDistance = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Downed")
	TSubclassOf<UGameplayAbility> DownedAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Presentation", meta=(ClampMin="0.0", ClampMax="1.0"))
	float DownedRedEdgeAlpha = 0.65f;

	// Cooldown entre entradas em Downed. Durante este periodo, dano letal causa morte final.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Cooldown", meta=(ClampMin="0.0", Units="s"))
	float DownedCooldownSeconds = 300.f;

	// Recursos restaurados ao reviver. WIS abaixo de 100 usa os valores de 100;
	// WIS acima de 140 usa os valores de 140; entre ambos, interpolacao linear.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveHealthPercentAt100Wisdom = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveHealthPercentAt140Wisdom = 0.30f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveManaPercentAt100Wisdom = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveManaPercentAt140Wisdom = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveStaminaPercentAt100Wisdom = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReviveStaminaPercentAt140Wisdom = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", Units="s"))
	float ReviveRecoverySecondsAt100Wisdom = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|LifeState|Revive", meta=(ClampMin="0.0", Units="s"))
	float ReviveRecoverySecondsAt140Wisdom = 5.f;

	bool HandleLethalHealth_Server();
	bool Revive_Server(ASpellRiseCharacterBase* Reviver);
	bool Gank_Server(ASpellRiseCharacterBase* Finisher);
	void AcceptDeath_Server();
	void ResetForSpawn_Server();
	bool ValidateInteractor_Server(const ASpellRiseCharacterBase* Interactor, FString* OutReason = nullptr) const;

	void ShowDownedActions_Local(ASpellRiseCharacterBase* TargetCharacter);
	void HideDownedActions_Local();

	UPROPERTY(BlueprintAssignable, Category="SpellRise|LifeState|Presentation")
	FSpellRiseShowDownedActionsSignature OnShowDownedActions;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|LifeState|Presentation")
	FSpellRiseHideDownedActionsSignature OnHideDownedActions;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|LifeState|Presentation")
	FSpellRiseLifeStateChangedSignature OnLifeStateChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_LifeState(ESpellRiseLifeState OldState);

private:
	void EnterDowned_Server();
	void FinalizeDeath_Server(FName Reason, ASpellRiseCharacterBase* Finalizer);
	void ClearDowned_Server();
	void StartDownedCooldown_Server();
	void ClearDownedCooldown_Server();
	void ClearReviveRecovery_Server();
	void ActivateDownedAbility_Server();
	void CancelDownedAbility_Server();
	void HandleDownedExpired_Server();
	void SetLifeState_Server(ESpellRiseLifeState NewState);
	ASpellRiseCharacterBase* GetCharacter() const;

	UPROPERTY(ReplicatedUsing=OnRep_LifeState)
	ESpellRiseLifeState LifeState = ESpellRiseLifeState::Alive;

	FTimerHandle DownedExpirationTimerHandle;
	FTimerHandle DownedCooldownTimerHandle;
	FTimerHandle ReviveRecoveryTimerHandle;
	FGameplayAbilitySpecHandle DownedAbilityHandle;
};
