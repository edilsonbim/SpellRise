// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
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
	constexpr int32 PostgresMaxVisualConfigurationJsonLength = 64 * 1024;

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

	bool IsValidOptionalPostgresJsonObjectString(const FString& JsonText)
	{
		if (JsonText.IsEmpty())
		{
			return true;
		}

		if (JsonText.Len() > PostgresMaxVisualConfigurationJsonLength)
		{
			return false;
		}

		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
		return FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid();
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
		bReady = false;
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresInitRejected] Reason=missing_connection_string"));
		return;
	}

	bReady = InitializeSchema();
	if (bReady)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Log,
			TEXT("[Persistence][PostgresInitSucceeded] PsqlPath=%s"),
			*PsqlPath);
	}
	else
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresInitRejected] PsqlPath=%s"),
			*PsqlPath);
	}
}

bool FSpellRisePostgresPersistenceProvider::LoadCharacterState(const FString& SteamId64, FSpellRiseCharacterSaveData& OutData, int64& OutRevision)
{
	FString SnapshotJson;
	if (!LoadRevisionedSnapshot(TEXT("spellrise_character_state"), TEXT("player_id"), SteamId64, SnapshotJson, OutRevision))
	{
		return false;
	}

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(SnapshotJson, &OutData, 0, 0))
	{
		return false;
	}

	if (OutData.SchemaVersion < 7)
	{
		OutData.bCharacterCreated = true;
	}
	return true;
}

bool FSpellRisePostgresPersistenceProvider::SaveCharacterState(const FString& SteamId64, const FSpellRiseCharacterSaveData& Data, int64 ExpectedRevision, int64 TargetRevision)
{
	FString SnapshotJson;
	if (!FJsonObjectConverter::UStructToJsonObjectString(Data, SnapshotJson))
	{
		return false;
	}

	if (!IsValidOptionalPostgresJsonObjectString(Data.VisualConfigurationJson))
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresSaveRejected] Reason=invalid_visual_configuration Player=%s"),
			*SteamId64);
		return false;
	}

	return SaveCharacterRevisionedSnapshot(SteamId64, Data.bCharacterCreated, Data.VisualConfigurationJson, SnapshotJson, ExpectedRevision, TargetRevision);
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

bool FSpellRisePostgresPersistenceProvider::SaveDeathEvent(const FSpellRiseDeathEventData& Data)
{
	if (Data.VictimPlayerId.IsEmpty() && Data.VictimName.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresDeathEventRejected] Reason=missing_victim"));
		return false;
	}

	FString EventJson;
	if (!FJsonObjectConverter::UStructToJsonObjectString(Data, EventJson))
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresDeathEventRejected] Reason=serialize_failed Victim=%s"),
			*Data.VictimName);
		return false;
	}

	const FString OccurredAt = Data.OccurredAtUtcIso8601.IsEmpty() ? FDateTime::UtcNow().ToIso8601() : Data.OccurredAtUtcIso8601;
	const FString CauseType = !Data.Fatal.CauseType.IsEmpty()
		? Data.Fatal.CauseType
		: (!Data.TopDamage.CauseType.IsEmpty() ? Data.TopDamage.CauseType : TEXT("unknown"));

	const FString Sql = FString::Printf(
		TEXT("INSERT INTO public.spellrise_death_events (")
		TEXT("occurred_at, world_id, victim_player_id, victim_name, victim_level, ")
		TEXT("top_damage_player_id, top_damage_name, top_damage_amount, ")
		TEXT("fatal_player_id, fatal_name, fatal_damage_amount, ")
		TEXT("cause_type, damage_type, location_x, location_y, location_z, message, event_json")
		TEXT(") VALUES (")
		TEXT("'%s'::timestamptz, '%s', '%s', '%s', %d, ")
		TEXT("NULLIF('%s',''), NULLIF('%s',''), %.6f, ")
		TEXT("NULLIF('%s',''), NULLIF('%s',''), %.6f, ")
		TEXT("'%s', '%s', %.6f, %.6f, %.6f, '%s', '%s'::jsonb);"),
		*EscapeSqlLiteral(OccurredAt),
		*EscapeSqlLiteral(Data.WorldId),
		*EscapeSqlLiteral(Data.VictimPlayerId),
		*EscapeSqlLiteral(Data.VictimName),
		Data.VictimLevel > 0 ? Data.VictimLevel : 0,
		*EscapeSqlLiteral(Data.TopDamage.PlayerId),
		*EscapeSqlLiteral(Data.TopDamage.DisplayName),
		FMath::Max(0.0f, Data.TopDamage.DamageAmount),
		*EscapeSqlLiteral(Data.Fatal.PlayerId),
		*EscapeSqlLiteral(Data.Fatal.DisplayName),
		FMath::Max(0.0f, Data.Fatal.DamageAmount),
		*EscapeSqlLiteral(CauseType),
		*EscapeSqlLiteral(Data.DamageType),
		Data.DeathLocation.X,
		Data.DeathLocation.Y,
		Data.DeathLocation.Z,
		*EscapeSqlLiteral(Data.Message),
		*EscapeSqlLiteral(EventJson));

	const bool bSaved = ExecMutatingSql(Sql, TEXT("save_death_event"));
	UE_LOG(LogSpellRisePersistencePostgres, Log,
		TEXT("[Persistence][PostgresDeathEvent%s] Victim=%s TopDamage=%s Fatal=%s"),
		bSaved ? TEXT("Saved") : TEXT("Rejected"),
		*Data.VictimName,
		*Data.TopDamage.DisplayName,
		*Data.Fatal.DisplayName);
	return bSaved;
}

