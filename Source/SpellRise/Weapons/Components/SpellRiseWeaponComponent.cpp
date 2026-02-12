#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"

#include "Net/UnrealNetwork.h"

#include "SpellRise/Weapons/Data/DA_WeaponDefinition.h"
#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// GAS
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

USpellRiseWeaponComponent::USpellRiseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// -----------------------------
	// AAA tags (preferred)
	// -----------------------------
	SetByCallerBaseWeaponDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseWeaponDamage"), /*ErrorIfNotFound=*/false);
	SetByCallerDamageScalingTag    = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageScaling"),   /*ErrorIfNotFound=*/false);

	// -----------------------------
	// Legacy tags (fallback)
	// -----------------------------
	SetByCallerBaseDamageTag_Legacy       = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseDamage"),       /*ErrorIfNotFound=*/false);
	SetByCallerDamageMultiplierTag_Legacy = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageMultiplier"), /*ErrorIfNotFound=*/false);
}

void USpellRiseWeaponComponent::HandleOwnerDeath()
{
	// Requirement: weapon must not exist while dead. Ensure replicated unequip on server.
	if (AActor* Owner = GetOwner())
	{
		if (Owner->HasAuthority())
		{
			EquippedWeapon = nullptr;
			if (bRebuildVisualOnServer)
			{
				OnRep_EquippedWeapon();
			}
			Owner->ForceNetUpdate();
			return;
		}
	}

	// Client-side visual cleanup
	DestroyEquippedWeaponActor();
}

void USpellRiseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (EquippedWeapon)
	{
		SpawnAndAttachWeaponActor();
	}
	else
	{
		DestroyEquippedWeaponActor();
	}
}

void USpellRiseWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyEquippedWeaponActor();
	Super::EndPlay(EndPlayReason);
}

void USpellRiseWeaponComponent::EquipWeapon(UDA_WeaponDefinition* NewWeapon)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (!Owner->HasAuthority())
	{
		if (NewWeapon)
	{
		ServerEquipWeapon(NewWeapon);
	}
	else
	{
		ServerUnequipWeapon();
	}
		return;
	}

	EquippedWeapon = NewWeapon;

	UE_LOG(LogTemp, Warning, TEXT("[WeaponComp] SERVER EquipWeapon: Owner=%s WeaponDA=%s"),
		*GetNameSafe(Owner),
		*GetNameSafe(EquippedWeapon));

	if (bRebuildVisualOnServer)
	{
		OnRep_EquippedWeapon();
	}

	Owner->ForceNetUpdate();
}

void USpellRiseWeaponComponent::UnequipWeapon()
{
	EquipWeapon(nullptr);
}

void USpellRiseWeaponComponent::ServerUnequipWeapon_Implementation()
{
	UnequipWeapon();
}


void USpellRiseWeaponComponent::ServerEquipWeapon_Implementation(UDA_WeaponDefinition* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void USpellRiseWeaponComponent::OnRep_EquippedWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("[WeaponComp] OnRep_EquippedWeapon: Owner=%s WeaponDA=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(EquippedWeapon));

	if (!EquippedWeapon)
	{
		DestroyEquippedWeaponActor();
		return;
	}

	DestroyEquippedWeaponActor();
	SpawnAndAttachWeaponActor();
}

void USpellRiseWeaponComponent::DestroyEquippedWeaponActor()
{
	if (EquippedWeaponVisualActor)
	{
		EquippedWeaponVisualActor->Destroy();
		EquippedWeaponVisualActor = nullptr;
	}
	EquippedWeaponActor = nullptr;
}

USkeletalMeshComponent* USpellRiseWeaponComponent::GetOwnerSkeletalMesh() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	if (ACharacter* Char = Cast<ACharacter>(Owner))
	{
		return Char->GetMesh();
	}

	return Owner->FindComponentByClass<USkeletalMeshComponent>();
}

