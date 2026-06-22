// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRisePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "InteractionComponent.h"
#include "Misc/DateTime.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/UnrealType.h"

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Components/SpellRiseChatComponent.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Core/SpellRiseGameState.h"
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePlayerControllerRuntime, Log, All);

namespace
{
	const FName NAME_SendChatToSERVER(TEXT("SendChatToSERVER"));
	constexpr int32 MaxChatNameChars = 32;
	constexpr int32 MaxChatTextChars = 256;
	constexpr int32 MaxCombatActorNameChars = 64;
	constexpr int32 MaxCombatLogMessageChars = 192;
	constexpr double MinimumAdminCommandIntervalSeconds = 0.20;
	constexpr double WhisperRateWindowSeconds = 2.0;
	constexpr int32 WhisperRateMaxCountPerWindow = 4;
	constexpr int32 MaxWhisperConversationIdChars = 64;
	constexpr int32 MaxChatTabIdChars = 64;

	const FGameplayTag& InputTag_Primary()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Primary"));
		return Tag;
	}

	const FGameplayTag& InputTag_Secondary()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Secondary"));
		return Tag;
	}

	const FGameplayTag& InputTag_Interact()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Utility.Interact"));
		return Tag;
	}

	const FGameplayTag& InputTag_Sprint()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Utility.Sprint"));
		return Tag;
	}

	FText BuildCombatTimeText()
	{
		const FDateTime Now = FDateTime::Now();
		return FText::FromString(Now.ToString(TEXT("%H:%M")));
	}

	FString SanitizeBoundedString(FString Value, const int32 MaxChars, const TCHAR* Fallback = TEXT(""))
	{
		Value.TrimStartAndEndInline();
		Value.ReplaceInline(TEXT("\r"), TEXT(" "));
		Value.ReplaceInline(TEXT("\n"), TEXT(" "));
		Value.ReplaceInline(TEXT("\t"), TEXT(" "));

		const int32 SafeMaxChars = FMath::Max(1, MaxChars);
		if (Value.Len() > SafeMaxChars)
		{
			Value.LeftInline(SafeMaxChars, EAllowShrinking::No);
		}

		if (Value.IsEmpty() && Fallback)
		{
			Value = Fallback;
		}

		return Value;
	}

	FSpellRiseChatMessage SanitizeChatMessageForLocalDelivery(const FSpellRiseChatMessage& Message)
	{
		FSpellRiseChatMessage Sanitized = Message;
		Sanitized.Name = FName(*SanitizeBoundedString(Message.Name.ToString(), MaxChatNameChars, TEXT("System")));
		Sanitized.Text = FText::FromString(SanitizeBoundedString(Message.Text.ToString(), MaxChatTextChars));
		Sanitized.TimeText = FText::FromString(SanitizeBoundedString(Message.TimeText.ToString(), 16));
		Sanitized.ConversationId = SanitizeBoundedString(Message.ConversationId, MaxWhisperConversationIdChars);
		Sanitized.ConversationName = SanitizeBoundedString(Message.ConversationName, MaxChatNameChars);
		return Sanitized;
	}

	FString ResolveChatTabId(const FSpellRiseChatMessage& Message)
	{
		if (Message.Channel == SpellRiseChatChannel::Whisper)
		{
			return SanitizeBoundedString(Message.ConversationId, MaxChatTabIdChars);
		}

		switch (Message.Channel)
		{
		case SpellRiseChatChannel::Global:
			return TEXT("GLOBAL");
		case SpellRiseChatChannel::Party:
			return TEXT("PARTY");
		case SpellRiseChatChannel::Guild:
			return TEXT("GUILD");
		case SpellRiseChatChannel::Combat:
			return TEXT("COMBAT");
		default:
			return FString();
		}
	}

	bool TryExtractChatText(UFunction* Function, void* Parameters, FString& OutText)
	{
		OutText.Reset();
		if (!Function || !Parameters)
		{
			return false;
		}

		for (TFieldIterator<FProperty> It(Function); It; ++It)
		{
			FProperty* Property = *It;
			if (!Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			const FString PropertyName = Property->GetName();
			if (!PropertyName.Contains(TEXT("Text"), ESearchCase::IgnoreCase)
				&& !PropertyName.Contains(TEXT("Message"), ESearchCase::IgnoreCase))
			{
				continue;
			}

			void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Parameters);
			if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
			{
				OutText = TextProperty->GetPropertyValue(ValuePtr).ToString();
				return true;
			}
			if (const FStrProperty* StringProperty = CastField<FStrProperty>(Property))
			{
				OutText = StringProperty->GetPropertyValue(ValuePtr);
				return true;
			}
		}

		return false;
	}

	bool TryReadChannelPropertyValue(const FProperty* Property, const void* ValuePtr, FString& OutChannelName, int64& OutChannelValue)
	{
		if (!Property || !ValuePtr)
		{
			return false;
		}

		if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			if (const FNumericProperty* UnderlyingNumeric = EnumProperty->GetUnderlyingProperty())
			{
				OutChannelValue = UnderlyingNumeric->GetSignedIntPropertyValue(ValuePtr);
				if (const UEnum* Enum = EnumProperty->GetEnum())
				{
					OutChannelName = Enum->GetNameStringByValue(OutChannelValue);
				}
				return true;
			}
		}

		if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			OutChannelValue = ByteProperty->GetPropertyValue(ValuePtr);
			if (ByteProperty->Enum)
			{
				OutChannelName = ByteProperty->Enum->GetNameStringByValue(OutChannelValue);
			}
			return true;
		}

		if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			OutChannelName = NameProperty->GetPropertyValue(ValuePtr).ToString();
			return true;
		}

		if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
		{
			OutChannelName = StrProperty->GetPropertyValue(ValuePtr);
			return true;
		}

		if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			OutChannelName = TextProperty->GetPropertyValue(ValuePtr).ToString();
			return true;
		}

		return false;
	}

	bool ExtractChannelFromPropertyRecursive(const FProperty* Property, const void* ContainerPtr, FString& OutChannelName, int64& OutChannelValue)
	{
		if (!Property || !ContainerPtr)
		{
			return false;
		}

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(const_cast<void*>(ContainerPtr));

		if (Property->GetName().Contains(TEXT("Channel"), ESearchCase::IgnoreCase) &&
			TryReadChannelPropertyValue(Property, ValuePtr, OutChannelName, OutChannelValue))
		{
			return true;
		}

		if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			const void* StructValuePtr = StructProperty->ContainerPtrToValuePtr<void>(const_cast<void*>(ContainerPtr));

			for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
			{
				if (ExtractChannelFromPropertyRecursive(*It, StructValuePtr, OutChannelName, OutChannelValue))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsCombatChannelDescriptor(const FString& ChannelName, int64 ChannelValue)
	{
		if (ChannelName.Contains(TEXT("Combat"), ESearchCase::IgnoreCase))
		{
			return true;
		}

		return ChannelValue == 3;
	}

	bool SR_IsAbilityInputTag(const FGameplayTag& InputTag)
	{
		if (!InputTag.IsValid())
		{
			return false;
		}

		return InputTag.ToString().StartsWith(TEXT("InputTag.Ability."));
	}

	bool SR_IsTalentTreeComponentForUI(const UActorComponent* Component)
	{
		if (!Component || !Component->GetClass())
		{
			return false;
		}

		const FString ComponentName = Component->GetName();
		const FString ClassPath = Component->GetClass()->GetPathName();
		return ComponentName.Contains(TEXT("TalentTreeComponent"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT("TalentTreeComponent"), ESearchCase::IgnoreCase);
	}

	UActorComponent* SR_FindTalentTreeComponentForUI(AActor* Owner)
	{
		if (!Owner)
		{
			return nullptr;
		}

		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (SR_IsTalentTreeComponentForUI(Component))
			{
				return Component;
			}
		}

		return nullptr;
	}

	bool SR_CanSendPawnOwnedInputRpc(const APlayerController* PlayerController, const APawn* Pawn, FString* OutReason = nullptr)
	{
		if (!PlayerController)
		{
			if (OutReason)
			{
				*OutReason = TEXT("missing_player_controller");
			}
			return false;
		}

		if (!PlayerController->IsLocalController())
		{
			if (OutReason)
			{
				*OutReason = TEXT("controller_not_local");
			}
			return false;
		}

		if (!Pawn)
		{
			if (OutReason)
			{
				*OutReason = TEXT("missing_pawn");
			}
			return false;
		}

		if (Pawn->GetController() != PlayerController)
		{
			if (OutReason)
			{
				*OutReason = TEXT("pawn_controller_mismatch");
			}
			return false;
		}

		if (!Pawn->IsLocallyControlled())
		{
			if (OutReason)
			{
				*OutReason = TEXT("pawn_not_locally_controlled");
			}
			return false;
		}

		return true;
	}

	void UpdateEnhancedInputContext(
		UEnhancedInputLocalPlayerSubsystem* Subsystem,
		UInputMappingContext* Context,
		int32 Priority,
		bool bShouldBeActive,
		const TCHAR* ContextLabel)
	{
		if (!Subsystem || !Context || !ContextLabel)
		{
			return;
		}

		const bool bHasContext = Subsystem->HasMappingContext(Context);
		if (bShouldBeActive)
		{
			if (!bHasContext)
			{
				Subsystem->AddMappingContext(Context, Priority);
			}

			return;
		}

		if (bHasContext)
		{
			Subsystem->RemoveMappingContext(Context);
		}
	}
}

ASpellRisePlayerController::ASpellRisePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bShowMouseCursor = false;
	NumberPopComponent = CreateDefaultSubobject<USpellRiseNumberPopComponent_NiagaraText>(TEXT("NumberPopComponent"));
}

