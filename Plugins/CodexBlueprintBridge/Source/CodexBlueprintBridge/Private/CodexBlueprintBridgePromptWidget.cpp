#include "CodexBlueprintBridgePromptWidget.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
constexpr int32 MaxPromptCharacters = 16000;
}
void SCodexBlueprintBridgePromptWidget::Construct(const FArguments& InArgs)
{
    StatusText = NSLOCTEXT("CodexBlueprintBridge", "ReadyStatus", "Ready.");
    StatusColor = FSlateColor::UseForeground();

    ChildSlot
    [
        SNew(SBorder)
        .Padding(12.0f)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(NSLOCTEXT("CodexBlueprintBridge", "PromptHeader", "Blueprint Prompt"))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 10.0f, 0.0f, 4.0f)
            [
                SNew(STextBlock)
                .Text(NSLOCTEXT("CodexBlueprintBridge", "AssetLabel", "Asset path"))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(SEditableTextBox)
                    .Text_Lambda([this] { return FText::FromString(AssetPath); })
                    .OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type)
                    {
                        AssetPath = Text.ToString();
                    })
                    .HintText(NSLOCTEXT("CodexBlueprintBridge", "AssetHint", "/Game/UI/Widgets/WBP_Example"))
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(8.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(SButton)
                    .Text(NSLOCTEXT("CodexBlueprintBridge", "UseSelectedAsset", "Use Selected"))
                    .OnClicked(this, &SCodexBlueprintBridgePromptWidget::UseSelectedAsset)
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 10.0f, 0.0f, 4.0f)
            [
                SNew(STextBlock)
                .Text(NSLOCTEXT("CodexBlueprintBridge", "PromptLabel", "Prompt"))
            ]

            + SVerticalBox::Slot()
            .FillHeight(0.45f)
            [
                SAssignNew(PromptTextBox, SMultiLineEditableTextBox)
                .HintText(NSLOCTEXT("CodexBlueprintBridge", "PromptHint", "Describe the analysis or change to propose."))
                .AllowMultiLine(true)
                .AutoWrapText(true)
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 8.0f)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SButton)
                    .Text(NSLOCTEXT("CodexBlueprintBridge", "StartConsumer", "Start Consumer"))
                    .OnClicked(this, &SCodexBlueprintBridgePromptWidget::StartConsumer)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(8.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(SButton)
                    .Text(NSLOCTEXT("CodexBlueprintBridge", "EnqueuePrompt", "Queue Prompt"))
                    .OnClicked(this, &SCodexBlueprintBridgePromptWidget::EnqueuePrompt)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(8.0f, 0.0f)
                [
                    SNew(SButton)
                    .Text(NSLOCTEXT("CodexBlueprintBridge", "RefreshResponse", "Refresh Response"))
                    .OnClicked(this, &SCodexBlueprintBridgePromptWidget::RefreshResponse)
                ]

                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .VAlign(VAlign_Center)
                .Padding(8.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(STextBlock)
                    .Text_Lambda([this] { return StatusText; })
                    .ColorAndOpacity_Lambda([this] { return StatusColor; })
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 4.0f)
            [
                SNew(STextBlock)
                .Text(NSLOCTEXT("CodexBlueprintBridge", "ResponseLabel", "Response / proposed patch"))
            ]

            + SVerticalBox::Slot()
            .FillHeight(0.55f)
            [
                SAssignNew(ResponseTextBox, SMultiLineEditableTextBox)
                .IsReadOnly(true)
                .AllowMultiLine(true)
                .AutoWrapText(true)
            ]
        ]
    ];

    RegisterActiveTimer(
        1.0f,
        FWidgetActiveTimerDelegate::CreateSP(this, &SCodexBlueprintBridgePromptWidget::PollResponse));
}

FReply SCodexBlueprintBridgePromptWidget::UseSelectedAsset()
{
    const FString SelectedPath = GetSelectedAssetPath();
    if (SelectedPath.IsEmpty())
    {
        SetStatus(TEXT("Select exactly one asset in the Content Browser."), true);
        return FReply::Handled();
    }

    AssetPath = SelectedPath;
    SetStatus(FString::Printf(TEXT("Selected %s"), *AssetPath));
    return FReply::Handled();
}

FReply SCodexBlueprintBridgePromptWidget::StartConsumer()
{
    if (!IFileManager::Get().MakeDirectory(*GetQueueDirectory(), true))
    {
        SetStatus(TEXT("Failed to create the prompt queue directory."), true);
        return FReply::Handled();
    }

    const FString LockPath = GetQueueDirectory() / TEXT("consumer.lock");
    if (FPaths::FileExists(LockPath)
        && !IFileManager::Get().Delete(*LockPath, false, true, true))
    {
        SetStatus(TEXT("Prompt consumer is already running."));
        return FReply::Handled();
    }

    const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("CodexBlueprintBridge"));
    if (!Plugin.IsValid())
    {
        SetStatus(TEXT("CodexBlueprintBridge plugin path was not found."), true);
        return FReply::Handled();
    }

    const FString ScriptPath = Plugin->GetBaseDir() / TEXT("Scripts/Run-PromptConsumer.ps1");
    if (!FPaths::FileExists(ScriptPath))
    {
        SetStatus(FString::Printf(TEXT("Consumer script not found: %s"), *ScriptPath), true);
        return FReply::Handled();
    }

    const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
    const FString Arguments = FString::Printf(
        TEXT("-NoLogo -NoProfile -ExecutionPolicy Bypass -File \"%s\" -ProjectPath \"%s\""),
        *ScriptPath,
        *ProjectPath);

    FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
        TEXT("powershell.exe"),
        *Arguments,
        true,
        true,
        true,
        nullptr,
        0,
        *FPaths::ProjectDir(),
        nullptr);

    if (!ProcessHandle.IsValid())
    {
        SetStatus(TEXT("Failed to start the prompt consumer."), true);
        return FReply::Handled();
    }

    FPlatformProcess::CloseProc(ProcessHandle);
    SetStatus(TEXT("Prompt consumer started."));
    return FReply::Handled();
}

