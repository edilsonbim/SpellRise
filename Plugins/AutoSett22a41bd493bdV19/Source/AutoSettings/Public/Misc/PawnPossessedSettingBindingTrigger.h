// Copyright Sam Bonifacio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SettingBindingTrigger.h"
#include "PawnPossessedSettingBindingTrigger.generated.h"

class APlayerController;

/**
 * Invalidates a player-scoped binding whenever its owning player controller possesses a new pawn.
 */
UCLASS(EditInlineNew, Blueprintable)
class AUTOSETTINGS_API UPawnPossessedSettingBindingTrigger : public USettingBindingTrigger
{
	GENERATED_BODY()

public:
	virtual void Activate_Implementation(const FSettingReference& Setting, USettingBinding* Binding) override;
	virtual void Deactivate_Implementation(const FSettingReference& Setting, USettingBinding* Binding) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> BoundPlayerController;

	FDelegateHandle PawnChangedHandle;
};
