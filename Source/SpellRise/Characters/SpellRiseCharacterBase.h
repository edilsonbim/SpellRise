#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SpellRise/Components/FallDamageComponent.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UAudioComponent;
class USkeletalMeshComponent;
class UAnimMontage;
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
class ASpellRisePlayerState;
class USpellRiseEquipmentManagerComponent;

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

USTRUCT()
struct FSpellRiseServerEventRateLimitState
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	double WindowStartServerTimeSeconds = 0.0;

	UPROPERTY(Transient)
	int32 EventsInWindow = 0;

	UPROPERTY(Transient)
	int32 RejectCount = 0;
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

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="SpellRise|GAS")
	void MultiSendGameplayEventToActor(AActor* TargetActor, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintPure, Category="SpellRise|Death")
	bool IsDead() const;

	const UInputMappingContext* GetDefaultInputMappingContext() const { return IMC_Default; }

	UFUNCTION()
	void OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="SpellRise|Death")
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MultiHandleDeath();

	UFUNCTION(BlueprintPure, Category="SpellRise|Catalyst")
	UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Catalyst")
	UCatalystComponent* GetCatalystComponent() const { return CatalystComponent; }

	UFUNCTION(NetMulticast, Unreliable)
	void MultiOnCatalystProc(int32 CatalystTier);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category="SpellRise|Equipment")
	void MultiRefreshEquipmentVisuals();

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	USpellRiseEquipmentManagerComponent* GetSpellRiseEquipmentManager() const { return EquipmentManager; }

	UFUNCTION(Server, Reliable)
	void ServerHandleNarrativeItemActivationForEquipment(UObject* ItemObject, bool bShouldEquip);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Catalyst")
	void BP_OnCatalystProc(int32 CatalystTier);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="SpellRise|UI")
	void MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category="SpellRise|Combat")
	void MultiPlayHitReactionMontage(float PlayRate = 1.0f);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI")
	void BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit);

	UFUNCTION(BlueprintPure, Category="SpellRise|Character|Mesh")
	USkeletalMeshComponent* GetVisualMeshComponent() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Character|Mesh")
	USkeletalMeshComponent* GetEquipmentAttachMeshComponent() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Character|Camera")
	UCameraComponent* GetActiveAimCameraComponent() const;

	UPROPERTY(ReplicatedUsing=OnRep_Archetype, BlueprintReadOnly, Category="SpellRise|Archetype")
	ESpellRiseArchetype Archetype = ESpellRiseArchetype::None;

	UFUNCTION()
	void OnRep_Archetype(ESpellRiseArchetype OldArchetype);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Archetype")
	void SetArchetype(ESpellRiseArchetype NewArchetype);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Archetype")
	void BP_OnArchetypeChanged(ESpellRiseArchetype NewArchetype, ESpellRiseArchetype OldArchetype);

	UFUNCTION(Server, Reliable)
	void ServerSetArchetype(ESpellRiseArchetype NewArchetype);

	void ApplyArchetypeToPrimaries_Server();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UFallDamageComponent> FallDamageComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Movement")
	float BaseWalkSpeed = 500.f;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseEquipmentManagerComponent> EquipmentManager = nullptr;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Combat|HitReaction")
	TObjectPtr<UAnimMontage> HitReactionMontage = nullptr;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	UPROPERTY(ReplicatedUsing=OnRep_SelectedAbilityInputTag, BlueprintReadOnly, Category="SpellRise|Abilities")
	FGameplayTag SelectedAbilityInputTag;

	UFUNCTION()
	void OnRep_SelectedAbilityInputTag(const FGameplayTag& OldTag);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Abilities")
	void BP_OnSelectedAbilityInputTagChanged(FGameplayTag NewTag, FGameplayTag OldTag);

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedAbilityInputTag(FGameplayTag NewTag);

