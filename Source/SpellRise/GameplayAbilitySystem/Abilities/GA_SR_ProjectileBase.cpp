#include "GA_SR_ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseProjectileSecurity, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseProjectileAbility, Log, All);

namespace
{
	struct FProjectileAbilityRuntimeCounters
	{
		int64 Activations = 0;
		int64 CommitAttempts = 0;
		int64 CommitFailures = 0;
		int64 ServerSpawns = 0;
	};

	FProjectileAbilityRuntimeCounters GProjectileAbilityRuntimeCounters;

	const USkeletalMeshComponent* ResolveVisualMesh(const AActor* AvatarActor)
	{
		if (const ASpellRiseCharacterBase* SpellRiseCharacter = Cast<ASpellRiseCharacterBase>(AvatarActor))
		{
			if (const USkeletalMeshComponent* VisualMesh = SpellRiseCharacter->GetVisualMeshComponent())
			{
				return VisualMesh;
			}
		}

		if (const ACharacter* Character = Cast<ACharacter>(AvatarActor))
		{
			return Character->GetMesh();
		}

		return nullptr;
	}

	const USkeletalMeshComponent* ResolveProjectileSpawnMesh(const AActor* AvatarActor)
	{
		if (const ASpellRiseCharacterBase* SpellRiseCharacter = Cast<ASpellRiseCharacterBase>(AvatarActor))
		{
			if (const USkeletalMeshComponent* AttachMesh = SpellRiseCharacter->GetEquipmentAttachMeshComponent())
			{
				return AttachMesh;
			}

			if (const USkeletalMeshComponent* VisualMesh = SpellRiseCharacter->GetVisualMeshComponent())
			{
				return VisualMesh;
			}
		}

		if (const ACharacter* Character = Cast<ACharacter>(AvatarActor))
		{
			return Character->GetMesh();
		}

		return nullptr;
	}

	bool ResolveSpawnPointFromAttachedActors(const AActor* AvatarActor, const FName SpawnPointName, FVector& OutLocation)
	{
		if (!AvatarActor || SpawnPointName.IsNone())
		{
			return false;
		}

		TArray<AActor*> AttachedActors;
		AvatarActor->GetAttachedActors(AttachedActors);

		for (AActor* AttachedActor : AttachedActors)
		{
			if (!IsValid(AttachedActor))
			{
				continue;
			}

			TArray<USceneComponent*> SceneComponents;
			AttachedActor->GetComponents<USceneComponent>(SceneComponents);

			for (USceneComponent* SceneComp : SceneComponents)
			{
				if (!IsValid(SceneComp))
				{
					continue;
				}

				if (SceneComp->GetFName() == SpawnPointName)
				{
					OutLocation = SceneComp->GetComponentLocation();
					return true;
				}
			}

			TArray<USkeletalMeshComponent*> MeshComponents;
			AttachedActor->GetComponents<USkeletalMeshComponent>(MeshComponents);

			for (USkeletalMeshComponent* MeshComp : MeshComponents)
			{
				if (!IsValid(MeshComp))
				{
					continue;
				}

				if (MeshComp->DoesSocketExist(SpawnPointName))
				{
					OutLocation = MeshComp->GetSocketLocation(SpawnPointName);
					return true;
				}
			}
		}

		return false;
	}

	const ASpellRisePlayerController* ResolveSpellRisePlayerController(const FGameplayAbilityActorInfo* ActorInfo)
	{
		if (!ActorInfo)
		{
			return nullptr;
		}

		if (const ASpellRisePlayerController* PlayerController = Cast<ASpellRisePlayerController>(ActorInfo->PlayerController.Get()))
		{
			return PlayerController;
		}

		const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
		if (!AvatarPawn)
		{
			return nullptr;
		}

		return Cast<ASpellRisePlayerController>(AvatarPawn->GetController());
	}
}

UGA_SR_ProjectileBase::UGA_SR_ProjectileBase()
{
	ActivationPolicy = ESpellRiseAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = ESpellRiseAbilityActivationGroup::Exclusive_Replaceable;

	static const FGameplayTag CastingTag = FGameplayTag::RequestGameplayTag(TEXT("State.Casting"), false);
	if (CastingTag.IsValid())
	{
		ActivationOwnedTags.AddTag(CastingTag);
	}
}

