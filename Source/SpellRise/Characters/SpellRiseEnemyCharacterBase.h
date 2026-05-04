#pragma once

// Base C++ autoritativa para inimigos GAS. Nao depende de PlayerState, input, camera ou UI local.

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"
#include "SpellRiseEnemyCharacterBase.generated.h"

class UAbilitySystemComponent;
class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;
class UDerivedStatsAttributeSet;
class UCatalystComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class USkeletalMeshComponent;
class USpellRiseAbilitySystemComponent;
class UNarrativeInventoryComponent;

USTRUCT(BlueprintType)
struct FSpellRiseEnemyGrantedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Grant")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Grant", meta=(ClampMin="1"))
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Grant", meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Grant")
	bool bAutoActivateIfNoInputTag = false;
};

UCLASS()
class SPELLRISE_API ASpellRiseEnemyCharacterBase
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRiseEnemyCharacterBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|GAS")
	USpellRiseAbilitySystemComponent* GetSpellRiseASC() const { return AbilitySystemComponent; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|GAS")
	UBasicAttributeSet* GetBasicAttributeSet() const { return BasicAttributeSet; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|GAS")
	UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|GAS")
	UResourceAttributeSet* GetResourceAttributeSet() const { return ResourceAttributeSet; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|GAS")
	UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|Catalyst")
	UCatalystComponent* GetCatalystComponent() const { return CatalystComponent; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|GAS")
	UDerivedStatsAttributeSet* GetDerivedStatsAttributeSet() const { return DerivedStatsAttributeSet; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Enemy|GAS")
	TArray<FGameplayAbilitySpecHandle> GrantEnemyStartupAbilities_Server();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Enemy|GAS")
	void ApplyEnemyStartupEffects_Server();

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|Death")
	bool IsEnemyDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="SpellRise|Enemy|Death")
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="SpellRise|Enemy|Combat")
	void MultiPlayHitReactionMontage(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="SpellRise|Enemy|UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Enemy|UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|Combat")
	FVector GetDamageNumberWorldLocation() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|Identity")
	FText GetEnemyDisplayName() const { return EnemyDisplayName; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|Loot")
	const TArray<FLootTableRoll>& GetEnemyLootTables() const { return EnemyLootTables; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBasicAttributeSet> BasicAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UResourceAttributeSet> ResourceAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystAttributeSet> CatalystAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UDerivedStatsAttributeSet> DerivedStatsAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystComponent> CatalystComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Minimal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Startup")
	TArray<FSpellRiseEnemyGrantedAbility> StartingAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Startup")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_RecalculateResources = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Startup")
	TSubclassOf<UGameplayEffect> GE_DerivedStatsInfinite = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Regen")
	TArray<TSubclassOf<UGameplayEffect>> GE_RegenEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Death")
	FGameplayTag DeadStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Death")
	TSubclassOf<UGameplayEffect> GE_Death = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|Death")
	bool bIsDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Combat")
	TObjectPtr<UAnimMontage> HitReactionMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|Identity")
	FText EnemyDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Enemy|Loot")
	TArray<FLootTableRoll> EnemyLootTables;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Loot")
	TSubclassOf<AActor> EnemyLootBagClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Loot|Network", meta=(ClampMin="1.0", ClampMax="30.0"))
	float EnemyLootBagNetUpdateFrequency = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Fallback", meta=(ClampMin="1.0"))
	float InitialPrimaryValue = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Fallback", meta=(ClampMin="1.0"))
	float InitialMaxHealth = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Fallback", meta=(ClampMin="0.0"))
	float InitialMaxMana = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|GAS|Fallback", meta=(ClampMin="0.0"))
	float InitialMaxStamina = 180.0f;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> StartupGrantedAbilityHandles;

	UPROPERTY(Transient)
	bool bStartupInitialized = false;

	UPROPERTY(Transient)
	bool bASCDelegatesBound = false;

	UPROPERTY(Transient)
	bool bEnemyLootProcessedForCurrentDeath = false;

	virtual void InitializeAbilitySystem();
	virtual void BindASCDelegates();
	virtual void ApplyEnemyAttributeFallbacks_Server();
	virtual void ApplyGameplayEffectToSelf_Server(TSubclassOf<UGameplayEffect> EffectClass);
	virtual void ApplyEnemyDeathState_Server();
	virtual void SpawnEnemyLootBag_Server();
	virtual FVector ResolveEnemyLootBagSpawnLocation_Server() const;

	UFUNCTION()
	void OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	void OnHealthChanged(const FOnAttributeChangeData& Data);
};