void USpellRiseWeaponComponent::SpawnAndAttachWeaponActor()
{
	if (!EquippedWeapon)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	USkeletalMeshComponent* SkelMesh = GetOwnerSkeletalMesh();
	if (!SkelMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponComp] Owner has no SkeletalMeshComponent: %s"), *GetNameSafe(GetOwner()));
		return;
	}

	if (!EquippedWeapon->WeaponActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponComp] WeaponActorClass not set on DA: %s"), *GetNameSafe(EquippedWeapon));
		return;
	}

	const FName SocketName = EquippedWeapon->AttachSocketName;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Spawned = World->SpawnActor<AActor>(EquippedWeapon->WeaponActorClass, FTransform::Identity, Params);
	if (!Spawned)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponComp] Failed to spawn weapon actor class: %s"), *GetNameSafe(EquippedWeapon->WeaponActorClass));
		return;
	}

	EquippedWeaponVisualActor = Spawned;
	EquippedWeaponActor = Cast<ASpellRiseWeaponActor>(Spawned);

	Spawned->AttachToComponent(
		SkelMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);

	Spawned->SetActorRelativeTransform(EquippedWeapon->AttachOffset);
}

void USpellRiseWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellRiseWeaponComponent, EquippedWeapon);
}

AActor* USpellRiseWeaponComponent::ResolveInstigatorActor(AActor* InInstigator) const
{
	AActor* Owner = GetOwner();

	if (!InInstigator)
	{
		return Owner;
	}

	if (Owner && InInstigator == Owner)
	{
		return InInstigator;
	}

	if (APlayerState* PS = Cast<APlayerState>(InInstigator))
	{
		if (APawn* Pawn = PS->GetPawn())
		{
			return Pawn;
		}
		return Owner ? Owner : InInstigator;
	}

	if (AController* C = Cast<AController>(InInstigator))
	{
		if (APawn* Pawn = C->GetPawn())
		{
			return Pawn;
		}
		return Owner ? Owner : InInstigator;
	}

	return InInstigator;
}