void UGA_SR_ProjectileBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	++GProjectileAbilityRuntimeCounters.Activations;

	bCastFinished = false;
	bInputReleasedBeforeFinish = false;
	InputHeldTime = 0.f;
	bProjectileFired = false;
	bHasCachedClientTargetData = false;
	CachedClientTargetLocation = FVector::ZeroVector;
	CachedClientAimHitResult = FHitResult();

	if (HasAuthority(&ActivationInfo) && ActorInfo)
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			const FPredictionKey PredictionKey = ActivationInfo.GetActivationPredictionKey();

			ASC->AbilityTargetDataSetDelegate(Handle, PredictionKey).RemoveAll(this);
			ASC->AbilityTargetDataCancelledDelegate(Handle, PredictionKey).RemoveAll(this);

			ASC->AbilityTargetDataSetDelegate(Handle, PredictionKey).AddUObject(this, &UGA_SR_ProjectileBase::OnReplicatedTargetDataReceived);
			ASC->AbilityTargetDataCancelledDelegate(Handle, PredictionKey).AddUObject(this, &UGA_SR_ProjectileBase::OnReplicatedTargetDataCancelled);

			ASC->CallReplicatedTargetDataDelegatesIfSet(Handle, PredictionKey);
		}
	}

	UE_LOG(
		LogSpellRiseProjectileAbility,
		Verbose,
		TEXT("[COMBAT][ABILITY_ACTIVATE] Ability=%s ActivationCount=%lld IsAuthority=%d"),
		*GetNameSafe(this),
		GProjectileAbilityRuntimeCounters.Activations,
		HasAuthority(&ActivationInfo) ? 1 : 0);

	StartCast();
}

void UGA_SR_ProjectileBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ClearRuntimeTasks();

	if (UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr)
	{
		const FPredictionKey PredictionKey = ActivationInfo.GetActivationPredictionKey();
		ASC->AbilityTargetDataSetDelegate(Handle, PredictionKey).RemoveAll(this);
		ASC->AbilityTargetDataCancelledDelegate(Handle, PredictionKey).RemoveAll(this);
	}

	if (bWasCancelled && !bProjectileFired)
	{
		OnProjectileCastCancelled();
	}

	bCastFinished = false;
	bInputReleasedBeforeFinish = false;
	InputHeldTime = 0.f;
	bProjectileFired = false;
	bHasCachedClientTargetData = false;
	CachedClientTargetLocation = FVector::ZeroVector;
	CachedClientAimHitResult = FHitResult();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SR_ProjectileBase::NativeOnAbilityInputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::NativeOnAbilityInputReleased(Handle, ActorInfo, ActivationInfo);

	if (!IsActive() || bProjectileFired)
	{
		return;
	}

	if (bCastFinished)
	{
		FireProjectile();
		return;
	}

	bInputReleasedBeforeFinish = true;

	if (bCancelIfReleasedEarly)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGA_SR_ProjectileBase::OnProjectileCastStarted_Implementation()
{
	PlayMontageIfValid(CastMontage, CastMontagePlayRate);
}

void UGA_SR_ProjectileBase::OnProjectileCastCompleted_Implementation()
{
}

void UGA_SR_ProjectileBase::OnProjectileCastCancelled_Implementation()
{
	StopMontageIfValid(CastMontage);
	StopMontageIfValid(FireMontage);
}

void UGA_SR_ProjectileBase::OnProjectileFired_Implementation(float ChargeAlpha)
{
	StopMontageIfValid(CastMontage);
	PlayMontageIfValid(FireMontage, FireMontagePlayRate);
}

bool UGA_SR_ProjectileBase::CanFireProjectile_Implementation() const
{
	return ProjectileClass != nullptr;
}

