// ============================================================================
// ASpellRiseMagicProjectileBase.cpp
// Path: Source/SpellRise/Combat/Spells/Projectiles/ASpellRiseMagicProjectileBase.cpp
// ============================================================================

#include "SpellRise/Combat/Spells/Projectiles/ASpellRiseMagicProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "GameplayCueManager.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

namespace SR_DamageTags
{
	inline const FGameplayTag& Data_BaseSpellDamage()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseSpellDamage"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageType_Spell()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell"), false);
		return Tag;
	}
}

ASpellRiseMagicProjectileBase::ASpellRiseMagicProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Collision->InitSphereRadius(CollisionRadius);

	// Hit + Overlap consistente
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetGenerateOverlapEvents(true);

	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Pawn: Overlap (dano em personagem)
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Mundo: Block (impact em cenário)
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	Collision->SetNotifyRigidBodyCollision(true);
	Collision->BodyInstance.bUseCCD = true;

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASpellRiseMagicProjectileBase::OnSphereBeginOverlap);
	Collision->OnComponentHit.AddDynamic(this, &ASpellRiseMagicProjectileBase::OnSphereHit);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;

	Movement->InitialSpeed = InitialSpeed;
	Movement->MaxSpeed = MaxSpeed;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.f;

	// evita tunneling em alta velocidade
	Movement->bSweepCollision = true;
	Movement->bForceSubStepping = true;
	Movement->MaxSimulationTimeStep = 0.016f;
	Movement->MaxSimulationIterations = 8;

	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
}

void ASpellRiseMagicProjectileBase::InitProjectile(
	AActor* InSourceActor,
	TSubclassOf<UGameplayEffect> InDamageEffectClass,
	float InBaseSpellDamage,
	const FGameplayTagContainer& InDynamicAssetTags,
	FGameplayTag InImpactCueTag,
	UObject* InSourceObject)
{
	SourceActor = InSourceActor;
	DamageEffectClass = InDamageEffectClass;
	BaseSpellDamage = FMath::Max(0.f, InBaseSpellDamage);
	DynamicAssetTags = InDynamicAssetTags;

	ImpactCueTag = InImpactCueTag;
	SourceObject = InSourceObject;

	// Ensure spell type tag is present
	if (SR_DamageTags::DamageType_Spell().IsValid())
	{
		DynamicAssetTags.AddTag(SR_DamageTags::DamageType_Spell());
	}
}

void ASpellRiseMagicProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (Collision)
	{
		Collision->SetSphereRadius(CollisionRadius);
	}

	if (Movement)
	{
		Movement->InitialSpeed = InitialSpeed;
		Movement->MaxSpeed = MaxSpeed;
	}

	if (HasAuthority())
	{
		SetLifeSpan(LifeSeconds > 0.f ? LifeSeconds : 5.f);
	}
}

bool ASpellRiseMagicProjectileBase::IsValidTarget_Server(AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this)
	{
		return false;
	}

	if (bIgnoreOwner)
	{
		if (OtherActor == GetOwner() || OtherActor == SourceActor.Get())
		{
			return false;
		}
	}

	if (bImpacted)
	{
		return false;
	}

	if (HitActors.Contains(OtherActor))
	{
		return false;
	}

	return true;
}

void ASpellRiseMagicProjectileBase::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Server decides impacts/damage
	if (!HasAuthority())
	{
		return;
	}

	if (bDebugCollision)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Proj] Overlap Auth=1 Self=%s Other=%s Comp=%s Sweep=%d"),
			*GetNameSafe(this), *GetNameSafe(OtherActor), *GetNameSafe(OtherComp), bFromSweep ? 1 : 0);
	}

	if (!IsValidTarget_Server(OtherActor))
	{
		return;
	}

	HandleImpact_Server(OtherActor, SweepResult);
}

void ASpellRiseMagicProjectileBase::OnSphereHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	// Server decides impacts/damage
	if (!HasAuthority())
	{
		return;
	}

	if (bDebugCollision)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Proj] Hit Auth=1 Self=%s Other=%s Comp=%s Blocking=%d"),
			*GetNameSafe(this), *GetNameSafe(OtherActor), *GetNameSafe(OtherComp), Hit.bBlockingHit ? 1 : 0);
	}

	// World hit (OtherActor may be null)
	if (!OtherActor)
	{
		if (!bImpacted)
		{
			bImpacted = true;
			FinishImpact_Server(nullptr, Hit);
		}
		return;
	}

	if (!IsValidTarget_Server(OtherActor))
	{
		return;
	}

	HandleImpact_Server(OtherActor, Hit);
}

