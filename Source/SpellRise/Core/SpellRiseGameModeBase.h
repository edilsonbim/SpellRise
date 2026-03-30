// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineSubsystemTypes.h"
#include "SpellRiseGameModeBase.generated.h"

class APlayerController;
class USpellRisePersistenceSubsystem;

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract, Config=Game)
class ASpellRiseGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	ASpellRiseGameModeBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category="SpellRise|Online")
	bool bRequireSteamAuthOnDedicatedServer = true;

	UPROPERTY(Config, EditDefaultsOnly, Category="SpellRise|Online")
	bool bAllowDevOfflineIdFallback = true;

	UPROPERTY(Config, EditDefaultsOnly, Category="SpellRise|Online")
	FString DevOfflineIdPrefix = TEXT("DEV");

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Persistence", meta=(ClampMin="5.0"))
	float PersistenceSnapshotIntervalSeconds = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Persistence")
	bool bLoadWorldPersistenceOnBeginPlay = true;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Persistence")
	bool bAutoSaveWorldOnShutdown = true;

private:
	void StartPersistenceSnapshotTimer();
	void StopPersistenceSnapshotTimer();
	void HandlePersistenceSnapshotTimer();
	USpellRisePersistenceSubsystem* GetPersistenceSubsystem() const;
	void SaveAllPlayersAndWorld();
	FString ResolvePersistentIdFromPreLoginData(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, bool& bOutUsedDevFallback) const;
	FString ResolvePersistentIdForController(APlayerController* PlayerController) const;
	void RegisterActiveSessionForController(APlayerController* NewPlayer);
	void UnregisterActiveSessionForController(const AController* ExitingController);
	bool ShouldSkipSaveDuringHandover(const AController* ExitingController) const;
	FString BuildDevOfflinePersistentId(const FString& Seed) const;
	FString NormalizeAddressKey(const FString& Address) const;

	FTimerHandle PersistenceSnapshotTimerHandle;
	TMap<FString, TWeakObjectPtr<APlayerController>> ActiveSessionByPersistentId;
	TMap<TWeakObjectPtr<const AController>, FString> PersistentIdByController;
	TMap<FString, FString> PendingPersistentIdByAddress;
	TSet<FString> SessionHandoverPersistentIds;
};
