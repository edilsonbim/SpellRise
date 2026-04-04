#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpellRise/Persistence/SpellRisePersistenceProvider.h"
#include "SpellRisePersistenceTypes.h"
#include "SpellRisePersistenceSubsystem.generated.h"

class AController;
class APlayerState;
class ASpellRisePawnBase;
class UWorld;
UCLASS()
class SPELLRISE_API USpellRisePersistenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool PreloadCharacterForController(AController* Controller);
	bool ApplyCachedCharacterToController(AController* Controller);
	bool SaveCharacterForController(AController* Controller);
	bool SaveWorld(UWorld* World);
	bool LoadWorld(UWorld* World);
	bool BuildRespawnTransformForController(AController* Controller, FTransform& OutSpawnTransform) const;
	void SetControllerPersistentId(const AController* Controller, const FString& PersistentId);
	void ClearControllerPersistentId(const AController* Controller);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Persistence")
	void MarkWorldDirty();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Persistence")
	void MarkPlayerDirtyBySteamId(const FString& SteamId64);

	UFUNCTION(BlueprintPure, Category="SpellRise|Persistence")
	bool GetSteamIdFromPlayerState(const APlayerState* PlayerState, FString& OutSteamId64) const;

private:
	FString ResolveSteamIdFromController(const AController* Controller) const;
	FString ResolveWorldId(const UWorld* World) const;

	bool CollectPawnData(AController* Controller, const FString& SteamId64, FSpellRiseCharacterSaveData& OutData) const;
	bool ApplyPawnDataToController(AController* Controller, const FSpellRiseCharacterSaveData& Data);
	void EnsureDefaultItemsForControllerIfNeeded(AController* Controller, const TCHAR* ContextTag);
	void BuildInventorySnapshotFromCharacterData(const FSpellRiseCharacterSaveData& CharacterData, const FString& SteamId64, FSpellRiseInventorySaveData& OutInventoryData) const;
	void MergeInventorySnapshotIntoCharacterData(const FSpellRiseInventorySaveData& InventoryData, FSpellRiseCharacterSaveData& InOutCharacterData) const;
	void RecordPersistenceTelemetry(const TCHAR* Operation, bool bSuccess, double LatencyMs, const TCHAR* Reason);
	AActor* ResolveSavedBedActor(UWorld* World, const FSpellRiseCharacterSaveData& Data) const;
	void ReconcilePawnVisualEquipment(class ASpellRisePawnBase* Pawn, class ASpellRisePlayerState* PlayerState) const;

	TUniquePtr<ISpellRisePersistenceProvider> Provider;
	TMap<FString, FSpellRiseCharacterSaveData> CachedCharacterDataBySteamId;
	TMap<FString, FSpellRiseInventorySaveData> CachedInventoryDataBySteamId;
	TMap<FString, int64> CharacterRevisionBySteamId;
	TMap<FString, int64> InventoryRevisionBySteamId;
	TMap<TWeakObjectPtr<const AController>, FString> PersistentIdByController;
	TSet<FString> DirtyCharacterIds;
	TSet<FString> SaveInProgressPersistentIds;
	TMap<FString, TArray<double>> LatencyByOperationMs;
	TMap<FString, int32> FailureCountByOperation;
	bool bWorldDirty = true;
};
