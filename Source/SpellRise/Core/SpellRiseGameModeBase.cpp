#include "SpellRiseGameModeBase.h"

#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Crc.h"
#include "Misc/Parse.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/GameSession.h"

#include "SpellRiseGameState.h"
#include "SpellRisePlayerController.h"
#include "SpellRisePlayerState.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseLoginPersistence, Log, All);

namespace
{
	constexpr int32 MaxPersistentIdLen = 64;

	FString BuildSafeIdToken(const FString& Source)
	{
		FString SafeToken;
		SafeToken.Reserve(Source.Len());

		for (const TCHAR Ch : Source)
		{
			if (FChar::IsAlnum(Ch) || Ch == TEXT('_') || Ch == TEXT('-'))
			{
				SafeToken.AppendChar(Ch);
			}
		}

		if (SafeToken.IsEmpty())
		{
			SafeToken = TEXT("Player");
		}

		if (SafeToken.Len() > 16)
		{
			SafeToken.LeftInline(16, EAllowShrinking::No);
		}

		return SafeToken;
	}

	FString DescribeUniqueId(const FUniqueNetIdRepl& UniqueIdRepl)
	{
		if (!UniqueIdRepl.IsValid())
		{
			return TEXT("INVALID");
		}

		const TSharedPtr<const FUniqueNetId> NetId = UniqueIdRepl.GetUniqueNetId();
		if (!NetId.IsValid())
		{
			return TEXT("INVALID_PTR");
		}

		const FName Type = NetId->GetType();
		return FString::Printf(TEXT("%s:%s"), *Type.ToString(), *NetId->ToString());
	}

	FString DescribeSubsystemPtr(const IOnlineSubsystem* Subsystem)
	{
		return Subsystem ? Subsystem->GetSubsystemName().ToString() : TEXT("NONE");
	}

	FString DescribeOnlineSubsystemState()
	{
		const IOnlineSubsystem* DefaultSubsystem = IOnlineSubsystem::Get();
		const IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(TEXT("STEAM"));
		const IOnlineSubsystem* NullSubsystem = IOnlineSubsystem::Get(TEXT("NULL"));

		return FString::Printf(
			TEXT("Default=%s SteamLoaded=%d NullLoaded=%d"),
			*DescribeSubsystemPtr(DefaultSubsystem),
			SteamSubsystem ? 1 : 0,
			NullSubsystem ? 1 : 0);
	}

	bool IsSteamUniqueId(const FUniqueNetIdRepl& UniqueIdRepl)
	{
		if (!UniqueIdRepl.IsValid())
		{
			return false;
		}

		const TSharedPtr<const FUniqueNetId> NetId = UniqueIdRepl.GetUniqueNetId();
		return NetId.IsValid() && NetId->GetType() == FName(TEXT("STEAM"));
	}
}

ASpellRiseGameModeBase::ASpellRiseGameModeBase()
{
	GameStateClass = ASpellRiseGameState::StaticClass();
	PlayerControllerClass = ASpellRisePlayerController::StaticClass();
	PlayerStateClass = ASpellRisePlayerState::StaticClass();
}

void ASpellRiseGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (bLoadWorldPersistenceOnBeginPlay)
	{
		if (USpellRisePersistenceSubsystem* Persistence = GetPersistenceSubsystem())
		{
			Persistence->LoadWorld(GetWorld());
		}
	}

	StartPersistenceSnapshotTimer();
}

void ASpellRiseGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority() && bAutoSaveWorldOnShutdown)
	{
		SaveAllPlayersAndWorld();
	}

	StopPersistenceSnapshotTimer();
	Super::EndPlay(EndPlayReason);
}

void ASpellRiseGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	const bool bNoSteamParam = IsNoSteamCommandLineParamPresent();
	const bool bNoSteamTestingActive = IsNoSteamTestingModeActive();
	const bool bRequireSteamInThisContext = ShouldRequireSteamAuthentication();
	UE_LOG(
		LogSpellRiseLoginPersistence,
		Log,
		TEXT("[Net][PreLoginStart] Address=%s UniqueId=%s Options=%s RequireSteamDS=%d NoSteamParam=%d NoSteamTesting=%d OSS={%s}"),
		*Address,
		*DescribeUniqueId(UniqueId),
		*Options,
		bRequireSteamAuthOnDedicatedServer ? 1 : 0,
		bNoSteamParam ? 1 : 0,
		bNoSteamTestingActive ? 1 : 0,
		*DescribeOnlineSubsystemState());

	if (bNoSteamParam && !bNoSteamTestingActive)
	{
		UE_LOG(
			LogSpellRiseLoginPersistence,
			Warning,
			TEXT("[Auth][NoSteamBlocked] Address=%s RequireSteam=%d NoSteamParam=%d NoSteamTesting=%d"),
			*Address,
			bRequireSteamAuthOnDedicatedServer ? 1 : 0,
			1,
			0);
	}

	if (!ErrorMessage.IsEmpty())
	{
		UE_LOG(
			LogSpellRiseLoginPersistence,
			Warning,
			TEXT("[Net][PreLoginRejectedBySuper] Address=%s UniqueId=%s Error=%s OSS={%s}"),
			*Address,
			*DescribeUniqueId(UniqueId),
			*ErrorMessage,
			*DescribeOnlineSubsystemState());

		if (ErrorMessage.Equals(TEXT("incompatible_unique_net_id"), ESearchCase::IgnoreCase))
		{
			UE_LOG(
				LogSpellRiseLoginPersistence,
				Warning,
				TEXT("[Net][PreLoginRejectedReason] Address=%s Cause=UniqueNetIdMismatch ExpectedSubsystem=%s"),
				*Address,
			*DescribeSubsystemPtr(IOnlineSubsystem::Get()));
		}
		return;
	}

	if (bRequireSteamInThisContext)
	{
		const IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
		if (!OSS || OSS->GetSubsystemName() != FName(TEXT("STEAM")))
		{
			ErrorMessage = TEXT("AUTH_STEAM_REQUIRED");
			UE_LOG(
				LogSpellRiseLoginPersistence,
				Warning,
				TEXT("[Auth][PreLoginReject] Address=%s Reason=steam_required UniqueId=%s OSS={%s}"),
				*Address,
				*DescribeUniqueId(UniqueId),
				*DescribeOnlineSubsystemState());
			return;
		}

		if (!UniqueId.IsValid())
		{
			ErrorMessage = TEXT("AUTH_STEAM_REQUIRED");
			UE_LOG(
				LogSpellRiseLoginPersistence,
				Warning,
				TEXT("[Auth][PreLoginReject] Address=%s Reason=steam_required UniqueId=%s OSS={%s}"),
				*Address,
				*DescribeUniqueId(UniqueId),
				*DescribeOnlineSubsystemState());
			return;
		}

		if (!IsSteamUniqueId(UniqueId))
		{
			ErrorMessage = TEXT("AUTH_STEAM_REQUIRED");
			UE_LOG(
				LogSpellRiseLoginPersistence,
				Warning,
				TEXT("[Auth][PreLoginReject] Address=%s Reason=unique_id_type_mismatch UniqueId=%s OSS={%s}"),
				*Address,
				*DescribeUniqueId(UniqueId),
				*DescribeOnlineSubsystemState());
			return;
		}
	}

	bool bUsedDevFallback = false;
	const FString PersistentId = ResolvePersistentIdFromPreLoginData(Options, Address, UniqueId, bUsedDevFallback);
	const bool bOfflineFallbackAllowedInThisContext = bAllowDevOfflineIdFallback || bNoSteamTestingActive;
	if (PersistentId.IsEmpty())
	{
		ErrorMessage = TEXT("AUTH_STEAM_REQUIRED");
		UE_LOG(
			LogSpellRiseLoginPersistence,
			Warning,
			TEXT("[Auth][PreLoginReject] Address=%s Reason=missing_persistent_id RequireSteam=%d NoSteamParam=%d FallbackEnabled=%d UniqueId=%s OSS={%s}"),
			*Address,
			bRequireSteamAuthOnDedicatedServer ? 1 : 0,
			bNoSteamParam ? 1 : 0,
			bOfflineFallbackAllowedInThisContext ? 1 : 0,
			*DescribeUniqueId(UniqueId),
			*DescribeOnlineSubsystemState());
		return;
	}

	const FString AddressKey = NormalizeAddressKey(Address);
	if (!AddressKey.IsEmpty())
	{
		PendingPersistentIdByAddress.Add(AddressKey, PersistentId);
	}

	UE_LOG(
		LogSpellRiseLoginPersistence,
		Log,
		TEXT("[Auth][PreLoginAccept] Address=%s PersistentId=%s Source=%s"),
		*Address,
		*PersistentId,
		bUsedDevFallback ? TEXT("DevFallback") : TEXT("Steam"));
}

void ASpellRiseGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (!HasAuthority() || !NewPlayer)
	{
		return;
	}

	RegisterActiveSessionForController(NewPlayer);

	if (USpellRisePersistenceSubsystem* Persistence = GetPersistenceSubsystem())
	{
		Persistence->PreloadCharacterForController(NewPlayer);
		Persistence->ApplyCachedCharacterToController(NewPlayer);
	}
}

void ASpellRiseGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!HasAuthority())
	{
		return;
	}

	RegisterActiveSessionForController(NewPlayer);

	FString UniqueIdText = TEXT("INVALID");
	if (NewPlayer && NewPlayer->PlayerState)
	{
		UniqueIdText = DescribeUniqueId(NewPlayer->PlayerState->GetUniqueId());
	}

	UE_LOG(
		LogSpellRiseLoginPersistence,
		Log,
		TEXT("[Net][PostLogin] Controller=%s Address=%s UniqueId=%s Pawn=%s"),
		*GetNameSafe(NewPlayer),
		NewPlayer ? *NewPlayer->GetPlayerNetworkAddress() : TEXT("NONE"),
		*UniqueIdText,
		NewPlayer ? *GetNameSafe(NewPlayer->GetPawn()) : TEXT("NONE"));

	// Preload/apply ja ocorre em HandleStartingNewPlayer_Implementation.
	// Evitar duplicidade aqui reduz hitch no login e side-effects de aplicacao repetida.

}

void ASpellRiseGameModeBase::Logout(AController* Exiting)
{
	if (HasAuthority())
	{
		APlayerController* ExitingPC = Cast<APlayerController>(Exiting);
		FString ExitingUniqueId = TEXT("INVALID");
		FString ExitingAddress = TEXT("NONE");
		if (ExitingPC && ExitingPC->PlayerState)
		{
			ExitingUniqueId = DescribeUniqueId(ExitingPC->PlayerState->GetUniqueId());
			ExitingAddress = ExitingPC->GetPlayerNetworkAddress();
		}

		UE_LOG(
			LogSpellRiseLoginPersistence,
			Log,
			TEXT("[Net][Logout] Controller=%s Address=%s UniqueId=%s Pawn=%s"),
			*GetNameSafe(Exiting),
			*ExitingAddress,
			*ExitingUniqueId,
			ExitingPC ? *GetNameSafe(ExitingPC->GetPawn()) : TEXT("NONE"));

		const bool bSkipSaveForHandover = ShouldSkipSaveDuringHandover(Exiting);
		if (bSkipSaveForHandover)
		{
			UE_LOG(
				LogSpellRiseLoginPersistence,
				Log,
				TEXT("[Persistence][Handover] Skip save for exiting old session Controller=%s"),
				*GetNameSafe(Exiting));
		}

		if (USpellRisePersistenceSubsystem* Persistence = GetPersistenceSubsystem())
		{
			if (!bSkipSaveForHandover)
			{
				Persistence->SaveCharacterForController(Exiting);
			}
			Persistence->SaveWorld(GetWorld());
			Persistence->ClearControllerPersistentId(Exiting);
		}

		UnregisterActiveSessionForController(Exiting);
	}

	Super::Logout(Exiting);
}

