// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h" // FGameplayEventData
#include "SpellRiseCharacterBase.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UGameplayEffect;

UCLASS()
class SPELLRISE_API ASpellRiseCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRiseCharacterBase();

	/** Ability System Component (GAS). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	/** Inventory/Defense (Carga/Resists/Armor). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UBasicAttributeSet* BasicAttributeSet = nullptr;

	/** Primaries (VIG/FOC/AGI/WIL/ATT + casting bonuses). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UCombatAttributeSet* CombatAttributeSet = nullptr;

	/** Resources (Health/Mana/Stamina/Shield + Max...) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UResourceAttributeSet* ResourceAttributeSet = nullptr;

protected:
	/** Replication mode for the ASC (applied at runtime). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

	/**
	 * Instant GE that OVERRIDES MaxHealth/MaxMana/MaxStamina/(MaxShield optional)
	 * using SetByCaller magnitudes:
	 *  - Data.MaxHealth
	 *  - Data.MaxMana
	 *  - Data.MaxStamina
	 *  - Data.MaxShield (optional)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem|Startup")
	TSubclassOf<UGameplayEffect> GE_RecalculateResources;

	/** Tag used to represent death state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	FGameplayTag DeadStateTag;

	/** GameplayEffect applied on death (should grant State.Dead and any other death-state tags). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	TSubclassOf<UGameplayEffect> GE_Death;

	/** Local guard to prevent double-death */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Death")
	bool bIsDead = false;

	/** Prevent binding ASC delegates multiple times (PossessedBy + OnRep_PlayerState). */
	bool bASCDelegatesBound = false;

	/** Applies startup effects (server-only). */
	void ApplyStartupEffects();

	/** Bind ASC delegates after InitAbilityActorInfo */
	void BindASCDelegates();

	/** Recalculate derived resources whenever primaries change (server-only). */
	void RecalculateDerivedStats();

	/** Delegate callback for primary stat changes */
	void OnPrimaryChanged(const FOnAttributeChangeData& Data);

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Called when Health changes; server applies death state when Health <= 0. */
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	/** Called by tag listener when "Dead" tag count changes. */
	UFUNCTION()
	virtual void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	/** Visual/simulated death (ragdoll etc). Implementable in BP too. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Death")
	void HandleDeath();

	/** Ensures everyone sees the death effect. */
	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	TArray<FGameplayAbilitySpecHandle> GrantAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant, int32 Level = 1);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void SendAbilitiesChangedEvent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "AbilitySystem")
	void ServerSendGameplayEventToSelf(const FGameplayEventData& EventData);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "AbilitySystem")
	void MultiSendGameplayEventToActor(AActor* TargetActor, const FGameplayEventData& EventData);

	// =========================================================
	// Damage Pop (Everyone sees)
	// =========================================================
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);
};
