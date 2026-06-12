// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRiseGameInstance.h"

#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineAuthInterfaceUtilsSteam.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Parse.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseGameInstanceRuntime, Log, All);

namespace
{
#if PLATFORM_WINDOWS
	void DisableQuickEditForDedicatedServerConsole()
	{
		if (!IsRunningDedicatedServer())
		{
			return;
		}

		HANDLE ConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);
		if (ConsoleInputHandle == INVALID_HANDLE_VALUE || ConsoleInputHandle == nullptr)
		{
			return;
		}

		DWORD ConsoleMode = 0;
		if (!GetConsoleMode(ConsoleInputHandle, &ConsoleMode))
		{
			return;
		}

		const DWORD NewConsoleMode = (ConsoleMode | ENABLE_EXTENDED_FLAGS) & ~ENABLE_QUICK_EDIT_MODE;
		if (NewConsoleMode == ConsoleMode)
		{
			return;
		}

		if (SetConsoleMode(ConsoleInputHandle, NewConsoleMode))
		{
			UE_LOG(LogSpellRiseGameInstanceRuntime, Log,
				TEXT("[ServerConsole][QuickEditDisabled] OldMode=%u NewMode=%u"),
				ConsoleMode,
				NewConsoleMode);
		}
		else
		{
			UE_LOG(LogSpellRiseGameInstanceRuntime, Warning,
				TEXT("[ServerConsole][QuickEditDisableFailed] OldMode=%u"),
				ConsoleMode);
		}
	}
#endif

	FString BuildSafeOfflineSessionIdentity(const int32 LocalUserNum)
	{
		return FString::Printf(TEXT("OfflineLocalUser%d"), LocalUserNum);
	}

	FString GetOnlineSubsystemNameSafe(const IOnlineSubsystem* OnlineSubsystem)
	{
		return OnlineSubsystem ? OnlineSubsystem->GetSubsystemName().ToString() : TEXT("None");
	}

	FString DescribeOnlineSubsystemState()
	{
		const IOnlineSubsystem* DefaultSubsystem = IOnlineSubsystem::Get();
		const IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(TEXT("STEAM"));
		const IOnlineSubsystem* NullSubsystem = IOnlineSubsystem::Get(TEXT("NULL"));

		return FString::Printf(
			TEXT("Default=%s SteamLoaded=%d NullLoaded=%d"),
			*GetOnlineSubsystemNameSafe(DefaultSubsystem),
			SteamSubsystem ? 1 : 0,
			NullSubsystem ? 1 : 0);
	}

	bool IsNoSteamCommandLineParamPresent()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("nosteam"));
	}

	bool GetSpellRiseGameModeConfigBool(const TCHAR* Key, const bool DefaultValue)
	{
		bool bValue = DefaultValue;
		if (GConfig)
		{
			GConfig->GetBool(TEXT("/Script/SpellRise.SpellRiseGameModeBase"), Key, bValue, GGameIni);
		}
		return bValue;
	}

	bool IsNoSteamTestingModeActive()
	{
		return GetSpellRiseGameModeConfigBool(TEXT("bEnableNoSteamTestingMode"), false) && IsNoSteamCommandLineParamPresent();
	}

	bool ShouldRequireSteamForDedicatedServer()
	{
		return IsRunningDedicatedServer() &&
			GetSpellRiseGameModeConfigBool(TEXT("bRequireSteamAuthOnDedicatedServer"), true) &&
			!IsNoSteamTestingModeActive();
	}

	bool IsSteamAuthEnabled()
	{
		FOnlineAuthUtilsSteam SteamAuthUtils;
		return SteamAuthUtils.IsSteamAuthEnabled();
	}

	bool ResolveLocalSessionIdentity(
		const IOnlineSubsystem* OnlineSubsystem,
		const int32 LocalUserNum,
		FString& OutSessionIdentity,
		FString& OutIdentitySource)
	{
		OutSessionIdentity.Reset();
		OutIdentitySource.Reset();

		if (OnlineSubsystem)
		{
			const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
			if (Identity.IsValid())
			{
				const TSharedPtr<const FUniqueNetId> UniqueNetId = Identity->GetUniquePlayerId(LocalUserNum);
				if (UniqueNetId.IsValid())
				{
					OutSessionIdentity = UniqueNetId->ToString();
					OutIdentitySource = OnlineSubsystem->GetSubsystemName() == FName(TEXT("STEAM")) ? TEXT("Steam") : TEXT("OnlineSubsystem");
					return true;
				}
			}
		}

		OutSessionIdentity = BuildSafeOfflineSessionIdentity(LocalUserNum);
		OutIdentitySource = TEXT("FallbackOffline");
		return false;
	}
}

