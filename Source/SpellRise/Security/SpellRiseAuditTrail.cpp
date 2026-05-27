// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Security/SpellRiseAuditTrail.h"

#include "HAL/FileManager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"

namespace
{
	FCriticalSection GAuditTrailLock;
	constexpr int64 MaxAuditFileSizeBytes = 20LL * 1024LL * 1024LL;
}

FString FSpellRiseAuditTrail::GetAuditFilePath()
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SpellRise"), TEXT("Audit"), TEXT("audit_trail.ndjson"));
}

void FSpellRiseAuditTrail::AppendEvent(const FString& Category, const FString& EventName, const FString& SubjectId, const FString& Details)
{
	const FScopeLock Guard(&GAuditTrailLock);
	const FString Path = GetAuditFilePath();
	RotateIfNeeded(Path);

	const FString Dir = FPaths::GetPath(Path);
	IFileManager::Get().MakeDirectory(*Dir, true);

	const FString SafeDetails = Details.Replace(TEXT("\n"), TEXT(" ")).Replace(TEXT("\r"), TEXT(" "));
	const FString SafeCategory = Category.Replace(TEXT("\""), TEXT("'"));
	const FString SafeEvent = EventName.Replace(TEXT("\""), TEXT("'"));
	const FString SafeSubject = SubjectId.Replace(TEXT("\""), TEXT("'"));

	const FString Line = FString::Printf(
		TEXT("{\"ts\":\"%s\",\"category\":\"%s\",\"event\":\"%s\",\"subject\":\"%s\",\"details\":\"%s\"}\n"),
		*FDateTime::UtcNow().ToIso8601(),
		*SafeCategory,
		*SafeEvent,
		*SafeSubject,
		*SafeDetails.Replace(TEXT("\""), TEXT("'")));

	FFileHelper::SaveStringToFile(Line, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), FILEWRITE_Append);
}

bool FSpellRiseAuditTrail::QueryRecent(int32 MaxEntries, TArray<FString>& OutEntries)
{
	OutEntries.Reset();
	const int32 SafeMaxEntries = FMath::Clamp(MaxEntries, 1, 5000);
	const FString Path = GetAuditFilePath();
	if (!FPaths::FileExists(Path))
	{
		return false;
	}

	FString Content;
	if (!FFileHelper::LoadFileToString(Content, *Path))
	{
		return false;
	}

	TArray<FString> Lines;
	Content.ParseIntoArrayLines(Lines, true);
	if (Lines.Num() <= SafeMaxEntries)
	{
		OutEntries = MoveTemp(Lines);
		return true;
	}

	OutEntries.Append(Lines.GetData() + (Lines.Num() - SafeMaxEntries), SafeMaxEntries);
	return true;
}

void FSpellRiseAuditTrail::RotateIfNeeded(const FString& Path)
{
	const int64 Size = IFileManager::Get().FileSize(*Path);
	if (Size < 0 || Size < MaxAuditFileSizeBytes)
	{
		return;
	}

	const FString ArchivePath = FString::Printf(TEXT("%s.%s"), *Path, *FDateTime::UtcNow().ToString(TEXT("%Y%m%d_%H%M%S")));
	IFileManager::Get().Move(*ArchivePath, *Path, true, true, false, true);
}