void UGA_SR_ProjectileBase::StartCast()
{
	if (!IsActive())
	{
		return;
	}

	OnProjectileCastStarted();

	if (bCommitOnCastStart)
	{
		++GProjectileAbilityRuntimeCounters.CommitAttempts;
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			++GProjectileAbilityRuntimeCounters.CommitFailures;
			UE_LOG(
				LogSpellRiseProjectileAbility,
				Warning,
				TEXT("[COMBAT][ABILITY_COMMIT_DENIED] Ability=%s Stage=CastStart Attempts=%lld Failures=%lld"),
				*GetNameSafe(this),
				GProjectileAbilityRuntimeCounters.CommitAttempts,
				GProjectileAbilityRuntimeCounters.CommitFailures);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		UE_LOG(
			LogSpellRiseProjectileAbility,
			Verbose,
			TEXT("[COMBAT][ABILITY_COMMIT_OK] Ability=%s Stage=CastStart Attempts=%lld"),
			*GetNameSafe(this),
			GProjectileAbilityRuntimeCounters.CommitAttempts);
	}

	if (!HasAuthority(&CurrentActivationInfo))
	{
		SendCurrentAimTargetDataToServer();
	}

	ClearRuntimeTasks();

	WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (WaitInputReleaseTask)
	{
		WaitInputReleaseTask->OnRelease.AddDynamic(this, &UGA_SR_ProjectileBase::OnReleaseBeforeCastFinished);
		WaitInputReleaseTask->ReadyForActivation();
	}

	if (CastTime <= 0.f)
	{
		FinishCast();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CastTimerHandle, this, &UGA_SR_ProjectileBase::FinishCast, CastTime, false);
	}
}

void UGA_SR_ProjectileBase::FinishCast()
{
	if (!IsActive() || bProjectileFired)
	{
		return;
	}

	bCastFinished = true;
	OnProjectileCastCompleted();

	// Click-cast:
	// se não cancela ao soltar cedo, terminou o cast -> dispara automaticamente.
	if (!bCancelIfReleasedEarly)
	{
		FireProjectile();
		return;
	}

	// Fluxo hold/release:
	// se o input já foi solto e a ability ainda não cancelou, dispara agora.
	if (bInputReleasedBeforeFinish)
	{
		FireProjectile();
	}
}

void UGA_SR_ProjectileBase::FireProjectile()
{
	if (!IsActive() || bProjectileFired)
	{
		return;
	}

	if (!CanFireProjectile())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	const bool bRequiresClientTargetData =
		HasAuthority(&CurrentActivationInfo) &&
		CurrentActorInfo &&
		!CurrentActorInfo->IsLocallyControlled() &&
		CurrentActorInfo->PlayerController.IsValid() &&
		bUseControllerViewForTargeting;

	// SERVER:
	// se depende do aim do client, espera os dados antes de commitar e antes de spawnar.
	if (bRequiresClientTargetData && !bHasCachedClientTargetData)
	{
		UE_LOG(LogSpellRiseProjectileAbility, Verbose, TEXT("[GA_SR_ProjectileBase] Waiting for replicated target data on %s"), *GetNameSafe(this));
		return;
	}

	if (!bCommitOnCastStart)
	{
		++GProjectileAbilityRuntimeCounters.CommitAttempts;
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			++GProjectileAbilityRuntimeCounters.CommitFailures;
			UE_LOG(
				LogSpellRiseProjectileAbility,
				Warning,
				TEXT("[COMBAT][ABILITY_COMMIT_DENIED] Ability=%s Stage=Fire Attempts=%lld Failures=%lld"),
				*GetNameSafe(this),
				GProjectileAbilityRuntimeCounters.CommitAttempts,
				GProjectileAbilityRuntimeCounters.CommitFailures);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		UE_LOG(
			LogSpellRiseProjectileAbility,
			Verbose,
			TEXT("[COMBAT][ABILITY_COMMIT_OK] Ability=%s Stage=Fire Attempts=%lld"),
			*GetNameSafe(this),
			GProjectileAbilityRuntimeCounters.CommitAttempts);
	}

	// CLIENT:
	// previsão visual/local. O spawn real continua no server.
	if (!HasAuthority(&CurrentActivationInfo))
	{
		const float ChargeAlpha = GetChargeAlpha();

		SendCurrentAimTargetDataToServer();

		bProjectileFired = true;
		ClearRuntimeTasks();
		OnProjectileFired(ChargeAlpha);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (!ProjectileClass)
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[GA_SR_ProjectileBase] ProjectileClass is null on %s"), *GetNameSafe(this));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FVector TargetLocation = FVector::ZeroVector;
	if (!GetProjectileTargetLocation(TargetLocation))
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[GA_SR_ProjectileBase] Failed to resolve target location on %s"), *GetNameSafe(this));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const float ChargeAlpha = GetChargeAlpha();
	bProjectileFired = true;
	ClearRuntimeTasks();
	OnProjectileFired(ChargeAlpha);

	const FVector SpawnLocation = GetProjectileSpawnLocation();
	const FRotator SpawnRotation = GetProjectileSpawnRotation(TargetLocation);
	FGameplayEffectSpecHandle DamageSpec = MakeProjectileDamageSpec();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	SpawnParams.Instigator = GetAvatarPawn();
	SpawnParams.SpawnCollisionHandlingOverride = SpawnCollisionHandling;

	UWorld* World = GetWorld();
	if (!World)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AActor* SpawnedProjectile = World->SpawnActorDeferred<AActor>(
		ProjectileClass,
		FTransform(SpawnRotation, SpawnLocation),
		SpawnParams.Owner,
		SpawnParams.Instigator,
		SpawnCollisionHandling
	);

	if (!SpawnedProjectile)
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[GA_SR_ProjectileBase] SpawnActorDeferred failed for %s"), *GetNameSafe(ProjectileClass));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	SpawnedProjectile->SetReplicates(true);
	SpawnedProjectile->SetReplicateMovement(true);

	if (UProjectileMovementComponent* ProjectileMoveComp = SpawnedProjectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		ProjectileMoveComp->InitialSpeed = ProjectileSpeed;
		ProjectileMoveComp->MaxSpeed = ProjectileSpeed;
		ProjectileMoveComp->SetIsReplicated(true);
	}

	OnProjectileSpawned(SpawnedProjectile, DamageSpec, TargetLocation);
	UGameplayStatics::FinishSpawningActor(SpawnedProjectile, FTransform(SpawnRotation, SpawnLocation));
	++GProjectileAbilityRuntimeCounters.ServerSpawns;
	UE_LOG(
		LogSpellRiseProjectileAbility,
		Verbose,
		TEXT("[COMBAT][PROJECTILE_SPAWN] Ability=%s Projectile=%s SpawnCount=%lld Target=%s"),
		*GetNameSafe(this),
		*GetNameSafe(SpawnedProjectile),
		GProjectileAbilityRuntimeCounters.ServerSpawns,
		*TargetLocation.ToCompactString());

	ConsumeClientTargetData();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_SR_ProjectileBase::ClearRuntimeTasks()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CastTimerHandle);
	}

	if (WaitInputReleaseTask)
	{
		WaitInputReleaseTask->EndTask();
		WaitInputReleaseTask = nullptr;
	}
}

