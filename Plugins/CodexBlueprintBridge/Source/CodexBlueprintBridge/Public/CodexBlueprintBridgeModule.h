#pragma once

#include "Modules/ModuleManager.h"

class SDockTab;
class FSpawnTabArgs;

class FCodexBlueprintBridgeModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenus();
    TSharedRef<SDockTab> SpawnPromptTab(const FSpawnTabArgs& SpawnTabArgs);
};
