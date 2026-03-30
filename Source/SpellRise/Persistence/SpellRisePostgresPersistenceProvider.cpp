#include "SpellRise/Persistence/SpellRisePostgresPersistenceProvider.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "SpellRise/Persistence/SpellRisePersistenceTypes.h"

namespace
{
	DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePersistencePostgres, Log, All);

	bool ParseRevisionAndSnapshot(const FString& JsonLine, int64& OutRevision, FString& OutSnapshotJson)
	{
		OutRevision = 0;
		OutSnapshotJson.Reset();

		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonLine);
		if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
		{
			return false;
		}

		OutRevision = static_cast<int64>(JsonObject->GetNumberField(TEXT("revision")));
		const TSharedPtr<FJsonObject>* SnapshotObject = nullptr;
		if (!JsonObject->TryGetObjectField(TEXT("snapshot"), SnapshotObject) || !SnapshotObject || !(*SnapshotObject).IsValid())
		{
			return false;
		}

		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutSnapshotJson);
		return FJsonSerializer::Serialize((*SnapshotObject).ToSharedRef(), Writer);
	}
}

FSpellRisePostgresPersistenceProvider::FSpellRisePostgresPersistenceProvider()
{
	FString CmdPsqlPath;
	FParse::Value(FCommandLine::Get(), TEXT("SRPsqlPath="), CmdPsqlPath);
	PsqlPath = CmdPsqlPath.IsEmpty() ? FPlatformMisc::GetEnvironmentVariable(TEXT("SR_PSQL_PATH")) : CmdPsqlPath;
	if (PsqlPath.IsEmpty())
	{
		PsqlPath = TEXT("psql");
	}

	FString CmdConn;
	FParse::Value(FCommandLine::Get(), TEXT("SRPgConn="), CmdConn);
	ConnectionString = CmdConn.IsEmpty() ? FPlatformMisc::GetEnvironmentVariable(TEXT("SR_PG_CONN")) : CmdConn;

	if (ConnectionString.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error, TEXT("[Persistence][Postgres][InitFailed] Reason=missing_connection_string Env=SR_PG_CONN Cmd=SRPgConn"));
		bReady = false;
		return;
	}

	bReady = InitializeSchema();
}

bool FSpellRisePostgresPersistenceProvider::LoadCharacterState(const FString& SteamId64, FSpellRiseCharacterSaveData& OutData, int64& OutRevision)
{
	FString SnapshotJson;
	if (!LoadRevisionedSnapshot(TEXT("spellrise_character_state"), TEXT("player_id"), SteamId64, SnapshotJson, OutRevision))
	{
		return false;
	}

	return FJsonObjectConverter::JsonObjectStringToUStruct(SnapshotJson, &OutData, 0, 0);
}

bool FSpellRisePostgresPersistenceProvider::SaveCharacterState(const FString& SteamId64, const FSpellRiseCharacterSaveData& Data, int64 ExpectedRevision, int64 TargetRevision)
{
	FString SnapshotJson;
	if (!FJsonObjectConverter::UStructToJsonObjectString(Data, SnapshotJson))
	{
		return false;
	}

	return SaveRevisionedSnapshot(TEXT("spellrise_character_state"), TEXT("player_id"), SteamId64, SnapshotJson, ExpectedRevision, TargetRevision);
}

bool FSpellRisePostgresPersistenceProvider::LoadInventoryState(const FString& SteamId64, FSpellRiseInventorySaveData& OutData, int64& OutRevision)
{
	FString SnapshotJson;
	if (!LoadRevisionedSnapshot(TEXT("spellrise_inventory_state"), TEXT("player_id"), SteamId64, SnapshotJson, OutRevision))
	{
		return false;
	}

	return FJsonObjectConverter::JsonObjectStringToUStruct(SnapshotJson, &OutData, 0, 0);
}

