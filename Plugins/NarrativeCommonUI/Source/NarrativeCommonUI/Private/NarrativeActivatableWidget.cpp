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
	BindingHandlesByActionName.Empty();

	Super::NativeDestruct();
}

void UNarrativeActivatableWidget::RegisterBinding(FDataTableRowHandle InputAction, const FInputActionExecutedDelegate& Callback, FInputActionBindingHandle& BindingHandle, FText OverrideDisplayName, const bool bShouldDisplayInActionBar/*=true*/)
{
	if (InputAction.IsNull() || InputAction.RowName.IsNone())
	{
		UE_LOG(
			LogNarrativeActivatableWidget,
			Warning,
			TEXT("Ignoring UI input binding with invalid data table row handle on widget '%s' Class='%s' Path='%s'."),
			*GetNameSafe(this),
			*GetNameSafe(GetClass()),
			*GetPathName());
		return;
	}

	if (FUIActionBindingHandle* ExistingHandle = BindingHandlesByActionName.Find(InputAction.RowName))
	{
		if (ExistingHandle->IsValid())
		{
			ExistingHandle->Unregister();
		}
		BindingHandles.Remove(*ExistingHandle);
		BindingHandlesByActionName.Remove(InputAction.RowName);
	}

	if (BindingHandle.Handle.IsValid())
	{
		BindingHandle.Handle.Unregister();
		BindingHandles.Remove(BindingHandle.Handle);
	}

	FBindUIActionArgs BindArgs(InputAction, FSimpleDelegate::CreateLambda([InputAction, Callback]()
		{
			Callback.ExecuteIfBound(InputAction.RowName);
		}));
	BindArgs.bDisplayInActionBar = bShouldDisplayInActionBar;
	BindArgs.OverrideDisplayName = OverrideDisplayName;

	BindingHandle.Handle = RegisterUIActionBinding(BindArgs);
	BindingHandles.Add(BindingHandle.Handle);
	BindingHandlesByActionName.Add(InputAction.RowName, BindingHandle.Handle);
}

void UNarrativeActivatableWidget::UnregisterBinding(FInputActionBindingHandle BindingHandle)
{
	RemoveActionBinding(BindingHandle.Handle);

	if (BindingHandle.Handle.IsValid())
	{
		BindingHandle.Handle.Unregister();
		BindingHandles.Remove(BindingHandle.Handle);
		for (TMap<FName, FUIActionBindingHandle>::TIterator It(BindingHandlesByActionName); It; ++It)
		{
			if (It.Value() == BindingHandle.Handle)
			{
				It.RemoveCurrent();
				break;
			}
		}
	}
}

void UNarrativeActivatableWidget::UnregisterAllBindings()
{
	for (FUIActionBindingHandle Handle : BindingHandles)
	{
		Handle.Unregister();
	}
	BindingHandles.Empty();
	BindingHandlesByActionName.Empty();
}

void UNarrativeActivatableWidget::SetBindingDisplayName(FInputActionBindingHandle BindingHandle, FText NewDisplayName)
{
	if (BindingHandle.Handle.IsValid())
	{
		BindingHandle.Handle.SetDisplayName(NewDisplayName);
	}
}
