// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Persistence/SpellRiseFilePersistenceProvider.h"

#include "JsonObjectConverter.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#include "SpellRise/Persistence/SpellRisePersistenceTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePersistenceFile, Log, All);

namespace
{
	enum class ESpellRisePersistenceSnapshotType : uint8
	{
		Character = 0,
		Inventory = 1,
		World = 2
	};

	struct FSpellRisePersistenceSnapshotCounters
	{
		int64 SuccessCount = 0;
		int64 FailureCount = 0;
	};

	FCriticalSection GSpellRisePersistenceSnapshotCountersLock;
	FSpellRisePersistenceSnapshotCounters GSpellRisePersistenceSnapshotCounters[3];

	const TCHAR* GetSnapshotTypeLabel(const ESpellRisePersistenceSnapshotType SnapshotType)
	{
		switch (SnapshotType)
		{
		case ESpellRisePersistenceSnapshotType::Character:
			return TEXT("Character");
		case ESpellRisePersistenceSnapshotType::Inventory:
			return TEXT("Inventory");
		case ESpellRisePersistenceSnapshotType::World:
			return TEXT("World");
		default:
			return TEXT("Unknown");
		}
	}

	FSpellRisePersistenceSnapshotCounters RecordSnapshotWriteResult(const ESpellRisePersistenceSnapshotType SnapshotType, const bool bSuccess)
	{
		FScopeLock ScopeLock(&GSpellRisePersistenceSnapshotCountersLock);
		FSpellRisePersistenceSnapshotCounters& Counters = GSpellRisePersistenceSnapshotCounters[static_cast<int32>(SnapshotType)];
		if (bSuccess)
		{
			++Counters.SuccessCount;
		}
		else
		{
			++Counters.FailureCount;
		}

		return Counters;
	}

