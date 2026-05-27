// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
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
	SphereCollision->InitSphereRadius(CollisionRadius);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
	ApplyCollisionSettings();

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
	const FVector& InInitialDirection,
	float InSpeed,
	const FGameplayEffectSpecHandle& InEffectSpecHandle,
	TSubclassOf<UGameplayEffect> InDebuffEffectClass)
{
	TargetLocation = InTargetLocation;
	InitialDirection = InInitialDirection;
	Speed = FMath::Max(0.f, InSpeed);
	EffectSpecHandle = InEffectSpecHandle;
	DebuffEffectClass = InDebuffEffectClass;

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
	}

	ApplyCollisionSettings();
}

void ASR_ProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (SphereCollision)
	{
		ApplyCollisionSettings();
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASR_ProjectileBase::OnSphereBeginOverlap);
		SphereCollision->OnComponentHit.AddDynamic(this, &ASR_ProjectileBase::OnSphereHit);
	}

	if (ProjectileMovement)
	{
		const FVector Direction = InitialDirection.IsNearlyZero()
			? GetActorForwardVector().GetSafeNormal()
			: InitialDirection.GetSafeNormal();
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
	if (!OtherActor)
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
		if (EffectSpecHandle.IsValid() && EffectSpecHandle.Data.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetASC);
		}
		if (DebuffEffectClass)
		{
			SourceASC->ApplyGameplayEffectToTarget(DebuffEffectClass.GetDefaultObject(), TargetASC, 1.0f);
		}
		return;
	}

	if (EffectSpecHandle.IsValid() && EffectSpecHandle.Data.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}

	if (DebuffEffectClass)
	{
		TargetASC->ApplyGameplayEffectToSelf(DebuffEffectClass.GetDefaultObject(), 1.0f, TargetASC->MakeEffectContext());
	}
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

void ASR_ProjectileBase::ApplyCollisionSettings()
{
	if (!SphereCollision)
	{
		return;
	}

	SphereCollision->SetSphereRadius(FMath::Max(0.0f, CollisionRadius));
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ToCollisionResponse(WorldStaticCollisionMode));
	SphereCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ToCollisionResponse(WorldDynamicCollisionMode));
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ToCollisionResponse(PawnCollisionMode));
	SphereCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ToCollisionResponse(PhysicsBodyCollisionMode));
	SphereCollision->SetGenerateOverlapEvents(bGenerateProjectileOverlapEvents);
	SphereCollision->SetNotifyRigidBodyCollision(bGenerateProjectileHitEvents);
}

ECollisionResponse ASR_ProjectileBase::ToCollisionResponse(ESpellRiseProjectileCollisionMode CollisionMode)
{
	switch (CollisionMode)
	{
	case ESpellRiseProjectileCollisionMode::Block:
		return ECR_Block;
	case ESpellRiseProjectileCollisionMode::Overlap:
		return ECR_Overlap;
	case ESpellRiseProjectileCollisionMode::Ignore:
	default:
		return ECR_Ignore;
	}
}
