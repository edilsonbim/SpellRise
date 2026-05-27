// Copyright Narrative Tools 2025. 


#include "NarrativeActivatableWidget.h"
#include "Input/CommonUIInputTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogNarrativeActivatableWidget, Log, All);

void UNarrativeActivatableWidget::NativeDestruct()
{
	for (FUIActionBindingHandle Handle : BindingHandles)
	{
		if (Handle.IsValid())
		{
			Handle.Unregister();
		}
	}
	BindingHandles.Empty();

	Super::NativeDestruct();
}

void UNarrativeActivatableWidget::RegisterBinding(FDataTableRowHandle InputAction, const FInputActionExecutedDelegate& Callback, FInputActionBindingHandle& BindingHandle, FText OverrideDisplayName, const bool bShouldDisplayInActionBar/*=true*/)
{
	if (InputAction.IsNull() || InputAction.RowName.IsNone())
	{
		UE_LOG(
			LogNarrativeActivatableWidget,
			Warning,
			TEXT("Ignoring UI input binding with invalid data table row handle on widget '%s'."),
			*GetNameSafe(this));
		return;
	}

	FBindUIActionArgs BindArgs(InputAction, FSimpleDelegate::CreateLambda([InputAction, Callback]()
		{
			Callback.ExecuteIfBound(InputAction.RowName);
		}));
	BindArgs.bDisplayInActionBar = bShouldDisplayInActionBar;
	BindArgs.OverrideDisplayName = OverrideDisplayName;

	BindingHandle.Handle = RegisterUIActionBinding(BindArgs);
	BindingHandles.Add(BindingHandle.Handle);
}

void UNarrativeActivatableWidget::UnregisterBinding(FInputActionBindingHandle BindingHandle)
{
	RemoveActionBinding(BindingHandle.Handle);

	if (BindingHandle.Handle.IsValid())
	{
		BindingHandle.Handle.Unregister();
		BindingHandles.Remove(BindingHandle.Handle);
	}
}

void UNarrativeActivatableWidget::UnregisterAllBindings()
{
	for (FUIActionBindingHandle Handle : BindingHandles)
	{
		Handle.Unregister();
	}
	BindingHandles.Empty();
}

void UNarrativeActivatableWidget::SetBindingDisplayName(FInputActionBindingHandle BindingHandle, FText NewDisplayName)
{
	if (BindingHandle.Handle.IsValid())
	{
		BindingHandle.Handle.SetDisplayName(NewDisplayName);
	}
}
