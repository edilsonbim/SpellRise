// Cabeçalho de implementação: executa spawn autoritativo de projétil preservando validação no servidor.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_AuthoritativeProjectile.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.h"

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
		return false;
	}

	if (bRequireServerAuthorityForSpawn && !HasServerAuthority())
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FTransform SpawnTransform = ResolveProjectileSpawnTransform();
	const FVector SpawnLocation = SpawnTransform.GetLocation();
	const FVector ToTarget = TargetLocation - SpawnLocation;
	if (ToTarget.IsNearlyZero())
	{
		return false;
	}

	if (MaxProjectileRange > 0.0f && ToTarget.SizeSquared() > FMath::Square(MaxProjectileRange))
	{
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
