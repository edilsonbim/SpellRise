// Serviço autoritativo de chat: valida identidade, payload, canal e abuso no servidor.
#include "SpellRise/Components/SpellRiseChatComponent.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Misc/DateTime.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseChatRuntime, Log, All);

namespace
{
	constexpr int32 ChatMaxNameChars = 32;
	constexpr int32 ChatMaxTextChars = 256;
	constexpr int32 ChatMaxTimeTextChars = 16;
	constexpr double PublicRateWindowSeconds = 2.0;
	constexpr int32 PublicRateMaxMessagesPerWindow = 4;

	FString SanitizeChatString(FString Value, const int32 MaxChars, const TCHAR* Fallback = TEXT(""))
	{
		Value.TrimStartAndEndInline();
		Value.ReplaceInline(TEXT("\r"), TEXT(" "));
		Value.ReplaceInline(TEXT("\n"), TEXT(" "));
		Value.ReplaceInline(TEXT("\t"), TEXT(" "));
		if (Value.Len() > MaxChars)
		{
			Value.LeftInline(MaxChars, EAllowShrinking::No);
		}
		if (Value.IsEmpty() && Fallback)
		{
			Value = Fallback;
		}
		return Value;
	}

	FText BuildTimeText()
	{
		return FText::FromString(FDateTime::Now().ToString(TEXT("%H:%M:%S")));
	}

	bool IsValidPlayerName(const FString& CandidateName, FString& OutError)
	{
		if (CandidateName.Len() < 3 || CandidateName.Len() > 24)
		{
			OutError = TEXT("Nome deve ter entre 3 e 24 caracteres.");
			return false;
		}
		for (const TCHAR Character : CandidateName)
		{
			if (!FChar::IsAlnum(Character)
				&& Character != TEXT(' ')
				&& Character != TEXT('_')
				&& Character != TEXT('-'))
			{
				OutError = TEXT("Use apenas letras, numeros, espaco, '_' e '-'.");
				return false;
			}
		}
		return true;
	}
}

USpellRiseChatComponent::USpellRiseChatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

bool USpellRiseChatComponent::HandleClientMessage(
	ASpellRisePlayerController* SenderController,
	const FString& RawText,
	const uint8 RequestedChannel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !SenderController || !SenderController->PlayerState)
	{
		return false;
	}

	const FString SafeText = SanitizeChatString(RawText, ChatMaxTextChars);
	if (SafeText.IsEmpty())
	{
		return false;
	}

	if (SafeText.StartsWith(TEXT("/")))
	{
		if (SenderController->HandleServerChatCommand(SafeText))
		{
			return true;
		}
		if (SafeText.StartsWith(TEXT("/name"), ESearchCase::IgnoreCase))
		{
			return HandleNameCommand(SenderController, SafeText);
		}
		SendPrivateSystemMessage(SenderController, TEXT("Comando desconhecido. Use /help."));
		return true;
	}

	if (RequestedChannel == SpellRiseChatChannel::Combat)
	{
		SendPrivateSystemMessage(SenderController, TEXT("Canal Combat e somente leitura."));
		return true;
	}

	if (RequestedChannel != SpellRiseChatChannel::Global)
	{
		SendPrivateSystemMessage(
			SenderController,
			TEXT("Canal ainda sem roteador de membros. Use Global."),
			SpellRiseChatChannel::Global);
		return true;
	}

	FString RejectReason;
	if (!CheckRateLimit(SenderController, RequestedChannel, RejectReason))
	{
		UE_LOG(LogSpellRiseChatRuntime, Warning,
			TEXT("[Chat][PublicRejected] Reason=%s PlayerState=%s"),
			*RejectReason,
			*GetNameSafe(SenderController->PlayerState));
		SendPrivateSystemMessage(SenderController, TEXT("Chat rejeitado: muitas mensagens em pouco tempo."));
		return true;
	}

	BroadcastPublicMessage(BuildPublicMessage(SenderController, SafeText, RequestedChannel));
	return true;
}

bool USpellRiseChatComponent::CheckRateLimit(
	ASpellRisePlayerController* SenderController,
	const uint8 Channel,
	FString& OutRejectReason)
{
	OutRejectReason.Reset();
	if (!SenderController || Channel != SpellRiseChatChannel::Global)
	{
		OutRejectReason = TEXT("invalid_context");
		return false;
	}

	const UWorld* World = GetWorld();
	const double Now = World ? World->GetTimeSeconds() : 0.0;
	FChatRateState& State = PublicRateStates.FindOrAdd(SenderController);
	if (Now - State.WindowStartSeconds > PublicRateWindowSeconds)
	{
		State.WindowStartSeconds = Now;
		State.AcceptedInWindow = 0;
		State.RejectedInWindow = 0;
	}
	if (State.AcceptedInWindow >= PublicRateMaxMessagesPerWindow)
	{
		++State.RejectedInWindow;
		OutRejectReason = TEXT("rate_limit");
		return false;
	}
	++State.AcceptedInWindow;
	return true;
}