bool FSpellRisePostgresPersistenceProvider::InitializeSchema()
{
	const FString SchemaSql = TEXT(
		"CREATE TABLE IF NOT EXISTS spellrise_character_state ("
		"player_id TEXT PRIMARY KEY, revision BIGINT NOT NULL, updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), "
		"character_created BOOLEAN NOT NULL DEFAULT FALSE, visual_configuration JSONB NULL, snapshot_json JSONB NOT NULL);"
		"ALTER TABLE spellrise_character_state ADD COLUMN IF NOT EXISTS character_created BOOLEAN NOT NULL DEFAULT FALSE;"
		"ALTER TABLE spellrise_character_state ADD COLUMN IF NOT EXISTS visual_configuration JSONB NULL;"
		"UPDATE spellrise_character_state SET character_created=TRUE WHERE NOT (snapshot_json ? 'bCharacterCreated');"
		"UPDATE spellrise_character_state SET character_created=(snapshot_json->>'bCharacterCreated')::BOOLEAN WHERE snapshot_json ? 'bCharacterCreated';"
		"CREATE TABLE IF NOT EXISTS spellrise_inventory_state ("
		"player_id TEXT PRIMARY KEY, revision BIGINT NOT NULL, updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), snapshot_json JSONB NOT NULL);"
		"CREATE TABLE IF NOT EXISTS spellrise_world_state ("
		"world_id TEXT PRIMARY KEY, revision BIGINT NOT NULL, updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), snapshot_json JSONB NOT NULL);"
		"CREATE TABLE IF NOT EXISTS spellrise_death_events ("
		"id BIGSERIAL PRIMARY KEY, occurred_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), world_id TEXT NOT NULL DEFAULT '', "
		"victim_player_id TEXT NOT NULL DEFAULT '', victim_name TEXT NOT NULL DEFAULT '', victim_level INT NULL, "
		"top_damage_player_id TEXT NULL, top_damage_name TEXT NULL, top_damage_amount REAL NOT NULL DEFAULT 0, "
		"fatal_player_id TEXT NULL, fatal_name TEXT NULL, fatal_damage_amount REAL NOT NULL DEFAULT 0, "
		"cause_type TEXT NOT NULL DEFAULT 'unknown', damage_type TEXT NOT NULL DEFAULT '', "
		"location_x REAL NULL, location_y REAL NULL, location_z REAL NULL, "
		"message TEXT NOT NULL DEFAULT '', event_json JSONB NOT NULL DEFAULT '{}'::jsonb);"
		"CREATE TABLE IF NOT EXISTS spellrise_portal_ip_bans ("
		"ip_address TEXT PRIMARY KEY, reason TEXT NOT NULL DEFAULT '', banned_until TIMESTAMPTZ NULL, "
		"created_by_steam_id64 TEXT NOT NULL DEFAULT '', created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW());"
		"CREATE INDEX IF NOT EXISTS idx_spellrise_death_events_occurred_at ON spellrise_death_events (occurred_at DESC);"
		"CREATE INDEX IF NOT EXISTS idx_spellrise_death_events_victim ON spellrise_death_events (victim_player_id, occurred_at DESC);"
		"CREATE INDEX IF NOT EXISTS idx_spellrise_portal_ip_bans_active ON spellrise_portal_ip_bans (ip_address, banned_until);");

	FString StdOut;
	FString StdErr;
	if (!ExecSql(SchemaSql, StdOut, StdErr))
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresSchemaRejected] Reason=exec_failed Stderr=%s"),
			*StdErr.Left(512));
		return false;
	}

	return true;
}

