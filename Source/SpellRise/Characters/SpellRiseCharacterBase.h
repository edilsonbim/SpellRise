#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

// 🔥 Forward declaration do novo Data Asset
class UDA_MeleeWeaponData;

#include "SpellRiseCharacterBase.generated.h"

// Forward Declarations
class USpellRiseAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;
class UDerivedStatsAttributeSet;

class USpellRiseWeaponComponent;

// Enum central do projeto
enum class EAbilityInputID : uint8;

UCLASS()
class SPELLRISE_API ASpellRiseCharacterBase
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRiseCharacterBase();

	// Engine
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Destroyed() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Weapon
	UFUNCTION(BlueprintPure, Category = "SpellRise|Weapon")
	FORCEINLINE USpellRiseWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

	// GAS Input bridge
	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS|Input")
	void SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category = "SpellRise|GAS|Input")
	void SR_ClearAbilityInput();

	// GAS API
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

	// Death
	UFUNCTION(BlueprintPure, Category = "SpellRise|Death")
	FORCEINLINE bool IsDead() const { return bIsDead; }

	UFUNCTION()
	void OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpellRise|Death")
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

	// Catalyst
	UFUNCTION(BlueprintPure, Category = "SpellRise|Catalyst")
	FORCEINLINE UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }

	UFUNCTION(NetMulticast, Unreliable)
	void MultiOnCatalystProc(int32 CatalystTier);

	UFUNCTION(BlueprintImplementableEvent, Category = "SpellRise|Catalyst")
	void BP_OnCatalystProc(int32 CatalystTier);

	// Damage Pop
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "SpellRise|UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintImplementableEvent, Category = "SpellRise|UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

protected:
	// Movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Movement")
	float BaseWalkSpeed = 500.f;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpellRiseWeaponComponent> WeaponComponent = nullptr;

	// 🔥 NOVO: Arma padrão (Data Asset unificado)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Weapon|Startup")
	TObjectPtr<UDA_MeleeWeaponData> DefaultMeleeWeaponData = nullptr;

	// GAS: Core + AttributeSets
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

	// GAS: Startup / Config
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

	// Input (Legacy)
	UPROPERTY(EditDefaultsOnly, Category = "SpellRise|Input|Legacy")
	FName Action_PrimaryAttack = TEXT("PrimaryAttack");

	UPROPERTY(EditDefaultsOnly, Category = "SpellRise|Input|Legacy")
	FName Action_SecondaryAttack = TEXT("SecondaryAttack");

	UPROPERTY(EditDefaultsOnly, Category = "SpellRise|Input|Legacy")
	FName Action_Cancel = TEXT("Cancel");

	void AbilityInputPressed(EAbilityInputID InputID);
	void AbilityInputReleased(EAbilityInputID InputID);

	void LI_PrimaryAttackPressed();
	void LI_PrimaryAttackReleased();

	void LI_SecondaryAttackPressed();
	void LI_SecondaryAttackReleased();

	void LI_CancelPressed();
	void LI_CancelReleased();

	// Death
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Death")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpellRise|Death")
	TSubclassOf<UGameplayEffect> GE_Death;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|Death")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|GAS")
	bool bASCDelegatesBound = false;

	// Internal helpers
	void InitASCActorInfo();
	void ApplyStartupEffects();
	void BindASCDelegates();
	void RecalculateDerivedStats();
	void ApplyDerivedStatsInfinite();
	void LogDerivedDebug();
	void ApplyMovementSpeedFromAttributes();
	void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);
	void OnPrimaryChanged(const FOnAttributeChangeData& Data);
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void ApplyRegenStartupEffects();
	void ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass);
	void ForceServerAnimTick();
	void EnsureAnimInstanceInitialized();
};