void ASpellRisePlayerController::ProcessEvent(UFunction* Function, void* Parameters)
{
	if (Function && IsControlledCharacterDead())
	{
		const FString FunctionName = Function->GetName();
		if (FunctionName.Contains(TEXT("InpActEvt_"), ESearchCase::CaseSensitive) ||
			FunctionName.Contains(TEXT("EnhancedInputAction"), ESearchCase::IgnoreCase))
		{
			return;
		}
	}

	if (HasAuthority() &&
		Function &&
		Function->GetFName() == NAME_SendChatToSERVER &&
		Function->HasAnyFunctionFlags(FUNC_NetServer))
	{
		FString RawMessage;
		FString ChannelName;
		int64 ChannelValue = SpellRiseChatChannel::Global;

		for (TFieldIterator<FProperty> It(Function); It; ++It)
		{
			FProperty* Property = *It;
			if (!Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			if (ExtractChannelFromPropertyRecursive(Property, Parameters, ChannelName, ChannelValue))
			{
				break;
			}
		}

		if (TryExtractChatText(Function, Parameters, RawMessage))
		{
			const uint8 SafeChannel = ChannelValue >= 0 && ChannelValue <= TNumericLimits<uint8>::Max()
				? static_cast<uint8>(ChannelValue)
				: SpellRiseChatChannel::Global;
			if (const ASpellRiseGameState* SRGameState =
				GetWorld() ? GetWorld()->GetGameState<ASpellRiseGameState>() : nullptr)
			{
				if (USpellRiseChatComponent* ChatComponent = SRGameState->GetChatComponent())
				{
					ChatComponent->HandleClientMessage(this, RawMessage, SafeChannel);
				}
			}
			return;
		}
	}

	Super::ProcessEvent(Function, Parameters);
}

void ASpellRisePlayerController::SubmitChatMessage(const FText& Text, const uint8 Channel)
{
	SubmitChatMessageForConversation(Text, Channel, ActiveWhisperConversationId);
}

void ASpellRisePlayerController::SubmitChatMessageForConversation(
	const FText& Text,
	const uint8 Channel,
	const FString& ConversationId)
{
	const FString SafeText = SanitizeBoundedString(Text.ToString(), MaxChatTextChars);
	if (!IsLocalController() || SafeText.IsEmpty())
	{
		return;
	}

	if (SafeText.Equals(TEXT("/clear"), ESearchCase::IgnoreCase))
	{
		ClearActiveChatHistory();
		return;
	}

	if (Channel == SpellRiseChatChannel::Whisper)
	{
		const FString SafeConversationId =
			SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
		if (SafeConversationId.IsEmpty())
		{
			UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
				TEXT("[Chat][SubmitRejected] Reason=missing_active_whisper_conversation Controller=%s"),
				*GetNameSafe(this));
			return;
		}

		ActiveWhisperConversationId = SafeConversationId;
		SendWhisperToConversation(SafeConversationId, FText::FromString(SafeText));
		return;
	}

	ServerSubmitChatMessage(SafeText, Channel);
}

void ASpellRisePlayerController::ServerSubmitChatMessage_Implementation(
	const FString& Text,
	const uint8 Channel)
{
	const FString SafeText = SanitizeBoundedString(Text, MaxChatTextChars);
	if (SafeText.IsEmpty())
	{
		return;
	}

	const ASpellRiseGameState* SRGameState =
		GetWorld() ? GetWorld()->GetGameState<ASpellRiseGameState>() : nullptr;
	USpellRiseChatComponent* ChatComponent = SRGameState ? SRGameState->GetChatComponent() : nullptr;
	if (!ChatComponent)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Error,
			TEXT("[Chat][SubmitRejected] Reason=missing_chat_service Controller=%s"),
			*GetNameSafe(this));
		return;
	}
	ChatComponent->HandleClientMessage(this, SafeText, Channel);
}

bool ASpellRisePlayerController::HandleServerChatCommand(const FString& RawMessage)
{
	return TryHandleWhisperChatCommand(RawMessage) || TryHandleAdminChatCommand(RawMessage);
}

void ASpellRisePlayerController::ClearActiveChatHistory()
{
	if (!IsLocalController())
	{
		return;
	}

	const FString TabId = ActiveChatTabId.IsEmpty() ? TEXT("GLOBAL") : ActiveChatTabId;
	NativeChatHistory.RemoveAll([&TabId](const FSpellRiseChatMessage& Message)
	{
		return ResolveChatTabId(Message).Equals(TabId, ESearchCase::CaseSensitive);
	});
	ChatUnreadByTabId.Remove(TabId);
	BP_OnChatUnreadChanged(TabId, 0);
	if (TabId.Equals(ActiveWhisperConversationId, ESearchCase::CaseSensitive))
	{
		WhisperUnreadByConversation.Remove(TabId);
		BP_OnWhisperUnreadChanged(TabId, 0);
	}
	BP_OnChatHistoryCleared(TabId);
}

FString ASpellRisePlayerController::ResolveWhisperConversationId_Server(
	const ASpellRisePlayerController* Controller) const
{
	if (!Controller || !Controller->PlayerState)
	{
		return FString();
	}

	if (const FUniqueNetIdRepl& UniqueId = Controller->PlayerState->GetUniqueId(); UniqueId.IsValid())
	{
		return SanitizeBoundedString(UniqueId->ToString(), MaxWhisperConversationIdChars);
	}

	return SanitizeBoundedString(
		FString::Printf(TEXT("NAME:%s"), *Controller->PlayerState->GetPlayerName()),
		MaxWhisperConversationIdChars);
}

ASpellRisePlayerController* ASpellRisePlayerController::FindPlayerControllerByConversationId_Server(
	const FString& ConversationId) const
{
	if (!GetWorld() || ConversationId.IsEmpty())
	{
		return nullptr;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		if (Candidate && ResolveWhisperConversationId_Server(Candidate).Equals(ConversationId, ESearchCase::CaseSensitive))
		{
			return Candidate;
		}
	}
	return nullptr;
}

bool ASpellRisePlayerController::CheckWhisperRateLimit_Server(FString& OutRejectReason)
{
	if (!HasAuthority())
	{
		OutRejectReason = TEXT("not_authority");
		return false;
	}

	const UWorld* World = GetWorld();
	const double Now = World ? World->GetTimeSeconds() : 0.0;
	if (Now - WhisperRateWindowStartSeconds > WhisperRateWindowSeconds)
	{
		WhisperRateWindowStartSeconds = Now;
		WhisperRateCountInWindow = 0;
	}

	++WhisperRateCountInWindow;
	if (WhisperRateCountInWindow > WhisperRateMaxCountPerWindow)
	{
		OutRejectReason = TEXT("rate_limit");
		return false;
	}
	return true;
}

bool ASpellRisePlayerController::DeliverWhisper_Server(
	ASpellRisePlayerController* TargetController,
	const FString& MessageText)
{
	if (!HasAuthority() || !PlayerState || !TargetController || !TargetController->PlayerState)
	{
		return false;
	}

	if (TargetController == this)
	{
		SendAdminSystemMessage(TEXT("Whisper rejeitado: voce nao pode enviar mensagem para si mesmo."));
		return true;
	}

	const FString SafeText = SanitizeBoundedString(MessageText, MaxChatTextChars);
	if (SafeText.IsEmpty())
	{
		SendAdminSystemMessage(TEXT("Uso: /w Player Mensagem"));
		return true;
	}

	FString RejectReason;
	if (!CheckWhisperRateLimit_Server(RejectReason))
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
			TEXT("[Chat][WhisperRejected] Reason=%s Sender=%s"),
			*RejectReason,
			*GetNameSafe(PlayerState));
		SendAdminSystemMessage(TEXT("Whisper rejeitado: muitas mensagens em pouco tempo."));
		return true;
	}

	const FString SenderName = SanitizeBoundedString(PlayerState->GetPlayerName(), MaxChatNameChars, TEXT("Player"));
	const FString TargetName = SanitizeBoundedString(TargetController->PlayerState->GetPlayerName(), MaxChatNameChars, TEXT("Player"));
	const FString SenderId = ResolveWhisperConversationId_Server(this);
	const FString TargetId = ResolveWhisperConversationId_Server(TargetController);
	if (SenderId.IsEmpty() || TargetId.IsEmpty())
	{
		SendAdminSystemMessage(TEXT("Whisper rejeitado: identidade da conversa indisponivel."));
		return true;
	}
	if (TargetController->BlockedWhisperConversationIds.Contains(SenderId))
	{
		SendAdminSystemMessage(TEXT("Whisper rejeitado: destinatario indisponivel."));
		return true;
	}

	FSpellRiseChatMessage SenderMessage;
	SenderMessage.Name = FName(*FString::Printf(TEXT("To %s"), *TargetName));
	SenderMessage.Text = FText::FromString(SafeText);
	SenderMessage.TimeText = BuildCombatTimeText();
	SenderMessage.Channel = SpellRiseChatChannel::Whisper;
	SenderMessage.ConversationId = TargetId;
	SenderMessage.ConversationName = TargetName;
	SenderMessage.bOutgoing = true;

	FSpellRiseChatMessage TargetMessage;
	TargetMessage.Name = FName(*FString::Printf(TEXT("From %s"), *SenderName));
	TargetMessage.Text = FText::FromString(SafeText);
	TargetMessage.TimeText = SenderMessage.TimeText;
	TargetMessage.Channel = SpellRiseChatChannel::Whisper;
	TargetMessage.ConversationId = SenderId;
	TargetMessage.ConversationName = SenderName;
	TargetMessage.bOutgoing = false;

	LastWhisperConversationId = TargetId;
	TargetController->LastWhisperConversationId = SenderId;
	ClientReceiveChatMessage(SenderMessage);
	TargetController->ClientReceiveChatMessage(TargetMessage);
	return true;
}

bool ASpellRisePlayerController::TryHandleWhisperChatCommand(const FString& RawMessage)
{
	if (!HasAuthority())
	{
		return false;
	}

	const FString Command = RawMessage.TrimStartAndEnd();
	const bool bReply = Command.StartsWith(TEXT("/r "), ESearchCase::IgnoreCase);
	const bool bWhisper = Command.StartsWith(TEXT("/w "), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/whisper "), ESearchCase::IgnoreCase);
	if (!bReply && !bWhisper)
	{
		return false;
	}

	if (bReply)
	{
		if (LastWhisperConversationId.IsEmpty())
		{
			SendAdminSystemMessage(TEXT("Whisper rejeitado: nenhuma conversa recente."));
			return true;
		}
		ASpellRisePlayerController* TargetController =
			FindPlayerControllerByConversationId_Server(LastWhisperConversationId);
		if (!TargetController)
		{
			SendAdminSystemMessage(TEXT("Whisper rejeitado: player offline."));
			return true;
		}
		return DeliverWhisper_Server(TargetController, Command.RightChop(3).TrimStartAndEnd());
	}

	const int32 PrefixLength = Command.StartsWith(TEXT("/whisper "), ESearchCase::IgnoreCase) ? 9 : 3;
	const FString Payload = Command.RightChop(PrefixLength).TrimStartAndEnd();
	FString TargetName;
	FString MessageText;
	if (Payload.StartsWith(TEXT("\"")))
	{
		const int32 ClosingQuote = Payload.Find(TEXT("\""), ESearchCase::CaseSensitive, ESearchDir::FromStart, 1);
		if (ClosingQuote == INDEX_NONE)
		{
			SendAdminSystemMessage(TEXT("Uso: /w \"Nome do Player\" Mensagem"));
			return true;
		}
		TargetName = Payload.Mid(1, ClosingQuote - 1);
		MessageText = Payload.Mid(ClosingQuote + 1).TrimStartAndEnd();
	}
	else if (!Payload.Split(TEXT(" "), &TargetName, &MessageText))
	{
		SendAdminSystemMessage(TEXT("Uso: /w Player Mensagem"));
		return true;
	}

	ASpellRisePlayerController* Match = nullptr;
	int32 MatchCount = 0;
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
			if (Candidate && Candidate->PlayerState
				&& Candidate->PlayerState->GetPlayerName().Equals(TargetName, ESearchCase::IgnoreCase))
			{
				Match = Candidate;
				++MatchCount;
			}
		}
	}

	if (MatchCount != 1 || !Match)
	{
		SendAdminSystemMessage(MatchCount > 1
			? TEXT("Whisper rejeitado: nome ambiguo.")
			: TEXT("Whisper rejeitado: player offline ou nao encontrado."));
		return true;
	}
	return DeliverWhisper_Server(Match, MessageText);
}