bool FSpellRisePostgresPersistenceProvider::ExecSql(const FString& Sql, FString& OutStdOut, FString& OutStdErr) const
{
	OutStdOut.Reset();
	OutStdErr.Reset();

	if (Sql.IsEmpty() || ConnectionString.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresSqlRejected] Reason=%s"),
			Sql.IsEmpty() ? TEXT("empty_sql") : TEXT("missing_connection_string"));
		return false;
	}

	const FString TempDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SpellRise"), TEXT("Persistence"), TEXT("Temp"));
	IFileManager::Get().MakeDirectory(*TempDir, true);
	const FString TempSqlPath = FPaths::Combine(TempDir, FString::Printf(TEXT("psql_%s.sql"), *FGuid::NewGuid().ToString(EGuidFormats::Digits)));
	if (!FFileHelper::SaveStringToFile(Sql, *TempSqlPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresSqlRejected] Reason=temp_sql_write_failed Path=%s"),
			*TempSqlPath);
		return false;
	}

	const FString Args = FString::Printf(TEXT("\"%s\" -X -A -t -q -v ON_ERROR_STOP=1 -f \"%s\""), *ConnectionString, *TempSqlPath);
	int32 ReturnCode = -1;
	FPlatformProcess::ExecProcess(*PsqlPath, *Args, &ReturnCode, &OutStdOut, &OutStdErr);
	IFileManager::Get().Delete(*TempSqlPath, false, true, true);
	if (ReturnCode != 0)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresSqlRejected] Reason=psql_failed ReturnCode=%d StdErr=%s"),
			ReturnCode,
			*OutStdErr.Left(512));
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
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresMutationRejected] Context=%s StdErr=%s"),
			Context ? Context : TEXT("unknown"),
			*StdErr.Left(512));
	}
	return bOk;
}

bool FSpellRisePostgresPersistenceProvider::LoadRevisionedSnapshot(const FString& TableName, const FString& KeyColumn, const FString& KeyValue, FString& OutSnapshotJson, int64& OutRevision) const
{
	OutSnapshotJson.Reset();
	OutRevision = 0;

	if (TableName.IsEmpty() || KeyColumn.IsEmpty() || KeyValue.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresLoadRejected] Reason=invalid_load_key Table=%s KeyColumn=%s"),
			*TableName,
			*KeyColumn);
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
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresLoadRejected] Reason=query_failed Table=%s Stderr=%s"),
			*TableName,
			*StdErr.Left(512));
		return false;
	}

	const FString JsonLine = StdOut.TrimStartAndEnd();
	if (JsonLine.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistencePostgres, Log,
			TEXT("[Persistence][PostgresLoadRejected] Reason=not_found Table=%s"),
			*TableName);
		return false;
	}

	const bool bParsed = ParseRevisionAndSnapshot(JsonLine, OutRevision, OutSnapshotJson);
	if (!bParsed)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresLoadRejected] Reason=parse_failed Table=%s"),
			*TableName);
	}
	return bParsed;
}

