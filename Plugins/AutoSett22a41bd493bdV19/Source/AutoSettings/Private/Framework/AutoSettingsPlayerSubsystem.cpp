// Copyright Sam Bonifacio. All Rights Reserved.

#include "Framework/AutoSettingsPlayerSubsystem.h"

#include "TimerManager.h"
#include "Misc/AutoSettingsPlayer.h"
#include "Engine/AssetManager.h"
#include "Framework/SettingType.h"
#include "Framework/SettingRegistry.h"
#include "Utility/SettingRegistryKeyUtils.h"
#include "Utility/SettingUtils.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ULocalPlayer* UAutoSettingsPlayerSubsystem::GetOwningLocalPlayerFromRegistry(const USettingRegistry* Registry)
{
	const UAutoSettingsPlayerSubsystem* Subsystem = Registry ? Registry->GetTypedOuter<UAutoSettingsPlayerSubsystem>() : nullptr;
	return Subsystem ? Subsystem->GetLocalPlayer() : nullptr;
}

APlayerController* UAutoSettingsPlayerSubsystem::GetOwningPlayerControllerFromRegistry(const USettingRegistry* Registry)
{
	const ULocalPlayer* LocalPlayer = GetOwningLocalPlayerFromRegistry(Registry);
	return LocalPlayer ? LocalPlayer->GetPlayerController(nullptr) : nullptr;
}

APlayerController* UAutoSettingsPlayerSubsystem::GetOwningPlayerControllerFromSetting(const FSettingReference& Setting)
{
	return GetOwningPlayerControllerFromRegistry(Setting.Context.Registry);
}

UAutoSettingsPlayerSubsystem::UAutoSettingsPlayerSubsystem()
{
	PlayerSettingRegistry = CreateDefaultSubobject<USettingRegistry>(TEXT("PlayerSettingRegistry"), true);
}

void UAutoSettingsPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Only initialize settings after the initial asset scan is complete
	UAssetManager::CallOrRegister_OnCompletedInitialScan(FSimpleMulticastDelegate::FDelegate::CreateLambda([this]()
		{
		// The PlayerController may not be available at this point, we may need to wait for it to be assigned
		TryInitializeSettingRegistry();

		GetLocalPlayer()->OnPlayerControllerChanged().AddWeakLambda(this, [this](APlayerController* NewPC)
		{
			TryInitializeSettingRegistry();
		}); }));
}

void UAutoSettingsPlayerSubsystem::Deinitialize()
{
	if (PlayerSettingRegistry->IsInitialized())
	{
		PlayerSettingRegistry->Deinitialize();
	}

	Super::Deinitialize();
}

void UAutoSettingsPlayerSubsystem::InitializeSettingRegistry()
{
	const APlayerController* PlayerController = GetLocalPlayer()->GetPlayerController(nullptr);
	if (!ensure(PlayerController))
	{
		return;
	}

	const FName PlayerId = IAutoSettingsPlayer::GetUniquePlayerIdentifier(PlayerController);
	ensure(!PlayerId.IsNone());
	PlayerSettingRegistry->BaseKey = SB::AutoSettings::SettingRegistries::GetBaseKey(ESettingDomain::Player, PlayerId);

	// Find all player setting assets
	const TArray<USettingType*> Settings = FSettingUtils::GetAllSettingTypes().FilterByPredicate(
		[](const USettingType* Setting)
		{
			return Setting->SettingDomain == ESettingDomain::Player;
		});

	PlayerSettingRegistry->Initialize(Settings);
}

void UAutoSettingsPlayerSubsystem::TryInitializeSettingRegistry()
{
	if (PlayerSettingRegistry->IsInitialized())
	{
		// Already initialized
		return;
	}

	if (!IsValid(GetLocalPlayer()->GetPlayerController(nullptr)))
	{
		// PlayerController not available yet
		return;
	}

	InitializeSettingRegistry();
}