void ASpellRisePlayerController::SendWhisperToConversation(
	const FString& ConversationId,
	const FText& Text)
{
	const FString SafeConversationId = SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
	const FString SafeText = SanitizeBoundedString(Text.ToString(), MaxChatTextChars);
	if (!IsLocalController() || SafeConversationId.IsEmpty() || SafeText.IsEmpty())
	{
		return;
	}
	ServerSendWhisperToConversation(SafeConversationId, SafeText);
}

bool ASpellRisePlayerController::OpenWhisperWithPlayerName(const FString& DisplayedPlayerName)
{
	if (!IsLocalController() || !GetWorld())
	{
		return false;
	}

	FString TargetName = SanitizeBoundedString(DisplayedPlayerName, MaxChatNameChars);
	const int32 LevelSuffixIndex = TargetName.Find(TEXT(" ["), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (LevelSuffixIndex != INDEX_NONE && TargetName.EndsWith(TEXT("]")))
	{
		TargetName.LeftInline(LevelSuffixIndex, EAllowShrinking::No);
		TargetName.TrimEndInline();
	}
	if (TargetName.StartsWith(TEXT("From "), ESearchCase::IgnoreCase))
	{
		TargetName.RightChopInline(5, EAllowShrinking::No);
	}
	else if (TargetName.StartsWith(TEXT("To "), ESearchCase::IgnoreCase))
	{
		TargetName.RightChopInline(3, EAllowShrinking::No);
	}
	if (TargetName.IsEmpty())
	{
		return false;
	}

	APlayerState* Match = nullptr;
	int32 MatchCount = 0;
	const AGameStateBase* GameState = GetWorld()->GetGameState();
	if (GameState)
	{
		for (APlayerState* Candidate : GameState->PlayerArray)
		{
			if (Candidate && Candidate->GetPlayerName().Equals(TargetName, ESearchCase::IgnoreCase))
			{
				Match = Candidate;
				++MatchCount;
			}
		}
	}
	if (MatchCount != 1 || !Match || Match == PlayerState)
	{
		return false;
	}

	const FString ConversationId = Match->GetUniqueId().IsValid()
		? SanitizeBoundedString(Match->GetUniqueId()->ToString(), MaxWhisperConversationIdChars)
		: SanitizeBoundedString(
			FString::Printf(TEXT("NAME:%s"), *Match->GetPlayerName()),
			MaxWhisperConversationIdChars);
	const FString ConversationName =
		SanitizeBoundedString(Match->GetPlayerName(), MaxChatNameChars, TEXT("Player"));
	if (ConversationId.IsEmpty())
	{
		return false;
	}

	ActiveWhisperConversationId = ConversationId;
	BP_OnWhisperConversationReceived(ConversationId, ConversationName, true);
	BP_OnWhisperOpenRequested(ConversationId, ConversationName);
	SetActiveChatTab(ConversationId);
	return true;
}

void ASpellRisePlayerController::SetWhisperBlocked(
	const FString& ConversationId,
	const bool bBlocked)
{
	const FString SafeConversationId = SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
	if (!IsLocalController() || SafeConversationId.IsEmpty())
	{
		return;
	}

	if (bBlocked)
	{
		BlockedWhisperConversationIds.Add(SafeConversationId);
	}
	else
	{
		BlockedWhisperConversationIds.Remove(SafeConversationId);
	}
	ServerSetWhisperBlocked(SafeConversationId, bBlocked);
}

void ASpellRisePlayerController::ServerSetWhisperBlocked_Implementation(
	const FString& ConversationId,
	const bool bBlocked)
{
	const FString SafeConversationId = SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
	if (SafeConversationId.IsEmpty() || SafeConversationId == ResolveWhisperConversationId_Server(this))
	{
		return;
	}
	if (bBlocked)
	{
		BlockedWhisperConversationIds.Add(SafeConversationId);
	}
	else
	{
		BlockedWhisperConversationIds.Remove(SafeConversationId);
	}
}

void ASpellRisePlayerController::SetActiveWhisperConversation(const FString& ConversationId)
{
	if (!IsLocalController())
	{
		return;
	}
	ActiveWhisperConversationId = SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
	if (!ActiveWhisperConversationId.IsEmpty())
	{
		SetActiveChatTab(ActiveWhisperConversationId);
	}
}

int32 ASpellRisePlayerController::GetWhisperUnreadCount(const FString& ConversationId) const
{
	return GetChatUnreadCount(ConversationId);
}

void ASpellRisePlayerController::SetActiveChatTab(const FString& TabId)
{
	if (!IsLocalController())
	{
		return;
	}

	ActiveChatTabId = SanitizeBoundedString(TabId, MaxChatTabIdChars);
	if (ActiveChatTabId.IsEmpty())
	{
		return;
	}

	ChatUnreadByTabId.Remove(ActiveChatTabId);
	BP_OnChatUnreadChanged(ActiveChatTabId, 0);

	if (!ActiveWhisperConversationId.IsEmpty()
		&& ActiveChatTabId.Equals(ActiveWhisperConversationId, ESearchCase::CaseSensitive))
	{
		WhisperUnreadByConversation.Remove(ActiveChatTabId);
		BP_OnWhisperUnreadChanged(ActiveChatTabId, 0);
	}
}

int32 ASpellRisePlayerController::GetChatUnreadCount(const FString& TabId) const
{
	const FString SafeTabId = SanitizeBoundedString(TabId, MaxChatTabIdChars);
	return ChatUnreadByTabId.FindRef(SafeTabId);
}

void ASpellRisePlayerController::ServerSendWhisperToConversation_Implementation(
	const FString& ConversationId,
	const FString& Text)
{
	const FString SafeConversationId = SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
	ASpellRisePlayerController* TargetController =
		FindPlayerControllerByConversationId_Server(SafeConversationId);
	if (!TargetController)
	{
		SendAdminSystemMessage(TEXT("Whisper rejeitado: player offline."));
		return;
	}
	DeliverWhisper_Server(TargetController, Text);
}

void ASpellRisePlayerController::SendAdminSystemMessage(const FString& MessageText)
{
	FSpellRiseChatMessage Message;
	Message.Name = FName(TEXT("System"));
	Message.Text = FText::FromString(SanitizeBoundedString(MessageText, MaxChatTextChars));
	Message.TimeText = BuildCombatTimeText();
	Message.Channel = SpellRiseChatChannel::Global;
	ClientReceiveChatMessage(Message);
}

bool ASpellRisePlayerController::IsAdminCommandRateLimited()
{
	const UWorld* World = GetWorld();
	const double Now = World ? World->GetTimeSeconds() : 0.0;
	if (LastAdminCommandTimeSeconds >= 0.0
		&& Now - LastAdminCommandTimeSeconds < MinimumAdminCommandIntervalSeconds)
	{
		return true;
	}
	LastAdminCommandTimeSeconds = Now;
	return false;
}

bool ASpellRisePlayerController::TryHandleAdminChatCommand(const FString& RawMessage)
{
	if (!HasAuthority())
	{
		return false;
	}

	const FString Command = RawMessage.TrimStartAndEnd();
	if (Command.Equals(TEXT("/online"), ESearchCase::IgnoreCase))
	{
		TArray<FString> OnlinePlayers;
		if (GetWorld())
		{
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				const APlayerController* OnlineController = It->Get();
				if (OnlineController && OnlineController->PlayerState)
				{
					OnlinePlayers.Add(OnlineController->PlayerState->GetPlayerName());
				}
			}
		}
		OnlinePlayers.Sort();
		SendAdminSystemMessage(FString::Printf(
			TEXT("Online (%d): %s"),
			OnlinePlayers.Num(),
			OnlinePlayers.IsEmpty() ? TEXT("nenhum") : *FString::Join(OnlinePlayers, TEXT(", "))));
		return true;
	}

	if (Command.Equals(TEXT("/help"), ESearchCase::IgnoreCase))
	{
		SendAdminSystemMessage(TEXT("Player: /help | /online | /party | /clear | /invite Player | /remove Player | /leader Player | /leave | /name NovoNome | /w Player Mensagem | /r Mensagem"));
		if (bAdminAuthenticated)
		{
			SendAdminSystemMessage(TEXT("Admin progressao: /set level <1-999> Player | /set resetpoints Player"));
			SendAdminSystemMessage(TEXT("Admin movimento: /goto Player | /bringto Player | /unstuck Player | /set fly | /set walk"));
			SendAdminSystemMessage(TEXT("Admin moderacao: /give ip Player | /kick Player Motivo | /ban Player <30m|12h|7d|permanent> Motivo"));
			SendAdminSystemMessage(TEXT("Admin personagem: /revive Player | /heal Player | /set invisible | /set visible"));
		}
		return true;
	}

	const bool bRecognized = Command.StartsWith(TEXT("/admin"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/set level"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/set resetpoints"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/goto"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/bringto"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/give ip"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/kick"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/ban"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/revive"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/heal"), ESearchCase::IgnoreCase)
		|| Command.StartsWith(TEXT("/unstuck"), ESearchCase::IgnoreCase)
		|| Command.Equals(TEXT("/set invisible"), ESearchCase::IgnoreCase)
		|| Command.Equals(TEXT("/set visible"), ESearchCase::IgnoreCase)
		|| Command.Equals(TEXT("/set fly"), ESearchCase::IgnoreCase)
		|| Command.Equals(TEXT("/set walk"), ESearchCase::IgnoreCase);
	if (!bRecognized)
	{
		return false;
	}

	if (IsAdminCommandRateLimited())
	{
		SendAdminSystemMessage(TEXT("Comando rejeitado: aguarde antes de tentar novamente."));
		return true;
	}

	if (Command.StartsWith(TEXT("/admin"), ESearchCase::IgnoreCase))
	{
		const FString SuppliedPassword = Command.RightChop(6).TrimStartAndEnd();
		FString ExpectedPassword = FPlatformMisc::GetEnvironmentVariable(TEXT("SR_ADMIN_PASSWORD"));
		if (ExpectedPassword.IsEmpty())
		{
			ExpectedPassword = TEXT("212223");
		}

		bAdminAuthenticated = SuppliedPassword == ExpectedPassword;
		if (bAdminAuthenticated)
		{
			UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
				TEXT("[Admin][LoginSucceeded] Controller=%s PlayerState=%s Mode=password_only_testing"),
				*GetNameSafe(this),
				*GetNameSafe(PlayerState));
		}
		else
		{
			UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
				TEXT("[Admin][LoginRejected] Controller=%s PlayerState=%s Reason=invalid_password Mode=password_only_testing"),
				*GetNameSafe(this),
				*GetNameSafe(PlayerState));
		}
		SendAdminSystemMessage(bAdminAuthenticated
			? TEXT("Admin autenticado.")
			: TEXT("Login de admin rejeitado."));
		return true;
	}

	if (!bAdminAuthenticated)
	{
		SendAdminSystemMessage(TEXT("Comando rejeitado: autentique com /admin <senha>."));
		return true;
	}

	auto FindPlayerByName = [this](const FString& PlayerName) -> ASpellRisePlayerController*
	{
		if (!GetWorld() || PlayerName.IsEmpty())
		{
			return nullptr;
		}
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
			if (Candidate && Candidate->PlayerState
				&& Candidate->PlayerState->GetPlayerName().Equals(PlayerName, ESearchCase::IgnoreCase))
			{
				return Candidate;
			}
		}
		return nullptr;
	};
	auto JoinCommandParts = [](const TArray<FString>& Parts, const int32 StartIndex, const int32 MaxChars) -> FString
	{
		FString Result;
		for (int32 Index = StartIndex; Index < Parts.Num(); ++Index)
		{
			if (!Result.IsEmpty())
			{
				Result += TEXT(" ");
			}
			Result += Parts[Index];
		}
		return Result.Left(MaxChars);
	};

	if (Command.StartsWith(TEXT("/set level"), ESearchCase::IgnoreCase))
	{
		TArray<FString> Parts;
		Command.ParseIntoArrayWS(Parts);
		if (Parts.Num() != 4)
		{
			SendAdminSystemMessage(TEXT("Uso: /set level <1-999> <Player>"));
			return true;
		}

		const int32 TargetLevel = FCString::Atoi(*Parts[2]);
		ASpellRisePlayerController* TargetController = FindPlayerByName(Parts[3]);
		ASpellRisePlayerState* TargetPlayerState = TargetController
			? TargetController->GetPlayerState<ASpellRisePlayerState>()
			: nullptr;
		USpellRiseProgressionComponent* Progression = TargetPlayerState
			? TargetPlayerState->GetProgressionComponent()
			: nullptr;
		if (!Progression || TargetLevel < 1 || TargetLevel > 999)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: level ou player invalido."));
			return true;
		}

		Progression->SetCharacterLevelByAdmin_Server(TargetLevel);
		TargetPlayerState->SetTalentPoints_Server(Progression->GetTalentPoints());
		TargetPlayerState->MarkCharacterProgressionDirtyForAdmin_Server();
		const int32 RequiredExperience = Progression->GetExperience();
		SendAdminSystemMessage(FString::Printf(TEXT("Level de %s definido para %d (XP %d)."),
			*Parts[3], TargetLevel, RequiredExperience));
		return true;
	}

	if (Command.StartsWith(TEXT("/set resetpoints"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(16).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		ASpellRisePlayerState* TargetPlayerState = TargetController
			? TargetController->GetPlayerState<ASpellRisePlayerState>()
			: nullptr;
		if (!TargetPlayerState || !TargetPlayerState->ResetProgressionPointsForAdmin_Server())
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: player ou progressao indisponivel."));
			return true;
		}
		SendAdminSystemMessage(FString::Printf(TEXT("Pontos de atributos e talentos de %s foram resetados."), *PlayerName));
		return true;
	}

	if (Command.StartsWith(TEXT("/goto"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(5).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		APawn* AdminPawn = GetPawn();
		APawn* TargetPawn = TargetController ? TargetController->GetPawn() : nullptr;
		if (!AdminPawn || !TargetPawn)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: player ou pawn indisponivel."));
			return true;
		}
		AdminPawn->TeleportTo(TargetPawn->GetActorLocation(), TargetPawn->GetActorRotation(), false, true);
		SendAdminSystemMessage(FString::Printf(TEXT("Teleportado para %s."), *PlayerName));
		return true;
	}

	if (Command.StartsWith(TEXT("/bringto"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(8).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		APawn* TargetPawn = TargetController ? TargetController->GetPawn() : nullptr;
		APawn* AdminLocationPawn = AdminOriginalPawn.IsValid()
			? AdminOriginalPawn.Get()
			: GetPawn().Get();
		if (!TargetPawn || !AdminLocationPawn || TargetController == this)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: player ou pawn indisponivel."));
			return true;
		}

		const FVector Destination = AdminLocationPawn->GetActorLocation()
			+ AdminLocationPawn->GetActorForwardVector() * 150.0f;
		if (!TargetPawn->TeleportTo(Destination, AdminLocationPawn->GetActorRotation(), false, true))
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: destino de teleporte bloqueado."));
			return true;
		}
		SendAdminSystemMessage(FString::Printf(TEXT("%s foi teleportado ate o admin."), *PlayerName));
		return true;
	}

	if (Command.StartsWith(TEXT("/give ip"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(8).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		if (!TargetController)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: player nao encontrado."));
			return true;
		}

		FString NetworkAddress = TargetController->GetPlayerNetworkAddress().TrimStartAndEnd();
		if (NetworkAddress.IsEmpty())
		{
			NetworkAddress = TEXT("indisponivel");
		}
		SendAdminSystemMessage(FString::Printf(TEXT("IP de %s: %s"), *PlayerName, *NetworkAddress));
		return true;
	}

	if (Command.StartsWith(TEXT("/kick"), ESearchCase::IgnoreCase))
	{
		TArray<FString> Parts;
		Command.ParseIntoArrayWS(Parts);
		if (Parts.Num() < 3)
		{
			SendAdminSystemMessage(TEXT("Uso: /kick Player Motivo"));
			return true;
		}
		ASpellRisePlayerController* TargetController = FindPlayerByName(Parts[1]);
		if (!TargetController || TargetController == this)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: player invalido."));
			return true;
		}
		const FString Reason = JoinCommandParts(Parts, 2, 160);
		const FText KickReason = FText::FromString(Reason);
		AGameModeBase* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
		bool bKicked = GameMode && GameMode->GameSession
			? GameMode->GameSession->KickPlayer(TargetController, KickReason)
			: false;
		if (!bKicked)
		{
			TargetController->ClientReturnToMainMenuWithTextReason(KickReason);
			bKicked = true;
		}
		SendAdminSystemMessage(bKicked
			? FString::Printf(TEXT("%s foi expulso."), *Parts[1])
			: TEXT("Falha ao expulsar player."));
		return true;
	}

	if (Command.StartsWith(TEXT("/ban"), ESearchCase::IgnoreCase))
	{
		TArray<FString> Parts;
		Command.ParseIntoArrayWS(Parts);
		if (Parts.Num() < 4)
		{
			SendAdminSystemMessage(TEXT("Uso: /ban Player <30m|12h|7d|permanent> Motivo"));
			return true;
		}
		ASpellRisePlayerController* TargetController = FindPlayerByName(Parts[1]);
		if (!TargetController || TargetController == this || !TargetController->PlayerState)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: player invalido."));
			return true;
		}

		const FString Duration = Parts[2].ToLower();
		FString BannedUntil;
		if (Duration != TEXT("permanent"))
		{
			const TCHAR Unit = Duration.IsEmpty() ? TEXT('\0') : Duration[Duration.Len() - 1];
			const int32 Amount = FCString::Atoi(*Duration.LeftChop(1));
			if (Amount <= 0 || (Unit != TEXT('m') && Unit != TEXT('h') && Unit != TEXT('d')))
			{
				SendAdminSystemMessage(TEXT("Tempo invalido. Use 30m, 12h, 7d ou permanent."));
				return true;
			}
			const FTimespan Span = Unit == TEXT('m')
				? FTimespan::FromMinutes(Amount)
				: (Unit == TEXT('h') ? FTimespan::FromHours(Amount) : FTimespan::FromDays(Amount));
			BannedUntil = (FDateTime::UtcNow() + Span).ToIso8601();
		}

		const FString Reason = JoinCommandParts(Parts, 3, 500);
		UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
		USpellRisePersistenceSubsystem* Persistence = GameInstance
			? GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>()
			: nullptr;
		if (!Persistence || !Persistence->BanPlayer(TargetController->PlayerState, Reason, BannedUntil, PlayerState))
		{
			SendAdminSystemMessage(TEXT("Ban rejeitado: persistencia PostgreSQL ou SteamID indisponivel."));
			return true;
		}
		const FText BanReason = FText::FromString(FString::Printf(TEXT("Banido: %s"), *Reason.Left(120)));
		AGameModeBase* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
		if (!GameMode || !GameMode->GameSession || !GameMode->GameSession->KickPlayer(TargetController, BanReason))
		{
			TargetController->ClientReturnToMainMenuWithTextReason(BanReason);
		}
		SendAdminSystemMessage(FString::Printf(TEXT("%s foi banido (%s)."), *Parts[1], *Duration));
		return true;
	}

	if (Command.StartsWith(TEXT("/revive"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(7).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		ASpellRiseCharacterBase* TargetCharacter = TargetController
			? Cast<ASpellRiseCharacterBase>(TargetController->GetPawn())
			: nullptr;
		if (!TargetCharacter || !TargetCharacter->ReviveFromDowned_Server(nullptr))
		{
			SendAdminSystemMessage(TEXT("Revive rejeitado: player nao esta downed ou indisponivel."));
			return true;
		}
		SendAdminSystemMessage(FString::Printf(TEXT("%s foi revivido."), *PlayerName));
		return true;
	}

	if (Command.StartsWith(TEXT("/heal"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(5).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		ASpellRisePlayerState* TargetPlayerState = TargetController
			? TargetController->GetPlayerState<ASpellRisePlayerState>()
			: nullptr;
		USpellRiseAbilitySystemComponent* TargetASC = TargetPlayerState
			? Cast<USpellRiseAbilitySystemComponent>(TargetPlayerState->GetAbilitySystemComponent())
			: nullptr;
		if (!TargetASC)
		{
			SendAdminSystemMessage(TEXT("Heal rejeitado: ASC indisponivel."));
			return true;
		}
		TargetASC->SetNumericAttributeBase(
			UResourceAttributeSet::GetHealthAttribute(),
			TargetASC->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute()));
		TargetASC->SetNumericAttributeBase(
			UResourceAttributeSet::GetManaAttribute(),
			TargetASC->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute()));
		TargetASC->SetNumericAttributeBase(
			UResourceAttributeSet::GetStaminaAttribute(),
			TargetASC->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute()));
		TargetPlayerState->ForceNetUpdate();
		SendAdminSystemMessage(FString::Printf(TEXT("%s foi curado completamente."), *PlayerName));
		return true;
	}

	if (Command.StartsWith(TEXT("/unstuck"), ESearchCase::IgnoreCase))
	{
		const FString PlayerName = Command.RightChop(8).TrimStartAndEnd();
		ASpellRisePlayerController* TargetController = FindPlayerByName(PlayerName);
		APawn* TargetPawn = TargetController ? TargetController->GetPawn() : nullptr;
		UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
		USpellRisePersistenceSubsystem* Persistence = GameInstance
			? GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>()
			: nullptr;
		FTransform SafeTransform;
		if (!TargetController || !TargetPawn || !Persistence
			|| !Persistence->BuildRespawnTransformForController(TargetController, SafeTransform)
			|| !TargetPawn->TeleportTo(SafeTransform.GetLocation(), SafeTransform.Rotator(), false, true))
		{
			SendAdminSystemMessage(TEXT("Unstuck rejeitado: local seguro indisponivel."));
			return true;
		}
		SendAdminSystemMessage(FString::Printf(TEXT("%s foi movido para um local seguro."), *PlayerName));
		return true;
	}

	APawn* AdminPawn = GetPawn();
	if (!AdminPawn)
	{
		SendAdminSystemMessage(TEXT("Comando rejeitado: pawn indisponivel."));
		return true;
	}

	if (Command.Equals(TEXT("/set invisible"), ESearchCase::IgnoreCase)
		|| Command.Equals(TEXT("/set visible"), ESearchCase::IgnoreCase))
	{
		const bool bInvisible = Command.EndsWith(TEXT("invisible"), ESearchCase::IgnoreCase);
		APawn* VisibilityPawn = AdminOriginalPawn.IsValid() ? AdminOriginalPawn.Get() : AdminPawn;
		if (bInvisible)
		{
			AdminInvisibleOriginalCollisionResponses.Reset();
			TArray<UPrimitiveComponent*> PrimitiveComponents;
			VisibilityPawn->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
			for (UPrimitiveComponent* Primitive : PrimitiveComponents)
			{
				if (!Primitive || !Primitive->IsCollisionEnabled())
				{
					continue;
				}
				AdminInvisibleOriginalCollisionResponses.Add(Primitive, Primitive->GetCollisionResponseToChannels());
				Primitive->SetCollisionResponseToAllChannels(ECR_Ignore);
				Primitive->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
				Primitive->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
			}
		}
		else
		{
			for (const TPair<TWeakObjectPtr<UPrimitiveComponent>, FCollisionResponseContainer>& Entry : AdminInvisibleOriginalCollisionResponses)
			{
				if (UPrimitiveComponent* Primitive = Entry.Key.Get())
				{
					Primitive->SetCollisionResponseToChannels(Entry.Value);
				}
			}
			AdminInvisibleOriginalCollisionResponses.Reset();
		}
		VisibilityPawn->SetActorHiddenInGame(bInvisible);
		VisibilityPawn->ForceNetUpdate();
		SendAdminSystemMessage(bInvisible ? TEXT("Personagem invisivel.") : TEXT("Personagem visivel."));
		return true;
	}

	if (Command.Equals(TEXT("/set fly"), ESearchCase::IgnoreCase))
	{
		if (AdminFlyPawn.IsValid())
		{
			SendAdminSystemMessage(TEXT("Modo fly ja esta ativo."));
			return true;
		}

		AdminOriginalPawn = AdminPawn;
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ASpectatorPawn* FlyPawn = GetWorld()->SpawnActor<ASpectatorPawn>(
			ASpectatorPawn::StaticClass(),
			AdminPawn->GetActorLocation(),
			GetControlRotation(),
			SpawnParameters);
		if (!FlyPawn)
		{
			AdminOriginalPawn.Reset();
			SendAdminSystemMessage(TEXT("Comando rejeitado: falha ao criar camera livre."));
			return true;
		}
		FlyPawn->SetActorEnableCollision(false);
		if (USpectatorPawnMovement* SpectatorMovement = Cast<USpectatorPawnMovement>(FlyPawn->GetMovementComponent()))
		{
			SpectatorMovement->MaxSpeed = 5000.0f;
			SpectatorMovement->Acceleration = 10000.0f;
			SpectatorMovement->Deceleration = 10000.0f;
		}
		AdminFlyPawn = FlyPawn;
		Possess(FlyPawn);
		SendAdminSystemMessage(TEXT("Modo fly ativo: camera livre sem colisao."));
		return true;
	}

	if (Command.Equals(TEXT("/set walk"), ESearchCase::IgnoreCase))
	{
		APawn* OriginalPawn = AdminOriginalPawn.Get();
		APawn* FlyPawn = AdminFlyPawn.Get();
		if (!OriginalPawn)
		{
			SendAdminSystemMessage(TEXT("Comando rejeitado: personagem original indisponivel."));
			return true;
		}
		Possess(OriginalPawn);
		if (FlyPawn)
		{
			FlyPawn->Destroy();
		}
		AdminFlyPawn.Reset();
		AdminOriginalPawn.Reset();
		SendAdminSystemMessage(TEXT("Modo walk ativo: controle devolvido ao personagem."));
		return true;
	}

	SendAdminSystemMessage(TEXT("Comando rejeitado: movement component indisponivel."));
	return true;
}

void ASpellRisePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(GetNetMode() != NM_DedicatedServer && IsLocalController());

	if (IsLocalController())
	{
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
		LogInputFocusSnapshot(TEXT("BeginPlay.SetInputModeGameOnly"));
	}

	SetupEnhancedInput();

	if (APawn* InitialPawn = GetPawn())
	{
		HandlePawnChangedRuntime(InitialPawn, TEXT("BeginPlay"));
	}

}

