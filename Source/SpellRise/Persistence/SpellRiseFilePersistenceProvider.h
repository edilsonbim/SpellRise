#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "SpellRisePersistenceProvider.h"

class FSpellRiseFilePersistenceProvider final : public ISpellRisePersistenceProvider
{
public:
	FSpellRiseFilePersistenceProvider();
	virtual bool IsReady() const override { return true; }
	virtual FString GetProviderName() const override { return TEXT("File"); }

	virtual bool LoadCharacterState(const FString& SteamId64, FSpellRiseCharacterSaveData& OutData, int64& OutRevision) override;
	virtual bool SaveCharacterState(const FString& SteamId64, const FSpellRiseCharacterSaveData& Data, int64 ExpectedRevision, int64 TargetRevision) override;

	virtual bool LoadInventoryState(const FString& SteamId64, FSpellRiseInventorySaveData& OutData, int64& OutRevision) override;
	virtual bool SaveInventoryState(const FString& SteamId64, const FSpellRiseInventorySaveData& Data, int64 ExpectedRevision, int64 TargetRevision) override;

	virtual bool LoadWorld(const FString& WorldId, FSpellRiseWorldSaveData& OutData) override;
	virtual bool SaveWorld(const FString& WorldId, const FSpellRiseWorldSaveData& Data) override;

private:
	FString BuildCharacterPath(const FString& SteamId64) const;
	FString BuildInventoryPath(const FString& SteamId64) const;
	FString BuildWorldPath(const FString& WorldId) const;
	FString RootDir;
};
