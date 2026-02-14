// ============================================================================
// SpellRiseWeaponComponent.cpp (COMPLETO - COM GRANT/CLEAR ABILITIES)
// Path: Source/SpellRise/Weapons/Components/SpellRiseWeaponComponent.cpp
// ============================================================================

#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"

#include "SpellRise/Combat/Melee/Runtime/SpellRiseMeleeComponent.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"
#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

USpellRiseWeaponComponent::USpellRiseWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	MeleeComponent = nullptr;
	bMeleeComponentInitialized = false;
}

void USpellRiseWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpellRiseWeaponComponent, EquippedWeapon);
}

UAbilitySystemComponent* USpellRiseWeaponComponent::GetOwnerASC() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
}

void USpellRiseWeaponComponent::ResolveMeleeComponentOnOwner()
{
	if (MeleeComponent)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	MeleeComponent = Owner->FindComponentByClass<USpellRiseMeleeComponent>();

	if (!MeleeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponComponent] ❌ Owner has no SpellRiseMeleeComponent. Add it to the Character/BP."));
	}
}

void USpellRiseWeaponComponent::OnRep_EquippedWeapon()
{
	bMeleeComponentInitialized = false;

	if (!EquippedWeapon)
	{
		DestroyEquippedWeaponActor();
		return;
	}

	DestroyEquippedWeaponActor();
	SpawnAndAttachWeaponActor();

	ResolveMeleeComponentOnOwner();
	InitializeMeleeComponent();
}

void USpellRiseWeaponComponent::InitializeMeleeComponent()
{
	ResolveMeleeComponentOnOwner();

	if (bMeleeComponentInitialized) return;
	if (!MeleeComponent || !EquippedWeaponActor || !EquippedWeapon) return;

	UStaticMeshComponent* WeaponStaticMesh = EquippedWeaponActor->Mesh;
	if (!WeaponStaticMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponComponent] ❌ EquippedWeaponActor->Mesh is NULL."));
		return;
	}

	USceneComponent* WeaponMeshAsScene = WeaponStaticMesh;
	MeleeComponent->InitializeWithWeaponData(EquippedWeapon, WeaponMeshAsScene);

	bMeleeComponentInitialized = true;

	UE_LOG(LogTemp, Log, TEXT("[WeaponComponent] ✅ Melee initialized. Weapon=%s Mesh=%s"),
		*GetNameSafe(EquippedWeapon), *GetNameSafe(WeaponStaticMesh));
}

void USpellRiseWeaponComponent::GrantWeaponAbilities_Server()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority() || !EquippedWeapon)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponComponent] ASC null, cannot grant weapon abilities."));
		return;
	}

	// GrantAbilities from weapon DA
	for (TSubclassOf<UGameplayAbility> AbilityClass : EquippedWeapon->GrantAbilities)
	{
		if (!AbilityClass) continue;

		FGameplayAbilitySpec Spec(AbilityClass, 1);

		// IMPORTANT: must match BP Press/Release InputID
		Spec.InputID = WeaponPrimaryAttackInputID;

		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		GrantedAbilityHandles.Add(Handle);
	}

	ASC->InitAbilityActorInfo(Owner, Owner);
}

void USpellRiseWeaponComponent::ClearWeaponAbilities_Server()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC)
	{
		GrantedAbilityHandles.Reset();
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	GrantedAbilityHandles.Reset();
}

void USpellRiseWeaponComponent::EquipWeapon(const UDA_MeleeWeaponData* NewWeapon)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (!Owner->HasAuthority())
	{
		if (NewWeapon) ServerEquipWeapon(NewWeapon);
		else ServerUnequipWeapon();
		return;
	}

	// SERVER
	EquippedWeapon = NewWeapon;
	bMeleeComponentInitialized = false;

	// GAS abilities
	ClearWeaponAbilities_Server();
	if (EquippedWeapon)
	{
		GrantWeaponAbilities_Server();
	}

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

void USpellRiseWeaponComponent::ServerEquipWeapon_Implementation(const UDA_MeleeWeaponData* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void USpellRiseWeaponComponent::ServerUnequipWeapon_Implementation()
{
	UnequipWeapon();
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

void USpellRiseWeaponComponent::SpawnAndAttachWeaponActor()
{
	if (!EquippedWeapon || !EquippedWeapon->WeaponActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	USkeletalMeshComponent* SkelMesh = GetOwnerSkeletalMesh();
	if (!SkelMesh) return;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Spawned = World->SpawnActor<AActor>(
		EquippedWeapon->WeaponActorClass,
		FTransform::Identity,
		Params
	);

	if (!Spawned) return;

	EquippedWeaponVisualActor = Spawned;
	EquippedWeaponActor = Cast<ASpellRiseWeaponActor>(Spawned);

	Spawned->AttachToComponent(
		SkelMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		EquippedWeapon->AttachSocketName
	);

	Spawned->SetActorRelativeTransform(EquippedWeapon->AttachOffset);
}

USkeletalMeshComponent* USpellRiseWeaponComponent::GetOwnerSkeletalMesh() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	if (ACharacter* Char = Cast<ACharacter>(Owner))
	{
		return Char->GetMesh();
	}

	return Owner->FindComponentByClass<USkeletalMeshComponent>();
}

void USpellRiseWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	ResolveMeleeComponentOnOwner();

	// Server may already have EquippedWeapon set by spawn defaults
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// ensure ability handles reflect EquippedWeapon at begin play
		ClearWeaponAbilities_Server();
		if (EquippedWeapon)
		{
			GrantWeaponAbilities_Server();
		}
	}

	// Client: EquippedWeapon arrives via OnRep
	if (EquippedWeapon)
	{
		SpawnAndAttachWeaponActor();
		InitializeMeleeComponent();
	}
}

void USpellRiseWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ClearWeaponAbilities_Server();
	}

	DestroyEquippedWeaponActor();
	Super::EndPlay(EndPlayReason);
}

void USpellRiseWeaponComponent::HandleOwnerDeath()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (Owner->HasAuthority())
	{
		// remove weapon abilities on death (server)
		ClearWeaponAbilities_Server();

		EquippedWeapon = nullptr;
		bMeleeComponentInitialized = false;

		if (bRebuildVisualOnServer)
		{
			OnRep_EquippedWeapon();
		}

		Owner->ForceNetUpdate();
		return;
	}

	DestroyEquippedWeaponActor();
}