void USpellRiseGameInstance::Init()
{
	Super::Init();

#if PLATFORM_WINDOWS
	DisableQuickEditForDedicatedServerConsole();
#endif

	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	const FName SubsystemName = OnlineSubsystem ? OnlineSubsystem->GetSubsystemName() : NAME_None;
	FString LocalSessionIdentity;
	FString LocalSessionSource;
	const bool bHasLocalSessionIdentity = ResolveLocalSessionIdentity(OnlineSubsystem, 0, LocalSessionIdentity, LocalSessionSource);
	const bool bNoSteamParam = IsNoSteamCommandLineParamPresent();
	const bool bNoSteamTestingMode = IsNoSteamTestingModeActive();
	const bool bRequireSteamForDedicatedServer = ShouldRequireSteamForDedicatedServer();
	const bool bSteamAuthEnabled = IsSteamAuthEnabled();

	UE_LOG(LogSpellRiseGameInstanceRuntime, Log,
		TEXT("[Auth][Bootstrap] DedicatedServer=%d NoSteamParam=%d NoSteamTestingMode=%d RequireSteamDS=%d SteamAuthEnabled=%d Subsystems=%s LocalIdentityValid=%d LocalIdentitySource=%s LocalIdentity=%s"),
		IsRunningDedicatedServer() ? 1 : 0,
		bNoSteamParam ? 1 : 0,
		bNoSteamTestingMode ? 1 : 0,
		bRequireSteamForDedicatedServer ? 1 : 0,
		bSteamAuthEnabled ? 1 : 0,
		*DescribeOnlineSubsystemState(),
		bHasLocalSessionIdentity ? 1 : 0,
		*LocalSessionSource,
		*LocalSessionIdentity);

	if (bNoSteamParam && !bNoSteamTestingMode)
	{
		UE_LOG(LogSpellRiseGameInstanceRuntime, Warning,
			TEXT("[Auth][BootstrapWarning] Reason=nosteam_param_ignored ConfigNoSteamTesting=0"));
	}

	if (bRequireSteamForDedicatedServer && SubsystemName != FName(TEXT("STEAM")))
	{
		UE_LOG(LogSpellRiseGameInstanceRuntime, Fatal,
			TEXT("[Auth][BootstrapFailure] Reason=steam_required_but_default_subsystem_not_steam Subsystems=%s Hint=fix_steam_runtime_or_launch_with_nosteam_for_dev_only"),
			*DescribeOnlineSubsystemState());
	}

	if (bRequireSteamForDedicatedServer && !bSteamAuthEnabled)
	{
		UE_LOG(LogSpellRiseGameInstanceRuntime, Fatal,
			TEXT("[Auth][BootstrapFailure] Reason=steam_auth_packet_handler_disabled Subsystems=%s Hint=enable_OnlineSubsystemSteam.SteamAuthComponentModuleInterface"),
			*DescribeOnlineSubsystemState());
	}

	if (!bHasLocalSessionIdentity)
	{
		bLoggedLocalSessionFallback = true;
	}

}

void USpellRiseGameInstance::RestoreGameplayInputMode()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetIgnoreMoveInput(false);
	PlayerController->SetIgnoreLookInput(false);

	UE_LOG(LogSpellRiseGameInstanceRuntime, Log,
		TEXT("[Input][RestoreGameplayInputMode] Controller=%s"),
		*GetNameSafe(PlayerController));
}

bool USpellRiseGameInstance::IsSteamSubsystemActive() const
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	return OnlineSubsystem && OnlineSubsystem->GetSubsystemName() == FName(TEXT("STEAM"));
}

bool USpellRiseGameInstance::HasLocalSessionIdentity(int32 LocalUserNum) const
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	FString SessionIdentity;
	FString IdentitySource;
	return ResolveLocalSessionIdentity(OnlineSubsystem, LocalUserNum, SessionIdentity, IdentitySource);
}

FString USpellRiseGameInstance::GetLocalSteamId64(int32 LocalUserNum) const
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		return FString();
	}

	const IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	if (!Identity.IsValid())
	{
		return FString();
	}

	const TSharedPtr<const FUniqueNetId> UniqueNetId = Identity->GetUniquePlayerId(LocalUserNum);
	return UniqueNetId.IsValid() ? UniqueNetId->ToString() : FString();
}

FString USpellRiseGameInstance::GetLocalSessionIdentity(int32 LocalUserNum) const
{
	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	FString SessionIdentity;
	FString IdentitySource;
	const bool bHasLocalSessionIdentity = ResolveLocalSessionIdentity(OnlineSubsystem, LocalUserNum, SessionIdentity, IdentitySource);

	if (!bHasLocalSessionIdentity && !bLoggedLocalSessionFallback)
	{
		bLoggedLocalSessionFallback = true;
	}

	return SessionIdentity;
}