void ASpellRiseMagicProjectileBase::HandleImpact_Server(AActor* OtherActor, const FHitResult& Hit)
{
	if (!HasAuthority() || bImpacted)
	{
		return;
	}

	HitActors.Add(OtherActor);

	ApplySpellDamage_Server(OtherActor, Hit);

	bImpacted = true;
	FinishImpact_Server(OtherActor, Hit);
}

void ASpellRiseMagicProjectileBase::ApplySpellDamage_Server(AActor* TargetActor, const FHitResult& Hit)
{
	if (!HasAuthority() || !TargetActor || !DamageEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = nullptr;

	if (AActor* Src = SourceActor.Get())
	{
		SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Src);
	}
	if (!SourceASC)
	{
		SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	}
	if (!SourceASC)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	// “EffectCauser” / source object: o próprio projétil ajuda debug e rastreio
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec)
	{
		return;
	}

	if (DynamicAssetTags.Num() > 0)
	{
		Spec->AppendDynamicAssetTags(DynamicAssetTags);
	}

	if (SR_DamageTags::Data_BaseSpellDamage().IsValid())
	{
		Spec->SetSetByCallerMagnitude(SR_DamageTags::Data_BaseSpellDamage(), BaseSpellDamage);
	}

	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec, TargetASC);
}

void ASpellRiseMagicProjectileBase::FinishImpact_Server(AActor* TargetActor, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	const FVector ImpactLoc = Hit.bBlockingHit ? FVector(Hit.ImpactPoint) : GetActorLocation();
	const FVector ImpactNormal = Hit.bBlockingHit ? FVector(Hit.ImpactNormal) : GetActorForwardVector();

	if (bDebugCollision)
	{
		DrawDebugSphere(GetWorld(), ImpactLoc, 24.f, 16, FColor::Red, false, 2.0f);
		UE_LOG(LogTemp, Warning, TEXT("[Proj] Impact Loc=%s Normal=%s Cue=%s Target=%s"),
			*ImpactLoc.ToString(),
			*ImpactNormal.ToString(),
			*ImpactCueTag.ToString(),
			*GetNameSafe(TargetActor));
	}

	// AAA-style: server triggers GameplayCue through ASC; clients render
	if (ImpactCueTag.IsValid())
	{
		// Prefer: Source ASC (ownership consistent), fallback: Target ASC
		UAbilitySystemComponent* CueASC = nullptr;

		if (AActor* Src = SourceActor.Get())
		{
			CueASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Src);
		}
		if (!CueASC && TargetActor)
		{
			CueASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		}

		FGameplayCueParameters Params;
		Params.Location = ImpactLoc;
		Params.Normal = ImpactNormal;
		Params.Instigator = SourceActor.Get() ? SourceActor.Get() : GetInstigator();
		Params.EffectCauser = this;
		Params.SourceObject = SourceObject.IsValid() ? SourceObject.Get() : nullptr;
		Params.PhysicalMaterial = Hit.PhysMaterial.Get();

		// ✅ UE 5.7: HitResult via EffectContext criado pelo ASC
		if (CueASC)
		{
			FGameplayEffectContextHandle CueContext = CueASC->MakeEffectContext();
			CueContext.AddSourceObject(this);
			CueContext.AddHitResult(Hit, /*bReset*/ true);
			Params.EffectContext = CueContext;

			CueASC->ExecuteGameplayCue(ImpactCueTag, Params);
		}
		else
		{
			// fallback (sem ASC): não terá replicação “bonita” do GAS
			if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
			{
				GCM->HandleGameplayCue(GetOwner(), ImpactCueTag, EGameplayCueEvent::Executed, Params);
			}
		}


		if (CueASC)
		{
			// ✅ caminho recomendado: replica via pipeline do ASC
			CueASC->ExecuteGameplayCue(ImpactCueTag, Params);
		}
		else
		{
			// Último recurso: não replica como um cue via ASC (evite depender disso)
			if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
			{
				GCM->HandleGameplayCue(GetOwner(), ImpactCueTag, EGameplayCueEvent::Executed, Params);
			}
		}
	}

	if (bDestroyOnImpact)
	{
		Destroy();
	}
}
