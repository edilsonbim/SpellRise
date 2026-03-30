#pragma once

#include "CoreMinimal.h"

class SPELLRISE_API FSpellRiseAuditTrail
{
public:
	static void AppendEvent(const FString& Category, const FString& EventName, const FString& SubjectId, const FString& Details);
	static bool QueryRecent(int32 MaxEntries, TArray<FString>& OutEntries);
	static FString GetAuditFilePath();

private:
	static void RotateIfNeeded(const FString& Path);
};