bool USpellRiseChatComponent::HandleNameCommand(
	ASpellRisePlayerController* SenderController,
	const FString& RawText)
{
	FString RequestedName = RawText.RightChop(5).TrimStartAndEnd();
	if (RequestedName.StartsWith(TEXT("\""))
		&& RequestedName.EndsWith(TEXT("\""))
		&& RequestedName.Len() >= 2)
	{
		RequestedName = RequestedName.Mid(1, RequestedName.Len() - 2).TrimStartAndEnd();
	}

	FString ValidationError;
	if (!IsValidPlayerName(RequestedName, ValidationError))
	{
		SendPrivateSystemMessage(SenderController, ValidationError);
		return true;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const APlayerController* Candidate = It->Get();
		if (Candidate != SenderController
			&& Candidate
			&& Candidate->PlayerState
			&& Candidate->PlayerState->GetPlayerName().Equals(RequestedName, ESearchCase::IgnoreCase))
		{
			SendPrivateSystemMessage(SenderController, TEXT("Nome ja esta em uso."));
			return true;
		}
	}

	SenderController->PlayerState->SetPlayerName(RequestedName);
	if (UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (USpellRisePersistenceSubsystem* Persistence =
			GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>())
		{
			Persistence->SaveCharacterForController(SenderController);
		}
	}
	SendPrivateSystemMessage(
		SenderController,
		FString::Printf(TEXT("Nome alterado para: %s"), *RequestedName));
	return true;
}

FSpellRiseChatMessage USpellRiseChatComponent::BuildPublicMessage(
	ASpellRisePlayerController* SenderController,
	const FString& Text,
	const uint8 Channel) const
{
	FSpellRiseChatMessage Message;
	const ASpellRisePlayerState* SenderPlayerState =
		SenderController ? SenderController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	const int32 Level = SenderPlayerState && SenderPlayerState->GetProgressionComponent()
		? SenderPlayerState->GetProgressionComponent()->GetCharacterLevel()
		: 1;
	const FString PlayerName = SenderPlayerState
		? SanitizeChatString(SenderPlayerState->GetPlayerName(), ChatMaxNameChars, TEXT("Player"))
		: TEXT("Player");
	Message.Name = FName(*SanitizeChatString(
		FString::Printf(TEXT("%s [%d]"), *PlayerName, Level),
		ChatMaxNameChars,
		TEXT("Player")));
	Message.Text = FText::FromString(SanitizeChatString(Text, ChatMaxTextChars));
	Message.TimeText = BuildTimeText();
	Message.Channel = Channel;
	return Message;
}

void USpellRiseChatComponent::BroadcastPublicMessage(const FSpellRiseChatMessage& Message)
{
	if (!GetWorld())
	{
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ASpellRisePlayerController* TargetController =
			Cast<ASpellRisePlayerController>(It->Get()))
		{
			TargetController->ClientReceivePublicChatMessage(Message);
		}
	}
}

void USpellRiseChatComponent::SendPrivateSystemMessage(
	ASpellRisePlayerController* TargetController,
	const FString& MessageText,
	const uint8 Channel)
{
	if (!TargetController || MessageText.IsEmpty())
	{
		return;
	}
	FSpellRiseChatMessage Message;
	Message.Name = FName(TEXT("System"));
	Message.Text = FText::FromString(SanitizeChatString(MessageText, ChatMaxTextChars));
	Message.TimeText = BuildTimeText();
	Message.Channel = Channel;
	TargetController->ClientReceiveChatMessage(Message);
}

void USpellRiseChatComponent::SendToMULTICAST(
	const FName Name,
	const FText& Text,
	const FText& TimeText,
	const uint8 Channel)
{
	UE_LOG(LogSpellRiseChatRuntime, Warning,
		TEXT("[Chat][LegacySendRejected] Name=%s Channel=%d Reason=use_explicit_controller_rpc"),
		*Name.ToString(),
		Channel);
}

void USpellRiseChatComponent::SendCombatToPlayer(
	ASpellRisePlayerController* TargetPlayerController,
	const FText& Text,
	const FText& TimeText)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !TargetPlayerController || Text.IsEmpty())
	{
		return;
	}
	FSpellRiseChatMessage Message;
	Message.Name = FName(TEXT("Combat"));
	Message.Text = FText::FromString(SanitizeChatString(Text.ToString(), ChatMaxTextChars));
	Message.TimeText = FText::FromString(SanitizeChatString(TimeText.ToString(), ChatMaxTimeTextChars));
	Message.Channel = SpellRiseChatChannel::Combat;
	TargetPlayerController->ClientReceiveChatMessage(Message);
}
