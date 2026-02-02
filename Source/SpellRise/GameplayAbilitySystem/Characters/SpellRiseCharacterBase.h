// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h" // FGameplayEventData
#include "SpellRiseCharacterBase.generated.h"

class USpellRiseAbilitySystemComponent;
class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;

UCLASS()
class SPELLRISE_API ASpellRiseCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRiseCharacterBase();

	// =========================================================
	// GAS: Core + AttributeSets
	// =========================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBasicAttributeSet> BasicAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UResourceAttributeSet> ResourceAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystAttributeSet> CatalystAttributeSet = nullptr;

protected:
	// =========================================================
	// GAS: Startup / Config
	// =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem|Startup")
	TSubclassOf<UGameplayEffect> GE_RecalculateResources;

	// ✅ Regen sempre ativo (recomendado usar 1 GE único infinito/periodic)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem|Startup")
	TSubclassOf<UGameplayEffect> GE_RegenBase;

	// ✅ Alternativa: 3 GEs separados (Health/Mana/Stamina). Use se não quiser GE único.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem|Startup")
	TArray<TSubclassOf<UGameplayEffect>> GE_RegenEffects;

	// =========================================================
	// Catalyst Proc
	// =========================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	float CatalystProcDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	float CatalystChargeThreshold = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	float CatalystChargeConsume = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	TSubclassOf<UGameplayEffect> GE_Catalyst_Active_Lv1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	TSubclassOf<UGameplayEffect> GE_Catalyst_Active_Lv2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
	TSubclassOf<UGameplayEffect> GE_Catalyst_Active_Lv3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Catalyst")
	bool bCatalystProcInProgress = false;

	// =========================================================
	// Death
	// =========================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	TSubclassOf<UGameplayEffect> GE_Death;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Death")
	bool bIsDead = false;

	bool bASCDelegatesBound = false;

protected:
	// =========================================================
	// Internal helpers
	// =========================================================

	void ApplyStartupEffects();
	void BindASCDelegates();
	void RecalculateDerivedStats();

	void OnPrimaryChanged(const FOnAttributeChangeData& Data);
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	// ✅ Regen: aplica GE(s) infinito(s) no servidor (sempre ativo)
	void ApplyRegenStartupEffects();

	// ✅ Helper: aplica um GE com spec (serve pra regen/catalyst/etc.)
	void ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass);

	/** Catalyst: called when CatalystCharge changes */
	void OnCatalystChargeChanged(const FOnAttributeChangeData& Data);

	/** Server-only: consumes charge and applies proc GE */
	void TryProcCatalyst_Server();

	/** Pick correct GE based on CatalystLevel */
	TSubclassOf<UGameplayEffect> GetCatalystProcGE() const;

protected:
	// =========================================================
	// Engine overrides
	// =========================================================
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// =========================================================
	// IAbilitySystemInterface
	// =========================================================
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// =========================================================
	// GAS API
	// =========================================================

	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	TArray<FGameplayAbilitySpecHandle> GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant, int32 Level = 1);

	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	void SendAbilitiesChangedEvent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="AbilitySystem")
	void ServerSendGameplayEventToSelf(const FGameplayEventData& EventData);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="AbilitySystem")
	void MultiSendGameplayEventToActor(AActor* TargetActor, const FGameplayEventData& EventData);

	// =========================================================
	// Death
	// =========================================================

	UFUNCTION()
	virtual void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Death")
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

	// =========================================================
	// Catalyst UI/VFX hook (optional)
	// =========================================================

	UFUNCTION(NetMulticast, Unreliable)
	void MultiOnCatalystProc(int32 ProcLevel);

	UFUNCTION(BlueprintImplementableEvent, Category="Catalyst")
	void BP_OnCatalystProc(int32 ProcLevel);

	// =========================================================
	// Damage Pop
	// =========================================================
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	// =========================================================
	// Convenience getters
	// =========================================================
	UFUNCTION(BlueprintPure, Category="AbilitySystem")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category="Catalyst")
	UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }
};