bool ASpellRisePlayerController::CanRunLocalHUDFlow(FString* OutSkipReason) const
{
	auto SetSkipReason = [&](const TCHAR* Reason)
	{
		if (OutSkipReason)
		{
			*OutSkipReason = Reason;
		}
		return false;
	};

	if (GetNetMode() == NM_DedicatedServer)
	{
		return SetSkipReason(TEXT("dedicated_server"));
	}

	if (FParse::Param(FCommandLine::Get(), TEXT("SpellRiseSkipLocalHUDFlow")))
	{
		return SetSkipReason(TEXT("load_test_local_hud_flow_disabled"));
	}

	if (!IsLocalController())
	{
		return SetSkipReason(TEXT("non_local_controller"));
	}

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return SetSkipReason(TEXT("local_player_missing"));
	}

	if (!LocalPlayer->ViewportClient)
	{
		return SetSkipReason(TEXT("viewport_client_missing"));
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return SetSkipReason(TEXT("world_missing"));
	}

	return true;
}

void ASpellRisePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const TCHAR* EndPlayReasonText = TEXT("Unknown");
	switch (EndPlayReason)
	{
	case EEndPlayReason::Destroyed:
		EndPlayReasonText = TEXT("Destroyed");
		break;
	case EEndPlayReason::LevelTransition:
		EndPlayReasonText = TEXT("LevelTransition");
		break;
	case EEndPlayReason::EndPlayInEditor:
		EndPlayReasonText = TEXT("EndPlayInEditor");
		break;
	case EEndPlayReason::RemovedFromWorld:
		EndPlayReasonText = TEXT("RemovedFromWorld");
		break;
	case EEndPlayReason::Quit:
		EndPlayReasonText = TEXT("Quit");
		break;
	default:
		break;
	}

	Super::EndPlay(EndPlayReason);
}

