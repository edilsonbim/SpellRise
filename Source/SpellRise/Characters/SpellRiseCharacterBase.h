#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SpellRise/Components/FallDamageComponent.h"
#include "InputActionValue.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USkeletalMeshComponent;
class USpellRiseAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;
class UCatalystComponent;
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

USTRUCT(BlueprintType)
struct FSpellRiseGrantedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Grant")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Grant", meta=(ClampMin="1"))
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Grant", meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Grant")
	bool bAutoActivateIfNoInputTag = false;
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
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ClearAbilityInput();

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS")
	TArray<FGameplayAbilitySpecHandle> GrantAbilities(const TArray<FSpellRiseGrantedAbility>& AbilitiesToGrant);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS")
	void RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS")
	void SendAbilitiesChangedEvent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|GAS")
	void ServerSendGameplayEventToSelf(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintPure, Category="SpellRise|Death")
	bool IsDead() const { return bIsDead; }

	UFUNCTION()
	void OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="SpellRise|Death")
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

	UFUNCTION(BlueprintPure, Category="SpellRise|Catalyst")
	UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }

	UFUNCTION(NetMulticast, Unreliable)
	void MultiOnCatalystProc(int32 CatalystTier);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Catalyst")
	void BP_OnCatalystProc(int32 CatalystTier);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="SpellRise|UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintPure, Category="SpellRise|Character|Mesh")
	USkeletalMeshComponent* GetVisualMeshComponent() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Character|Mesh")
	USkeletalMeshComponent* GetEquipmentAttachMeshComponent() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Character|Camera")
	UCameraComponent* GetActiveAimCameraComponent() const;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="SpellRise|Archetype")
	ESpellRiseArchetype Archetype = ESpellRiseArchetype::None;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Archetype")
	void SetArchetype(ESpellRiseArchetype NewArchetype);

	UFUNCTION(Server, Reliable)
	void ServerSetArchetype(ESpellRiseArchetype NewArchetype);

	void ApplyArchetypeToPrimaries_Server();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UFallDamageComponent> FallDamageComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Movement")
	float BaseWalkSpeed = 500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBasicAttributeSet> BasicAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UResourceAttributeSet> ResourceAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystAttributeSet> CatalystAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UDerivedStatsAttributeSet> DerivedStatsAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystComponent> CatalystComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Catalyst")
	bool bEnableCatalyst = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|GAS")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|GAS|Startup")
	TArray<FSpellRiseGrantedAbility> StartingAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_RecalculateResources;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_DerivedStatsInfinite = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Regen")
	TSubclassOf<UGameplayEffect> GE_RegenBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Regen")
	TArray<TSubclassOf<UGameplayEffect>> GE_RegenEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Character|Mesh")
	FName VisualMeshComponentName = TEXT("VisualOverride");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Character|Mesh")
	FName EquipmentAttachMeshComponentName = TEXT("VisualOverride");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Character|Camera")
	FName AimCameraComponentName = TEXT("GameplayCamera");

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	UPROPERTY(Replicated, BlueprintReadOnly, Category="SpellRise|Abilities")
	FGameplayTag SelectedAbilityInputTag;

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedAbilityInputTag(FGameplayTag NewTag);

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void SelectAbilitySlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void ClearSelectedAbility();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void AbilityWheelInputPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void AbilityWheelInputReleased(int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death")
	TSubclassOf<UGameplayEffect> GE_Death;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Death")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS")
	bool bASCDelegatesBound = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Default = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Primary = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Secondary = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_Interact = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_ClearSelection = nullptr;

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
	USkeletalMeshComponent* FindCharacterSkeletalMeshComponentByName(FName ComponentName) const;
	UCameraComponent* FindCharacterCameraComponentByName(FName ComponentName) const;
};