bool FSpellRisePostgresPersistenceProvider::SaveInventoryState(const FString& SteamId64, const FSpellRiseInventorySaveData& Data, int64 ExpectedRevision, int64 TargetRevision)
{
	FString SnapshotJson;
	if (!FJsonObjectConverter::UStructToJsonObjectString(Data, SnapshotJson))
	{
		return false;
	}

	return SaveRevisionedSnapshot(TEXT("spellrise_inventory_state"), TEXT("player_id"), SteamId64, SnapshotJson, ExpectedRevision, TargetRevision);
}

bool FSpellRisePostgresPersistenceProvider::LoadWorld(const FString& WorldId, FSpellRiseWorldSaveData& OutData)
{
	FString SnapshotJson;
	int64 IgnoredRevision = 0;
	if (!LoadRevisionedSnapshot(TEXT("spellrise_world_state"), TEXT("world_id"), WorldId, SnapshotJson, IgnoredRevision))
	{
		return false;
	}

	return FJsonObjectConverter::JsonObjectStringToUStruct(SnapshotJson, &OutData, 0, 0);
}

bool FSpellRisePostgresPersistenceProvider::SaveWorld(const FString& WorldId, const FSpellRiseWorldSaveData& Data)
{
	FString SnapshotJson;
	if (!FJsonObjectConverter::UStructToJsonObjectString(Data, SnapshotJson))
	{
		return false;
	}

	FString ExistingJson;
	int64 ExistingRevision = 0;
	const bool bExists = LoadRevisionedSnapshot(TEXT("spellrise_world_state"), TEXT("world_id"), WorldId, ExistingJson, ExistingRevision);
	const int64 ExpectedRevision = bExists ? ExistingRevision : 0;
	const int64 TargetRevision = ExpectedRevision + 1;
	return SaveRevisionedSnapshot(TEXT("spellrise_world_state"), TEXT("world_id"), WorldId, SnapshotJson, ExpectedRevision, TargetRevision);
}

bool FSpellRisePostgresPersistenceProvider::InitializeSchema()
{
	const FString SchemaSql = TEXT(
		"CREATE TABLE IF NOT EXISTS spellrise_character_state ("
		"player_id TEXT PRIMARY KEY, revision BIGINT NOT NULL, updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), snapshot_json JSONB NOT NULL);"
		"CREATE TABLE IF NOT EXISTS spellrise_inventory_state ("
		"player_id TEXT PRIMARY KEY, revision BIGINT NOT NULL, updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), snapshot_json JSONB NOT NULL);"
		"CREATE TABLE IF NOT EXISTS spellrise_world_state ("
		"world_id TEXT PRIMARY KEY, revision BIGINT NOT NULL, updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), snapshot_json JSONB NOT NULL);");

	FString StdOut;
	FString StdErr;
	if (!ExecSql(SchemaSql, StdOut, StdErr))
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error, TEXT("[Persistence][Postgres][SchemaFailed] StdErr=%s"), *StdErr);
		return false;
	}

	UE_LOG(LogSpellRisePersistencePostgres, Log, TEXT("[Persistence][Postgres][SchemaOk]"));
	return true;
}

bool FSpellRisePostgresPersistenceProvider::ExecSql(const FString& Sql, FString& OutStdOut, FString& OutStdErr) const
{
	OutStdOut.Reset();
	OutStdErr.Reset();

	if (Sql.IsEmpty() || ConnectionString.IsEmpty())
	{
		return false;
	}

	const FString TempDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SpellRise"), TEXT("Persistence"), TEXT("Temp"));
	IFileManager::Get().MakeDirectory(*TempDir, true);
	const FString TempSqlPath = FPaths::Combine(TempDir, FString::Printf(TEXT("psql_%s.sql"), *FGuid::NewGuid().ToString(EGuidFormats::Digits)));
	if (!FFileHelper::SaveStringToFile(Sql, *TempSqlPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		return false;
	}

	const FString Args = FString::Printf(TEXT("\"%s\" -X -A -t -q -v ON_ERROR_STOP=1 -f \"%s\""), *ConnectionString, *TempSqlPath);
	int32 ReturnCode = -1;
	FPlatformProcess::ExecProcess(*PsqlPath, *Args, &ReturnCode, &OutStdOut, &OutStdErr);
	IFileManager::Get().Delete(*TempSqlPath, false, true, true);
	if (ReturnCode != 0)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning, TEXT("[Persistence][Postgres][ExecFailed] ReturnCode=%d StdErr=%s"), ReturnCode, *OutStdErr);
	}
	return ReturnCode == 0;
}

