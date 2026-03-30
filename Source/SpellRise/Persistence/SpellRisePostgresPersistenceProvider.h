#pragma once

#include "CoreMinimal.h"
#include "SpellRisePersistenceProvider.h"

class FSpellRisePostgresPersistenceProvider final : public ISpellRisePersistenceProvider
{
public:
	FSpellRisePostgresPersistenceProvider();

	virtual bool IsReady() const override { return bReady; }
	virtual FString GetProviderName() const override { return TEXT("PostgreSQL"); }

	virtual bool LoadCharacterState(const FString& SteamId64, FSpellRiseCharacterSaveData& OutData, int64& OutRevision) override;
	virtual bool SaveCharacterState(const FString& SteamId64, const FSpellRiseCharacterSaveData& Data, int64 ExpectedRevision, int64 TargetRevision) override;

	virtual bool LoadInventoryState(const FString& SteamId64, FSpellRiseInventorySaveData& OutData, int64& OutRevision) override;
	virtual bool SaveInventoryState(const FString& SteamId64, const FSpellRiseInventorySaveData& Data, int64 ExpectedRevision, int64 TargetRevision) override;

	virtual bool LoadWorld(const FString& WorldId, FSpellRiseWorldSaveData& OutData) override;
	virtual bool SaveWorld(const FString& WorldId, const FSpellRiseWorldSaveData& Data) override;

private:
	bool InitializeSchema();
	bool ExecSql(const FString& Sql, FString& OutStdOut, FString& OutStdErr) const;
	bool ExecMutatingSql(const FString& Sql, const TCHAR* Context) const;
	bool LoadRevisionedSnapshot(const FString& TableName, const FString& KeyColumn, const FString& KeyValue, FString& OutSnapshotJson, int64& OutRevision) const;
	bool SaveRevisionedSnapshot(const FString& TableName, const FString& KeyColumn, const FString& KeyValue, const FString& SnapshotJson, int64 ExpectedRevision, int64 TargetRevision) const;

	static FString EscapeSqlLiteral(const FString& InValue);

private:
	FString PsqlPath;
	FString ConnectionString;
	bool bReady = false;
};
