#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SMultiLineEditableTextBox;

class SCodexBlueprintBridgePromptWidget final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SCodexBlueprintBridgePromptWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply UseSelectedAsset();
    FReply StartConsumer();
    FReply EnqueuePrompt();
    FReply RefreshResponse();
    EActiveTimerReturnType PollResponse(double CurrentTime, float DeltaTime);

    FString GetQueueDirectory() const;
    FString GetSelectedAssetPath() const;
    bool TryLoadCurrentResponse(bool bReportPending);
    void SetStatus(const FString& Message, bool bIsError = false);

    TSharedPtr<SMultiLineEditableTextBox> PromptTextBox;
    TSharedPtr<SMultiLineEditableTextBox> ResponseTextBox;
    FString AssetPath;
    FString CurrentRequestId;
    bool bRequestPending = false;
    FText StatusText;
    FSlateColor StatusColor;
};
