// Cabeçalho de implementação: executa spawn autoritativo de projétil preservando validação no servidor.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_AuthoritativeProjectile.h"

#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseProjectileAbility, Log, All);

namespace
{
	bool IsFiniteVector(const FVector& Vector)
	{
		return FMath::IsFinite(Vector.X) && FMath::IsFinite(Vector.Y) && FMath::IsFinite(Vector.Z);
	}
}

USpellRiseGameplayAbility_AuthoritativeProjectile::USpellRiseGameplayAbility_AuthoritativeProjectile()
{
}

void USpellRiseGameplayAbility_AuthoritativeProjectile::NativeOnSpellExecuted()
{
	bAwaitingProjectileTargetConfirmation = true;
	K2_OnProjectileExecutionStarted();
	Super::NativeOnSpellExecuted();
}

void USpellRiseGameplayAbility_AuthoritativeProjectile::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ResetProjectileRuntimeState();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USpellRiseGameplayAbility_AuthoritativeProjectile::SR_ConfirmProjectileTargetAndSpawn(const FVector& TargetLocation)
{
	if (!IsActive() || !bAwaitingProjectileTargetConfirmation)
	{
		return false;
	}

	const bool bSpawned = SpawnAuthoritativeProjectile(TargetLocation);
	if (!bSpawned)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return false;
	}

	bAwaitingProjectileTargetConfirmation = false;
	K2_OnProjectileSpawned();

	if (bEndAbilityAfterExecution)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

	return true;
}

bool USpellRiseGameplayAbility_AuthoritativeProjectile::SpawnAuthoritativeProjectile(const FVector& TargetLocation)
{
	if (!ProjectileClass)
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[Projectile][TargetRejected] Reason=missing_projectile_class Ability=%s"),
			*GetNameSafe(this));
		return false;
	}

	if (bRequireServerAuthorityForSpawn && !HasServerAuthority())
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[Projectile][TargetRejected] Reason=missing_server_authority Ability=%s"),
			*GetNameSafe(this));
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[Projectile][TargetRejected] Reason=missing_world Ability=%s"),
			*GetNameSafe(this));
		return false;
	}

	const FTransform SpawnTransform = ResolveProjectileSpawnTransform();
	const FVector SpawnLocation = SpawnTransform.GetLocation();
	FString TargetRejectReason;
	if (!ValidateProjectileTargetLocation(SpawnLocation, TargetLocation, TargetRejectReason))
	{
		UE_LOG(LogSpellRiseProjectileAbility, Warning, TEXT("[Projectile][TargetRejected] Reason=%s Ability=%s Spawn=%s Target=%s"),
			*TargetRejectReason,
			*GetNameSafe(this),
			*SpawnLocation.ToCompactString(),
			*TargetLocation.ToCompactString());
		return false;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = AvatarActor;
	SpawnParameters.Instigator = Cast<APawn>(AvatarActor);
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FRotator SpawnRotation = ResolveProjectileSpawnRotation(SpawnLocation, TargetLocation);
	ASR_ProjectileBase* Projectile = World->SpawnActor<ASR_ProjectileBase>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParameters);

	if (!Projectile)
	{
		return false;
	}

	Projectile->InitializeProjectile(
		TargetLocation,
		SpawnRotation.Vector(),
		ProjectileSpeed,
		BuildProjectileEffectSpec(),
		DebuffEffectClass);

	UE_LOG(LogSpellRiseProjectileAbility, Verbose, TEXT("[Projectile][Spawned] Ability=%s Projectile=%s Spawn=%s Target=%s"),
		*GetNameSafe(this),
		*GetNameSafe(Projectile),
		*SpawnLocation.ToCompactString(),
		*TargetLocation.ToCompactString());

	return true;
}

