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
	constexpr double PartyInviteLifetimeSeconds = 30.0;

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
		return FText::FromString(FDateTime::Now().ToString(TEXT("%H:%M")));
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

	if (HandlePartyResponse(SenderController, SafeText))
	{
		return true;
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
		if (SafeText.StartsWith(TEXT("/invite"), ESearchCase::IgnoreCase))
		{
			return HandleInviteCommand(SenderController, SafeText);
		}
		if (SafeText.StartsWith(TEXT("/remove"), ESearchCase::IgnoreCase))
		{
			return HandleRemoveCommand(SenderController, SafeText);
		}
		if (SafeText.StartsWith(TEXT("/leader"), ESearchCase::IgnoreCase))
		{
			return HandleLeaderCommand(SenderController, SafeText);
		}
		if (SafeText.Equals(TEXT("/leave"), ESearchCase::IgnoreCase))
		{
			return HandleLeaveCommand(SenderController);
		}
		if (SafeText.Equals(TEXT("/party"), ESearchCase::IgnoreCase))
		{
			return HandlePartyListCommand(SenderController);
		}
		SendPrivateSystemMessage(SenderController, TEXT("Comando desconhecido. Use /help."));
		return true;
	}

	if (RequestedChannel == SpellRiseChatChannel::Combat)
	{
		SendPrivateSystemMessage(SenderController, TEXT("Canal Combat e somente leitura."));
		return true;
	}

	if (RequestedChannel == SpellRiseChatChannel::Party)
	{
		const ASpellRisePlayerState* SenderPlayerState =
			SenderController->GetPlayerState<ASpellRisePlayerState>();
		if (!SenderPlayerState || !SenderPlayerState->IsInParty())
		{
			SendPrivateSystemMessage(SenderController, TEXT("Voce nao esta em uma Party."), SpellRiseChatChannel::Party);
			return true;
		}
		FString RejectReason;
		if (!CheckRateLimit(SenderController, RequestedChannel, RejectReason))
		{
			SendPrivateSystemMessage(SenderController, TEXT("Chat rejeitado: muitas mensagens em pouco tempo."), SpellRiseChatChannel::Party);
			return true;
		}
		BroadcastPartyMessage(SenderController, BuildPublicMessage(SenderController, SafeText, RequestedChannel));
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
	if (!SenderController
		|| (Channel != SpellRiseChatChannel::Global && Channel != SpellRiseChatChannel::Party))
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

bool USpellRiseChatComponent::HandleInviteCommand(
	ASpellRisePlayerController* SenderController,
	const FString& RawText)
{
	if (!SenderController || !SenderController->PlayerState || !GetWorld())
	{
		return true;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	double& LastInvite = LastPartyInviteSeconds.FindOrAdd(SenderController);
	if (Now - LastInvite < 1.0)
	{
		UE_LOG(LogSpellRiseChatRuntime, Warning,
			TEXT("[Party][InviteRejected] Reason=rate_limit Sender=%s"),
			*GetNameSafe(SenderController->PlayerState));
		SendPrivateSystemMessage(SenderController, TEXT("Invite rejeitado: aguarde antes de tentar novamente."));
		return true;
	}
	LastInvite = Now;

	FString TargetName = RawText.RightChop(7).TrimStartAndEnd();
	if (TargetName.StartsWith(TEXT("\"")) && TargetName.EndsWith(TEXT("\"")) && TargetName.Len() >= 2)
	{
		TargetName = TargetName.Mid(1, TargetName.Len() - 2).TrimStartAndEnd();
	}
	if (TargetName.IsEmpty() || TargetName.Len() > 24)
	{
		SendPrivateSystemMessage(SenderController, TEXT("Uso: /invite Player"));
		return true;
	}

	ASpellRisePlayerController* TargetController = nullptr;
	int32 MatchCount = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		if (Candidate && Candidate->PlayerState
			&& Candidate->PlayerState->GetPlayerName().Equals(TargetName, ESearchCase::IgnoreCase))
		{
			TargetController = Candidate;
			++MatchCount;
		}
	}

	if (MatchCount != 1 || !TargetController || TargetController == SenderController)
	{
		SendPrivateSystemMessage(SenderController,
			MatchCount > 1 ? TEXT("Invite rejeitado: nome ambiguo.") : TEXT("Invite rejeitado: player nao encontrado."));
		return true;
	}

	ASpellRisePlayerState* SenderPS = SenderController->GetPlayerState<ASpellRisePlayerState>();
	ASpellRisePlayerState* TargetPS = TargetController->GetPlayerState<ASpellRisePlayerState>();
	if (!SenderPS || !TargetPS)
	{
		SendPrivateSystemMessage(SenderController, TEXT("Invite rejeitado: PlayerState indisponivel."));
		return true;
	}
	if (TargetPS->IsInParty())
	{
		SendPrivateSystemMessage(SenderController,
			SenderPS->IsInSamePartyWith(TargetPS)
				? TEXT("Esse player ja esta na sua Party.")
				: TEXT("Invite rejeitado: esse player ja esta em outra Party."));
		return true;
	}
	if (SenderPS->IsInParty() && !SenderPS->IsPartyLeader())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Invite rejeitado: somente o leader pode convidar."));
		return true;
	}
	if (const FPendingPartyInvite* ExistingInvite = PendingPartyInvites.Find(TargetController))
	{
		if (ExistingInvite->ExpiresAtSeconds > Now)
		{
			SendPrivateSystemMessage(SenderController, TEXT("Invite rejeitado: esse player ja possui convite pendente."));
			return true;
		}
	}

	FPendingPartyInvite& PendingInvite = PendingPartyInvites.FindOrAdd(TargetController);
	PendingInvite.Inviter = SenderController;
	PendingInvite.ExpiresAtSeconds = Now + PartyInviteLifetimeSeconds;
	SendPrivateSystemMessage(SenderController,
		FString::Printf(TEXT("Convite de Party enviado para %s."), *TargetPS->GetPlayerName()),
		SpellRiseChatChannel::Party);
	SendPrivateSystemMessage(TargetController,
		FString::Printf(TEXT("%s convidou voce para a Party. Digite Y para aceitar ou N para recusar."), *SenderPS->GetPlayerName()),
		SpellRiseChatChannel::Party);
	UE_LOG(LogSpellRiseChatRuntime, Log,
		TEXT("[Party][InviteCreated] Sender=%s Target=%s ExpiresIn=%.0f"),
		*SenderPS->GetPlayerName(),
		*TargetPS->GetPlayerName(),
		PartyInviteLifetimeSeconds);
	return true;
}