	bool SaveJsonToPath(const FString& Path, const TSharedPtr<FJsonObject>& JsonObject)
	{
		if (!JsonObject.IsValid())
		{
			return false;
		}

		FString JsonString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
		{
			return false;
		}

		const FString Directory = FPaths::GetPath(Path);
		if (!Directory.IsEmpty())
		{
			IFileManager::Get().MakeDirectory(*Directory, true);
		}

		return FFileHelper::SaveStringToFile(JsonString, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	bool LoadJsonFromPath(const FString& Path, TSharedPtr<FJsonObject>& OutJsonObject)
	{
		FString JsonString;
		if (!FFileHelper::LoadFileToString(JsonString, *Path))
		{
			return false;
		}

		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		return FJsonSerializer::Deserialize(Reader, OutJsonObject) && OutJsonObject.IsValid();
	}

	template <typename TData>
	bool LoadRevisionedSnapshot(const FString& Path, TData& OutData, int64& OutRevision)
	{
		OutRevision = 0;

		TSharedPtr<FJsonObject> JsonObject;
		if (!LoadJsonFromPath(Path, JsonObject))
		{
			return false;
		}

		if (JsonObject->HasField(TEXT("snapshot")))
		{
			OutRevision = static_cast<int64>(JsonObject->GetNumberField(TEXT("revision")));
			const TSharedPtr<FJsonObject> SnapshotObject = JsonObject->GetObjectField(TEXT("snapshot"));
			return SnapshotObject.IsValid() && FJsonObjectConverter::JsonObjectToUStruct(SnapshotObject.ToSharedRef(), &OutData);
		}

		return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &OutData);
	}

	void LogSnapshotWriteResult(const ESpellRisePersistenceSnapshotType SnapshotType, const FString& Path, const bool bSuccess, const TCHAR* Reason)
	{
		const FSpellRisePersistenceSnapshotCounters Counters = RecordSnapshotWriteResult(SnapshotType, bSuccess);
		if (bSuccess)
		{
		}
		else
		{
		}
	}

	template <typename TData>
	bool SaveRevisionedSnapshot(const FString& Path, const ESpellRisePersistenceSnapshotType SnapshotType, const TData& Data, const int64 ExpectedRevision, const int64 TargetRevision)
	{
		if (TargetRevision <= 0)
		{
			LogSnapshotWriteResult(SnapshotType, Path, false, TEXT("invalid_target_revision"));
			return false;
		}

		int64 CurrentRevision = 0;
		TData Discard;
		const bool bExists = LoadRevisionedSnapshot(Path, Discard, CurrentRevision);
		if (bExists)
		{
			if (CurrentRevision != ExpectedRevision)
			{
				LogSnapshotWriteResult(SnapshotType, Path, false, TEXT("revision_mismatch"));
				return false;
			}
		}
		else if (ExpectedRevision != 0)
		{
			LogSnapshotWriteResult(SnapshotType, Path, false, TEXT("missing_expected_revision"));
			return false;
		}

		TSharedPtr<FJsonObject> SnapshotObject = FJsonObjectConverter::UStructToJsonObject(Data);
		if (!SnapshotObject.IsValid())
		{
			LogSnapshotWriteResult(SnapshotType, Path, false, TEXT("serialize_failed"));
			return false;
		}

		TSharedPtr<FJsonObject> Envelope = MakeShared<FJsonObject>();
		Envelope->SetNumberField(TEXT("revision"), static_cast<double>(TargetRevision));
		Envelope->SetStringField(TEXT("updated_at_utc"), FDateTime::UtcNow().ToIso8601());
		Envelope->SetObjectField(TEXT("snapshot"), SnapshotObject);

		const bool bSaved = SaveJsonToPath(Path, Envelope);
		LogSnapshotWriteResult(SnapshotType, Path, bSaved, bSaved ? TEXT("ok") : TEXT("file_write_failed"));
		return bSaved;
	}
}

FSpellRiseFilePersistenceProvider::FSpellRiseFilePersistenceProvider()
	: RootDir(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SpellRise"), TEXT("Persistence")))
{
}

bool FSpellRiseFilePersistenceProvider::LoadCharacterState(const FString& SteamId64, FSpellRiseCharacterSaveData& OutData, int64& OutRevision)
{
	if (SteamId64.IsEmpty())
	{
		return false;
	}

	return LoadRevisionedSnapshot(BuildCharacterPath(SteamId64), OutData, OutRevision);
}

bool FSpellRiseFilePersistenceProvider::SaveCharacterState(const FString& SteamId64, const FSpellRiseCharacterSaveData& Data, int64 ExpectedRevision, int64 TargetRevision)
{
	if (SteamId64.IsEmpty())
	{
		return false;
	}

	return SaveRevisionedSnapshot(BuildCharacterPath(SteamId64), ESpellRisePersistenceSnapshotType::Character, Data, ExpectedRevision, TargetRevision);
}

bool FSpellRiseFilePersistenceProvider::LoadInventoryState(const FString& SteamId64, FSpellRiseInventorySaveData& OutData, int64& OutRevision)
{
	if (SteamId64.IsEmpty())
	{
		return false;
	}

	return LoadRevisionedSnapshot(BuildInventoryPath(SteamId64), OutData, OutRevision);
}

bool FSpellRiseFilePersistenceProvider::SaveInventoryState(const FString& SteamId64, const FSpellRiseInventorySaveData& Data, int64 ExpectedRevision, int64 TargetRevision)
{
	if (SteamId64.IsEmpty())
	{
		return false;
	}

	return SaveRevisionedSnapshot(BuildInventoryPath(SteamId64), ESpellRisePersistenceSnapshotType::Inventory, Data, ExpectedRevision, TargetRevision);
}

bool FSpellRiseFilePersistenceProvider::LoadWorld(const FString& WorldId, FSpellRiseWorldSaveData& OutData)
{
	if (WorldId.IsEmpty())
	{
		return false;
	}

	const FString Path = BuildWorldPath(WorldId);
	TSharedPtr<FJsonObject> JsonObject;
	if (!LoadJsonFromPath(Path, JsonObject))
	{
		return false;
	}

	return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &OutData);
}

bool FSpellRiseFilePersistenceProvider::SaveWorld(const FString& WorldId, const FSpellRiseWorldSaveData& Data)
{
	if (WorldId.IsEmpty())
	{
		return false;
	}

	const FString Path = BuildWorldPath(WorldId);
	TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(Data);
	if (!JsonObject.IsValid())
	{
		LogSnapshotWriteResult(ESpellRisePersistenceSnapshotType::World, Path, false, TEXT("serialize_failed"));
		return false;
	}

	const bool bSaved = SaveJsonToPath(Path, JsonObject);
	LogSnapshotWriteResult(ESpellRisePersistenceSnapshotType::World, Path, bSaved, bSaved ? TEXT("ok") : TEXT("file_write_failed"));
	return bSaved;
}

FString FSpellRiseFilePersistenceProvider::BuildCharacterPath(const FString& SteamId64) const
{
	return FPaths::Combine(RootDir, TEXT("Characters"), FString::Printf(TEXT("%s.json"), *SteamId64));
}

FString FSpellRiseFilePersistenceProvider::BuildInventoryPath(const FString& SteamId64) const
{
	return FPaths::Combine(RootDir, TEXT("Inventories"), FString::Printf(TEXT("%s.json"), *SteamId64));
}

FString FSpellRiseFilePersistenceProvider::BuildWorldPath(const FString& WorldId) const
{
	const FString SanitizedWorldId = WorldId.Replace(TEXT("/"), TEXT("_")).Replace(TEXT("\\"), TEXT("_"));
	return FPaths::Combine(RootDir, TEXT("Worlds"), FString::Printf(TEXT("%s.json"), *SanitizedWorldId));
}
