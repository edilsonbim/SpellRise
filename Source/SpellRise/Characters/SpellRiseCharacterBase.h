// SpellRiseCharacterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "InputActionValue.h"

class UInputMappingContext;
class UInputAction;

class USpellRiseAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;
class UDerivedStatsAttributeSet;

class UAbilitySystemComponent;

#include "SpellRiseCharacterBase.generated.h"

UENUM(BlueprintType)
enum class ESpellRiseArchetype : uint8
{
	None UMETA(DisplayName="None"),
	Warrior UMETA(DisplayName="Warrior"),
	Assassin UMETA(DisplayName="Assassin"),
	Mage UMETA(DisplayName="Mage"),
	Battlemage UMETA(DisplayName="Battlemage"),
	Cleric UMETA(DisplayName="Cleric")
};

UCLASS()
class SPELLRISE_API ASpellRiseCharacterBase
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRiseCharacterBase();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Destroyed() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS|Input")
	void SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS|Input")
	void SR_ClearAbilityInput();

	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS")
	TArray<FGameplayAbilitySpecHandle> GrantAbilities(
		const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant,
		int32 Level = 1);

	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS")
	void RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS")
	void SendAbilitiesChangedEvent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "SpellRise|GAS")
	void ServerSendGameplayEventToSelf(const FGameplayEventData& EventData);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "SpellRise|GAS")
	void MultiSendGameplayEventToActor(AActor* TargetActor, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintPure, Category = "SpellRise|Death")
	FORCEINLINE bool IsDead() const { return bIsDead; }

	UFUNCTION()
	void OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpellRise|Death")
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

	UFUNCTION(BlueprintPure, Category = "SpellRise|Catalyst")
	FORCEINLINE UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }

	UFUNCTION(NetMulticast, Unreliable)
	void MultiOnCatalystProc(int32 CatalystTier);

	UFUNCTION(BlueprintImplementableEvent, Category = "SpellRise|Catalyst")
	void BP_OnCatalystProc(int32 CatalystTier);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "SpellRise|UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintImplementableEvent, Category = "SpellRise|UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UPROPERTY(ReplicatedUsing=OnRep_Archetype, BlueprintReadOnly, Category="SpellRise|Archetype")
	ESpellRiseArchetype Archetype = ESpellRiseArchetype::None;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Archetype")
	void SetArchetype(ESpellRiseArchetype NewArchetype);

	UFUNCTION(Server, Reliable)
	void ServerSetArchetype(ESpellRiseArchetype NewArchetype);

	UFUNCTION()
	void OnRep_Archetype();

	void ApplyArchetypeToPrimaries_Server();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Movement")
	float BaseWalkSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBasicAttributeSet> BasicAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UResourceAttributeSet> ResourceAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCatalystAttributeSet> CatalystAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDerivedStatsAttributeSet> DerivedStatsAttributeSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpellRise|GAS")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpellRise|GAS|Startup")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_RecalculateResources;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_DerivedStatsInfinite = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|GAS|Regen")
	TSubclassOf<UGameplayEffect> GE_RegenBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|GAS|Regen")
	TArray<TSubclassOf<UGameplayEffect>> GE_RegenEffects;

	void AbilityInputPressed(EAbilityInputID InputID);
	void AbilityInputReleased(EAbilityInputID InputID);

	UPROPERTY(ReplicatedUsing=OnRep_SelectedAbilityInputID, BlueprintReadOnly, Category="SpellRise|Abilities")
	EAbilityInputID SelectedAbilityInputID = EAbilityInputID::None;

	UFUNCTION()
	void OnRep_SelectedAbilityInputID();

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedAbilityInputID(EAbilityInputID NewID);

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void SelectAbilitySlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void ClearSelectedAbility();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void AbilityWheelInputPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void AbilityWheelInputReleased(int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Death")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Death")
	TSubclassOf<UGameplayEffect> GE_Death;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|Death")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS")
	bool bASCDelegatesBound = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Default = nullptr;

	// Universal inputs
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Primary = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Secondary = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Interact = nullptr;

	// Utility
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_ClearSelection = nullptr;

	// Slots
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability1 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability2 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability3 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability4 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability5 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability6 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability7 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Ability8 = nullptr;

	UFUNCTION()
	void Input_Primary_Pressed(const FInputActionValue& Value);

	UFUNCTION()
	void Input_Primary_Released(const FInputActionValue& Value);

	UFUNCTION()
	void Input_Secondary_Pressed(const FInputActionValue& Value);

	UFUNCTION()
	void Input_Secondary_Released(const FInputActionValue& Value);

	UFUNCTION()
	void Input_Interact_Pressed(const FInputActionValue& Value);

	UFUNCTION()
	void Input_Interact_Released(const FInputActionValue& Value);

	UFUNCTION()
	void Input_ClearSelection(const FInputActionValue& Value);

	void InitASCActorInfo();
	void ApplyStartupEffects();
	void BindASCDelegates();
	void RecalculateDerivedStats();
	void ApplyDerivedStatsInfinite();
	void LogDerivedDebug();
	void OnPrimaryChanged(const FOnAttributeChangeData& Data);
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void ApplyRegenStartupEffects();
	void ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass);
	void ForceServerAnimTick();
	void EnsureAnimInstanceInitialized();
};