bool USpellRiseChatComponent::HandlePartyResponse(
	ASpellRisePlayerController* SenderController,
	const FString& RawText)
{
	if (!RawText.Equals(TEXT("Y"), ESearchCase::IgnoreCase)
		&& !RawText.Equals(TEXT("N"), ESearchCase::IgnoreCase))
	{
		return false;
	}

	FPendingPartyInvite* PendingInvite = PendingPartyInvites.Find(SenderController);
	if (!PendingInvite)
	{
		return false;
	}

	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	ASpellRisePlayerController* Inviter = PendingInvite->Inviter.Get();
	const double InviteExpiresAtSeconds = PendingInvite->ExpiresAtSeconds;
	PendingPartyInvites.Remove(SenderController);
	if (!Inviter || InviteExpiresAtSeconds <= Now)
	{
		SendPrivateSystemMessage(SenderController, TEXT("Convite de Party expirado."), SpellRiseChatChannel::Party);
		return true;
	}

	ASpellRisePlayerState* InviterPS = Inviter->GetPlayerState<ASpellRisePlayerState>();
	ASpellRisePlayerState* TargetPS = SenderController->GetPlayerState<ASpellRisePlayerState>();
	if (!InviterPS || !TargetPS || TargetPS->IsInParty())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Convite de Party nao e mais valido."), SpellRiseChatChannel::Party);
		return true;
	}
	if (RawText.Equals(TEXT("N"), ESearchCase::IgnoreCase))
	{
		SendPrivateSystemMessage(SenderController, TEXT("Convite de Party recusado."), SpellRiseChatChannel::Party);
		SendPrivateSystemMessage(Inviter,
			FString::Printf(TEXT("%s recusou o convite de Party."), *TargetPS->GetPlayerName()),
			SpellRiseChatChannel::Party);
		return true;
	}
	if (InviterPS->IsInParty() && !InviterPS->IsPartyLeader())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Convite de Party nao e mais valido."), SpellRiseChatChannel::Party);
		return true;
	}

	FString PartyId = InviterPS->GetPartyId();
	FString LeaderId = InviterPS->GetPartyLeaderId();
	if (PartyId.IsEmpty())
	{
		PartyId = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
		LeaderId = ResolveStablePlayerId(InviterPS);
		InviterPS->SetPartyState_Server(PartyId, LeaderId);
	}
	TargetPS->SetPartyState_Server(PartyId, LeaderId);
	SendPrivateSystemMessage(SenderController,
		FString::Printf(TEXT("Voce entrou na Party de %s."), *InviterPS->GetPlayerName()),
		SpellRiseChatChannel::Party);
	BroadcastPartyMessage(Inviter, BuildPublicMessage(
		Inviter,
		FString::Printf(TEXT("%s entrou na Party."), *TargetPS->GetPlayerName()),
		SpellRiseChatChannel::Party));
	return true;
}

