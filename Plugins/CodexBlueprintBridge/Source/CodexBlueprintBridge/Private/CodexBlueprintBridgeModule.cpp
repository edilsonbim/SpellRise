#include "CodexBlueprintBridgeModule.h"

#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexBlueprintBridgeModule, Log, All);

void FCodexBlueprintBridgeModule::StartupModule()
{
    UE_LOG(LogCodexBlueprintBridgeModule, Log, TEXT("CodexBlueprintBridge module started."));
}

void FCodexBlueprintBridgeModule::ShutdownModule()
{
    UE_LOG(LogCodexBlueprintBridgeModule, Log, TEXT("CodexBlueprintBridge module stopped."));
}

IMPLEMENT_MODULE(FCodexBlueprintBridgeModule, CodexBlueprintBridge)
