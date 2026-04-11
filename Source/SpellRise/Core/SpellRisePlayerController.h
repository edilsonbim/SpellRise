#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "SpellRiseChatTypes.h"
#include "SpellRisePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UGameplayAbility;
class USpellRiseNumberPopComponent_NiagaraText;
class USpellRiseAbilitySystemComponent;
class USpellRiseCombatHUDWidget;
class UNarrativeInteractionComponent;
class UUserWidget;
class UResourceAttributeSet;
class ASpellRisePlayerState;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FSpellRiseAimTraceResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	bool bHit = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FHitResult HitResult;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector TraceStart = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector TraceEnd = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector AimDirection = FVector::ForwardVector;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Aim")
	FVector TargetPoint = FVector::ZeroVector;
};

UCLASS()
class SPELLRISE_API ASpellRisePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpellRisePlayerController();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_StartCastBar(float CastDuration);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_StopCastBar();

	virtual void ProcessEvent(UFunction* Function, void* Parameters) override;

	void ShowDamageNumber(
		float Damage,
		const FVector& WorldLocation,
		const FGameplayTagContainer& SourceTags,
		const FGameplayTagContainer& TargetTags,
		bool bIsCritical
	);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Aim")
	bool GetCurrentAimTraceResult(FSpellRiseAimTraceResult& OutResult) const;

	UFUNCTION(Client, Reliable, Category="SpellRise|Chat")
	void ClientReceiveCombatLogEntry(float Damage, const FString& OtherActorName, bool bIsOutgoing);

	UFUNCTION(Client, Reliable, Category="SpellRise|Chat")
	void ClientReceiveChatMessage(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void ReceiveChatMessageLocal(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category="SpellRise|HUD")
	void SetCombatHUDSuppressedByDeath(bool bSuppressed);


	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|Inventory")
	void InventorySplitSlotSERVER(UObject* FromContainer, int32 Slot, int32 Amount);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|Inventory")
	void OnInventorySlotDropSERVER(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|Inventory")
	void Route_InventorySortBy_SERVER(UObject* InventoryRef, int32 SortBy);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat")
	void BP_OnCombatLogMessage(const FString& MessageText);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat")
	void BP_OnChatMessageReceived(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Input")
	bool IsConstructionModeActive() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void AcknowledgePossession(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced", meta=(ClampMin="0"))
	int32 DefaultMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Contexts")
	TObjectPtr<UInputMappingContext> IMC_CoreMovement = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Contexts")
	TObjectPtr<UInputMappingContext> IMC_CoreCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Contexts")
	TObjectPtr<UInputMappingContext> IMC_Combat = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Contexts")
	TObjectPtr<UInputMappingContext> IMC_Interaction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Contexts")
	TObjectPtr<UInputMappingContext> IMC_UI = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Contexts")
	TObjectPtr<UInputMappingContext> IMC_System = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Priority")
	int32 IMC_CoreMovementPriority = 0;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Priority")
	int32 IMC_CoreCameraPriority = 1;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Priority")
	int32 IMC_CombatPriority = 5;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Priority")
	int32 IMC_InteractionPriority = 6;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Priority")
	int32 IMC_UIPriority = 20;

	UPROPERTY(EditDefaultsOnly, Category="Input|Enhanced|Priority")
	int32 IMC_SystemPriority = 30;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Attack = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Primary = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Secondary = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Interact = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_ClearSelection = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Sprint = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability1 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability2 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability3 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability4 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability5 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability6 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability7 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TObjectPtr<UInputAction> IA_Ability8 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TSubclassOf<UGameplayAbility> AttackAbilityClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Feedback", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseNumberPopComponent_NiagaraText> NumberPopComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim", meta=(ClampMin="1.0"))
	float DefaultAimTraceDistance = 10000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim", meta=(ClampMin="0.0"))
	float DefaultAimTraceRadius = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim")
	TEnumAsByte<ECollisionChannel> DefaultAimTraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Aim")
	bool bDefaultAimTraceComplex = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|HUD")
	TSubclassOf<USpellRiseCombatHUDWidget> CombatHUDWidgetClass = nullptr;

	void SetupEnhancedInput();
	void RefreshEnhancedInputContexts();
	void OnAttackPressed();
	void OnAttackReleased();
	void OnPrimaryPressed();
	void OnPrimaryReleased();
	void OnSecondaryPressed();
	void OnSecondaryReleased();
	void OnInteractPressed();
	void OnInteractReleased();
	void OnClearSelectionPressed();
	void OnSprintPressed();
	void OnSprintReleased();
	void OnAbility1Pressed();
	void OnAbility1Released();
	void OnAbility2Pressed();
	void OnAbility2Released();
	void OnAbility3Pressed();
	void OnAbility3Released();
	void OnAbility4Pressed();
	void OnAbility4Released();
	void OnAbility5Pressed();
	void OnAbility5Released();
	void OnAbility6Pressed();
	void OnAbility6Released();
	void OnAbility7Pressed();
	void OnAbility7Released();
	void OnAbility8Pressed();
	void OnAbility8Released();

private:
	void LogInputFocusSnapshot(const TCHAR* SourceLabel);
	bool CanRunLocalHUDFlow(FString* OutSkipReason = nullptr) const;
	bool CanRunLocalPawnRuntime(APawn* CandidatePawn, FString* OutSkipReason = nullptr) const;
	void LogASCBindSkipReason(const FString& SkipReason);
	void HandlePawnChangedRuntime(APawn* NewPawn, const TCHAR* SourceLabel);
	void EnsureCombatHUDCreated();
	void CleanupCombatHUD();
	void TryBindHUDToCurrentASC();
	void UnbindHUDFromASC();
	void UpdateCombatHUDVisibility();
	void SetPlayerHUDWidgetsSuppressed(bool bSuppressed);
	bool IsKnownPlayerHUDWidget(const UUserWidget* Widget) const;
	void BroadcastResourcesToHUD();
	void BroadcastAbilitySlotsToHUD();
	void BroadcastTargetToHUD(const FSpellRiseAimTraceResult& AimResult);
	void OnResourceAttributeChanged(const FOnAttributeChangeData& Data);

	USpellRiseAbilitySystemComponent* GetSpellRiseASCFromPawn() const;
	void SendAbilityInputTagPressed(FGameplayTag InputTag);
	void SendAbilityInputTagReleased(FGameplayTag InputTag);
	void HandleAbilitySlotPressed(int32 SlotIndex);
	void HandleAbilitySlotReleased(int32 SlotIndex);
	bool TryExecuteNarrativeInteract(bool bPressed) const;
	bool IsControlledCharacterDead() const;
	bool IsGameplayInputBlocked() const;
	bool IsPersistenceProfileReady() const;
	UNarrativeInteractionComponent* ResolveNarrativeInteractionComponent() const;

	bool BuildAimTraceResult(
		FSpellRiseAimTraceResult& OutResult,
		float TraceDistance,
		float TraceRadius,
		ECollisionChannel TraceChannel,
		bool bTraceComplex) const;

	void CollectAimIgnoredActors(TArray<AActor*>& OutActorsToIgnore) const;

	void PushCombatLogMessage(const FString& MessageText);
	void FlushPendingChatMessages();
	bool TryAppendPendingChatMessage(const FSpellRiseChatMessage& Message) const;
	bool IsChatWidgetReadyForAppend() const;
	bool TryAppendCombatLogToChatChannel(const FString& MessageText) const;
	bool ShouldEnableUIInputContext() const;
	UObject* FindChatComponentObject() const;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Chat")
	bool bEnableLegacyReflectionChatRouting = false;

	UPROPERTY(Transient)
	TArray<FSpellRiseChatMessage> PendingChatMessages;

	UPROPERTY(Transient)
	TObjectPtr<USpellRiseCombatHUDWidget> CombatHUDWidget = nullptr;

	UPROPERTY(Transient)
	bool bCombatHUDHiddenByDeath = false;

	UPROPERTY(Transient)
	bool bCombatHUDSuppressedByDeath = false;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UUserWidget>> DeathSuppressedHUDWidgets;

	TWeakObjectPtr<USpellRiseAbilitySystemComponent> HUDObservedASC;
	FDelegateHandle HUDHealthChangedHandle;
	FDelegateHandle HUDManaChangedHandle;
	UPROPERTY(Transient)
	double LastInputFocusSnapshotTimeSeconds = 0.0;

	UPROPERTY(Transient)
	FString LastInputFocusSnapshotSignature;
	FDelegateHandle HUDStaminaChangedHandle;

	double NextHUDAbilitiesRefreshTimeSeconds = 0.0;
	double NextHUDTargetRefreshTimeSeconds = 0.0;
	bool bLastKnownPersistenceProfileReady = false;

	UPROPERTY(Transient)
	bool bLoggedHUDCreateAttempt = false;

	UPROPERTY(Transient)
	bool bLoggedHUDCreateSuccess = false;

	UPROPERTY(Transient)
	FString LastHUDCreateFailureReason;

	UPROPERTY(Transient)
	FString LastASCBindSkipReason;

protected:
};
