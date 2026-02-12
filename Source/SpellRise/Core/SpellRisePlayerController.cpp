#include "SpellRisePlayerController.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

ASpellRisePlayerController::ASpellRisePlayerController()
{
	// ✅ Construtor roda também no CDO (startup). Não use nada que dependa de World/ViewPort aqui.
	bShowMouseCursor = false;
}

void ASpellRisePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ✅ Agora sim é seguro (já existe World/Viewport no jogo)
	if (IsLocalController())
	{
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
	}

	SetupEnhancedInput();
}

void ASpellRisePlayerController::SetupEnhancedInput()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] PlayerController: DefaultMappingContext is null (ok por enquanto)."));
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext.Get(), DefaultMappingPriority);

	const UObject* Obj = DefaultMappingContext.Get();
	UE_LOG(LogTemp, Log, TEXT("[SpellRise] PlayerController: MappingContext aplicado (%s)"),
		*GetNameSafe(Obj));
}