void ASpellRiseGameModeBase::RestartPlayer(AController* NewPlayer)
{
	if (!NewPlayer)
	{
		return;
	}

	bool bUsedCustomSpawn = false;
	if (HasAuthority())
	{
		if (USpellRisePersistenceSubsystem* Persistence = GetPersistenceSubsystem())
		{
			FTransform SavedSpawnTransform = FTransform::Identity;
			if (Persistence->BuildRespawnTransformForController(NewPlayer, SavedSpawnTransform))
			{
				RestartPlayerAtTransform(NewPlayer, SavedSpawnTransform);
				bUsedCustomSpawn = true;
			}
		}
	}

	if (!bUsedCustomSpawn)
	{
		Super::RestartPlayer(NewPlayer);
	}

}

void ASpellRiseGameModeBase::StartPersistenceSnapshotTimer()
{
	UWorld* World = GetWorld();
	if (!World || !HasAuthority())
	{
		return;
	}

	const float Interval = FMath::Max(5.0f, PersistenceSnapshotIntervalSeconds);
	World->GetTimerManager().SetTimer(
		PersistenceSnapshotTimerHandle,
		this,
		&ASpellRiseGameModeBase::HandlePersistenceSnapshotTimer,
		Interval,
		true);
}

void ASpellRiseGameModeBase::StopPersistenceSnapshotTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PersistenceSnapshotTimerHandle);
	}
}

void ASpellRiseGameModeBase::HandlePersistenceSnapshotTimer()
{
	if (!HasAuthority())
	{
		return;
	}

	SaveAllPlayersAndWorld();
}

USpellRisePersistenceSubsystem* ASpellRiseGameModeBase::GetPersistenceSubsystem() const
{
	const UWorld* World = GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>() : nullptr;
}

void ASpellRiseGameModeBase::SaveAllPlayersAndWorld()
{
	USpellRisePersistenceSubsystem* Persistence = GetPersistenceSubsystem();
	UWorld* World = GetWorld();
	if (!Persistence || !World)
	{
		return;
	}

	for (APlayerController* PlayerController : TActorRange<APlayerController>(World))
	{
		if (PlayerController && PlayerController->PlayerState)
		{
			Persistence->SaveCharacterForController(PlayerController);
		}
	}

	Persistence->SaveWorld(World);
}

FString ASpellRiseGameModeBase::ResolvePersistentIdFromPreLoginData(
	const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	bool& bOutUsedDevFallback) const
{
	bOutUsedDevFallback = false;

	const bool bNoSteamParam = IsNoSteamCommandLineParamPresent();
	const bool bNoSteamTestingActive = IsNoSteamTestingModeActive();
	const bool bRequireSteamInThisContext = bRequireSteamAuthOnDedicatedServer && IsRunningDedicatedServer() && !bNoSteamTestingActive;
	const bool bAllowOfflineFallbackInThisContext = bAllowDevOfflineIdFallback || bNoSteamTestingActive;
	const FString DevAuthId = UGameplayStatics::ParseOption(Options, TEXT("DevAuthId")).TrimStartAndEnd();
	const bool bSteamIdValid = IsSteamUniqueId(UniqueId);

	if (!DevAuthId.IsEmpty() && bAllowOfflineFallbackInThisContext && !bRequireSteamInThisContext)
	{
		bOutUsedDevFallback = true;
		return BuildDevOfflinePersistentId(DevAuthId);
	}

	if (bSteamIdValid)
	{
		const IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
		if (bRequireSteamInThisContext && (!OSS || OSS->GetSubsystemName() != FName(TEXT("STEAM"))))
		{
			return FString();
		}

		const TSharedPtr<const FUniqueNetId> NetId = UniqueId.GetUniqueNetId();
		if (NetId.IsValid())
		{
			return NetId->ToString();
		}
	}

	if (bRequireSteamInThisContext && !bAllowOfflineFallbackInThisContext)
	{
		return FString();
	}

	if (!bAllowOfflineFallbackInThisContext)
	{
		return FString();
	}

	const FString NameOption = UGameplayStatics::ParseOption(Options, TEXT("Name")).TrimStartAndEnd();
	const FString Seed = !DevAuthId.IsEmpty() ? DevAuthId : (!NameOption.IsEmpty() ? NameOption : NormalizeAddressKey(Address));
	bOutUsedDevFallback = true;
	return BuildDevOfflinePersistentId(Seed);
}

