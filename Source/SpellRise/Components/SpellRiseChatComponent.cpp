// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Components/SpellRiseChatComponent.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
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

	static FText BuildChatTimeText()
	{
		return FText::FromString(FDateTime::Now().ToString(TEXT("%H:%M:%S")));
	}

	static FString SanitizeBoundedChatString(FString Value, const int32 MaxChars, const TCHAR* Fallback = TEXT(""))
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

	static FSpellRiseChatMessage BuildBoundedChatMessage(FName Name, const FText& Text, const FText& TimeText, const uint8 Channel)
	{
		FSpellRiseChatMessage Message;
		Message.Name = FName(*SanitizeBoundedChatString(Name.ToString(), ChatMaxNameChars, TEXT("System")));
		Message.Text = FText::FromString(SanitizeBoundedChatString(Text.ToString(), ChatMaxTextChars));
		Message.TimeText = FText::FromString(SanitizeBoundedChatString(TimeText.ToString(), ChatMaxTimeTextChars));
		Message.Channel = Channel;
		return Message;
	}

	static ASpellRisePlayerController* FindSenderControllerByName(UWorld* World, const FName DeclaredName)
	{
		if (!World || DeclaredName.IsNone())
		{
			return nullptr;
		}

		const FString SenderName = DeclaredName.ToString();
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			ASpellRisePlayerController* Controller = Cast<ASpellRisePlayerController>(It->Get());
			if (!Controller)
			{
				continue;
			}

			const APlayerState* PS = Controller->PlayerState;
			if (!PS)
			{
				continue;
			}

			if (PS->GetPlayerName().Equals(SenderName, ESearchCase::IgnoreCase))
			{
				return Controller;
			}
		}

		return nullptr;
	}

	static bool TryParseNameCommand(const FString& MessageText, FString& OutNewName)
	{
		const FString TrimmedText = MessageText.TrimStartAndEnd();
		if (!TrimmedText.StartsWith(TEXT("/name"), ESearchCase::IgnoreCase))
		{
			return false;
		}

		FString Payload = TrimmedText.RightChop(5).TrimStartAndEnd();
		if (Payload.StartsWith(TEXT("\"")) && Payload.EndsWith(TEXT("\"")) && Payload.Len() >= 2)
		{
			Payload = Payload.Mid(1, Payload.Len() - 2);
		}

		OutNewName = Payload.TrimStartAndEnd();
		return true;
	}

	static bool IsValidPlayerNameForChatCommand(const FString& CandidateName, FString& OutError)
	{
		const int32 NameLen = CandidateName.Len();
		if (NameLen < 3)
		{
			OutError = TEXT("Nome muito curto. Minimo: 3 caracteres.");
			return false;
		}

		if (NameLen > 24)
		{
			OutError = TEXT("Nome muito longo. Maximo: 24 caracteres.");
			return false;
		}

		for (TCHAR Ch : CandidateName)
		{
			const bool bAllowed =
				FChar::IsAlnum(Ch) ||
				Ch == TEXT(' ') ||
				Ch == TEXT('_') ||
				Ch == TEXT('-');
			if (!bAllowed)
			{
				OutError = TEXT("Use only letters, numbers, spaces, '_' and '-'.");
				return false;
			}
		}

		return true;
	}

	static void SendPrivateSystemMessage(ASpellRisePlayerController* TargetController, uint8 Channel, const FString& MessageText)
	{
		if (!TargetController || MessageText.IsEmpty())
		{
			return;
		}

		const FSpellRiseChatMessage Message = BuildBoundedChatMessage(
			FName(TEXT("System")),
			FText::FromString(MessageText),
			BuildChatTimeText(),
			Channel);
		TargetController->ClientReceiveChatMessage(Message);
	}
}

USpellRiseChatComponent::USpellRiseChatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USpellRiseChatComponent::SendToMULTICAST(FName Name, const FText& Text, const FText& TimeText, uint8 Channel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (Channel == SpellRiseChatChannel::Combat)
	{
		return;
	}

	if (Text.IsEmpty())
	{
		return;
	}

	const FString RawText = Text.ToString();
	FString RequestedName;
	if (TryParseNameCommand(RawText, RequestedName))
	{
		ASpellRisePlayerController* SenderController = FindSenderControllerByName(GetWorld(), Name);
		if (!SenderController || !SenderController->PlayerState)
		{
			return;
		}

		if (Channel != SpellRiseChatChannel::Global)
		{
			SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, TEXT("Use /name only in global chat."));
			return;
		}

		FString ValidationError;
		if (!IsValidPlayerNameForChatCommand(RequestedName, ValidationError))
		{
			SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, ValidationError);
			return;
		}

		const FString OldName = SenderController->PlayerState->GetPlayerName();
		if (OldName.Equals(RequestedName, ESearchCase::IgnoreCase))
		{
			SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, TEXT("Your name is already set to that."));
			return;
		}

		SenderController->PlayerState->SetPlayerName(RequestedName);
		if (UWorld* World = SenderController->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (USpellRisePersistenceSubsystem* Persistence = GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>())
				{
					const bool bSaved = Persistence->SaveCharacterForController(SenderController);
				}
			}
		}

		const FString SuccessText = FString::Printf(TEXT("Name changed to: %s"), *RequestedName);
		SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, SuccessText);
		return;
	}

	FName DisplayName = Name;
	if (ASpellRisePlayerController* SenderController = FindSenderControllerByName(GetWorld(), Name))
	{
		if (const ASpellRisePlayerState* SenderPlayerState = SenderController->GetPlayerState<ASpellRisePlayerState>())
		{
			const int32 Level = SenderPlayerState->GetProgressionComponent()
				? SenderPlayerState->GetProgressionComponent()->GetCharacterLevel()
				: 1;
			DisplayName = FName(*FString::Printf(TEXT("%s [%d]"), *Name.ToString(), Level));
		}
	}

	const FSpellRiseChatMessage Message = BuildBoundedChatMessage(DisplayName, Text, TimeText, Channel);

	Multi_ReceivePublicMessage(Message);
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

	const FSpellRiseChatMessage Message = BuildBoundedChatMessage(
		FName(TEXT("Combat")),
		Text,
		TimeText,
		SpellRiseChatChannel::Combat);

	TargetPlayerController->ClientReceiveChatMessage(Message);
}

void USpellRiseChatComponent::Multi_ReceivePublicMessage_Implementation(const FSpellRiseChatMessage& Message)
{
	if (ASpellRisePlayerController* LocalPlayerController = Cast<ASpellRisePlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		LocalPlayerController->ReceiveChatMessageLocal(Message);
	}
}
