// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/UI/SpellRiseDeathScreenWidget.h"

#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"

void USpellRiseDeathScreenWidget::SetMessage(const FText& InMessage)
{
	Message = InMessage;
	if (TextBlock_InteractableName)
	{
		TextBlock_InteractableName->SetText(InMessage);
	}
	Invalidate(EInvalidateWidget::Paint);
}

void USpellRiseDeathScreenWidget::RequestAcceptDeath()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(PC->GetPawn()))
		{
			if (Character->IsDowned())
			{
				Character->ServerAcceptDeath();
			}
		}
	}
}

void USpellRiseDeathScreenWidget::ConfigureDownedAction(ASpellRiseCharacterBase* TargetCharacter)
{
	DownedActionTarget = TargetCharacter;
	SetMessage(FText::FromString(TEXT("Escolha: Reviver ou Gankar")));
	if (Text_YesAction)
	{
		Text_YesAction->SetText(FText::FromString(TEXT("Revive")));
	}
	if (Text_NoAction)
	{
		Text_NoAction->SetText(FText::FromString(TEXT("Gank")));
	}
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	SetKeyboardFocus();
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
	OnShowDownedActions(TargetCharacter);
}

void USpellRiseDeathScreenWidget::RequestDownedAction(bool bRevive)
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(PC->GetPawn()))
		{
			Character->ServerResolveDownedAction(DownedActionTarget, bRevive);
		}
	}

	DownedActionTarget = nullptr;
	OnHideDownedActions();
	SetVisibility(ESlateVisibility::Collapsed);
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
}

void USpellRiseDeathScreenWidget::HandleReviveAction()
{
	if (DownedActionTarget)
	{
		RequestDownedAction(true);
	}
}

void USpellRiseDeathScreenWidget::HandleGankAction()
{
	if (DownedActionTarget)
	{
		RequestDownedAction(false);
	}
}

void USpellRiseDeathScreenWidget::HandleLifeStateChanged(
	ESpellRiseLifeState NewState,
	ESpellRiseLifeState OldState)
{
	OnLifeStateChanged(NewState, OldState);
}

void USpellRiseDeathScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	SetVisibility(ESlateVisibility::Visible);
	SetIsEnabled(true);

	if (Message.IsEmpty())
	{
		Message = FText::FromString(TEXT("You are dead."));
	}
}

FReply USpellRiseDeathScreenWidget::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (DownedActionTarget)
	{
		if (InKeyEvent.GetKey() == EKeys::Y)
		{
			HandleReviveAction();
			return FReply::Handled();
		}
		if (InKeyEvent.GetKey() == EKeys::N)
		{
			HandleGankAction();
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply USpellRiseDeathScreenWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