void ASpellRisePlayerController::SetupEnhancedInput()
{
	if (!IsLocalController())
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	const bool bHasEnhancedInputContexts =
		IMC_CoreMovement.Get() ||
		IMC_CoreCamera.Get() ||
		IMC_Combat.Get() ||
		IMC_Interaction.Get() ||
		IMC_UI.Get() ||
		IMC_System.Get();

	if (bHasEnhancedInputContexts)
	{
		RefreshEnhancedInputContexts();
		return;
	}

	UInputMappingContext* MappingContextToApply = DefaultMappingContext.Get();
	int32 MappingPriority = DefaultMappingPriority;
	if (!MappingContextToApply)
	{
		if (const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
		{
			MappingContextToApply = const_cast<UInputMappingContext*>(ControlledCharacter->GetDefaultInputMappingContext());
			MappingPriority = 0;
		}
	}

	if (!MappingContextToApply)
	{
		return;
	}

	if (!Subsystem->HasMappingContext(MappingContextToApply))
	{
		Subsystem->AddMappingContext(MappingContextToApply, MappingPriority);
	}
}

void ASpellRisePlayerController::RefreshEnhancedInputContexts()
{
	if (!IsLocalController())
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	const bool bUIInputContextActive = ShouldEnableUIInputContext();

	UpdateEnhancedInputContext(Subsystem, IMC_CoreMovement.Get(), IMC_CoreMovementPriority, true, TEXT("CoreMovement"));
	UpdateEnhancedInputContext(Subsystem, IMC_CoreCamera.Get(), IMC_CoreCameraPriority, true, TEXT("CoreCamera"));
	UpdateEnhancedInputContext(
		Subsystem,
		IMC_Combat.Get(),
		IMC_CombatPriority,
		!bUIInteractionModeActive,
		TEXT("Combat"));
	UpdateEnhancedInputContext(Subsystem, IMC_Interaction.Get(), IMC_InteractionPriority, true, TEXT("Interaction"));
	UpdateEnhancedInputContext(Subsystem, IMC_UI.Get(), IMC_UIPriority, bUIInputContextActive, TEXT("UI"));
	UpdateEnhancedInputContext(Subsystem, IMC_System.Get(), IMC_SystemPriority, true, TEXT("System"));
}

void ASpellRisePlayerController::RestoreGameplayInputAfterUI(const FName Source)
{
	if (!IsLocalController() || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	bUIInteractionModeActive = false;
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->EnableInput(this);
	}

	SetupEnhancedInput();
	RefreshEnhancedInputContexts();

	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[InputFocus][RestoreGameplay] Source=%s Controller=%s Pawn=%s"),
		*Source.ToString(),
		*GetNameSafe(this),
		*GetNameSafe(GetPawn()));
	LogInputFocusSnapshot(TEXT("RestoreGameplayInputAfterUI"));
}

