#include "SpellRise/GameplayAbilitySystem/Projectiles/SR_ProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayCueManager.h"

ASR_ProjectileBase::ASR_ProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	SphereCollision->InitSphereRadius(16.f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SphereCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetNotifyRigidBodyCollision(true);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(SphereCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereCollision;
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->SetIsReplicated(true);

	InitialLifeSpan = 5.f;
}

void ASR_ProjectileBase::InitializeProjectile(
	const FVector& InTargetLocation,
	float InSpeed,
	const FGameplayEffectSpecHandle& InEffectSpecHandle)
{
	TargetLocation = InTargetLocation;
	Speed = FMath::Max(0.f, InSpeed);
	EffectSpecHandle = InEffectSpecHandle;

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
	}
}

void ASR_ProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASR_ProjectileBase::OnSphereBeginOverlap);
		SphereCollision->OnComponentHit.AddDynamic(this, &ASR_ProjectileBase::OnSphereHit);
	}

	if (ProjectileMovement)
	{
		const FVector SpawnLocation = GetActorLocation();
		FVector Direction = GetActorForwardVector();

		if (!TargetLocation.IsNearlyZero())
		{
			const FVector ToTarget = TargetLocation - SpawnLocation;
			if (!ToTarget.IsNearlyZero())
			{
				Direction = ToTarget.GetSafeNormal();
			}
		}

		ProjectileMovement->Velocity = Direction * Speed;
		ProjectileMovement->Activate(true);
	}

	TriggerSpawnCue();
}

void ASR_ProjectileBase::Destroyed()
{
	TriggerImpactCue();
	Super::Destroyed();
}

void ASR_ProjectileBase::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator())
	{
		return;
	}

	if (HasAuthority())
	{
		ApplyDamageToActor(OtherActor);
	}

	Destroy();
}

void ASR_ProjectileBase::OnSphereHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Destroy();
}

void ASR_ProjectileBase::ApplyDamageToActor(AActor* OtherActor)
{
	if (!OtherActor || !EffectSpecHandle.IsValid() || !EffectSpecHandle.Data.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!TargetASC)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = nullptr;
	if (AActor* SourceActor = GetInstigator())
	{
		SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	}

	if (SourceASC)
	{
		SourceASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetASC);
		return;
	}

	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void ASR_ProjectileBase::TriggerSpawnCue()
{
	if (!GameplayCueSpawnTag.IsValid())
	{
		return;
	}

	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetActorLocation();
	if (UGameplayCueManager* CueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		CueManager->HandleGameplayCue(this, GameplayCueSpawnTag, EGameplayCueEvent::Executed, CueParameters);
	}
}

void ASR_ProjectileBase::TriggerImpactCue()
{
	if (!GameplayCueImpactTag.IsValid() || bImpactCueTriggered)
	{
		return;
	}

	bImpactCueTriggered = true;

	FGameplayCueParameters CueParameters;
	CueParameters.Location = GetActorLocation();
	if (UGameplayCueManager* CueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		CueManager->HandleGameplayCue(this, GameplayCueImpactTag, EGameplayCueEvent::Executed, CueParameters);
	}
}
