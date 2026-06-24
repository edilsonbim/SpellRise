#include "SpellRiseInputRouterComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "SpellRise/Core/SpellRisePlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseInputRouter, Log, All);

namespace
{
	constexpr int32 GlobalPriority = 100;
	constexpr int32 UIPriority = 80;
	constexpr int32 BuildingPriority = 60;
	constexpr int32 CombatPriority = 40;
	constexpr int32 MovementPriority = 20;

	bool MatchesActionName(const UInputAction* Action, const FName ExpectedName)
	{
		if (!Action || ExpectedName.IsNone())
		{
			return false;
		}

		const FString Actual = Action->GetName();
		const FString Expected = ExpectedName.ToString();
		return Actual.Equals(Expected, ESearchCase::IgnoreCase)
			|| Actual.Equals(FString::Printf(TEXT("IA_%s"), *Expected), ESearchCase::IgnoreCase);
	}
}

USpellRiseInputRouterComponent::USpellRiseInputRouterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

ASpellRisePlayerController* USpellRiseInputRouterComponent::GetSpellRiseController() const
{
	return Cast<ASpellRisePlayerController>(GetOwner());
}

const UInputAction* USpellRiseInputRouterComponent::FindAction(const FName ActionName) const
{
	const UInputMappingContext* Contexts[] =
	{
		IMC_Global.Get(),
		IMC_Movement.Get(),
		IMC_Combat.Get(),
		IMC_Building.Get(),
		IMC_UI.Get()
	};

	const UInputAction* FoundAction = nullptr;
	for (const UInputMappingContext* Context : Contexts)
	{
		if (!Context)
		{
			continue;
		}

		for (const FEnhancedActionKeyMapping& Mapping : Context->GetMappings())
		{
			if (!MatchesActionName(Mapping.Action, ActionName))
			{
				continue;
			}

			if (FoundAction && FoundAction != Mapping.Action)
			{
				UE_LOG(
					LogSpellRiseInputRouter,
					Error,
					TEXT("[InputRouter][DuplicateActionName] Name=%s First=%s Duplicate=%s Context=%s"),
					*ActionName.ToString(),
					*GetNameSafe(FoundAction),
					*GetNameSafe(Mapping.Action),
					*GetNameSafe(Context));
				return nullptr;
			}

			FoundAction = Mapping.Action;
		}
	}

	return FoundAction;
}

bool USpellRiseInputRouterComponent::HasMappedAction(const FName ActionName) const
{
	return FindAction(ActionName) != nullptr;
}

void USpellRiseInputRouterComponent::InitializeLocalInput()
{
	RefreshContexts();
}

void USpellRiseInputRouterComponent::UpdateContext(
	UInputMappingContext* Context,
	const int32 Priority,
	const bool bShouldBeActive) const
{
	const ASpellRisePlayerController* Controller = GetSpellRiseController();
	ULocalPlayer* LocalPlayer = Controller ? Controller->GetLocalPlayer() : nullptr;
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
	if (!Subsystem || !Context)
	{
		return;
	}

	const bool bContextIsActive = Subsystem->HasMappingContext(Context);
	if (bShouldBeActive && !bContextIsActive)
	{
		Subsystem->AddMappingContext(Context, Priority);
	}
	else if (!bShouldBeActive && bContextIsActive)
	{
		Subsystem->RemoveMappingContext(Context);
	}
}

void USpellRiseInputRouterComponent::RefreshContexts()
{
	const ASpellRisePlayerController* Controller = GetSpellRiseController();
	if (!Controller || !Controller->IsLocalController())
	{
		return;
	}

	const bool bUIActive = Controller->ShouldEnableUIInputContext();
	const bool bDead = Controller->IsControlledCharacterDead();

	UpdateContext(IMC_Global, GlobalPriority, true);
	UpdateContext(IMC_Movement, MovementPriority, !bUIActive && !bDead);
	UpdateContext(IMC_Combat, CombatPriority, !bUIActive && !bDead && !bBuildingInputActive);
	UpdateContext(IMC_Building, BuildingPriority, !bUIActive && !bDead && bBuildingInputActive);
	UpdateContext(IMC_UI, UIPriority, bUIActive);
}

void USpellRiseInputRouterComponent::SetBuildingInputActive(const bool bActive)
{
	const ASpellRisePlayerController* Controller = GetSpellRiseController();
	if (!Controller || !Controller->IsLocalController() || bBuildingInputActive == bActive)
	{
		return;
	}

	bBuildingInputActive = bActive;
	RefreshContexts();
}

