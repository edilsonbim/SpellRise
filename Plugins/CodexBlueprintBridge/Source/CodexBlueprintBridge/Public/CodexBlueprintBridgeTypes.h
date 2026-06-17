#pragma once

#include "CoreMinimal.h"

struct FCodexBridgeError
{
    FString Code;
    FString Message;
    FString Field;
};

struct FCodexBridgeResult
{
    bool bSuccess = false;
    FString Operation;
    TArray<FString> Warnings;
    TArray<FCodexBridgeError> Errors;
    TSharedPtr<FJsonObject> Data;
};