void UGA_SR_ProjectileBase::OnReleaseBeforeCastFinished(float HeldTime)
{
	InputHeldTime = HeldTime;

	if (!IsActive() || bProjectileFired)
	{
		return;
	}

	if (bCastFinished)
	{
		FireProjectile();
		return;
	}

	bInputReleasedBeforeFinish = true;

	if (bCancelIfReleasedEarly)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

float UGA_SR_ProjectileBase::GetChargeAlpha() const
{
	if (CastTime <= 0.f || bCastFinished)
	{
		return 1.f;
	}

	return FMath::Clamp(InputHeldTime / CastTime, 0.f, 1.f);
}

void UGA_SR_ProjectileBase::OnReplicatedTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	bHasCachedClientTargetData = false;
	CachedClientTargetLocation = FVector::ZeroVector;
	CachedClientAimHitResult = FHitResult();

	for (int32 Index = 0; Index < DataHandle.Data.Num(); ++Index)
	{
		const TSharedPtr<FGameplayAbilityTargetData>& TargetData = DataHandle.Data[Index];
		if (!TargetData.IsValid())
		{
			continue;
		}

		const FHitResult* HitResult = TargetData->GetHitResult();
		if (!HitResult)
		{
			continue;
		}

		FVector ValidatedTargetLocation = HitResult->bBlockingHit ? HitResult->ImpactPoint : HitResult->TraceEnd;
		if (HasAuthority(&CurrentActivationInfo) && bValidateReplicatedTargetDataOnServer)
		{
			FString RejectReason;
			if (!ValidateReplicatedTargetDataOnServer(*HitResult, ValidatedTargetLocation, RejectReason))
			{
				UE_LOG(
					LogSpellRiseProjectileSecurity,
					Warning,
					TEXT("[SEC][Projectile] Rejected replicated target data. Ability=%s Avatar=%s Reason=%s"),
					*GetNameSafe(this),
					*GetNameSafe(GetAvatarActorFromActorInfo()),
					*RejectReason);
				continue;
			}
		}

		CachedClientAimHitResult = *HitResult;
		CachedClientTargetLocation = ValidatedTargetLocation;
		bHasCachedClientTargetData = true;
		break;
	}

	ConsumeClientTargetData();

	if (!bHasCachedClientTargetData)
	{
		UE_LOG(LogSpellRiseProjectileSecurity, Warning, TEXT("[SEC][Projectile] No valid target extracted from replicated target data on %s"), *GetNameSafe(this));
		if (HasAuthority(&CurrentActivationInfo) && IsActive() && !bProjectileFired)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		}
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) && IsActive() && bCastFinished && !bProjectileFired)
	{
		FireProjectile();
	}
}