FString ASpellRiseGameModeBase::ResolvePersistentIdForController(APlayerController* PlayerController) const
{
	if (!PlayerController || !PlayerController->PlayerState)
	{
		return FString();
	}

	if (const FString* ExistingPersistentId = PersistentIdByController.Find(TWeakObjectPtr<const AController>(PlayerController)))
	{
		return *ExistingPersistentId;
	}

	const FString AddressKey = NormalizeAddressKey(PlayerController->GetPlayerNetworkAddress());
	if (const FString* PendingId = PendingPersistentIdByAddress.Find(AddressKey))
	{
		return *PendingId;
	}

	const FUniqueNetIdRepl UniqueIdRepl = PlayerController->PlayerState->GetUniqueId();
	if (UniqueIdRepl.IsValid())
	{
		const TSharedPtr<const FUniqueNetId> NetId = UniqueIdRepl.GetUniqueNetId();
		if (NetId.IsValid())
		{
			return NetId->ToString();
		}
	}

	if (!(bAllowDevOfflineIdFallback || IsNoSteamTestingModeActive()))
	{
		return FString();
	}

	const FString NameSeed = PlayerController->PlayerState->GetPlayerName();
	const FString Seed = !NameSeed.IsEmpty() ? NameSeed : AddressKey;
	return BuildDevOfflinePersistentId(Seed);
}

void ASpellRiseGameModeBase::RegisterActiveSessionForController(APlayerController* NewPlayer)
{
	if (!NewPlayer)
	{
		return;
	}

	const FString PersistentId = ResolvePersistentIdForController(NewPlayer);
	if (PersistentId.IsEmpty())
	{
		UE_LOG(
			LogSpellRiseLoginPersistence,
			Warning,
			TEXT("[Auth][SessionRegisterFailed] Controller=%s Reason=empty_persistent_id"),
			*GetNameSafe(NewPlayer));
		return;
	}

	PersistentIdByController.Add(NewPlayer, PersistentId);
	PendingPersistentIdByAddress.Remove(NormalizeAddressKey(NewPlayer->GetPlayerNetworkAddress()));

	if (USpellRisePersistenceSubsystem* Persistence = GetPersistenceSubsystem())
	{
		Persistence->SetControllerPersistentId(NewPlayer, PersistentId);
	}

	if (const TWeakObjectPtr<APlayerController>* ExistingSession = ActiveSessionByPersistentId.Find(PersistentId))
	{
		APlayerController* OldPlayer = ExistingSession->Get();
		if (OldPlayer && OldPlayer != NewPlayer)
		{
			SessionHandoverPersistentIds.Add(PersistentId);

			const FText KickReason = FText::FromString(TEXT("Nova sessão detectada para esta conta. Conexão antiga encerrada."));
			bool bKickSent = false;
			if (GameSession)
			{
				bKickSent = GameSession->KickPlayer(OldPlayer, KickReason);
			}
			if (!bKickSent)
			{
				OldPlayer->ClientReturnToMainMenuWithTextReason(KickReason);
			}

			UE_LOG(
				LogSpellRiseLoginPersistence,
				Warning,
				TEXT("[Auth][DuplicateLoginKick] PersistentId=%s Old=%s New=%s"),
				*PersistentId,
				*GetNameSafe(OldPlayer),
				*GetNameSafe(NewPlayer));
		}
	}

	ActiveSessionByPersistentId.Add(PersistentId, NewPlayer);
	UE_LOG(
		LogSpellRiseLoginPersistence,
		Log,
		TEXT("[Auth][SessionActive] PersistentId=%s Controller=%s"),
		*PersistentId,
		*GetNameSafe(NewPlayer));
}