bool USpellRiseWeaponComponent::PerformLightAttackTrace(
	AActor* InstigatorActor,
	ECollisionChannel TraceChannel,
	bool bDebug,
	FHitResult& OutHit
) const
{
	OutHit = FHitResult();

	AActor* InstigatorResolved = ResolveInstigatorActor(InstigatorActor);

	if (!InstigatorResolved || !EquippedWeapon)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	USceneComponent* SocketProvider = nullptr;

	if (EquippedWeaponActor && EquippedWeaponActor->Mesh)
	{
		SocketProvider = EquippedWeaponActor->Mesh;
	}
	else if (EquippedWeaponVisualActor)
	{
		if (UStaticMeshComponent* SM = EquippedWeaponVisualActor->FindComponentByClass<UStaticMeshComponent>())
		{
			SocketProvider = SM;
		}
		else if (USkeletalMeshComponent* SK = EquippedWeaponVisualActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			SocketProvider = SK;
		}
		else
		{
			SocketProvider = EquippedWeaponVisualActor->GetRootComponent();
		}
	}

	static const FName SocketBase(TEXT("Melee_Base"));
	static const FName SocketTip(TEXT("Melee_Tip"));

	FVector Start = FVector::ZeroVector;
	FVector End = FVector::ZeroVector;
	bool bUsedSockets = false;

	if (SocketProvider &&
		SocketProvider->DoesSocketExist(SocketBase) &&
		SocketProvider->DoesSocketExist(SocketTip))
	{
		Start = SocketProvider->GetSocketLocation(SocketBase);
		End   = SocketProvider->GetSocketLocation(SocketTip);
		bUsedSockets = true;
	}
	else
	{
		Start = InstigatorResolved->GetActorLocation() + FVector(0.f, 0.f, 60.f);
		End   = Start + InstigatorResolved->GetActorForwardVector() * FMath::Max(0.f, EquippedWeapon->LightAttack.Range);
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SpellRiseMeleeTrace), false);
	Params.AddIgnoredActor(InstigatorResolved);

	if (EquippedWeaponVisualActor)
	{
		Params.AddIgnoredActor(EquippedWeaponVisualActor);
	}

	TArray<FHitResult> Hits;
	Hits.Reserve(32);

	bool bAnyHit = false;

	switch (EquippedWeapon->LightAttack.AttackShape)
	{
	case EWeaponAttackShape::Sphere:
	{
		const float Radius = FMath::Max(1.f, EquippedWeapon->LightAttack.SphereRadius);
		bAnyHit = World->SweepMultiByChannel(
			Hits, Start, End, FQuat::Identity, TraceChannel,
			FCollisionShape::MakeSphere(Radius), Params
		);
		break;
	}
	case EWeaponAttackShape::Capsule:
	{
		const float Radius = FMath::Max(1.f, EquippedWeapon->LightAttack.CapsuleRadius);
		const float HalfHeight = FMath::Max(Radius, EquippedWeapon->LightAttack.CapsuleHalfHeight);

		bAnyHit = World->SweepMultiByChannel(
			Hits, Start, End, FQuat::Identity, TraceChannel,
			FCollisionShape::MakeCapsule(Radius, HalfHeight), Params
		);
		break;
	}
	case EWeaponAttackShape::Box:
	default:
	{
		const FVector Extent = EquippedWeapon->LightAttack.BoxExtent;
		bAnyHit = World->SweepMultiByChannel(
			Hits, Start, End, FQuat::Identity, TraceChannel,
			FCollisionShape::MakeBox(Extent), Params
		);
		break;
	}
	}

	AActor* BestActor = nullptr;
	const UPrimitiveComponent* BestComp = nullptr;
	FHitResult BestHit;
	bool bHasBest = false;

	for (const FHitResult& HR : Hits)
	{
		AActor* A = HR.GetActor();
		UPrimitiveComponent* C = HR.GetComponent();

		if (!A || A == InstigatorResolved) continue;
		if (EquippedWeaponVisualActor && A == EquippedWeaponVisualActor) continue;

		if (!bHasBest)
		{
			BestActor = A;
			BestComp = C;
			BestHit = HR;
			bHasBest = true;
			continue;
		}

		const bool bSameActor = (A == BestActor);
		if (bSameActor)
		{
			const bool bBestIsSkeletalMesh = BestComp && BestComp->IsA(USkeletalMeshComponent::StaticClass());
			const bool bThisIsSkeletalMesh = C && C->IsA(USkeletalMeshComponent::StaticClass());

			if (bBestIsSkeletalMesh && !bThisIsSkeletalMesh)
			{
				BestComp = C;
				BestHit = HR;
			}
			continue;
		}

		const float BestDistSq = FVector::DistSquared(BestHit.ImpactPoint, Start);
		const float ThisDistSq = FVector::DistSquared(HR.ImpactPoint, Start);
		if (ThisDistSq < BestDistSq)
		{
			BestActor = A;
			BestComp = C;
			BestHit = HR;
		}
	}

	const bool bHit = (bHasBest && BestActor != nullptr);
	if (bHit)
	{
		OutHit = BestHit;
	}

	if (bDebug)
	{
		const float Life = 2.0f;
		const float Thick = 6.0f;
		const FColor Cc = bHit ? FColor::Green : FColor::Red;

		UE_LOG(LogTemp, Warning, TEXT("[MeleeTrace][DEBUG] InstigatorIn=%s InstigatorResolved=%s UsedSockets=%d Channel=%d AnyHit=%d Hits=%d"),
			*GetNameSafe(InstigatorActor),
			*GetNameSafe(InstigatorResolved),
			bUsedSockets ? 1 : 0,
			(int32)TraceChannel,
			bAnyHit ? 1 : 0,
			Hits.Num());

		DrawDebugSphere(World, Start, 10.f, 10, FColor::Yellow, false, Life);
		DrawDebugSphere(World, End,   10.f, 10, FColor::Cyan,   false, Life);
		DrawDebugLine(World, Start, End, Cc, false, Life, 0, Thick);

		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MeleeTrace] HIT %s Comp=%s"),
				*GetNameSafe(OutHit.GetActor()),
				*GetNameSafe(BestComp));
		}
	}

	return bHit;
}