FReply SCodexBlueprintBridgePromptWidget::EnqueuePrompt()
{
    if (bRequestPending)
    {
        SetStatus(TEXT("Wait for the current request to finish."), true);
        return FReply::Handled();
    }

    FString Prompt = PromptTextBox.IsValid() ? PromptTextBox->GetText().ToString() : FString();
    Prompt.TrimStartAndEndInline();
    AssetPath.TrimStartAndEndInline();

    if (Prompt.IsEmpty())
    {
        SetStatus(TEXT("Prompt is required."), true);
        return FReply::Handled();
    }

    if (Prompt.Len() > MaxPromptCharacters)
    {
        SetStatus(FString::Printf(TEXT("Prompt exceeds %d characters."), MaxPromptCharacters), true);
        return FReply::Handled();
    }

    if (!AssetPath.IsEmpty() && (!AssetPath.StartsWith(TEXT("/Game/")) || AssetPath.Contains(TEXT(".."))))
    {
        SetStatus(TEXT("Asset path must be empty or under /Game/."), true);
        return FReply::Handled();
    }

    const FString QueueDirectory = GetQueueDirectory();
    if (!IFileManager::Get().MakeDirectory(*QueueDirectory, true))
    {
        SetStatus(TEXT("Failed to create the prompt queue directory."), true);
        return FReply::Handled();
    }

    CurrentRequestId = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString RequestPath = QueueDirectory / FString::Printf(TEXT("%s.request.json"), *CurrentRequestId);

    const TSharedRef<FJsonObject> Request = MakeShared<FJsonObject>();
    Request->SetNumberField(TEXT("version"), 1);
    Request->SetStringField(TEXT("kind"), TEXT("CodexBlueprintPrompt"));
    Request->SetStringField(TEXT("requestId"), CurrentRequestId);
    Request->SetStringField(TEXT("createdUtc"), FDateTime::UtcNow().ToIso8601());
    Request->SetStringField(TEXT("status"), TEXT("pending"));
    Request->SetStringField(TEXT("asset"), AssetPath);
    Request->SetStringField(TEXT("prompt"), Prompt);
    Request->SetBoolField(TEXT("allowAssetMutation"), false);

    FString Json;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
    if (!FJsonSerializer::Serialize(Request, Writer) || !FFileHelper::SaveStringToFile(Json, *RequestPath))
    {
        SetStatus(TEXT("Failed to write the prompt request."), true);
        return FReply::Handled();
    }

    if (ResponseTextBox.IsValid())
    {
        ResponseTextBox->SetText(FText::GetEmpty());
    }
    bRequestPending = true;
    SetStatus(FString::Printf(TEXT("Queued request %s"), *CurrentRequestId));
    return FReply::Handled();
}

FReply SCodexBlueprintBridgePromptWidget::RefreshResponse()
{
    if (CurrentRequestId.IsEmpty())
    {
        SetStatus(TEXT("Queue a prompt first."), true);
        return FReply::Handled();
    }

    TryLoadCurrentResponse(true);
    return FReply::Handled();
}

EActiveTimerReturnType SCodexBlueprintBridgePromptWidget::PollResponse(double CurrentTime, float DeltaTime)
{
    if (bRequestPending)
    {
        TryLoadCurrentResponse(false);
    }
    return EActiveTimerReturnType::Continue;
}

bool SCodexBlueprintBridgePromptWidget::TryLoadCurrentResponse(bool bReportPending)
{
    const FString ResponsePath =
        GetQueueDirectory() / FString::Printf(TEXT("%s.response.json"), *CurrentRequestId);
    FString Response;
    if (!FFileHelper::LoadFileToString(Response, *ResponsePath))
    {
        const FString ErrorPath = GetQueueDirectory() / FString::Printf(TEXT("%s.error.json"), *CurrentRequestId);
        if (FFileHelper::LoadFileToString(Response, *ErrorPath))
        {
            if (ResponseTextBox.IsValid())
            {
                ResponseTextBox->SetText(FText::FromString(Response));
            }
            bRequestPending = false;
            SetStatus(FString::Printf(TEXT("Request %s failed."), *CurrentRequestId), true);
            return true;
        }

        if (bReportPending)
        {
            SetStatus(TEXT("Response is not available yet."));
        }
        return false;
    }

    if (ResponseTextBox.IsValid())
    {
        ResponseTextBox->SetText(FText::FromString(Response));
    }
    bRequestPending = false;
    SetStatus(FString::Printf(TEXT("Loaded response for %s"), *CurrentRequestId));
    return true;
}

FString SCodexBlueprintBridgePromptWidget::GetQueueDirectory() const
{
    return FPaths::ProjectSavedDir() / TEXT("CodexBlueprintBridge/PromptQueue");
}

FString SCodexBlueprintBridgePromptWidget::GetSelectedAssetPath() const
{
    const FContentBrowserModule& ContentBrowserModule =
        FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    TArray<FAssetData> SelectedAssets;
    ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
    return SelectedAssets.Num() == 1 ? SelectedAssets[0].GetSoftObjectPath().GetLongPackageName() : FString();
}

void SCodexBlueprintBridgePromptWidget::SetStatus(const FString& Message, bool bIsError)
{
    StatusText = FText::FromString(Message);
    StatusColor = bIsError
        ? FSlateColor(FLinearColor(0.85f, 0.15f, 0.12f))
        : FSlateColor::UseForeground();
}