ASpellRisePlayerController* USpellRiseChatComponent::FindUniquePlayerByName(
	const FString& PlayerName,
	int32& OutMatchCount) const
{
	OutMatchCount = 0;
	ASpellRisePlayerController* Match = nullptr;
	if (!GetWorld())
	{
		return nullptr;
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		if (Candidate && Candidate->PlayerState
			&& Candidate->PlayerState->GetPlayerName().Equals(PlayerName, ESearchCase::IgnoreCase))
		{
			Match = Candidate;
			++OutMatchCount;
		}
	}
	return Match;
}

FString USpellRiseChatComponent::ResolveStablePlayerId(const ASpellRisePlayerState* PlayerState) const
{
	if (!PlayerState)
	{
		return FString();
	}
	return PlayerState->GetUniqueId().IsValid()
		? PlayerState->GetUniqueId()->ToString().Left(64)
		: FString::Printf(TEXT("NAME:%s"), *PlayerState->GetPlayerName()).Left(64);
}

bool USpellRiseChatComponent::HandleRemoveCommand(
	ASpellRisePlayerController* SenderController,
	const FString& RawText)
{
	ASpellRisePlayerState* SenderPS = SenderController ? SenderController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (!SenderPS || !SenderPS->IsPartyLeader())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Remove rejeitado: somente o leader pode remover."), SpellRiseChatChannel::Party);
		return true;
	}
	const FString TargetName = RawText.RightChop(7).TrimStartAndEnd();
	int32 MatchCount = 0;
	ASpellRisePlayerController* Target = FindUniquePlayerByName(TargetName, MatchCount);
	ASpellRisePlayerState* TargetPS = Target ? Target->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (MatchCount != 1 || !TargetPS || Target == SenderController || !SenderPS->IsInSamePartyWith(TargetPS))
	{
		SendPrivateSystemMessage(SenderController, TEXT("Remove rejeitado: membro nao encontrado na sua Party."), SpellRiseChatChannel::Party);
		return true;
	}
	const FString RemovedName = TargetPS->GetPlayerName();
	TargetPS->SetPartyState_Server(FString(), FString());
	SendPrivateSystemMessage(Target, TEXT("Voce foi removido da Party."), SpellRiseChatChannel::Party);
	BroadcastPartyMessage(SenderController, BuildPublicMessage(
		SenderController,
		FString::Printf(TEXT("%s foi removido da Party."), *RemovedName),
		SpellRiseChatChannel::Party));
	DissolvePartyIfNeeded(SenderPS->GetPartyId());
	return true;
}