bool USpellRiseWeaponComponent::ApplyDamageToTarget_Server(
	AActor* InstigatorActor,
	AActor* TargetActor,
	TSubclassOf<UGameplayEffect> DamageGE,
	float BaseDamage,
	float DamageScaling
) const
{
	AActor* InstigatorResolved = ResolveInstigatorActor(InstigatorActor);

	if (!InstigatorResolved || !TargetActor || !DamageGE)
	{
		return false;
	}

	// ✅ Autoridade real do GAS: o SourceASC precisa ser authoritative
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorResolved);
	if (!SourceASC || !SourceASC->IsOwnerActorAuthoritative())
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponComp][Damage] Missing TargetASC. InstigatorResolved=%s Target=%s"),
			*GetNameSafe(InstigatorResolved),
			*GetNameSafe(TargetActor));
		return false;
	}

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);

	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageGE, 1.f, Ctx);
	if (!Spec.IsValid() || !Spec.Data.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponComp][Damage] Failed to MakeOutgoingSpec. GE=%s"), *GetNameSafe(DamageGE.Get()));
		return false;
	}

	// ✅ AAA: carimba DamageTypeTag no Spec (ExecCalc lê DynamicAssetTags)
	if (EquippedWeapon && EquippedWeapon->LightAttack.DamageTypeTag.IsValid())
	{
		Spec.Data->AddDynamicAssetTag(EquippedWeapon->LightAttack.DamageTypeTag);
	}

	const float ClampedBaseDamage    = FMath::Max(0.f, BaseDamage);
	const float ClampedDamageScaling = FMath::Max(0.f, DamageScaling);

	// AAA tags
	if (SetByCallerBaseWeaponDamageTag.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SetByCallerBaseWeaponDamageTag, ClampedBaseDamage);
	}

	if (SetByCallerDamageScalingTag.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SetByCallerDamageScalingTag, ClampedDamageScaling);
	}

	// Legacy support
	if (SetByCallerBaseDamageTag_Legacy.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SetByCallerBaseDamageTag_Legacy, ClampedBaseDamage);
	}

	if (SetByCallerDamageMultiplierTag_Legacy.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SetByCallerDamageMultiplierTag_Legacy, ClampedDamageScaling);
	}

	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);

	UE_LOG(LogTemp, Warning, TEXT("[WeaponComp][Damage] Applied GE=%s BaseWeaponDamage=%.2f Scaling=%.2f DamageTypeTag=%s InstigatorResolved=%s Target=%s"),
		*GetNameSafe(DamageGE.Get()),
		ClampedBaseDamage,
		ClampedDamageScaling,
		EquippedWeapon ? *EquippedWeapon->LightAttack.DamageTypeTag.ToString() : TEXT("None"),
		*GetNameSafe(InstigatorResolved),
		*GetNameSafe(TargetActor));

	return true;
}

bool USpellRiseWeaponComponent::PerformLightAttack_TraceAndApplyDamage(
	AActor* InstigatorActor,
	TSubclassOf<UGameplayEffect> DamageGE,
	float BaseDamage,
	float DamageScaling,
	ECollisionChannel TraceChannel,
	bool bDebug
)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	// ✅ Sempre resolve instigator localmente (no server vamos ignorar o que o client mandar)
	AActor* InstigatorResolved = ResolveInstigatorActor(InstigatorActor);

	// Client -> RPC
	if (!Owner->HasAuthority())
	{
		// ✅ Anti-exploit: no server, vamos recomputar instigator pelo Owner, então aqui podemos mandar Owner ou nullptr.
		ServerPerformLightAttack_TraceAndApplyDamage(Owner, DamageGE, BaseDamage, DamageScaling, TraceChannel, bDebug);
		return true;
	}

	FHitResult Hit;
	const bool bHit = PerformLightAttackTrace(InstigatorResolved, TraceChannel, bDebug, Hit);
	if (!bHit)
	{
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		return false;
	}

	ApplyDamageToTarget_Server(InstigatorResolved, HitActor, DamageGE, BaseDamage, DamageScaling);
	return true;
}

void USpellRiseWeaponComponent::ServerPerformLightAttack_TraceAndApplyDamage_Implementation(
	AActor* InstigatorActor,
	TSubclassOf<UGameplayEffect> DamageGE,
	float BaseDamage,
	float DamageScaling,
	ECollisionChannel TraceChannel,
	bool bDebug
)
{
	// ✅ Anti-exploit: ignora instigator que veio do client e resolve pelo Owner no server
	AActor* ServerInstigator = GetOwner();
	this->PerformLightAttack_TraceAndApplyDamage(ServerInstigator, DamageGE, BaseDamage, DamageScaling, TraceChannel, bDebug);
}
