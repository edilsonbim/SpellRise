// Copyright Sam Bonifacio. All Rights Reserved.

#include "Misc/PawnPossessedSettingBindingTrigger.h"

#include "Framework/AutoSettingsPlayerSubsystem.h"
#include "Framework/SettingBinding.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

void UPawnPossessedSettingBindingTrigger::Activate_Implementation(const FSettingReference& Setting, USettingBinding* Binding)
{
	Super::Activate_Implementation(Setting, Binding);

	if (!Binding)
	{
		return;
	}

	BoundPlayerController = UAutoSettingsPlayerSubsystem::GetOwningPlayerControllerFromSetting(Setting);
	if (!BoundPlayerController)
	{
		return;
	}

	PawnChangedHandle = BoundPlayerController->GetOnNewPawnNotifier().AddWeakLambda(this,
		[Binding](APawn* NewPawn)
		{
			if (Binding)
			{
				Binding->NotifyBindingInvalidated();
			}
		});
}

void UPawnPossessedSettingBindingTrigger::Deactivate_Implementation(const FSettingReference& Setting, USettingBinding* Binding)
{
	if (BoundPlayerController && PawnChangedHandle.IsValid())
	{
		BoundPlayerController->GetOnNewPawnNotifier().Remove(PawnChangedHandle);
	}

	PawnChangedHandle.Reset();
	BoundPlayerController = nullptr;

	Super::Deactivate_Implementation(Setting, Binding);
}