bool FSpellRisePostgresPersistenceProvider::SaveRevisionedSnapshot(const FString& TableName, const FString& KeyColumn, const FString& KeyValue, const FString& SnapshotJson, int64 ExpectedRevision, int64 TargetRevision) const
{
	if (TableName.IsEmpty() || KeyColumn.IsEmpty() || KeyValue.IsEmpty() || SnapshotJson.IsEmpty() || TargetRevision <= 0)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresSaveRejected] Reason=invalid_save_args Table=%s TargetRevision=%lld"),
			*TableName,
			TargetRevision);
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
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresSaveRejected] Reason=query_failed Table=%s Stderr=%s"),
			*TableName,
			*StdErr.Left(512));
		return false;
	}

	const FString Result = StdOut.TrimStartAndEnd();
	const bool bSaved = Result == TEXT("1");
	if (!bSaved)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresSaveRejected] Reason=revision_conflict Table=%s ExpectedRevision=%lld TargetRevision=%lld Result=%s"),
			*TableName,
			ExpectedRevision,
			TargetRevision,
			*Result.Left(64));
	}
	return bSaved;
}

bool FSpellRisePostgresPersistenceProvider::SaveCharacterRevisionedSnapshot(const FString& SteamId64, bool bCharacterCreated, const FString& VisualConfigurationJson, const FString& SnapshotJson, int64 ExpectedRevision, int64 TargetRevision) const
{
	if (SteamId64.IsEmpty() || SnapshotJson.IsEmpty() || TargetRevision <= 0)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresSaveRejected] Reason=invalid_character_save_args TargetRevision=%lld"),
			TargetRevision);
		return false;
	}

	const FString EscapedKey = EscapeSqlLiteral(SteamId64);
	const FString EscapedJson = EscapeSqlLiteral(SnapshotJson);
	const FString EscapedVisualJson = EscapeSqlLiteral(VisualConfigurationJson);
	const TCHAR* CharacterCreatedSql = bCharacterCreated ? TEXT("TRUE") : TEXT("FALSE");
	const FString VisualConfigurationSql = VisualConfigurationJson.IsEmpty()
		? TEXT("NULL")
		: FString::Printf(TEXT("'%s'::jsonb"), *EscapedVisualJson);

	const FString UpdateSql = FString::Printf(
		TEXT("WITH upsert AS (")
		TEXT("INSERT INTO spellrise_character_state (player_id, revision, character_created, visual_configuration, snapshot_json, updated_at) ")
		TEXT("VALUES ('%s', %lld, %s, %s, '%s'::jsonb, NOW()) ")
		TEXT("ON CONFLICT (player_id) DO UPDATE SET ")
		TEXT("revision=EXCLUDED.revision, character_created=EXCLUDED.character_created, visual_configuration=EXCLUDED.visual_configuration, ")
		TEXT("snapshot_json=EXCLUDED.snapshot_json, updated_at=NOW() ")
		TEXT("WHERE spellrise_character_state.revision = %lld ")
		TEXT("RETURNING 1) ")
		TEXT("SELECT COALESCE(MAX(1),0) FROM upsert;"),
		*EscapedKey,
		TargetRevision,
		CharacterCreatedSql,
		*VisualConfigurationSql,
		*EscapedJson,
		ExpectedRevision);

	FString StdOut;
	FString StdErr;
	if (!ExecSql(UpdateSql, StdOut, StdErr))
	{
		UE_LOG(LogSpellRisePersistencePostgres, Error,
			TEXT("[Persistence][PostgresSaveRejected] Reason=query_failed Table=spellrise_character_state Stderr=%s"),
			*StdErr.Left(512));
		return false;
	}

	const FString Result = StdOut.TrimStartAndEnd();
	const bool bSaved = Result == TEXT("1");
	if (!bSaved)
	{
		UE_LOG(LogSpellRisePersistencePostgres, Warning,
			TEXT("[Persistence][PostgresSaveRejected] Reason=revision_conflict Table=spellrise_character_state ExpectedRevision=%lld TargetRevision=%lld Result=%s"),
			ExpectedRevision,
			TargetRevision,
			*Result.Left(64));
	}
	return bSaved;
}

FString FSpellRisePostgresPersistenceProvider::EscapeSqlLiteral(const FString& InValue)
{
	return InValue.Replace(TEXT("'"), TEXT("''"));
}