public:
	UFUNCTION(BlueprintPure, Category="SpellRise|Abilities")
	FGameplayTag GetSelectedAbilityInputTag() const { return SelectedAbilityInputTag; }

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Loot")
	TSubclassOf<AActor> FullLootBagClass = nullptr;

	UPROPERTY(Transient)
	bool bFullLootProcessedForCurrentDeath = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|Respawn", meta=(ClampMin="0.0"))
	float RespawnDelaySeconds = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|Respawn", meta=(ClampMin="0.0"))
	float DeathMessageHideLeadTimeSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|Camera")
	bool bEnableLocalDeathCameraEffect = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|Camera", meta=(ClampMin="-89.0", ClampMax="89.0"))
	float DeathCameraPitchDegrees = -75.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|Camera", meta=(ClampMin="0.0"))
	float DeathCameraFadeOutDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|Camera", meta=(ClampMin="0.0"))
	float DeathCameraFadeInDurationOnSpawn = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Damage|Camera")
	bool bEnableLocalDamageScreenEffect = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Damage|Camera", meta=(ClampMin="0.0", ClampMax="1.0"))
	float DamageScreenEffectAlpha = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Damage|Camera", meta=(ClampMin="0.0"))
	float DamageScreenEffectFadeInDuration = 0.06f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Damage|Camera", meta=(ClampMin="0.0"))
	float DamageScreenEffectFadeOutDuration = 0.28f;

	UPROPERTY(Transient)
	TObjectPtr<class USpellRiseDamageEdgeWidget> LocalDamageEdgeWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<class USpellRiseDeathScreenWidget> LocalDeathScreenWidget = nullptr;

	FTimerHandle RespawnTimerHandle;
	FTimerHandle LocalDeathScreenTimerHandle;
	FTimerHandle LocalDeathScreenHideTimerHandle;
	FTimerHandle ASCInitializationRetryTimerHandle;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> StartupGrantedAbilityHandles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|GAS")
	bool bASCDelegatesBound = false;

	UPROPERTY(Transient)
	TObjectPtr<USpellRiseAbilitySystemComponent> ASCDelegatesBoundSource = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Default = nullptr;

	void InitASCActorInfo();
	void ScheduleASCInitializationRetry();
	void HandleASCInitializationRetry();
	bool HasValidASCActorInfo() const;
	USpellRiseAbilitySystemComponent* GetSpellRiseASC() const;
	bool InitializeAbilitySystemFromPlayerState();
	void ApplyStartupEffects();
	void BindASCDelegates();
	void RecalculateDerivedStats();
	void ApplyDerivedStatsInfinite();
	void LogDerivedDebug();
	void ResetDeathStateAndResources_Server();
	void OnPrimaryChanged(const FOnAttributeChangeData& Data);
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void ApplyRegenStartupEffects();
	void ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass);
	void SetCharacterInputEnabled(bool bEnabled);
	void ScheduleRespawn_Server();
	void ExecuteRespawn_Server();
	void RefreshCombatLockFromDamage_Server(float DamageDelta);
	bool IsCombatLockActive_Server(double* OutSecondsRemaining = nullptr) const;
	void StopAllCharacterAudio(bool bIncludeAttachedActors);
	void ResetLocalDeathPresentation();
	void TriggerLocalDamageScreenEffect();
	void ShowLocalDeathScreenText();
	void HideLocalDeathScreenText();
	void RemoveRuntimeGrantedAbilitiesOnDeath_Server();
	void DestroyDeathAttachments_Server(USkeletalMeshComponent* VisualMesh);
	void ForceServerAnimTick();
	void EnsureAnimInstanceInitialized();
	USkeletalMeshComponent* FindCharacterSkeletalMeshComponentByName(FName ComponentName) const;
	UCameraComponent* FindCharacterCameraComponentByName(FName ComponentName) const;
	void HandleArchetypeChanged(ESpellRiseArchetype OldArchetype);
	void HandleSelectedAbilityInputTagChanged(const FGameplayTag& OldTag);
	void SyncDeadStateFromASC(const TCHAR* Context);
	void SyncASCSelectedSpellFromReplicatedTag();
	bool IsAllowedServerEventTag(const FGameplayTag& EventTag) const;
	bool ValidateServerGameplayEventPayload(const FGameplayEventData& EventData, FString& OutRejectReason) const;
	bool CheckServerGameplayEventRateLimit(const FGameplayTag& EventTag, FString& OutRejectReason);
	bool ValidateServerRpcOwnerContext(FString& OutRejectReason) const;
	bool CheckServerRpcRateLimit(
		FSpellRiseServerEventRateLimitState& RateState,
		float WindowSeconds,
		int32 MaxCountPerWindow,
		const TCHAR* RpcName,
		FString& OutRejectReason);
	void AuditRejectedServerRpc(const TCHAR* RpcName, const FString& RejectReason);
	void AuditRejectedServerGameplayEvent(const FGameplayTag& EventTag, const FString& RejectReason);
	float ResolveMaxAbsServerEventMagnitude(const FGameplayTag& EventTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|GameplayEvent")
	FGameplayTagContainer AllowedServerEventTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|GameplayEvent", meta=(ClampMin="0.01"))
	float ServerGameplayEventRateLimitWindowSeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|GameplayEvent", meta=(ClampMin="1"))
	int32 ServerGameplayEventRateLimitMaxCountPerWindow = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|GameplayEvent", meta=(ClampMin="0.0"))
	float ServerGameplayEventDefaultMaxAbsMagnitude = 1.0f;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FSpellRiseServerEventRateLimitState> ServerEventRateLimitByTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|RPC", meta=(ClampMin="0.01"))
	float ServerArchetypeRpcRateLimitWindowSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|RPC", meta=(ClampMin="1"))
	int32 ServerArchetypeRpcRateLimitMaxCountPerWindow = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|RPC", meta=(ClampMin="0.01"))
	float ServerAbilitySelectionRpcRateLimitWindowSeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Security|RPC", meta=(ClampMin="1"))
	int32 ServerAbilitySelectionRpcRateLimitMaxCountPerWindow = 5;

	UPROPERTY(Transient)
	FSpellRiseServerEventRateLimitState ArchetypeRpcRateState;

	UPROPERTY(Transient)
	FSpellRiseServerEventRateLimitState AbilitySelectionRpcRateState;

	UPROPERTY(Transient)
	int32 ServerRejectedGameplayEvents = 0;

	UPROPERTY(Transient)
	int32 ServerRejectedGenericRpcs = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Death|CombatLock", meta=(ClampMin="0.0"))
	float CombatLockDurationSeconds = 8.0f;

	UPROPERTY(Transient)
	double CombatLockExpireAtServerTimeSeconds = -1.0;
};
