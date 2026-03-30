#include "SpellRise/Components/SpellRiseChatComponent.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseChatRuntime, Log, All);

namespace
{
	static FText BuildChatTimeText()
	{
		return FText::FromString(FDateTime::Now().ToString(TEXT("%H:%M:%S")));
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
				OutError = TEXT("Use apenas letras, numeros, espaco, '_' e '-'.");
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

		FSpellRiseChatMessage Message;
		Message.Name = FName(TEXT("Sistema"));
		Message.Text = FText::FromString(MessageText);
		Message.TimeText = BuildChatTimeText();
		Message.Channel = Channel;
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
		UE_LOG(LogSpellRiseChatRuntime, Verbose, TEXT("[Chat] Ignored manual SendToMULTICAST in combat channel."));
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
			SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, TEXT("Use /name apenas no chat geral."));
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
			SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, TEXT("Seu nome ja esta definido assim."));
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
					UE_LOG(LogSpellRiseChatRuntime, Log, TEXT("[Chat][/name] Persisted immediately Controller=%s Success=%d"),
						*GetNameSafe(SenderController),
						bSaved ? 1 : 0);
				}
			}
		}

		const FString SuccessText = FString::Printf(TEXT("Nome alterado para: %s"), *RequestedName);
		SendPrivateSystemMessage(SenderController, SpellRiseChatChannel::Global, SuccessText);
		return;
	}

	FSpellRiseChatMessage Message;
	Message.Name = Name;
	Message.Text = Text;
	Message.TimeText = TimeText;
	Message.Channel = Channel;

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

	FSpellRiseChatMessage Message;
	Message.Name = FName(TEXT("Combat"));
	Message.Text = Text;
	Message.TimeText = TimeText;
	Message.Channel = SpellRiseChatChannel::Combat;

	UE_LOG(LogSpellRiseChatRuntime, VeryVerbose, TEXT("[CombatChat] Routed combat feed to %s (inventory/full-loot untouched)."),
		*GetNameSafe(TargetPlayerController));

	TargetPlayerController->ClientReceiveChatMessage(Message);
}

void USpellRiseChatComponent::Multi_ReceivePublicMessage_Implementation(const FSpellRiseChatMessage& Message)
{
	if (ASpellRisePlayerController* LocalPlayerController = Cast<ASpellRisePlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		LocalPlayerController->ReceiveChatMessageLocal(Message);
	}
}

