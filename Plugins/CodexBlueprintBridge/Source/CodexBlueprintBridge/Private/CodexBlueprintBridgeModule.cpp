#include "CodexBlueprintBridgeModule.h"

#include "CodexBlueprintBridgePromptWidget.h"
#include "Framework/Docking/TabManager.h"
#include "Logging/LogMacros.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexBlueprintBridgeModule, Log, All);

namespace
{
const FName CodexBlueprintBridgePromptTabName(TEXT("CodexBlueprintBridgePrompt"));
}

void FCodexBlueprintBridgeModule::StartupModule()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        CodexBlueprintBridgePromptTabName,
        FOnSpawnTab::CreateRaw(this, &FCodexBlueprintBridgeModule::SpawnPromptTab))
        .SetDisplayName(NSLOCTEXT("CodexBlueprintBridge", "PromptTabTitle", "Codex Blueprint Bridge"))
        .SetTooltipText(NSLOCTEXT("CodexBlueprintBridge", "PromptTabTooltip", "Open the local Codex prompt queue."))
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Terminal"))
        .SetMenuType(ETabSpawnerMenuType::Enabled);

    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCodexBlueprintBridgeModule::RegisterMenus));

    UE_LOG(LogCodexBlueprintBridgeModule, Log, TEXT("CodexBlueprintBridge module started."));
}

void FCodexBlueprintBridgeModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CodexBlueprintBridgePromptTabName);

    UE_LOG(LogCodexBlueprintBridgeModule, Log, TEXT("CodexBlueprintBridge module stopped."));
}

void FCodexBlueprintBridgeModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);
    UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
    FToolMenuSection& Section = ToolsMenu->FindOrAddSection("Programming");
    Section.AddMenuEntry(
        "OpenCodexBlueprintBridge",
        NSLOCTEXT("CodexBlueprintBridge", "OpenPromptTabLabel", "Codex Blueprint Bridge"),
        NSLOCTEXT("CodexBlueprintBridge", "OpenPromptTabTooltip", "Open the local prompt queue for Blueprint analysis."),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Terminal"),
        FUIAction(FExecuteAction::CreateLambda([]
        {
            FGlobalTabmanager::Get()->TryInvokeTab(CodexBlueprintBridgePromptTabName);
        })));
}

TSharedRef<SDockTab> FCodexBlueprintBridgeModule::SpawnPromptTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SCodexBlueprintBridgePromptWidget)
        ];
}

IMPLEMENT_MODULE(FCodexBlueprintBridgeModule, CodexBlueprintBridge)