void UGA_SR_ProjectileBase::OnReplicatedTargetDataCancelled()
{
	if (IsActive() && !bProjectileFired)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UGA_SR_ProjectileBase::SendCurrentAimTargetDataToServer()
{
	if (!CurrentActorInfo)
	{
		return;
	}

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	if (!BuildClientTargetDataHandle(TargetDataHandle))
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(ASC);

	ASC->ServerSetReplicatedTargetData(
		CurrentSpecHandle,
		CurrentActivationInfo.GetActivationPredictionKey(),
		TargetDataHandle,
		FGameplayTag(),
		ASC->ScopedPredictionKey);
}

void UGA_SR_ProjectileBase::ConsumeClientTargetData()
{
	if (!CurrentActorInfo)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
	{
		ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	}
}

bool UGA_SR_ProjectileBase::BuildClientTargetDataHandle(FGameplayAbilityTargetDataHandle& OutTargetDataHandle) const
{
	if (!CurrentActorInfo)
	{
		return false;
	}

	const ASpellRisePlayerController* SRPC = ResolveSpellRisePlayerController(CurrentActorInfo);
	if (!SRPC)
	{
		return false;
	}

	FSpellRiseAimTraceResult AimTraceResult;
	if (!SRPC->GetCurrentAimTraceResult(AimTraceResult))
	{
		return false;
	}

	FHitResult AbilityHitResult = AimTraceResult.HitResult;
	if (!AimTraceResult.bHit)
	{
		AbilityHitResult = FHitResult();
		AbilityHitResult.TraceStart = AimTraceResult.TraceStart;
		AbilityHitResult.TraceEnd = AimTraceResult.TraceEnd;
		AbilityHitResult.Location = AimTraceResult.TargetPoint;
		AbilityHitResult.ImpactPoint = AimTraceResult.TargetPoint;
	}

	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(AbilityHitResult);
	OutTargetDataHandle.Add(TargetData);

	return true;
}

bool UGA_SR_ProjectileBase::GetProjectileTargetLocation(FVector& OutTargetLocation) const
{
	if (bHasCachedClientTargetData)
	{
		OutTargetLocation = CachedClientTargetLocation;
		return true;
	}

	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UWorld* World = GetWorld();

	if (!AvatarActor || !World)
	{
		return false;
	}

	if (bUseControllerViewForTargeting)
	{
		if (const ASpellRisePlayerController* SRPC = ResolveSpellRisePlayerController(CurrentActorInfo))
		{
			FSpellRiseAimTraceResult AimTraceResult;
			if (SRPC->GetCurrentAimTraceResult(AimTraceResult))
			{
				OutTargetLocation = AimTraceResult.TargetPoint;
				return true;
			}
		}
	}

	FVector TraceStart = AvatarActor->GetActorLocation();
	FVector TraceDirection = AvatarActor->GetActorForwardVector();

	if (const FGameplayAbilityActorInfo* ActorInfo = CurrentActorInfo)
	{
		if (AController* Controller = ActorInfo->PlayerController.Get())
		{
			FRotator ViewRotation = FRotator::ZeroRotator;
			Controller->GetPlayerViewPoint(TraceStart, ViewRotation);
			TraceDirection = ViewRotation.Vector();
		}
		else if (const APawn* AvatarPawn = Cast<APawn>(AvatarActor))
		{
			if (AController* ControllerFromPawn = AvatarPawn->GetController())
			{
				FRotator ViewRotation = FRotator::ZeroRotator;
				ControllerFromPawn->GetPlayerViewPoint(TraceStart, ViewRotation);
				TraceDirection = ViewRotation.Vector();
			}
			else
			{
				FVector EyeLocation = FVector::ZeroVector;
				FRotator EyeRotation = FRotator::ZeroRotator;
				AvatarPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
				TraceStart = EyeLocation;
				TraceDirection = EyeRotation.Vector();
			}
		}
	}

	TraceDirection = TraceDirection.GetSafeNormal();
	if (TraceDirection.IsNearlyZero())
	{
		return false;
	}

	const FVector TraceEnd = TraceStart + (TraceDirection * TargetTraceRange);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(GA_SR_ProjectileBase_TargetTrace), bTraceComplex);
	QueryParams.AddIgnoredActor(AvatarActor);

	TArray<AActor*> AttachedActors;
	AvatarActor->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (IsValid(AttachedActor))
		{
			QueryParams.AddIgnoredActor(AttachedActor);
		}
	}

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TargetTraceChannel,
		QueryParams);

	OutTargetLocation = bHit ? HitResult.ImpactPoint : TraceEnd;
	return true;
}

