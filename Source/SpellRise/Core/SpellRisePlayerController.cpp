#include "SpellRisePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"

#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"

ASpellRisePlayerController::ASpellRisePlayerController()
{
	bShowMouseCursor = false;

	NumberPopComponent = CreateDefaultSubobject<USpellRiseNumberPopComponent_NiagaraText>(TEXT("NumberPopComponent"));
}

void ASpellRisePlayerController::BeginPlay()
{
	Super::BeginPlay();

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
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] PlayerController: DefaultMappingContext is null."));
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

	UE_LOG(LogTemp, Log, TEXT("[SpellRise] PlayerController: MappingContext aplicado (%s)"),
		*GetNameSafe(DefaultMappingContext.Get()));
}

void ASpellRisePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC)
	{
		UE_LOG(LogTemp, Error, TEXT("[SpellRise] PlayerController: InputComponent is not EnhancedInputComponent."));
		return;
	}

	if (!IA_Attack)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] PlayerController: IA_Attack is null (set it in BP)."));
		return;
	}

	EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAttackPressed);
	EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAttackReleased);
}

void ASpellRisePlayerController::OnAttackPressed()
{
	APawn* P = GetPawn();
	if (!P)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(P);
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] AttackPressed: ASC is null on Pawn=%s"), *GetNameSafe(P));
		return;
	}

	if (!AttackAbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] AttackPressed: AttackAbilityClass is null (set GA_Weapon_MeleeLight)."));
		return;
	}

	const bool bActivated = ASC->TryActivateAbilityByClass(AttackAbilityClass);

	UE_LOG(LogTemp, VeryVerbose, TEXT("[SpellRise] AttackPressed: TryActivate=%d Ability=%s"),
		bActivated ? 1 : 0, *GetNameSafe(AttackAbilityClass.Get()));
}

void ASpellRisePlayerController::OnAttackReleased()
{
	// optional
}

void ASpellRisePlayerController::ShowDamageNumber(
	float Damage,
	const FVector& WorldLocation,
	const FGameplayTagContainer& SourceTags,
	const FGameplayTagContainer& TargetTags,
	bool bIsCritical)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!NumberPopComponent)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[POP][PC] Damage=%.1f Loc=%s Local=%d Controller=%s"),
	Damage,
	*WorldLocation.ToString(),
	IsLocalController() ? 1 : 0,
	*GetNameSafe(this));

	FSpellRiseNumberPopRequest Request;
	Request.WorldLocation = WorldLocation;
	Request.SourceTags = SourceTags;
	Request.TargetTags = TargetTags;
	Request.NumberToDisplay = FMath::RoundToInt(Damage);
	Request.bIsCriticalDamage = bIsCritical;

	NumberPopComponent->AddNumberPop(Request);
}