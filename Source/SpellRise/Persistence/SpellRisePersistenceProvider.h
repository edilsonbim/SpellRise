#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"

struct FSpellRiseCharacterSaveData;
struct FSpellRiseInventorySaveData;
struct FSpellRiseWorldSaveData;

class ISpellRisePersistenceProvider
{
public:
	virtual ~ISpellRisePersistenceProvider() = default;
	virtual bool IsReady() const = 0;
	virtual FString GetProviderName() const = 0;

	virtual bool LoadCharacterState(const FString& SteamId64, FSpellRiseCharacterSaveData& OutData, int64& OutRevision) = 0;
	virtual bool SaveCharacterState(const FString& SteamId64, const FSpellRiseCharacterSaveData& Data, int64 ExpectedRevision, int64 TargetRevision) = 0;

	virtual bool LoadInventoryState(const FString& SteamId64, FSpellRiseInventorySaveData& OutData, int64& OutRevision) = 0;
	virtual bool SaveInventoryState(const FString& SteamId64, const FSpellRiseInventorySaveData& Data, int64 ExpectedRevision, int64 TargetRevision) = 0;

	virtual bool LoadWorld(const FString& WorldId, FSpellRiseWorldSaveData& OutData) = 0;
	virtual bool SaveWorld(const FString& WorldId, const FSpellRiseWorldSaveData& Data) = 0;
};
