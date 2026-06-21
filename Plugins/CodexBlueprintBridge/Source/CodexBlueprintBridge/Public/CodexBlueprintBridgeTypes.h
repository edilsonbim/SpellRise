#pragma once

#include "CoreMinimal.h"

enum class ECodexBridgeMutationKind : uint8
{
    None,
    Graph,
    Widget,
    Binding
};

struct FCodexBridgeRequestPolicy
{
    bool bDryRun = true;
    bool bAllowGraphMutation = false;
    bool bAllowWidgetMutation = false;
    bool bAllowBindingMutation = false;
    bool bAllowAuthoritySensitiveMutation = false;
    int32 MutationCount = 0;
    int32 PayloadBytes = 0;
};

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
