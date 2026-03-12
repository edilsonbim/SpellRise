#include "GA_SR_ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

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

	bCastFinished = false;
	bInputReleasedBeforeFinish = false;
	InputHeldTime = 0.f;
	bProjectileFired = false;

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

	if (bWasCancelled && !bProjectileFired)
	{
		OnProjectileCastCancelled();
	}

	bCastFinished = false;
	bInputReleasedBeforeFinish = false;
	InputHeldTime = 0.f;
	bProjectileFired = false;

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
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
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

	if (!bCommitOnCastStart)
	{
		if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}
	}

	bProjectileFired = true;
	ClearRuntimeTasks();

	const float ChargeAlpha = GetChargeAlpha();
	OnProjectileFired(ChargeAlpha);

	if (!HasAuthority(&CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_SR_ProjectileBase] ProjectileClass is null on %s"), *GetNameSafe(this));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FVector TargetLocation = FVector::ZeroVector;
	if (!GetProjectileTargetLocation(TargetLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_SR_ProjectileBase] Failed to resolve target location on %s"), *GetNameSafe(this));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

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
		UE_LOG(LogTemp, Warning, TEXT("[GA_SR_ProjectileBase] SpawnActorDeferred failed for %s"), *GetNameSafe(ProjectileClass));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (UProjectileMovementComponent* ProjectileMoveComp = SpawnedProjectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		ProjectileMoveComp->InitialSpeed = ProjectileSpeed;
		ProjectileMoveComp->MaxSpeed = ProjectileSpeed;
	}

	OnProjectileSpawned(SpawnedProjectile, DamageSpec, TargetLocation);
	UGameplayStatics::FinishSpawningActor(SpawnedProjectile, FTransform(SpawnRotation, SpawnLocation));

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
	bInputReleasedBeforeFinish = true;

	if (bCastFinished)
	{
		FireProjectile();
	}
	else if (bCancelIfReleasedEarly)
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

bool UGA_SR_ProjectileBase::GetProjectileTargetLocation(FVector& OutTargetLocation) const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return false;
	}

	FVector TraceStart = AvatarActor->GetActorLocation();
	FVector TraceDirection = AvatarActor->GetActorForwardVector();

	if (bUseControllerViewForTargeting)
	{
		if (const FGameplayAbilityActorInfo* ActorInfo = CurrentActorInfo)
		{
			if (AController* Controller = ActorInfo->PlayerController.Get())
			{
				FRotator ViewRotation;
				Controller->GetPlayerViewPoint(TraceStart, ViewRotation);
				TraceDirection = ViewRotation.Vector();
			}
			else if (const APawn* AvatarPawn = Cast<APawn>(AvatarActor))
			{
				if (AController* ControllerFromPawn = AvatarPawn->GetController())
				{
					FRotator ViewRotation;
					ControllerFromPawn->GetPlayerViewPoint(TraceStart, ViewRotation);
					TraceDirection = ViewRotation.Vector();
				}
			}
		}
	}

	const FVector TraceEnd = TraceStart + (TraceDirection * TargetTraceRange);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(GA_SR_ProjectileBase_TargetTrace), bTraceComplex);
	QueryParams.AddIgnoredActor(AvatarActor);

	FHitResult HitResult;
	const bool bHit = GetWorld() && GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TargetTraceChannel,
		QueryParams
	);

	OutTargetLocation = bHit ? HitResult.ImpactPoint : TraceEnd;
	return true;
}

FVector UGA_SR_ProjectileBase::GetProjectileSpawnLocation() const
{
	if (const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (const USkeletalMeshComponent* MeshComp = Character->GetMesh())
		{
			if (ProjectileSpawnSocketName != NAME_None && MeshComp->DoesSocketExist(ProjectileSpawnSocketName))
			{
				return MeshComp->GetSocketLocation(ProjectileSpawnSocketName);
			}
		}

		return Character->GetActorLocation() + (Character->GetActorForwardVector() * 100.f) + FVector(0.f, 0.f, 50.f);
	}

	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		return AvatarActor->GetActorLocation() + (AvatarActor->GetActorForwardVector() * 100.f);
	}

	return FVector::ZeroVector;
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
}

void UGA_SR_ProjectileBase::PlayMontageIfValid(UAnimMontage* MontageToPlay, float PlayRate) const
{
	if (!MontageToPlay)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr)
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
		if (UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInstance->Montage_Stop(0.2f, MontageToStop);
		}
	}
}

APawn* UGA_SR_ProjectileBase::GetAvatarPawn() const
{
	return Cast<APawn>(GetAvatarActorFromActorInfo());
}