bool USpellRiseChatComponent::HandleLeaderCommand(
	ASpellRisePlayerController* SenderController,
	const FString& RawText)
{
	ASpellRisePlayerState* SenderPS = SenderController ? SenderController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (!SenderPS || !SenderPS->IsPartyLeader())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Leader rejeitado: somente o leader atual pode transferir."), SpellRiseChatChannel::Party);
		return true;
	}
	const FString TargetName = RawText.RightChop(7).TrimStartAndEnd();
	int32 MatchCount = 0;
	ASpellRisePlayerController* Target = FindUniquePlayerByName(TargetName, MatchCount);
	ASpellRisePlayerState* TargetPS = Target ? Target->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (MatchCount != 1 || !TargetPS || Target == SenderController || !SenderPS->IsInSamePartyWith(TargetPS))
	{
		SendPrivateSystemMessage(SenderController, TEXT("Leader rejeitado: membro nao encontrado na sua Party."), SpellRiseChatChannel::Party);
		return true;
	}
	const FString NewLeaderId = ResolveStablePlayerId(TargetPS);
	if (NewLeaderId.IsEmpty())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Leader rejeitado: identidade do player indisponivel."), SpellRiseChatChannel::Party);
		return true;
	}
	UpdatePartyLeaderForMembers(SenderPS->GetPartyId(), NewLeaderId);
	BroadcastPartyMessage(Target, BuildPublicMessage(
		Target,
		FString::Printf(TEXT("%s agora e o leader da Party."), *TargetPS->GetPlayerName()),
		SpellRiseChatChannel::Party));
	return true;
}

bool USpellRiseChatComponent::HandleLeaveCommand(ASpellRisePlayerController* SenderController)
{
	ASpellRisePlayerState* SenderPS = SenderController ? SenderController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (!SenderPS || !SenderPS->IsInParty())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Voce nao esta em uma Party."), SpellRiseChatChannel::Party);
		return true;
	}
	const FString PartyId = SenderPS->GetPartyId();
	const bool bWasLeader = SenderPS->IsPartyLeader();
	const FString LeavingName = SenderPS->GetPlayerName();
	SenderPS->SetPartyState_Server(FString(), FString());
	SendPrivateSystemMessage(SenderController, TEXT("Voce saiu da Party."), SpellRiseChatChannel::Party);

	ASpellRisePlayerState* NewLeader = nullptr;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		ASpellRisePlayerState* CandidatePS = Candidate ? Candidate->GetPlayerState<ASpellRisePlayerState>() : nullptr;
		if (CandidatePS && CandidatePS->GetPartyId().Equals(PartyId, ESearchCase::CaseSensitive))
		{
			NewLeader = CandidatePS;
			break;
		}
	}
	if (bWasLeader && NewLeader)
	{
		UpdatePartyLeaderForMembers(PartyId, ResolveStablePlayerId(NewLeader));
	}
	if (NewLeader)
	{
		ASpellRisePlayerController* PartyController = Cast<ASpellRisePlayerController>(NewLeader->GetPlayerController());
		BroadcastPartyMessage(PartyController, BuildPublicMessage(
			PartyController,
			FString::Printf(TEXT("%s saiu da Party."), *LeavingName),
			SpellRiseChatChannel::Party));
	}
	DissolvePartyIfNeeded(PartyId);
	return true;
}

bool USpellRiseChatComponent::HandlePartyListCommand(ASpellRisePlayerController* SenderController)
{
	const ASpellRisePlayerState* SenderPS =
		SenderController ? SenderController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (!SenderPS || !SenderPS->IsInParty() || !GetWorld())
	{
		SendPrivateSystemMessage(SenderController, TEXT("Voce nao esta em uma Party."), SpellRiseChatChannel::Party);
		return true;
	}

	TArray<FString> MemberNames;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		const ASpellRisePlayerState* CandidatePS =
			Candidate ? Candidate->GetPlayerState<ASpellRisePlayerState>() : nullptr;
		if (CandidatePS && SenderPS->IsInSamePartyWith(CandidatePS))
		{
			const FString Suffix = CandidatePS->IsPartyLeader() ? TEXT(" [Leader]") : FString();
			MemberNames.Add(SanitizeChatString(CandidatePS->GetPlayerName(), 24, TEXT("Player")) + Suffix);
		}
	}
	MemberNames.Sort();
	SendPrivateSystemMessage(
		SenderController,
		FString::Printf(TEXT("Party (%d): %s"), MemberNames.Num(), *FString::Join(MemberNames, TEXT(", "))),
		SpellRiseChatChannel::Party);
	return true;
}

