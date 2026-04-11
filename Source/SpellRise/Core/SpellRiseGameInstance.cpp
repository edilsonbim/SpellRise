// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRiseGameInstance.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseGameInstanceRuntime, Log, All);

namespace
{
	FString BuildSafeOfflineSessionIdentity(const int32 LocalUserNum)
	{
		return FString::Printf(TEXT("OfflineLocalUser%d"), LocalUserNum);
	}

	FString GetOnlineSubsystemNameSafe(const IOnlineSubsystem* OnlineSubsystem)
	{
		return OnlineSubsystem ? OnlineSubsystem->GetSubsystemName().ToString() : TEXT("None");
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

	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	const FName SubsystemName = OnlineSubsystem ? OnlineSubsystem->GetSubsystemName() : NAME_None;
	FString LocalSessionIdentity;
	FString LocalSessionSource;
	const bool bHasLocalSessionIdentity = ResolveLocalSessionIdentity(OnlineSubsystem, 0, LocalSessionIdentity, LocalSessionSource);
	if (!bHasLocalSessionIdentity)
	{
		bLoggedLocalSessionFallback = true;
	}

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
