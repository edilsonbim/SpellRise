#include "SpellRisePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"

namespace
{
	FVector ResolveTraceOrigin(const APlayerController* PlayerController)
	{
		if (!PlayerController)
		{
			return FVector::ZeroVector;
		}

		if (const APawn* ControlledPawn = PlayerController->GetPawn())
		{
			return ControlledPawn->GetPawnViewLocation();
		}

		if (const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			return CameraManager->GetCameraLocation();
		}

		return FVector::ZeroVector;
	}
}

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
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] PlayerController: IA_Attack is null."));
		return;
	}

	EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAttackPressed);
	EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAttackReleased);
	EIC->BindAction(IA_Attack, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAttackReleased);
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
		UE_LOG(LogTemp, Warning, TEXT("[SpellRise] AttackPressed: AttackAbilityClass is null."));
		return;
	}

	const bool bActivated = ASC->TryActivateAbilityByClass(AttackAbilityClass);

	UE_LOG(LogTemp, VeryVerbose, TEXT("[SpellRise] AttackPressed: TryActivate=%d Ability=%s"),
		bActivated ? 1 : 0,
		*GetNameSafe(AttackAbilityClass.Get()));
}

void ASpellRisePlayerController::OnAttackReleased()
{
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

bool ASpellRisePlayerController::GetCurrentAimTraceResult(FSpellRiseAimTraceResult& OutResult) const
{
	return BuildAimTraceResult(
		OutResult,
		DefaultAimTraceDistance,
		DefaultAimTraceRadius,
		DefaultAimTraceChannel,
		bDefaultAimTraceComplex);
}

bool ASpellRisePlayerController::BuildAimTraceResult(
	FSpellRiseAimTraceResult& OutResult,
	float TraceDistance,
	float TraceRadius,
	ECollisionChannel TraceChannel,
	bool bTraceComplex) const
{
	OutResult = FSpellRiseAimTraceResult();

	if (!IsLocalController())
	{
		return false;
	}

	if (!PlayerCameraManager)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	const FVector CameraDirection = CameraRotation.Vector().GetSafeNormal();

	if (CameraDirection.IsNearlyZero())
	{
		return false;
	}

	const float SafeTraceDistance = FMath::Max(1.f, TraceDistance);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpellRise_CurrentAimTrace), bTraceComplex);
	QueryParams.bReturnPhysicalMaterial = false;

	TArray<AActor*> ActorsToIgnore;
	CollectAimIgnoredActors(ActorsToIgnore);

	for (AActor* IgnoredActor : ActorsToIgnore)
	{
		if (IsValid(IgnoredActor))
		{
			QueryParams.AddIgnoredActor(IgnoredActor);
		}
	}

	// Stage 1: camera trace to acquire the desired aim point.
	const FVector CameraTraceStart = CameraLocation;
	const FVector CameraTraceEnd = CameraTraceStart + (CameraDirection * SafeTraceDistance);

	FHitResult CameraHitResult;
	const bool bCameraHit = World->LineTraceSingleByChannel(
		CameraHitResult,
		CameraTraceStart,
		CameraTraceEnd,
		TraceChannel,
		QueryParams);

	const FVector DesiredTargetPoint = bCameraHit ? CameraHitResult.ImpactPoint : CameraTraceEnd;

	// Stage 2: validate from the controlled pawn view location.
	// This avoids camera/muzzle parallax and prevents aiming through near walls.
	const FVector SourceTraceStart = ResolveTraceOrigin(this);
	if (SourceTraceStart.IsNearlyZero())
	{
		return false;
	}

	FVector SourceToTarget = DesiredTargetPoint - SourceTraceStart;
	if (SourceToTarget.IsNearlyZero())
	{
		SourceToTarget = CameraDirection * SafeTraceDistance;
	}

	const FVector SourceDirection = SourceToTarget.GetSafeNormal();
	if (SourceDirection.IsNearlyZero())
	{
		return false;
	}

	const FVector SourceTraceEnd = SourceTraceStart + (SourceDirection * SafeTraceDistance);

	FHitResult SourceHitResult;
	const bool bSourceHit = World->LineTraceSingleByChannel(
		SourceHitResult,
		SourceTraceStart,
		SourceTraceEnd,
		TraceChannel,
		QueryParams);

	OutResult.bHit = bSourceHit;
	OutResult.HitResult = SourceHitResult;
	OutResult.TraceStart = SourceTraceStart;
	OutResult.TraceEnd = SourceTraceEnd;
	OutResult.AimDirection = SourceDirection;
	OutResult.TargetPoint = bSourceHit ? SourceHitResult.ImpactPoint : DesiredTargetPoint;

	// Mantém compatibilidade com quem usa HitResult.TraceStart/TraceEnd.
	if (!bSourceHit)
	{
		OutResult.HitResult = FHitResult();
	}

	OutResult.HitResult.TraceStart = SourceTraceStart;
	OutResult.HitResult.TraceEnd = SourceTraceEnd;

	return true;
}

void ASpellRisePlayerController::CollectAimIgnoredActors(TArray<AActor*>& OutActorsToIgnore) const
{
	if (APawn* ControlledPawn = GetPawn())
	{
		OutActorsToIgnore.AddUnique(ControlledPawn);

		TArray<AActor*> AttachedActors;
		ControlledPawn->GetAttachedActors(AttachedActors);

		for (AActor* AttachedActor : AttachedActors)
		{
			if (IsValid(AttachedActor))
			{
				OutActorsToIgnore.AddUnique(AttachedActor);
			}
		}
	}

	if (AActor* ViewTargetActor = GetViewTarget())
	{
		OutActorsToIgnore.AddUnique(ViewTargetActor);
	}
}