void USpellRiseChatComponent::HandleControllerLogout(ASpellRisePlayerController* ExitingController)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ExitingController)
	{
		return;
	}

	PendingPartyInvites.Remove(ExitingController);
	LastPartyInviteSeconds.Remove(ExitingController);
	PublicRateStates.Remove(ExitingController);
	for (auto It = PendingPartyInvites.CreateIterator(); It; ++It)
	{
		if (!It.Value().Inviter.IsValid() || It.Value().Inviter.Get() == ExitingController)
		{
			It.RemoveCurrent();
		}
	}

	ASpellRisePlayerState* ExitingPS = ExitingController->GetPlayerState<ASpellRisePlayerState>();
	if (!ExitingPS || !ExitingPS->IsInParty())
	{
		return;
	}

	const FString PartyId = ExitingPS->GetPartyId();
	const bool bWasLeader = ExitingPS->IsPartyLeader();
	const FString ExitingName = ExitingPS->GetPlayerName();
	ExitingPS->SetPartyState_Server(FString(), FString());

	ASpellRisePlayerController* RemainingController = nullptr;
	ASpellRisePlayerState* NewLeaderPS = nullptr;
	if (GetWorld())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
			ASpellRisePlayerState* CandidatePS =
				Candidate ? Candidate->GetPlayerState<ASpellRisePlayerState>() : nullptr;
			if (Candidate != ExitingController
				&& CandidatePS
				&& CandidatePS->GetPartyId().Equals(PartyId, ESearchCase::CaseSensitive))
			{
				RemainingController = Candidate;
				NewLeaderPS = CandidatePS;
				break;
			}
		}
	}
	if (bWasLeader && NewLeaderPS)
	{
		UpdatePartyLeaderForMembers(PartyId, ResolveStablePlayerId(NewLeaderPS));
	}
	if (RemainingController)
	{
		BroadcastPartyMessage(RemainingController, BuildPublicMessage(
			RemainingController,
			FString::Printf(TEXT("%s desconectou e saiu da Party."), *ExitingName),
			SpellRiseChatChannel::Party));
	}
	DissolvePartyIfNeeded(PartyId);
	UE_LOG(LogSpellRiseChatRuntime, Log,
		TEXT("[Party][MemberLogout] PartyId=%s Player=%s WasLeader=%d"),
		*PartyId,
		*ExitingName,
		bWasLeader ? 1 : 0);
}

void USpellRiseChatComponent::UpdatePartyLeaderForMembers(
	const FString& PartyId,
	const FString& LeaderId)
{
	if (!GetWorld() || PartyId.IsEmpty() || LeaderId.IsEmpty())
	{
		return;
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		ASpellRisePlayerState* CandidatePS = Candidate ? Candidate->GetPlayerState<ASpellRisePlayerState>() : nullptr;
		if (CandidatePS && CandidatePS->GetPartyId().Equals(PartyId, ESearchCase::CaseSensitive))
		{
			CandidatePS->SetPartyState_Server(PartyId, LeaderId);
		}
	}
}

void USpellRiseChatComponent::DissolvePartyIfNeeded(const FString& PartyId)
{
	if (!GetWorld() || PartyId.IsEmpty())
	{
		return;
	}
	TArray<ASpellRisePlayerState*> Members;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* Candidate = Cast<ASpellRisePlayerController>(It->Get());
		ASpellRisePlayerState* CandidatePS = Candidate ? Candidate->GetPlayerState<ASpellRisePlayerState>() : nullptr;
		if (CandidatePS && CandidatePS->GetPartyId().Equals(PartyId, ESearchCase::CaseSensitive))
		{
			Members.Add(CandidatePS);
		}
	}
	if (Members.Num() <= 1)
	{
		for (ASpellRisePlayerState* Member : Members)
		{
			Member->SetPartyState_Server(FString(), FString());
		}
	}
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

void USpellRiseChatComponent::BroadcastPartyMessage(
	ASpellRisePlayerController* SenderController,
	const FSpellRiseChatMessage& Message)
{
	const ASpellRisePlayerState* SenderPS =
		SenderController ? SenderController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
	if (!SenderPS || !SenderPS->IsInParty() || !GetWorld())
	{
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASpellRisePlayerController* TargetController = Cast<ASpellRisePlayerController>(It->Get());
		const ASpellRisePlayerState* TargetPS =
			TargetController ? TargetController->GetPlayerState<ASpellRisePlayerState>() : nullptr;
		if (TargetController && TargetPS && SenderPS->IsInSamePartyWith(TargetPS))
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
