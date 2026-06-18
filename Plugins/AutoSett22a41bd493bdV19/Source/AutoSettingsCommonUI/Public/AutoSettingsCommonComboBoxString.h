// Copyright Sam Bonifacio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "Input/UIActionBindingHandle.h"
#include "AutoSettingsCommonComboBoxString.generated.h"

/**
 * A ComboBox with some extra glue to register its own CommonUI back action when the ComboBox is open
 * This is required for the ComboBox to consume the back action when it's closed so that it doesn't fall through
 * to parent menus
 */
UCLASS()
class AUTOSETTINGSCOMMONUI_API UAutoSettingsCommonComboBoxString : public UComboBoxString
{
	GENERATED_BODY()

public:
	virtual void OnWidgetRebuilt() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
	virtual void HandleOpening() override;

private:

	FDelegateHandle FocusChangingHandle;

	FUIActionBindingHandle BackActionBindingHandle;

	// Flag to track pending async unregister and prevent race conditions during rapid open/close cycles
	bool bPendingBackHandlerUnregister = false;

	void HandleClosing();

	void RegisterBackHandler();
	void UnregisterBackHandler();

	UFUNCTION()
	UWidget* HandleMenuNavigation(EUINavigation NavDirection);
	
};
