// Copyright Sam Bonifacio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SettingTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "AutoSettingsPlayerSubsystem.generated.h"

class USettingRegistry;
class APlayerController;
class ULocalPlayer;
/**
 * A LocalPlayerSubsystem that manages player-specific settings.
 *
 * This subsystem creates and maintains a player-specific SettingRegistry
 * that handles settings which are unique to each player, such as input bindings,
 * player preferences, and other per-player configurations.
 *
 * Each local player will have their own instance of this subsystem.
 */
UCLASS()
class AUTOSETTINGS_API UAutoSettingsPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UAutoSettingsPlayerSubsystem();

	/**
	 * Initializes the subsystem and sets up dependencies.
	 * This will defer the initialization of the setting registry until all
	 * prerequisites are met.
	 *
	 * @param Collection - The subsystem collection handling initialization
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Cleans up the subsystem when it's being destroyed.
	 * This ensures all player settings are properly saved.
	 */
	virtual void Deinitialize() override;

	/**
	 * Returns the player-specific setting registry.
	 * This registry contains all settings that are specific to this player.
	 *
	 * @return The player's setting registry instance
	 */
	UFUNCTION(BlueprintPure, Category = "Auto Settings")
	USettingRegistry* GetPlayerSettingRegistry() const
	{
		return PlayerSettingRegistry;
	}

	/**
	 * Resolve the owning local player from a setting registry if the registry is player-scoped.
	 * Returns null for global registries or if no player context is available yet.
	 */
	static ULocalPlayer* GetOwningLocalPlayerFromRegistry(const USettingRegistry* Registry);

	/**
	 * Resolve the owning player controller from a setting registry if the registry is player-scoped.
	 * Returns null for global registries or if no player/controller is available yet.
	 */
	UFUNCTION(BlueprintPure, Category = "Auto Settings")
	static APlayerController* GetOwningPlayerControllerFromRegistry(const USettingRegistry* Registry);

	/**
	 * Resolve the owning player controller from a setting reference if it is player-scoped.
	 * Returns null for global settings or if no player/controller is available yet.
	 */
	UFUNCTION(BlueprintPure, Category = "Auto Settings")
	static APlayerController* GetOwningPlayerControllerFromSetting(const FSettingReference& Setting);

private:
	/** The registry that manages all player-specific settings */
	UPROPERTY()
	USettingRegistry* PlayerSettingRegistry;

	/**
	 * Performs the actual initialization of the setting registry.
	 * This is called once all prerequisites are met.
	 */
	void InitializeSettingRegistry();

	/**
	 * Attempts to initialize the setting registry if prerequisites are met.
	 * This is used as a deferred initialization mechanism.
	 */
	void TryInitializeSettingRegistry();
};