FVector UGA_SR_ProjectileBase::GetProjectileSpawnLocation() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return FVector::ZeroVector;
	}

	FVector WeaponSpawnLocation = FVector::ZeroVector;
	if (ResolveSpawnPointFromAttachedActors(AvatarActor, ProjectileSpawnSocketName, WeaponSpawnLocation))
	{
		return WeaponSpawnLocation;
	}

	if (const USkeletalMeshComponent* MeshComp = ResolveProjectileSpawnMesh(AvatarActor))
	{
		if (ProjectileSpawnSocketName != NAME_None && MeshComp->DoesSocketExist(ProjectileSpawnSocketName))
		{
			return MeshComp->GetSocketLocation(ProjectileSpawnSocketName);
		}
	}

	if (const ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		const FRotator AimRotation = Character->GetBaseAimRotation();
		return Character->GetActorLocation() + (AimRotation.Vector() * 100.f) + FVector(0.f, 0.f, 50.f);
	}

	return AvatarActor->GetActorLocation() + (AvatarActor->GetActorForwardVector() * 100.f);
}

FRotator UGA_SR_ProjectileBase::GetProjectileSpawnRotation(const FVector& TargetLocation) const
{
	const FVector SpawnLocation = GetProjectileSpawnLocation();
	return (TargetLocation - SpawnLocation).GetSafeNormal().Rotation();
}

FGameplayEffectSpecHandle UGA_SR_ProjectileBase::MakeProjectileDamageSpec() const
{
	if (!DamageEffectClass)
	{
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectContextHandle EffectContext = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
	EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return FGameplayEffectSpecHandle();
	}

	SpecHandle.Data->SetContext(EffectContext);

	if (DamageMagnitudeSetByCallerTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DamageMagnitudeSetByCallerTag, DamageMagnitude);
	}

	if (DamageTypeTags.Num() > 0)
	{
		SpecHandle.Data->AppendDynamicAssetTags(DamageTypeTags);
	}

	return SpecHandle;
}

void UGA_SR_ProjectileBase::OnProjectileSpawned_Implementation(AActor* SpawnedProjectile, const FGameplayEffectSpecHandle& DamageSpec, const FVector& TargetLocation)
{
	if (ASR_ProjectileBase* SpellRiseProjectile = Cast<ASR_ProjectileBase>(SpawnedProjectile))
	{
		SpellRiseProjectile->InitializeProjectile(TargetLocation, ProjectileSpeed, DamageSpec);
	}
}

void UGA_SR_ProjectileBase::PlayMontageIfValid(UAnimMontage* MontageToPlay, float PlayRate) const
{
	if (!MontageToPlay)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		const USkeletalMeshComponent* VisualMesh = ResolveVisualMesh(Character);
		if (UAnimInstance* AnimInstance = VisualMesh ? VisualMesh->GetAnimInstance() : nullptr)
		{
			AnimInstance->Montage_Play(MontageToPlay, PlayRate);
		}
	}
}

void UGA_SR_ProjectileBase::StopMontageIfValid(UAnimMontage* MontageToStop) const
{
	if (!MontageToStop)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		const USkeletalMeshComponent* VisualMesh = ResolveVisualMesh(Character);
		if (UAnimInstance* AnimInstance = VisualMesh ? VisualMesh->GetAnimInstance() : nullptr)
		{
			AnimInstance->Montage_Stop(0.2f, MontageToStop);
		}
	}
}

