// Copyright Narrative Tools 2025. 

#include "InventoryWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
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

	SetIsFocusable(true);
	if (APlayerController* OwningPlayer = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		OwningPlayer->SetInputMode(InputMode);
		OwningPlayer->bShowMouseCursor = true;
		OwningPlayer->bEnableClickEvents = true;
		OwningPlayer->bEnableMouseOverEvents = true;
	}

	SetKeyboardFocus();
	LogFocusState(TEXT("NativeConstruct"));
}

void UInventoryWidget::NativeDestruct()
{
	LogFocusState(TEXT("NativeDestruct"));
	RestoreOwningPlayerGameplayInput(TEXT("NativeDestruct"));
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

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	LogFocusState(TEXT("KeyDown"));

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		RequestStopLootingFromOwningInventory();
		RemoveFromParent();
		RestoreOwningPlayerGameplayInput(TEXT("Escape"));
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

bool UInventoryWidget::RequestStopLootingFromOwningInventory() const
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer)
	{
		return false;
	}

	UNarrativeInventoryComponent* Inventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(OwningPlayer);
	if (!Inventory)
	{
		return false;
	}

	UFunction* StopLootingFunction = Inventory->FindFunction(TEXT("StopLooting"));
	if (!StopLootingFunction)
	{
		return false;
	}

	Inventory->ProcessEvent(StopLootingFunction, nullptr);
	return true;
}

void UInventoryWidget::RestoreOwningPlayerGameplayInput(const TCHAR* SourceLabel) const
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer || !OwningPlayer->IsLocalController())
	{
		return;
	}

	OwningPlayer->bShowMouseCursor = false;
	OwningPlayer->bEnableClickEvents = false;
	OwningPlayer->bEnableMouseOverEvents = false;

	FInputModeGameOnly InputMode;
	OwningPlayer->SetInputMode(InputMode);
	OwningPlayer->SetIgnoreMoveInput(false);
	OwningPlayer->SetIgnoreLookInput(false);

	if (APawn* ControlledPawn = OwningPlayer->GetPawn())
	{
		ControlledPawn->EnableInput(OwningPlayer);
	}

	UE_LOG(
		LogNarrativeInventoryWidgetFocus,
		Log,
		TEXT("[InventoryWidget][RestoreGameplayInput] Source=%s Widget=%s Controller=%s Pawn=%s"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetNameSafe(OwningPlayer),
		*GetNameSafe(OwningPlayer->GetPawn()));
}

void UInventoryWidget::LogFocusState(const TCHAR* SourceLabel) const
{
	const APlayerController* OwningPlayer = GetOwningPlayer();
	const ACharacter* ControlledCharacter = OwningPlayer ? Cast<ACharacter>(OwningPlayer->GetPawn()) : nullptr;
	const UCharacterMovementComponent* Movement = ControlledCharacter ? ControlledCharacter->GetCharacterMovement() : nullptr;

	UE_LOG(
		LogNarrativeInventoryWidgetFocus,
		Log,
		TEXT("[InventoryWidget][Focus] Source=%s Widget=%s Class=%s Visibility=%d SlateFocus=%s Controller=%s Pawn=%s Cursor=%d Click=%d MouseOver=%d MoveIgnored=%d LookIgnored=%d MovementMode=%d MaxWalkSpeed=%.2f"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetClass()->GetName(),
		static_cast<int32>(GetVisibility()),
		*DescribeSlateFocus(),
		*GetNameSafe(OwningPlayer),
		*GetNameSafe(OwningPlayer ? OwningPlayer->GetPawn() : nullptr),
		OwningPlayer && OwningPlayer->bShowMouseCursor ? 1 : 0,
		OwningPlayer && OwningPlayer->bEnableClickEvents ? 1 : 0,
		OwningPlayer && OwningPlayer->bEnableMouseOverEvents ? 1 : 0,
		OwningPlayer && OwningPlayer->IsMoveInputIgnored() ? 1 : 0,
		OwningPlayer && OwningPlayer->IsLookInputIgnored() ? 1 : 0,
		Movement ? static_cast<int32>(Movement->MovementMode) : INDEX_NONE,
		Movement ? Movement->MaxWalkSpeed : 0.0f);
}