void ASpellRiseGameModeBase::UnregisterActiveSessionForController(const AController* ExitingController)
{
	if (!ExitingController)
	{
		return;
	}

	const TWeakObjectPtr<const AController> ControllerKey(ExitingController);
	const FString* PersistentIdPtr = PersistentIdByController.Find(ControllerKey);
	if (!PersistentIdPtr)
	{
		return;
	}

	const FString PersistentId = *PersistentIdPtr;
	PersistentIdByController.Remove(ControllerKey);

	if (const TWeakObjectPtr<APlayerController>* ActivePlayerPtr = ActiveSessionByPersistentId.Find(PersistentId))
	{
		APlayerController* ActivePlayer = ActivePlayerPtr->Get();
		if (ActivePlayer == ExitingController)
		{
			ActiveSessionByPersistentId.Remove(PersistentId);
			SessionHandoverPersistentIds.Remove(PersistentId);
		}
		else if (!ActivePlayer)
		{
			ActiveSessionByPersistentId.Remove(PersistentId);
			SessionHandoverPersistentIds.Remove(PersistentId);
		}
		else
		{
			SessionHandoverPersistentIds.Remove(PersistentId);
		}
	}

	UE_LOG(
		LogSpellRiseLoginPersistence,
		Log,
		TEXT("[Auth][SessionUnregister] PersistentId=%s Controller=%s"),
		*PersistentId,
		*GetNameSafe(ExitingController));
}

bool ASpellRiseGameModeBase::ShouldSkipSaveDuringHandover(const AController* ExitingController) const
{
	if (!ExitingController)
	{
		return false;
	}

	const FString* PersistentIdPtr = PersistentIdByController.Find(TWeakObjectPtr<const AController>(ExitingController));
	if (!PersistentIdPtr || !SessionHandoverPersistentIds.Contains(*PersistentIdPtr))
	{
		return false;
	}

	const TWeakObjectPtr<APlayerController>* ActiveSession = ActiveSessionByPersistentId.Find(*PersistentIdPtr);
	return ActiveSession && ActiveSession->IsValid() && ActiveSession->Get() != ExitingController;
}

FString ASpellRiseGameModeBase::BuildDevOfflinePersistentId(const FString& Seed) const
{
	const FString Prefix = DevOfflineIdPrefix.IsEmpty() ? TEXT("DEV") : BuildSafeIdToken(DevOfflineIdPrefix).ToUpper();
	const FString SanitizedSeed = BuildSafeIdToken(Seed);
	const uint32 SeedCrc = FCrc::StrCrc32(*Seed);
	const FString CrcPart = FString::Printf(TEXT("%08X"), SeedCrc);
	FString Result = FString::Printf(TEXT("%s_%s_%s"), *Prefix, *SanitizedSeed, *CrcPart);
	if (Result.Len() > MaxPersistentIdLen)
	{
		Result.LeftInline(MaxPersistentIdLen, EAllowShrinking::No);
	}
	return Result;
}

FString ASpellRiseGameModeBase::NormalizeAddressKey(const FString& Address) const
{
	FString Result = Address.TrimStartAndEnd().ToLower();
	if (Result.IsEmpty())
	{
		return Result;
	}

	int32 MutableLastColonIndex = INDEX_NONE;
	Result.FindLastChar(TEXT(':'), MutableLastColonIndex);
	if (MutableLastColonIndex != INDEX_NONE)
	{
		const FString RightPart = Result.Mid(MutableLastColonIndex + 1);
		if (!RightPart.IsEmpty() && RightPart.IsNumeric())
		{
			Result.LeftInline(MutableLastColonIndex, EAllowShrinking::No);
		}
	}

	return Result.TrimStartAndEnd();
}

bool ASpellRiseGameModeBase::IsNoSteamCommandLineParamPresent() const
{
	return FParse::Param(FCommandLine::Get(), TEXT("nosteam"));
}

bool ASpellRiseGameModeBase::IsNoSteamTestingModeActive() const
{
	return bEnableNoSteamTestingMode && IsNoSteamCommandLineParamPresent();
}

bool ASpellRiseGameModeBase::ShouldRequireSteamAuthentication() const
{
	return bRequireSteamAuthOnDedicatedServer && IsRunningDedicatedServer() && !IsNoSteamTestingModeActive();
}