bool FSpellRisePostgresPersistenceProvider::ExecMutatingSql(const FString& Sql, const TCHAR* Context) const
{
	FString StdOut;
	FString StdErr;
	const bool bOk = ExecSql(Sql, StdOut, StdErr);
	if (!bOk)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning, TEXT("[Persistence][Postgres][ExecFailed] Context=%s StdErr=%s"), Context ? Context : TEXT("unknown"), *StdErr);
	}
	return bOk;
}

bool FSpellRisePostgresPersistenceProvider::LoadRevisionedSnapshot(const FString& TableName, const FString& KeyColumn, const FString& KeyValue, FString& OutSnapshotJson, int64& OutRevision) const
{
	OutSnapshotJson.Reset();
	OutRevision = 0;

	if (TableName.IsEmpty() || KeyColumn.IsEmpty() || KeyValue.IsEmpty())
	{
		return false;
	}

	const FString EscapedKey = EscapeSqlLiteral(KeyValue);
	const FString Sql = FString::Printf(
		TEXT("SELECT json_build_object('revision', revision, 'snapshot', snapshot_json)::text FROM %s WHERE %s='%s' LIMIT 1;"),
		*TableName,
		*KeyColumn,
		*EscapedKey);

	FString StdOut;
	FString StdErr;
	if (!ExecSql(Sql, StdOut, StdErr))
	{
		return false;
	}

	const FString JsonLine = StdOut.TrimStartAndEnd();
	if (JsonLine.IsEmpty())
	{
		return false;
	}

	return ParseRevisionAndSnapshot(JsonLine, OutRevision, OutSnapshotJson);
}

bool FSpellRisePostgresPersistenceProvider::SaveRevisionedSnapshot(const FString& TableName, const FString& KeyColumn, const FString& KeyValue, const FString& SnapshotJson, int64 ExpectedRevision, int64 TargetRevision) const
{
	if (TableName.IsEmpty() || KeyColumn.IsEmpty() || KeyValue.IsEmpty() || SnapshotJson.IsEmpty() || TargetRevision <= 0)
	{
		return false;
	}

	const FString EscapedKey = EscapeSqlLiteral(KeyValue);
	const FString EscapedJson = EscapeSqlLiteral(SnapshotJson);

	const FString UpdateSql = FString::Printf(
		TEXT("WITH upsert AS (")
		TEXT("INSERT INTO %s (%s, revision, snapshot_json, updated_at) VALUES ('%s', %lld, '%s'::jsonb, NOW()) ")
		TEXT("ON CONFLICT (%s) DO UPDATE SET revision=EXCLUDED.revision, snapshot_json=EXCLUDED.snapshot_json, updated_at=NOW() ")
		TEXT("WHERE %s.revision = %lld ")
		TEXT("RETURNING 1) ")
		TEXT("SELECT COALESCE(MAX(1),0) FROM upsert;"),
		*TableName,
		*KeyColumn,
		*EscapedKey,
		TargetRevision,
		*EscapedJson,
		*KeyColumn,
		*TableName,
		ExpectedRevision);

	FString StdOut;
	FString StdErr;
	if (!ExecSql(UpdateSql, StdOut, StdErr))
	{
		return false;
	}

	const FString Result = StdOut.TrimStartAndEnd();
	return Result == TEXT("1");
}

FString FSpellRisePostgresPersistenceProvider::EscapeSqlLiteral(const FString& InValue)
{
	return InValue.Replace(TEXT("'"), TEXT("''"));
}
