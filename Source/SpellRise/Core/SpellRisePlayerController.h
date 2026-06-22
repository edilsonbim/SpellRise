#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"
#include "SpellRiseChatTypes.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityHotbarComponent.h"
#include "SpellRisePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UGameplayAbility;
class USpellRiseNumberPopComponent_NiagaraText;
class USpellRiseAbilitySystemComponent;
class UNarrativeInteractionComponent;
class UNarrativeInventoryComponent;
class UActorComponent;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellRiseActiveAbilityHotbarGroupChanged,
	ESpellRiseAbilityHotbarGroup,
	NewGroup);

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

	UFUNCTION(Client, Reliable, Category="SpellRise|Chat")
	void ClientReceiveCombatLogEntry(float Damage, const FString& OtherActorName, bool bIsOutgoing);

	UFUNCTION(Client, Reliable, Category="SpellRise|Chat")
	void ClientReceiveChatMessage(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void ReceiveChatMessageLocal(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SubmitChatMessage(const FText& Text, uint8 Channel);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SubmitChatMessageForConversation(
		const FText& Text,
		uint8 Channel,
		const FString& ConversationId);

	UFUNCTION(Server, Reliable)
	void ServerSubmitChatMessage(const FString& Text, uint8 Channel);

	UFUNCTION(Client, Unreliable)
	void ClientReceivePublicChatMessage(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintPure, Category="SpellRise|Chat")
	const TArray<FSpellRiseChatMessage>& GetNativeChatHistory() const { return NativeChatHistory; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Chat|Whisper")
	TArray<FSpellRiseChatMessage> GetWhisperConversationHistory(const FString& ConversationId) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat|Whisper")
	void SetActiveWhisperConversation(const FString& ConversationId);

	UFUNCTION(BlueprintPure, Category="SpellRise|Chat|Whisper")
	int32 GetWhisperUnreadCount(const FString& ConversationId) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SetActiveChatTab(const FString& TabId);

	UFUNCTION(BlueprintPure, Category="SpellRise|Chat")
	int32 GetChatUnreadCount(const FString& TabId) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat|Whisper")
	void SetWhisperBlocked(const FString& ConversationId, bool bBlocked);

	UFUNCTION(Server, Reliable)
	void ServerSetWhisperBlocked(const FString& ConversationId, bool bBlocked);

	UFUNCTION(BlueprintPure, Category="SpellRise|Chat")
	static uint8 GetWhisperChatChannel() { return SpellRiseChatChannel::Whisper; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat|Whisper")
	void SendWhisperToConversation(const FString& ConversationId, const FText& Text);

	UFUNCTION(Server, Reliable)
	void ServerSendWhisperToConversation(const FString& ConversationId, const FString& Text);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|Inventory")
	void InventorySplitSlotSERVER(UObject* FromContainer, int32 Slot, int32 Amount);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|Inventory")
	void OnInventorySlotDropSERVER(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="SpellRise|Inventory")
	void Route_InventorySortBy_SERVER(UObject* InventoryRef, int32 SortBy);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Input")
	void RestoreGameplayInputAfterUI(const FName Source);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Input")
	void ToggleUIInteractionMode();

	UFUNCTION(BlueprintPure, Category="SpellRise|Input")
	bool IsUIInteractionModeActive() const { return bUIInteractionModeActive; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Talents")
	UActorComponent* ResolveTalentTreeComponentForUI() const;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Hotbar|Events")
	FSpellRiseActiveAbilityHotbarGroupChanged OnActiveAbilityHotbarGroupChanged;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar|Input")
	void SetActiveAbilityHotbarGroup(ESpellRiseAbilityHotbarGroup NewGroup);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar|Input")
	void ToggleActiveAbilityHotbarGroup();

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar|Input")
	ESpellRiseAbilityHotbarGroup GetActiveAbilityHotbarGroup() const { return ActiveAbilityHotbarGroup; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Hotbar|Input")
	FText GetAbilityHotbarPhysicalSlotInputText(int32 PhysicalSlotIndex) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar|Input")
	void PressAbilityHotbarPhysicalSlot(int32 PhysicalSlotIndex);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Hotbar|Input")
	void ReleaseAbilityHotbarPhysicalSlot(int32 PhysicalSlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_OnLocalHUDContextChanged(APawn* NewPawn, APawn* PreviousPawn, FName Source);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat")
	void BP_OnCombatLogMessage(const FString& MessageText);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat")
	void BP_OnChatMessageReceived(const FSpellRiseChatMessage& Message);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat|Whisper")
	void BP_OnWhisperConversationReceived(
		const FString& ConversationId,
		const FString& ConversationName,
		bool bOutgoing);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat|Whisper")
	void BP_OnWhisperUnreadChanged(const FString& ConversationId, int32 UnreadCount);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Chat")
	void BP_OnChatUnreadChanged(const FString& TabId, int32 UnreadCount);

	APawn* GetLastSpellRiseControlledPawn() const;

	bool HandleServerChatCommand(const FString& RawMessage);

protected:
	virtual void BeginPlay() override;
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

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Debug|Progression")
	FKey DebugGrantExperienceKey = EKeys::F9;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Debug|Progression", meta=(ClampMin="1.0", UIMin="1.0"))
	float DebugExperienceGrantAmount = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category="Input|UI")
	FKey ToggleUIInteractionKey = EKeys::F10;

	UPROPERTY(EditDefaultsOnly, Category="Input|Actions")
	TSubclassOf<UGameplayAbility> AttackAbilityClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Feedback", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpellRiseNumberPopComponent_NiagaraText> NumberPopComponent = nullptr;

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
	void OnToggleUIInteractionPressed();
	void OnDebugGrantExperiencePressed();

	UFUNCTION(Server, Reliable)
	void ServerGrantDebugExperience();

private:
	bool TryHandleAdminChatCommand(const FString& RawMessage);
	bool TryHandleWhisperChatCommand(const FString& RawMessage);
	bool DeliverWhisper_Server(ASpellRisePlayerController* TargetController, const FString& MessageText);
	bool CheckWhisperRateLimit_Server(FString& OutRejectReason);
	ASpellRisePlayerController* FindPlayerControllerByConversationId_Server(const FString& ConversationId) const;
	FString ResolveWhisperConversationId_Server(const ASpellRisePlayerController* Controller) const;
	void SendAdminSystemMessage(const FString& MessageText);
	bool IsAdminCommandRateLimited();

	void LogInputFocusSnapshot(const TCHAR* SourceLabel);
	bool CanRunLocalHUDFlow(FString* OutSkipReason = nullptr) const;
	void HandlePawnChangedRuntime(APawn* NewPawn, const TCHAR* SourceLabel);

	USpellRiseAbilitySystemComponent* GetSpellRiseASCFromPawn() const;
	void SendAbilityInputTagPressed(FGameplayTag InputTag);
	void SendAbilityInputTagReleased(FGameplayTag InputTag);
	void HandleAbilitySlotPressed(int32 SlotIndex);
	void HandleAbilitySlotReleased(int32 SlotIndex);
	bool TryExecuteNarrativeInteract(bool bPressed) const;
	bool IsControlledCharacterDead() const;
	bool IsGameplayInputBlocked() const;
	UNarrativeInteractionComponent* ResolveNarrativeInteractionComponent() const;
	UNarrativeInventoryComponent* ResolveNarrativeInventoryComponentForUI() const;
	bool TryStopLootingFromUIInput(const FName Source);
	void AuditRejectedInventoryRpc(const TCHAR* RpcName, const FString& RejectReason);

	void PushCombatLogMessage(const FString& MessageText);
	bool ShouldEnableUIInputContext() const;

	UPROPERTY(Transient)
	double LastDebugExperienceGrantTimeSeconds = -1.0;

	UPROPERTY(Transient)
	double LastAdminCommandTimeSeconds = -1.0;

	UPROPERTY(Transient)
	double WhisperRateWindowStartSeconds = 0.0;

	UPROPERTY(Transient)
	int32 WhisperRateCountInWindow = 0;

	UPROPERTY(Transient)
	FString LastWhisperConversationId;

	UPROPERTY(Transient)
	FString ActiveWhisperConversationId;

	UPROPERTY(Transient)
	TMap<FString, int32> WhisperUnreadByConversation;

	UPROPERTY(Transient)
	FString ActiveChatTabId;

	UPROPERTY(Transient)
	TMap<FString, int32> ChatUnreadByTabId;

	UPROPERTY(Transient)
	TSet<FString> BlockedWhisperConversationIds;

	UPROPERTY(Transient, BlueprintReadOnly, Category="SpellRise|Chat", meta=(AllowPrivateAccess="true"))
	TArray<FSpellRiseChatMessage> NativeChatHistory;

	UPROPERTY(Transient)
	bool bAdminAuthenticated = false;

	UPROPERTY(Transient)
	float SavedMaxFlySpeed = 600.0f;

	TMap<TWeakObjectPtr<UPrimitiveComponent>, FCollisionResponseContainer> AdminInvisibleOriginalCollisionResponses;

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> AdminOriginalPawn;

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> AdminFlyPawn;

	UPROPERTY(Transient)
	bool bUIInteractionModeActive = false;

	UPROPERTY(Transient)
	double LastInputFocusSnapshotTimeSeconds = 0.0;

	UPROPERTY(Transient)
	FString LastInputFocusSnapshotSignature;

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> LastSpellRiseControlledPawn;

	UPROPERTY(Transient)
	ESpellRiseAbilityHotbarGroup ActiveAbilityHotbarGroup = ESpellRiseAbilityHotbarGroup::Weapon;

	UPROPERTY(Transient)
	TMap<FString, int32> RejectedInventoryRpcCountByReason;

protected:
};