bool USpellRiseGameplayAbility_AuthoritativeProjectile::ValidateProjectileTargetLocation(
	const FVector& SpawnLocation,
	const FVector& TargetLocation,
	FString& OutRejectReason) const
{
	OutRejectReason.Reset();

	if (!IsFiniteVector(SpawnLocation) || !IsFiniteVector(TargetLocation))
	{
		OutRejectReason = TEXT("non_finite_location");
		return false;
	}

	const FVector ToTarget = TargetLocation - SpawnLocation;
	if (ToTarget.IsNearlyZero())
	{
		OutRejectReason = TEXT("zero_length_target");
		return false;
	}

	const double TargetDistanceSq = ToTarget.SizeSquared();
	if (MaxProjectileRange > 0.0f && TargetDistanceSq > FMath::Square(static_cast<double>(MaxProjectileRange)))
	{
		OutRejectReason = TEXT("range_exceeded");
		return false;
	}

	if (MaxTargetAngleFromAvatarForwardDegrees > 0.0f && MaxTargetAngleFromAvatarForwardDegrees < 180.0f)
	{
		const AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (!AvatarActor)
		{
			OutRejectReason = TEXT("missing_avatar");
			return false;
		}

		const FVector Forward = AvatarActor->GetActorForwardVector().GetSafeNormal();
		const FVector Direction = ToTarget.GetSafeNormal();
		const double MinDot = FMath::Cos(FMath::DegreesToRadians(static_cast<double>(MaxTargetAngleFromAvatarForwardDegrees)));
		if (FVector::DotProduct(Forward, Direction) < MinDot)
		{
			OutRejectReason = TEXT("angle_exceeded");
			return false;
		}
	}

	if (bRequireServerLineOfSightToTarget)
	{
		const UWorld* World = GetWorld();
		const AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (!World || !AvatarActor)
		{
			OutRejectReason = TEXT("missing_los_context");
			return false;
		}

		FHitResult Hit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpellRiseProjectileTargetValidation), false);
		QueryParams.AddIgnoredActor(AvatarActor);
		if (const APawn* InstigatorPawn = Cast<APawn>(AvatarActor))
		{
			QueryParams.AddIgnoredActor(InstigatorPawn);
		}

		const bool bHit = World->LineTraceSingleByChannel(
			Hit,
			SpawnLocation,
			TargetLocation,
			TargetValidationTraceChannel,
			QueryParams);

		if (bHit && Hit.bBlockingHit && FVector::DistSquared(Hit.ImpactPoint, TargetLocation) > FMath::Square(75.0))
		{
			OutRejectReason = TEXT("line_of_sight_blocked");
			return false;
		}
	}

	OutRejectReason = TEXT("accepted");
	return true;
}

FGameplayEffectSpecHandle USpellRiseGameplayAbility_AuthoritativeProjectile::BuildProjectileEffectSpec() const
{
	if (!DamageEffectClass)
	{
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectSpecHandle SpecHandle = const_cast<USpellRiseGameplayAbility_AuthoritativeProjectile*>(this)->
		MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());

	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return FGameplayEffectSpecHandle();
	}

	if (DamageMagnitude > 0.0f)
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.Damage"), false), DamageMagnitude);
	}

	if (DamageTypeTags.Num() > 0)
	{
		SpecHandle.Data->DynamicGrantedTags.AppendTags(DamageTypeTags);
	}

	if (DebuffEffectClass)
	{
		SpecHandle.Data->AddDynamicAssetTag(FGameplayTag::RequestGameplayTag(TEXT("Data.HasDebuffPayload"), false));
	}

	return SpecHandle;
}

FTransform USpellRiseGameplayAbility_AuthoritativeProjectile::ResolveProjectileSpawnTransform() const
{
	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		return FTransform(AvatarActor->GetActorRotation(), AvatarActor->GetActorLocation());
	}

	return FTransform::Identity;
}

FRotator USpellRiseGameplayAbility_AuthoritativeProjectile::ResolveProjectileSpawnRotation(
	const FVector& SpawnLocation,
	const FVector& TargetLocation) const
{
	const FVector ToTarget = (TargetLocation - SpawnLocation).GetSafeNormal();
	return ToTarget.Rotation();
}

void USpellRiseGameplayAbility_AuthoritativeProjectile::ResetProjectileRuntimeState()
{
	bAwaitingProjectileTargetConfirmation = false;
}
