#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h" // ✅ necessário para EGameplayEffectReplicationMode
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

// ✅ Usa o enum central já existente (não redefinir!)
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" // contém EAbilityInputID

class UDA_MeleeWeaponData;

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
class USpellRiseMeleeComponent;
class UAbilitySystemComponent;

#include "SpellRiseCharacterBase.generated.h"

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

	// Melee
	UFUNCTION(BlueprintPure, Category = "SpellRise|Melee")
	FORCEINLINE USpellRiseMeleeComponent* GetMeleeComponent() const { return MeleeComponent; }

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

	// Combat (melee runtime)
	// Exists as a default subobject on the base Character so every BP child inherits it consistently.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpellRise|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpellRiseMeleeComponent> MeleeComponent = nullptr;

	// Default weapon data
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

	// GAS config
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

	// GAS input bridge (symbols must exist in .cpp)
	void AbilityInputPressed(EAbilityInputID InputID);
	void AbilityInputReleased(EAbilityInputID InputID);

    // =========================================================
    // Ability Wheel
    // =========================================================
    /** Currently selected ability input ID from the radial ability wheel.
     *  When set to a value other than None or PrimaryAttack, the left mouse button
     *  will fire the selected ability instead of the weapon's basic attack.
     *  This value is replicated to the owning client only so that the server has
     *  authoritative knowledge of which ability should consume primary input.
     */
    UPROPERTY(ReplicatedUsing=OnRep_SelectedAbilityInputID, BlueprintReadOnly, Category="SpellRise|Abilities")
    EAbilityInputID SelectedAbilityInputID = EAbilityInputID::None;

    /** Called on clients when the selected ability input ID changes.  Can be used
     *  for UI updates (e.g., highlight the selected slot).  */
    UFUNCTION()
    void OnRep_SelectedAbilityInputID();

protected:
    /** Server RPC to update the selected ability input ID.  Clients call this
     *  function when selecting an ability in the radial menu.  */
    UFUNCTION(Server, Reliable)
    void ServerSetSelectedAbilityInputID(EAbilityInputID NewID);

public:
    /** Selects an ability slot by index (0-7).  Index 0 corresponds to Ability1,
     *  index 7 corresponds to Ability8.  Passing a negative index or an index
     *  outside the valid range will clear the selection.  */
    UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
    void SelectAbilitySlot(int32 SlotIndex);

    /** Clears any selected ability slot, returning to primary attack.  */
    UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
    void ClearSelectedAbility();

    // Replication override
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
