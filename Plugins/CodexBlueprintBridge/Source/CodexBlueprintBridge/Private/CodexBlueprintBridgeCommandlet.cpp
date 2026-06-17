#include "CodexBlueprintBridgeCommandlet.h"

#include "CodexBlueprintBridgeJson.h"
#include "CodexBlueprintBridgeService.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexBlueprintBridgeCommandlet, Log, All);

UCodexBlueprintBridgeCommandlet::UCodexBlueprintBridgeCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UCodexBlueprintBridgeCommandlet::Main(const FString& Params)
{
    FString InputPath;
    FString OutputPath;

    FParse::Value(*Params, TEXT("Input="), InputPath);
    FParse::Value(*Params, TEXT("Output="), OutputPath);

    if (InputPath.IsEmpty() || OutputPath.IsEmpty())
    {
        UE_LOG(LogCodexBlueprintBridgeCommandlet, Error, TEXT("Missing -Input or -Output parameter."));
        return 2;
    }

    TSharedPtr<FJsonObject> Request;
    FString LoadError;
    if (!FCodexBlueprintBridgeJson::LoadJsonFile(InputPath, Request, LoadError))
    {
        FCodexBridgeResult Result;
        Result.Operation = TEXT("Unknown");
        Result.Errors.Add({TEXT("InvalidInputJson"), LoadError, TEXT("Input")});
        FCodexBlueprintBridgeJson::SaveResult(OutputPath, Result);
        return 3;
    }

    FCodexBlueprintBridgeService Service;
    const FCodexBridgeResult Result = Service.Execute(Request.ToSharedRef());

    FString SaveError;
    if (!FCodexBlueprintBridgeJson::SaveResult(OutputPath, Result, &SaveError))
    {
        UE_LOG(LogCodexBlueprintBridgeCommandlet, Error, TEXT("Failed to save output JSON: %s"), *SaveError);
        return 4;
    }

    return Result.bSuccess ? 0 : 1;
}