APawn* UGA_SR_ProjectileBase::GetAvatarPawn() const
{
	return Cast<APawn>(GetAvatarActorFromActorInfo());
}

bool UGA_SR_ProjectileBase::ValidateReplicatedTargetDataOnServer(const FHitResult& ClientHitResult, FVector& OutTargetLocation, FString& OutRejectReason) const
{
	const auto IsFiniteVector = [](const FVector& V) -> bool
	{
		return FMath::IsFinite(V.X) && FMath::IsFinite(V.Y) && FMath::IsFinite(V.Z);
	};

	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		OutRejectReason = TEXT("missing_avatar");
		return false;
	}

	const FVector RawTargetLocation = ClientHitResult.bBlockingHit ? ClientHitResult.ImpactPoint : ClientHitResult.TraceEnd;
	if (!IsFiniteVector(RawTargetLocation))
	{
		OutRejectReason = TEXT("target_location_non_finite");
		return false;
	}

	const FVector SpawnLocation = GetProjectileSpawnLocation();
	const float DistanceToTarget = FVector::Distance(SpawnLocation, RawTargetLocation);
	const float MaxDistance = FMath::Max(100.f, MaxReplicatedTargetDataDistance);
	if (DistanceToTarget > MaxDistance)
	{
		OutRejectReason = FString::Printf(TEXT("target_out_of_range(%.1f>%.1f)"), DistanceToTarget, MaxDistance);
		return false;
	}

	FVector ExpectedViewLocation = AvatarActor->GetActorLocation();
	FVector ExpectedForward = AvatarActor->GetActorForwardVector().GetSafeNormal();
	if (const APawn* AvatarPawn = Cast<APawn>(AvatarActor))
	{
		FVector EyeLocation = FVector::ZeroVector;
		FRotator EyeRotation = FRotator::ZeroRotator;
		AvatarPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		if (IsFiniteVector(EyeLocation))
		{
			ExpectedViewLocation = EyeLocation;
		}
		ExpectedForward = EyeRotation.Vector().GetSafeNormal();
	}

	if (const FGameplayAbilityActorInfo* ActorInfo = CurrentActorInfo)
	{
		if (AController* Controller = ActorInfo->PlayerController.Get())
		{
			FVector ViewLocation = FVector::ZeroVector;
			FRotator ViewRotation = FRotator::ZeroRotator;
			Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
			if (IsFiniteVector(ViewLocation))
			{
				ExpectedViewLocation = ViewLocation;
			}
			ExpectedForward = ViewRotation.Vector().GetSafeNormal();
		}
	}

	const FVector TraceStart = ClientHitResult.TraceStart;
	if (IsFiniteVector(TraceStart) && !TraceStart.IsNearlyZero())
	{
		const float MaxStartOffset = FMath::Max(0.f, MaxReplicatedTargetDataStartOffset);
		const float StartOffset = FVector::Distance(TraceStart, ExpectedViewLocation);
		if (StartOffset > MaxStartOffset)
		{
			OutRejectReason = FString::Printf(TEXT("trace_start_mismatch(%.1f>%.1f)"), StartOffset, MaxStartOffset);
			return false;
		}
	}

	const FVector ToTargetDir = (RawTargetLocation - SpawnLocation).GetSafeNormal();
	if (ToTargetDir.IsNearlyZero())
	{
		OutRejectReason = TEXT("invalid_target_direction");
		return false;
	}

	if (ExpectedForward.IsNearlyZero())
	{
		ExpectedForward = AvatarActor->GetActorForwardVector().GetSafeNormal();
	}

	const float MaxHalfAngleDeg = FMath::Clamp(MaxReplicatedTargetDataConeHalfAngleDegrees, 1.f, 89.f);
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(MaxHalfAngleDeg));
	const float Dot = FVector::DotProduct(ExpectedForward, ToTargetDir);
	if (Dot < MinDot)
	{
		OutRejectReason = FString::Printf(TEXT("outside_cone(dot=%.3f min=%.3f)"), Dot, MinDot);
		return false;
	}

	OutTargetLocation = RawTargetLocation;
	return true;
}