void ASpellRisePlayerController::ToggleUIInteractionMode()
{
	if (!IsLocalController() || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (bUIInteractionModeActive)
	{
		RestoreGameplayInputAfterUI(TEXT("ToggleUIInteractionMode"));
		return;
	}

	bUIInteractionModeActive = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(true);

	RefreshEnhancedInputContexts();

	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[InputFocus][UIInteraction] Active=1 Controller=%s Pawn=%s"),
		*GetNameSafe(this),
		*GetNameSafe(GetPawn()));
	LogInputFocusSnapshot(TEXT("ToggleUIInteractionMode.Enable"));
}

void ASpellRisePlayerController::InventorySplitSlotSERVER_Implementation(UObject* FromContainer, int32 Slot, int32 Amount)
{
	if (!FromContainer || Slot < 0 || Amount <= 0)
	{
		AuditRejectedInventoryRpc(TEXT("InventorySplitSlotSERVER"), TEXT("invalid_payload"));
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
		TEXT("InventorySplitSlotSERVER ainda nao implementado no PlayerController. Controller=%s Container=%s"),
		*GetNameSafe(this),
		*GetNameSafe(FromContainer));
}

void ASpellRisePlayerController::OnInventorySlotDropSERVER_Implementation(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex)
{
	if (!FromContainer || FromIndex < 0 || ToIndex < 0)
	{
		AuditRejectedInventoryRpc(TEXT("OnInventorySlotDropSERVER"), TEXT("invalid_payload"));
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
		TEXT("OnInventorySlotDropSERVER ainda nao implementado no PlayerController. Controller=%s From=%s To=%s"),
		*GetNameSafe(this),
		*GetNameSafe(FromContainer),
		*GetNameSafe(ToInventoryComponent));
}

void ASpellRisePlayerController::Route_InventorySortBy_SERVER_Implementation(UObject* InventoryRef, int32 SortBy)
{
	constexpr int32 MinSupportedSortMode = 0;
	constexpr int32 MaxSupportedSortMode = 16;
	if (!InventoryRef || SortBy < MinSupportedSortMode || SortBy > MaxSupportedSortMode)
	{
		AuditRejectedInventoryRpc(TEXT("Route_InventorySortBy_SERVER"), TEXT("invalid_payload"));
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
		TEXT("Route_InventorySortBy_SERVER ainda nao implementado no PlayerController. Controller=%s Inventory=%s SortBy=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InventoryRef),
		SortBy);
}

void ASpellRisePlayerController::AuditRejectedInventoryRpc(const TCHAR* RpcName, const FString& RejectReason)
{
	const FString CounterKey = FString::Printf(TEXT("%s|%s"), RpcName ? RpcName : TEXT("unknown"), *RejectReason);
	const int32 RejectCount = ++RejectedInventoryRpcCountByReason.FindOrAdd(CounterKey);
	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
		TEXT("[RPC][Rejected] Rpc=%s Reason=%s Count=%d Controller=%s PlayerState=%s Pawn=%s"),
		RpcName ? RpcName : TEXT("unknown"),
		*RejectReason,
		RejectCount,
		*GetNameSafe(this),
		*GetNameSafe(PlayerState.Get()),
		*GetNameSafe(GetPawn()));
}


void ASpellRisePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC)
	{
		return;
	}

	if (!IA_Attack)
	{

	}
	else
	{
		EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAttackPressed);
		EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAttackReleased);
		EIC->BindAction(IA_Attack, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAttackReleased);
	}

	if (IA_Primary)
	{
		EIC->BindAction(IA_Primary, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnPrimaryPressed);
		EIC->BindAction(IA_Primary, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnPrimaryReleased);
		EIC->BindAction(IA_Primary, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnPrimaryReleased);
	}

	if (IA_Secondary)
	{
		EIC->BindAction(IA_Secondary, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnSecondaryPressed);
		EIC->BindAction(IA_Secondary, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnSecondaryReleased);
		EIC->BindAction(IA_Secondary, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnSecondaryReleased);
	}

	if (IA_Interact)
	{
		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnInteractPressed);
		EIC->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnInteractReleased);
		EIC->BindAction(IA_Interact, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnInteractReleased);
	}

	if (IA_ClearSelection)
	{
		EIC->BindAction(IA_ClearSelection, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnClearSelectionPressed);
	}

	if (IA_Sprint)
	{
		EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnSprintPressed);
		EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnSprintReleased);
		EIC->BindAction(IA_Sprint, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnSprintReleased);
	}

	if (IA_Ability1)
	{
		EIC->BindAction(IA_Ability1, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility1Pressed);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility1Released);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility1Released);
	}

	if (IA_Ability2)
	{
		EIC->BindAction(IA_Ability2, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility2Pressed);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility2Released);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility2Released);
	}

	if (IA_Ability3)
	{
		EIC->BindAction(IA_Ability3, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility3Pressed);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility3Released);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility3Released);
	}

	if (IA_Ability4)
	{
		EIC->BindAction(IA_Ability4, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility4Pressed);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility4Released);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility4Released);
	}

	if (IA_Ability5)
	{
		EIC->BindAction(IA_Ability5, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility5Pressed);
		EIC->BindAction(IA_Ability5, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility5Released);
		EIC->BindAction(IA_Ability5, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility5Released);
	}

	if (IA_Ability6)
	{
		EIC->BindAction(IA_Ability6, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility6Pressed);
		EIC->BindAction(IA_Ability6, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility6Released);
		EIC->BindAction(IA_Ability6, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility6Released);
	}

	if (IA_Ability7)
	{
		EIC->BindAction(IA_Ability7, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility7Pressed);
		EIC->BindAction(IA_Ability7, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility7Released);
		EIC->BindAction(IA_Ability7, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility7Released);
	}

	if (IA_Ability8)
	{
		EIC->BindAction(IA_Ability8, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility8Pressed);
		EIC->BindAction(IA_Ability8, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility8Released);
		EIC->BindAction(IA_Ability8, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility8Released);
	}

	if (ToggleUIInteractionKey.IsValid())
	{
		InputComponent->BindKey(
			ToggleUIInteractionKey,
			IE_Pressed,
			this,
			&ASpellRisePlayerController::OnToggleUIInteractionPressed);
	}

#if !UE_BUILD_SHIPPING
	if (DebugGrantExperienceKey.IsValid())
	{
		InputComponent->BindKey(
			DebugGrantExperienceKey,
			IE_Pressed,
			this,
			&ASpellRisePlayerController::OnDebugGrantExperiencePressed);
	}
#endif

}

void ASpellRisePlayerController::OnToggleUIInteractionPressed()
{
	ToggleUIInteractionMode();
}

void ASpellRisePlayerController::OnDebugGrantExperiencePressed()
{
#if !UE_BUILD_SHIPPING
	if (HasAuthority())
	{
		ServerGrantDebugExperience_Implementation();
		return;
	}

	ServerGrantDebugExperience();
#endif
}

void ASpellRisePlayerController::ServerGrantDebugExperience_Implementation()
{
#if !UE_BUILD_SHIPPING
	constexpr double MinimumGrantIntervalSeconds = 0.25;
	constexpr float MaximumDebugExperienceGrant = 100000000.0f;

	const UWorld* World = GetWorld();
	const double CurrentTimeSeconds = World ? World->GetTimeSeconds() : 0.0;
	if (LastDebugExperienceGrantTimeSeconds >= 0.0
		&& CurrentTimeSeconds - LastDebugExperienceGrantTimeSeconds < MinimumGrantIntervalSeconds)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
			TEXT("[DebugXP][Rejected] Reason=rate_limit Controller=%s"),
			*GetNameSafe(this));
		return;
	}

	if (!FMath::IsFinite(DebugExperienceGrantAmount)
		|| DebugExperienceGrantAmount < 1.0f
		|| DebugExperienceGrantAmount > MaximumDebugExperienceGrant)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
			TEXT("[DebugXP][Rejected] Reason=invalid_amount Controller=%s Amount=%.2f"),
			*GetNameSafe(this),
			DebugExperienceGrantAmount);
		return;
	}

	ASpellRisePlayerState* SpellRisePlayerState = GetPlayerState<ASpellRisePlayerState>();
	USpellRiseProgressionComponent* ProgressionComponent = SpellRisePlayerState
		? SpellRisePlayerState->GetProgressionComponent()
		: nullptr;
	if (!ProgressionComponent)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
			TEXT("[DebugXP][Rejected] Reason=missing_progression Controller=%s PlayerState=%s"),
			*GetNameSafe(this),
			*GetNameSafe(SpellRisePlayerState));
		return;
	}

	const int32 ExperienceToGrant = FMath::RoundToInt(DebugExperienceGrantAmount);
	if (!ProgressionComponent->AddExperience_Server(ExperienceToGrant))
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
			TEXT("[DebugXP][Rejected] Reason=grant_failed Controller=%s PlayerState=%s Amount=%d"),
			*GetNameSafe(this),
			*GetNameSafe(SpellRisePlayerState),
			ExperienceToGrant);
		return;
	}

	LastDebugExperienceGrantTimeSeconds = CurrentTimeSeconds;
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[DebugXP][Granted] Controller=%s PlayerState=%s Amount=%d Level=%d Experience=%d"),
		*GetNameSafe(this),
		*GetNameSafe(SpellRisePlayerState),
		ExperienceToGrant,
		ProgressionComponent->GetCharacterLevel(),
		ProgressionComponent->GetExperience());
#endif
}

void ASpellRisePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PCOnPossess] Controller=%s Pawn=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InPawn),
		*GetNameSafe(PlayerState.Get()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);
	HandlePawnChangedRuntime(InPawn, TEXT("OnPossess"));
}

APawn* ASpellRisePlayerController::GetLastSpellRiseControlledPawn() const
{
	return LastSpellRiseControlledPawn.Get();
}

UActorComponent* ASpellRisePlayerController::ResolveTalentTreeComponentForUI() const
{
	if (UActorComponent* PlayerStateComponent = SR_FindTalentTreeComponentForUI(PlayerState.Get()))
	{
		return PlayerStateComponent;
	}

	return SR_FindTalentTreeComponentForUI(GetPawn());
}