void USpellRiseInputRouterComponent::BindInput(UInputComponent* InputComponent)
{
	ASpellRisePlayerController* Controller = GetSpellRiseController();
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!Controller || !Controller->IsLocalController() || !EnhancedInput)
	{
		return;
	}

	auto BindPressRelease = [this, EnhancedInput, Controller](
		const FName ActionName,
		void (ASpellRisePlayerController::*Pressed)(),
		void (ASpellRisePlayerController::*Released)())
	{
		const UInputAction* Action = FindAction(ActionName);
		if (!Action)
		{
			return;
		}
		EnhancedInput->BindAction(Action, ETriggerEvent::Started, Controller, Pressed);
		EnhancedInput->BindAction(Action, ETriggerEvent::Completed, Controller, Released);
		EnhancedInput->BindAction(Action, ETriggerEvent::Canceled, Controller, Released);
	};

	BindPressRelease(TEXT("Primary"), &ASpellRisePlayerController::OnPrimaryPressed, &ASpellRisePlayerController::OnPrimaryReleased);
	BindPressRelease(TEXT("Secondary"), &ASpellRisePlayerController::OnSecondaryPressed, &ASpellRisePlayerController::OnSecondaryReleased);
	BindPressRelease(TEXT("Interact"), &ASpellRisePlayerController::OnInteractPressed, &ASpellRisePlayerController::OnInteractReleased);
	BindPressRelease(TEXT("Sprint"), &ASpellRisePlayerController::OnSprintPressed, &ASpellRisePlayerController::OnSprintReleased);
	BindPressRelease(TEXT("Ability1"), &ASpellRisePlayerController::OnAbility1Pressed, &ASpellRisePlayerController::OnAbility1Released);
	BindPressRelease(TEXT("Ability2"), &ASpellRisePlayerController::OnAbility2Pressed, &ASpellRisePlayerController::OnAbility2Released);
	BindPressRelease(TEXT("Ability3"), &ASpellRisePlayerController::OnAbility3Pressed, &ASpellRisePlayerController::OnAbility3Released);
	BindPressRelease(TEXT("Ability4"), &ASpellRisePlayerController::OnAbility4Pressed, &ASpellRisePlayerController::OnAbility4Released);
	BindPressRelease(TEXT("Ability5"), &ASpellRisePlayerController::OnAbility5Pressed, &ASpellRisePlayerController::OnAbility5Released);
	BindPressRelease(TEXT("Ability6"), &ASpellRisePlayerController::OnAbility6Pressed, &ASpellRisePlayerController::OnAbility6Released);
	BindPressRelease(TEXT("Ability7"), &ASpellRisePlayerController::OnAbility7Pressed, &ASpellRisePlayerController::OnAbility7Released);
	BindPressRelease(TEXT("Ability8"), &ASpellRisePlayerController::OnAbility8Pressed, &ASpellRisePlayerController::OnAbility8Released);

	if (const UInputAction* Action = FindAction(TEXT("ClearSelection")))
	{
		EnhancedInput->BindAction(
			Action,
			ETriggerEvent::Started,
			Controller,
			&ASpellRisePlayerController::OnClearSelectionPressed);
	}

	if (const UInputAction* Action = FindAction(TEXT("ToggleUIInteraction")))
	{
		EnhancedInput->BindAction(
			Action,
			ETriggerEvent::Started,
			Controller,
			&ASpellRisePlayerController::OnToggleUIInteractionPressed);
	}

#if !UE_BUILD_SHIPPING
	if (const UInputAction* Action = FindAction(TEXT("DebugGrantExperience")))
	{
		EnhancedInput->BindAction(
			Action,
			ETriggerEvent::Started,
			Controller,
			&ASpellRisePlayerController::OnDebugGrantExperiencePressed);
	}
#endif
}

FText USpellRiseInputRouterComponent::GetAbilitySlotInputText(const int32 PhysicalSlotIndex) const
{
	if (PhysicalSlotIndex < 0 || PhysicalSlotIndex >= 8)
	{
		return FText::AsNumber(PhysicalSlotIndex + 1);
	}

	const UInputAction* Action = FindAction(
		FName(*FString::Printf(TEXT("Ability%d"), PhysicalSlotIndex + 1)));
	if (!Action)
	{
		return FText::AsNumber(PhysicalSlotIndex + 1);
	}

	const UInputMappingContext* Contexts[] =
	{
		IMC_Global.Get(),
		IMC_Movement.Get(),
		IMC_Combat.Get(),
		IMC_Building.Get(),
		IMC_UI.Get()
	};
	for (const UInputMappingContext* Context : Contexts)
	{
		if (!Context)
		{
			continue;
		}

		for (const FEnhancedActionKeyMapping& Mapping : Context->GetMappings())
		{
			if (Mapping.Action == Action && Mapping.Key.IsValid())
			{
				return Mapping.Key.GetDisplayName(false);
			}
		}
	}

	return FText::AsNumber(PhysicalSlotIndex + 1);
}
