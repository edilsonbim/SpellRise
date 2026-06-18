// Copyright Sam Bonifacio. All Rights Reserved.


#include "AutoSettingsCommonComboBoxString.h"

#include "AutoSettingsLogs.h"
#include "CommonUITypes.h"
#include "ICommonInputModule.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputTypes.h"
#include "Engine/World.h"
#include "Async/Async.h"
#include "Logging/StructuredLog.h"

void UAutoSettingsCommonComboBoxString::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	/**
	 * Register an callback to detect when the focus changes to the main ComboBox button
	 * We use this to infer when the ComboBox is closed, because the widget API has no event for that
	 */
	if (!FocusChangingHandle.IsValid())
	{
		FocusChangingHandle = FSlateApplication::Get().OnFocusChanging().AddWeakLambda(this, [this](
			const FFocusEvent& Event,
			const FWeakWidgetPath& OldPath,
			const TSharedPtr<SWidget>& OldWidget,
			const FWidgetPath& NewPath,
			const TSharedPtr<SWidget>& NewWidget)
			{
				// Only care if the focused widget is the ComboBox
				if (NewWidget.IsValid() && NewWidget == MyWidget.Pin())
				{
					HandleClosing();
				}
			});
	}
}

void UAutoSettingsCommonComboBoxString::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	// Application may already be uninitialized if shutting down
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnFocusChanging().Remove(FocusChangingHandle);
	}

	UnregisterBackHandler();
}

void UAutoSettingsCommonComboBoxString::HandleOpening()
{
	Super::HandleOpening();

	RegisterBackHandler();

	FCustomWidgetNavigationDelegate Delegate;
	Delegate.BindDynamic(this, &ThisClass::HandleMenuNavigation);
	SetNavigationRuleCustom(EUINavigation::Up, Delegate);
	SetNavigationRuleCustom(EUINavigation::Down, Delegate);
	SetNavigationRuleCustom(EUINavigation::Left, Delegate);
	SetNavigationRuleCustom(EUINavigation::Right, Delegate);
}

void UAutoSettingsCommonComboBoxString::HandleClosing()
{
	// Mark that we have a pending unregister to prevent race conditions
	bPendingBackHandlerUnregister = true;

	/**
	 * Schedule task to unregister the back handler next tick, because the CommonUI action will be triggered later in the frame,
	 * and we need to keep the action registered until then so that it actually blocks it from falling back to a parent.
	 * Note: We use AsyncTask rather than World TimerManager so that it works with the game paused.
	 */
	AsyncTask(ENamedThreads::GameThread, [WeakThis = TWeakObjectPtr<ThisClass>(this)]()
	{
		if (WeakThis.IsValid() && WeakThis->bPendingBackHandlerUnregister)
		{
			WeakThis->UnregisterBackHandler();
			WeakThis->bPendingBackHandlerUnregister = false;
		}
	});

	SetNavigationRuleBase(EUINavigation::Up, EUINavigationRule::Escape);
	SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Escape);
	SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Escape);
	SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Escape);
}

void UAutoSettingsCommonComboBoxString::RegisterBackHandler()
{
	if (UCommonUIActionRouterBase* ActionRouter = UCommonUIActionRouterBase::Get(*this))
	{
		// Handle re-registration: unregister existing handler to prevent leaks
		if (BackActionBindingHandle.IsValid())
		{
			UE_LOGFMT(LogAutoSettings, Warning, "RegisterBackHandler called while handler already registered - unregistering existing handler to prevent leak");
			UnregisterBackHandler();
		}

		// Cancel any pending async unregister to prevent race condition
		bPendingBackHandlerUnregister = false;

		FSimpleDelegate BackActionDelegate = FSimpleDelegate::CreateWeakLambda(this,
			[this]()
			{
				// This delegate doesn't need to do anything
				// The important thing is that it consumes the input so that it doesn't bubble up through CommonUI
				// The actual logic to close is handled by the native SComboBox
			});

		const bool bUseEnhancedInput = CommonUI::IsEnhancedInputSupportEnabled() && ICommonInputModule::GetSettings().
		                               GetEnhancedInputBackAction();
		FBindUIActionArgs BindArgs = bUseEnhancedInput
			                             ? FBindUIActionArgs(ICommonInputModule::GetSettings().GetEnhancedInputBackAction(),
				                             BackActionDelegate)
			                             : FBindUIActionArgs(ICommonInputModule::GetSettings().GetDefaultBackAction(),
				                             BackActionDelegate);
		BindArgs.bDisplayInActionBar = true;

		BackActionBindingHandle = ActionRouter->RegisterUIActionBinding(*this, BindArgs);
		ensure(BackActionBindingHandle.IsValid());
	}
}

void UAutoSettingsCommonComboBoxString::UnregisterBackHandler()
{
	if (UCommonUIActionRouterBase* ActionRouter = UCommonUIActionRouterBase::Get(*this))
	{
		if (BackActionBindingHandle.IsValid())
		{
			BackActionBindingHandle.Unregister();
		}
	}
}

UWidget* UAutoSettingsCommonComboBoxString::HandleMenuNavigation(EUINavigation NavDirection)
{
	// Return null to block directional navigation while the combo box menu is open
	// This prevents focus from moving away from the menu when using directional inputs
	return {};
}