void ASpellRisePlayerController::OnUnPossess()
{
	APawn* PreviousPawn = GetPawn();
	Super::OnUnPossess();
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PCOnUnPossess] Controller=%s PreviousPawn=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(PreviousPawn),
		*GetNameSafe(PlayerState.Get()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);
	HandlePawnChangedRuntime(nullptr, TEXT("OnUnPossess"));

}

void ASpellRisePlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PCAcknowledgePossession] Controller=%s Pawn=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InPawn),
		*GetNameSafe(PlayerState.Get()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);
	HandlePawnChangedRuntime(InPawn, TEXT("AcknowledgePossession"));
}

void ASpellRisePlayerController::HandlePawnChangedRuntime(APawn* NewPawn, const TCHAR* SourceLabel)
{
	APawn* PreviousPawn = LastSpellRiseControlledPawn.Get();
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PawnChanged] Source=%s Controller=%s Pawn=%s CurrentPawn=%s ViewTarget=%s Local=%d Authority=%d"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetNameSafe(NewPawn),
		*GetNameSafe(GetPawn()),
		*GetNameSafe(GetViewTarget()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);

	if (NewPawn && IsLocalController())
	{
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
		SetupEnhancedInput();
		RefreshEnhancedInputContexts();
		LogInputFocusSnapshot(SourceLabel ? SourceLabel : TEXT("HandlePawnChangedRuntime"));
	}

	if (NewPawn && IsLocalController() && GetViewTarget() != NewPawn)
	{
		FString CameraFlowSkipReason;
		if (CanRunLocalHUDFlow(&CameraFlowSkipReason))
		{
			SetViewTarget(NewPawn);
		}
		else
		{
		}
	}

	if (IsLocalController())
	{
		FString LocalHUDFlowSkipReason;
		if (CanRunLocalHUDFlow(&LocalHUDFlowSkipReason))
		{
			BP_OnLocalHUDContextChanged(
				NewPawn,
				PreviousPawn,
				SourceLabel ? FName(SourceLabel) : NAME_None);
		}
		else
		{
			UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
				TEXT("[HUD][LocalFlowSkipped] Source=%s Reason=%s Controller=%s Pawn=%s"),
				SourceLabel ? SourceLabel : TEXT("Unknown"),
				*LocalHUDFlowSkipReason,
				*GetNameSafe(this),
				*GetNameSafe(NewPawn));
		}
	}

	if (NewPawn)
	{
		LastSpellRiseControlledPawn = NewPawn;
	}
}

void ASpellRisePlayerController::OnAttackPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	// Evita press/release duplicado quando IA_Attack e IA_Primary mapeiam o mesmo botão.
	if (IA_Primary != nullptr)
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Primary());
}

void ASpellRisePlayerController::OnAttackReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (IA_Primary != nullptr)
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Primary());
}

USpellRiseAbilitySystemComponent* ASpellRisePlayerController::GetSpellRiseASCFromPawn() const
{
	APawn* ControlledPawn = GetPawn();
	FString SkipReason;
	if (!SR_CanSendPawnOwnedInputRpc(this, ControlledPawn, &SkipReason))
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	return Cast<USpellRiseAbilitySystemComponent>(ASC);
}

bool ASpellRisePlayerController::IsControlledCharacterDead() const
{
	const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn());
	return ControlledCharacter && ControlledCharacter->IsDead();
}

bool ASpellRisePlayerController::IsGameplayInputBlocked() const
{
	return IsControlledCharacterDead() || ShouldEnableUIInputContext();
}

void ASpellRisePlayerController::SendAbilityInputTagPressed(FGameplayTag InputTag)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (!InputTag.IsValid())
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	FString SkipReason;
	if (!SR_CanSendPawnOwnedInputRpc(this, ControlledPawn, &SkipReason))
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASCFromPawn())
	{
		SRASC->SR_AbilityInputTagPressed(InputTag);
	}
}

void ASpellRisePlayerController::LogInputFocusSnapshot(const TCHAR* SourceLabel)
{
	const double NowSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (NowSeconds > 0.0 && (NowSeconds - LastInputFocusSnapshotTimeSeconds) < 0.35)
	{
		return;
	}

	FString FocusWidgetName(TEXT("NoSlateFocus"));
	FString FocusWidgetClass(TEXT("None"));

	if (FSlateApplication::IsInitialized())
	{
		if (TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0))
		{
			FocusWidgetName = FocusedWidget->ToString();
			FocusWidgetClass = FocusedWidget->GetTypeAsString();
		}
	}

	const FString Signature = FString::Printf(
		TEXT("%d|%d|%d|%s|%s|%s|%s"),
		bShowMouseCursor ? 1 : 0,
		bEnableClickEvents ? 1 : 0,
		bEnableMouseOverEvents ? 1 : 0,
		*FocusWidgetName,
		*FocusWidgetClass,
		*GetNameSafe(GetPawn()),
		*GetNameSafe(this));

	if (Signature == LastInputFocusSnapshotSignature)
	{
		return;
	}

	LastInputFocusSnapshotSignature = Signature;
	LastInputFocusSnapshotTimeSeconds = NowSeconds;

	const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn());
	const UCharacterMovementComponent* Movement = ControlledCharacter ? ControlledCharacter->GetCharacterMovement() : nullptr;

	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[InputFocus] Source=%s Controller=%s Pawn=%s Cursor=%d Click=%d MouseOver=%d UIContext=%d MoveIgnored=%d LookIgnored=%d Focus=%s|%s MovementMode=%d MaxWalkSpeed=%.2f"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetNameSafe(GetPawn()),
		bShowMouseCursor ? 1 : 0,
		bEnableClickEvents ? 1 : 0,
		bEnableMouseOverEvents ? 1 : 0,
		ShouldEnableUIInputContext() ? 1 : 0,
		IsMoveInputIgnored() ? 1 : 0,
		IsLookInputIgnored() ? 1 : 0,
		*FocusWidgetName,
		*FocusWidgetClass,
		Movement ? static_cast<int32>(Movement->MovementMode) : INDEX_NONE,
		Movement ? Movement->MaxWalkSpeed : 0.0f);
}

void ASpellRisePlayerController::SendAbilityInputTagReleased(FGameplayTag InputTag)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (!InputTag.IsValid())
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	FString SkipReason;
	if (!SR_CanSendPawnOwnedInputRpc(this, ControlledPawn, &SkipReason))
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASCFromPawn())
	{
		SRASC->SR_AbilityInputTagReleased(InputTag);
	}
}

void ASpellRisePlayerController::HandleAbilitySlotPressed(int32 SlotIndex)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	const int32 LogicalSlotIndex = USpellRiseAbilityHotbarComponent::MakeSlotIndex(ActiveAbilityHotbarGroup, SlotIndex);
	if (LogicalSlotIndex == INDEX_NONE)
	{
		return;
	}

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->AbilityWheelInputPressed(LogicalSlotIndex);
	}
}

void ASpellRisePlayerController::PressAbilityHotbarPhysicalSlot(const int32 PhysicalSlotIndex)
{
	HandleAbilitySlotPressed(PhysicalSlotIndex);
}

void ASpellRisePlayerController::HandleAbilitySlotReleased(int32 SlotIndex)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	const int32 LogicalSlotIndex = USpellRiseAbilityHotbarComponent::MakeSlotIndex(ActiveAbilityHotbarGroup, SlotIndex);
	if (LogicalSlotIndex == INDEX_NONE)
	{
		return;
	}

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->AbilityWheelInputReleased(LogicalSlotIndex);
	}
}

void ASpellRisePlayerController::ReleaseAbilityHotbarPhysicalSlot(const int32 PhysicalSlotIndex)
{
	HandleAbilitySlotReleased(PhysicalSlotIndex);
}

void ASpellRisePlayerController::SetActiveAbilityHotbarGroup(const ESpellRiseAbilityHotbarGroup NewGroup)
{
	if (!IsLocalController() || ActiveAbilityHotbarGroup == NewGroup)
	{
		return;
	}

	ActiveAbilityHotbarGroup = NewGroup;

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->SR_ClearAbilityInput();
		ControlledCharacter->ClearSelectedAbility();
	}

	OnActiveAbilityHotbarGroupChanged.Broadcast(ActiveAbilityHotbarGroup);
}

void ASpellRisePlayerController::ToggleActiveAbilityHotbarGroup()
{
	const ESpellRiseAbilityHotbarGroup NewGroup = ActiveAbilityHotbarGroup == ESpellRiseAbilityHotbarGroup::Weapon
		? ESpellRiseAbilityHotbarGroup::Common
		: ESpellRiseAbilityHotbarGroup::Weapon;
	SetActiveAbilityHotbarGroup(NewGroup);
}

FText ASpellRisePlayerController::GetAbilityHotbarPhysicalSlotInputText(const int32 PhysicalSlotIndex) const
{
	const UInputAction* AbilityAction = nullptr;
	switch (PhysicalSlotIndex)
	{
	case 0:
		AbilityAction = IA_Ability1.Get();
		break;
	case 1:
		AbilityAction = IA_Ability2.Get();
		break;
	case 2:
		AbilityAction = IA_Ability3.Get();
		break;
	case 3:
		AbilityAction = IA_Ability4.Get();
		break;
	case 4:
		AbilityAction = IA_Ability5.Get();
		break;
	case 5:
		AbilityAction = IA_Ability6.Get();
		break;
	case 6:
		AbilityAction = IA_Ability7.Get();
		break;
	case 7:
		AbilityAction = IA_Ability8.Get();
		break;
	default:
		break;
	}

	if (!AbilityAction)
	{
		return FText::AsNumber(PhysicalSlotIndex + 1);
	}

	const UInputMappingContext* ContextsToSearch[] =
	{
		IMC_Combat.Get(),
		IMC_CoreMovement.Get(),
		IMC_CoreCamera.Get(),
		IMC_Interaction.Get(),
		IMC_UI.Get(),
		IMC_System.Get()
	};

	for (const UInputMappingContext* Context : ContextsToSearch)
	{
		if (!Context)
		{
			continue;
		}

		for (const FEnhancedActionKeyMapping& Mapping : Context->GetMappings())
		{
			if (Mapping.Action == AbilityAction && Mapping.Key.IsValid())
			{
				return Mapping.Key.GetDisplayName(false);
			}
		}
	}

	return FText::AsNumber(PhysicalSlotIndex + 1);
}

void ASpellRisePlayerController::OnPrimaryPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Primary());
}

void ASpellRisePlayerController::OnPrimaryReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Primary());
}

void ASpellRisePlayerController::OnSecondaryPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Secondary());
}

void ASpellRisePlayerController::OnSecondaryReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Secondary());
}

void ASpellRisePlayerController::OnInteractPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (TryExecuteNarrativeInteract(true))
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Interact());
}

