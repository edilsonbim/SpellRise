// Copyright Narrative Tools 2025. 

#include "InventoryWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogNarrativeInventoryWidgetFocus, Log, All);

namespace
{
	FString DescribeSlateFocus()
	{
		if (!FSlateApplication::IsInitialized())
		{
			return TEXT("SlateUninitialized");
		}

		if (TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0))
		{
			return FString::Printf(TEXT("%s|%s"), *FocusedWidget->ToString(), *FocusedWidget->GetTypeAsString());
		}

		return TEXT("NoSlateFocus");
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	LogFocusState(TEXT("NativeConstruct"));
}

void UInventoryWidget::NativeDestruct()
{
	LogFocusState(TEXT("NativeDestruct"));
	Super::NativeDestruct();
}

void UInventoryWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);
	LogFocusState(TEXT("AddedToFocusPath"));
}

void UInventoryWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	LogFocusState(TEXT("RemovedFromFocusPath"));
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

FReply UInventoryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	LogFocusState(TEXT("FocusReceived"));
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UInventoryWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	LogFocusState(TEXT("FocusLost"));
	Super::NativeOnFocusLost(InFocusEvent);
}

void UInventoryWidget::LogFocusState(const TCHAR* SourceLabel) const
{
	UE_LOG(
		LogNarrativeInventoryWidgetFocus,
		Log,
		TEXT("[InventoryWidget][Focus] Source=%s Widget=%s Class=%s Visibility=%d SlateFocus=%s"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetClass()->GetName(),
		static_cast<int32>(GetVisibility()),
		*DescribeSlateFocus());
}