void ASpellRisePlayerController::OnInteractReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (TryExecuteNarrativeInteract(false))
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Interact());
}

bool ASpellRisePlayerController::TryExecuteNarrativeInteract(bool bPressed) const
{
	if (!IsLocalController())
	{
		return false;
	}

	UNarrativeInteractionComponent* InteractionComponent = ResolveNarrativeInteractionComponent();
	if (!InteractionComponent)
	{
		return false;
	}

	const FName FunctionName = bPressed ? TEXT("BeginInteract") : TEXT("EndInteract");
	UFunction* Function = InteractionComponent->FindFunction(FunctionName);
	if (!Function)
	{
		return false;
	}

	InteractionComponent->ProcessEvent(Function, nullptr);
	return true;
}

UNarrativeInteractionComponent* ASpellRisePlayerController::ResolveNarrativeInteractionComponent() const
{
	if (UNarrativeInteractionComponent* ControllerInteraction = FindComponentByClass<UNarrativeInteractionComponent>())
	{
		return ControllerInteraction;
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		if (UNarrativeInteractionComponent* PawnInteraction = ControlledPawn->FindComponentByClass<UNarrativeInteractionComponent>())
		{
			return PawnInteraction;
		}
	}

	return nullptr;
}

UNarrativeInventoryComponent* ASpellRisePlayerController::ResolveNarrativeInventoryComponentForUI() const
{
	if (AActor* PlayerStateActor = GetPlayerState<APlayerState>())
	{
		if (UNarrativeInventoryComponent* PlayerStateInventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(PlayerStateActor))
		{
			return PlayerStateInventory;
		}
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		if (UNarrativeInventoryComponent* PawnInventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(ControlledPawn))
		{
			return PawnInventory;
		}
	}

	return UInventoryFunctionLibrary::GetInventoryComponentFromTarget(const_cast<ASpellRisePlayerController*>(this));
}

bool ASpellRisePlayerController::TryStopLootingFromUIInput(const FName Source)
{
	if (!IsLocalController() || GetNetMode() == NM_DedicatedServer)
	{
		return false;
	}

	UNarrativeInventoryComponent* Inventory = ResolveNarrativeInventoryComponentForUI();
	if (!Inventory)
	{
		return false;
	}

	static const FObjectProperty* LootSourceProperty = FindFProperty<FObjectProperty>(
		UNarrativeInventoryComponent::StaticClass(),
		TEXT("LootSource"));
	if (!LootSourceProperty)
	{
		return false;
	}

	UObject* LootSourceObject = LootSourceProperty->GetObjectPropertyValue_InContainer(Inventory);
	if (!LootSourceObject)
	{
		return false;
	}

	Inventory->RequestStopLootingFromUI();
	RestoreGameplayInputAfterUI(Source);

	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[InventoryUI][StopLooting] Source=%s Controller=%s Inventory=%s LootSource=%s"),
		*Source.ToString(),
		*GetNameSafe(this),
		*GetNameSafe(Inventory),
		*GetNameSafe(LootSourceObject));

	return true;
}

void ASpellRisePlayerController::OnClearSelectionPressed()
{
	if (TryStopLootingFromUIInput(TEXT("ClearSelection")))
	{
		return;
	}

	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->ClearSelectedAbility();
	}
}

void ASpellRisePlayerController::OnSprintPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Sprint());
}

void ASpellRisePlayerController::OnSprintReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Sprint());
}

void ASpellRisePlayerController::OnAbility1Pressed() { HandleAbilitySlotPressed(0); }
void ASpellRisePlayerController::OnAbility1Released() { HandleAbilitySlotReleased(0); }
void ASpellRisePlayerController::OnAbility2Pressed() { HandleAbilitySlotPressed(1); }
void ASpellRisePlayerController::OnAbility2Released() { HandleAbilitySlotReleased(1); }
void ASpellRisePlayerController::OnAbility3Pressed() { HandleAbilitySlotPressed(2); }
void ASpellRisePlayerController::OnAbility3Released() { HandleAbilitySlotReleased(2); }
void ASpellRisePlayerController::OnAbility4Pressed() { HandleAbilitySlotPressed(3); }
void ASpellRisePlayerController::OnAbility4Released() { HandleAbilitySlotReleased(3); }
void ASpellRisePlayerController::OnAbility5Pressed() { HandleAbilitySlotPressed(4); }
void ASpellRisePlayerController::OnAbility5Released() { HandleAbilitySlotReleased(4); }
void ASpellRisePlayerController::OnAbility6Pressed() { HandleAbilitySlotPressed(5); }
void ASpellRisePlayerController::OnAbility6Released() { HandleAbilitySlotReleased(5); }
void ASpellRisePlayerController::OnAbility7Pressed() { HandleAbilitySlotPressed(6); }
void ASpellRisePlayerController::OnAbility7Released() { HandleAbilitySlotReleased(6); }
void ASpellRisePlayerController::OnAbility8Pressed() { HandleAbilitySlotPressed(7); }
void ASpellRisePlayerController::OnAbility8Released() { HandleAbilitySlotReleased(7); }

bool ASpellRisePlayerController::ShouldEnableUIInputContext() const
{
	return bShowMouseCursor || bEnableClickEvents || bEnableMouseOverEvents;
}

void ASpellRisePlayerController::ShowDamageNumber(
	float Damage,
	const FVector& WorldLocation,
	const FGameplayTagContainer& SourceTags,
	const FGameplayTagContainer& TargetTags,
	bool bIsCritical)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!NumberPopComponent)
	{
		return;
	}

	FSpellRiseNumberPopRequest Request;
	Request.WorldLocation = WorldLocation;
	Request.SourceTags = SourceTags;
	Request.TargetTags = TargetTags;
	Request.NumberToDisplay = FMath::RoundToInt(Damage);
	Request.bIsCriticalDamage = bIsCritical;

	NumberPopComponent->AddNumberPop(Request);
}

void ASpellRisePlayerController::ClientReceiveCombatLogEntry_Implementation(float Damage, const FString& OtherActorName, bool bIsOutgoing)
{
	if (!IsLocalController() || Damage <= 0.f)
	{
		return;
	}

	const int32 RoundedDamage = FMath::RoundToInt(Damage);
	const FString SafeOtherName = SanitizeBoundedString(OtherActorName, MaxCombatActorNameChars, TEXT("unknown target"));
	const FString MessageText = bIsOutgoing
		? FString::Printf(TEXT("You dealt %d damage to %s."), RoundedDamage, *SafeOtherName)
		: FString::Printf(TEXT("You took %d damage from %s."), RoundedDamage, *SafeOtherName);

	PushCombatLogMessage(SanitizeBoundedString(MessageText, MaxCombatLogMessageChars));
}

void ASpellRisePlayerController::ClientReceiveChatMessage_Implementation(const FSpellRiseChatMessage& Message)
{
	ReceiveChatMessageLocal(SanitizeChatMessageForLocalDelivery(Message));
}

void ASpellRisePlayerController::ClientReceivePublicChatMessage_Implementation(
	const FSpellRiseChatMessage& Message)
{
	ReceiveChatMessageLocal(SanitizeChatMessageForLocalDelivery(Message));
}

void ASpellRisePlayerController::ReceiveChatMessageLocal(const FSpellRiseChatMessage& Message)
{
	const FSpellRiseChatMessage SanitizedMessage = SanitizeChatMessageForLocalDelivery(Message);
	if (!IsLocalController() || SanitizedMessage.Text.IsEmpty())
	{
		return;
	}

	NativeChatHistory.Add(SanitizedMessage);
	constexpr int32 MaxLocalChatHistory = 300;
	if (NativeChatHistory.Num() > MaxLocalChatHistory)
	{
		NativeChatHistory.RemoveAt(0, NativeChatHistory.Num() - MaxLocalChatHistory, EAllowShrinking::No);
	}

	BP_OnChatMessageReceived(SanitizedMessage);
	const FString MessageTabId = ResolveChatTabId(SanitizedMessage);
	if (!MessageTabId.IsEmpty()
		&& !MessageTabId.Equals(ActiveChatTabId, ESearchCase::CaseSensitive))
	{
		int32& UnreadCount = ChatUnreadByTabId.FindOrAdd(MessageTabId);
		UnreadCount = FMath::Min(UnreadCount + 1, 999);
		BP_OnChatUnreadChanged(MessageTabId, UnreadCount);
	}

	if (SanitizedMessage.Channel == SpellRiseChatChannel::Whisper
		&& !SanitizedMessage.ConversationId.IsEmpty())
	{
		if (!SanitizedMessage.ConversationId.Equals(ActiveChatTabId, ESearchCase::CaseSensitive))
		{
			WhisperUnreadByConversation.Add(
				SanitizedMessage.ConversationId,
				ChatUnreadByTabId.FindRef(SanitizedMessage.ConversationId));
			BP_OnWhisperUnreadChanged(
				SanitizedMessage.ConversationId,
				ChatUnreadByTabId.FindRef(SanitizedMessage.ConversationId));
		}
		BP_OnWhisperConversationReceived(
			SanitizedMessage.ConversationId,
			SanitizedMessage.ConversationName,
			SanitizedMessage.bOutgoing);
	}

}

TArray<FSpellRiseChatMessage> ASpellRisePlayerController::GetWhisperConversationHistory(
	const FString& ConversationId) const
{
	TArray<FSpellRiseChatMessage> Result;
	const FString SafeConversationId = SanitizeBoundedString(ConversationId, MaxWhisperConversationIdChars);
	if (SafeConversationId.IsEmpty())
	{
		return Result;
	}

	for (const FSpellRiseChatMessage& Message : NativeChatHistory)
	{
		if (Message.Channel == SpellRiseChatChannel::Whisper
			&& Message.ConversationId.Equals(SafeConversationId, ESearchCase::CaseSensitive))
		{
			Result.Add(Message);
		}
	}
	return Result;
}

void ASpellRisePlayerController::PushCombatLogMessage(const FString& MessageText)
{
	if (!IsLocalController() || MessageText.IsEmpty())
	{
		return;
	}

	const FString SafeMessageText = SanitizeBoundedString(MessageText, MaxCombatLogMessageChars);
	BP_OnCombatLogMessage(SafeMessageText);

	FSpellRiseChatMessage CombatMessage;
	CombatMessage.Name = FName(TEXT("Combat"));
	CombatMessage.Text = FText::FromString(SafeMessageText);
	CombatMessage.TimeText = BuildCombatTimeText();
	CombatMessage.Channel = SpellRiseChatChannel::Combat;

	ReceiveChatMessageLocal(CombatMessage);
}
