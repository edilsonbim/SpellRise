// ============================================================================
// SpellRiseWeaponComponent.cpp
// Path: Source/SpellRise/Weapons/Components/SpellRiseWeaponComponent.cpp
// ============================================================================

#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"

#include "SpellRise/Combat/Melee/Runtime/SpellRiseMeleeComponent.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"
#include "SpellRise/Combat/Spells/Data/DA_StaffWeaponData.h"
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

const UDA_MeleeWeaponData* USpellRiseWeaponComponent::GetEquippedMeleeWeapon() const
{
	return Cast<UDA_MeleeWeaponData>(EquippedWeapon);
}

const UDA_StaffWeaponData* USpellRiseWeaponComponent::GetEquippedStaffWeapon() const
{
	return Cast<UDA_StaffWeaponData>(EquippedWeapon);
}

UStaticMeshComponent* USpellRiseWeaponComponent::GetEquippedWeaponMesh() const
{
	if (!EquippedWeaponActor)
	{
		return nullptr;
	}
	return EquippedWeaponActor->Mesh;
}

bool USpellRiseWeaponComponent::GetWeaponSocketTransform(const FName SocketName, FTransform& OutTransform) const
{
	UStaticMeshComponent* WeaponMesh = GetEquippedWeaponMesh();
	if (!WeaponMesh)
	{
		return false;
	}

	if (!WeaponMesh->DoesSocketExist(SocketName))
	{
		return false;
	}

	OutTransform = WeaponMesh->GetSocketTransform(SocketName, RTS_World);
	return true;
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

	if (ASpellRiseCharacterBase* SRChar = Cast<ASpellRiseCharacterBase>(Owner))
	{
		MeleeComponent = SRChar->GetMeleeComponent();
	}

	if (!MeleeComponent)
	{
		MeleeComponent = Owner->FindComponentByClass<USpellRiseMeleeComponent>();
	}

	if (!MeleeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponComponent] ❌ Owner has no SpellRiseMeleeComponent. Add it to the Character/BP."));
	}
}

bool USpellRiseWeaponComponent::GetWeaponVisualData(
	TSubclassOf<AActor>& OutWeaponActorClass,
	FName& OutAttachSocketName,
	FTransform& OutAttachOffset
) const
{
	OutWeaponActorClass = nullptr;
	OutAttachSocketName = NAME_None;
	OutAttachOffset = FTransform::Identity;

	if (!EquippedWeapon)
	{
		return false;
	}

	if (const UDA_MeleeWeaponData* MeleeDA = Cast<UDA_MeleeWeaponData>(EquippedWeapon))
	{
		OutWeaponActorClass = MeleeDA->WeaponActorClass;
		OutAttachSocketName = MeleeDA->AttachSocketName;
		OutAttachOffset = MeleeDA->AttachOffset;
		return true;
	}

	if (const UDA_StaffWeaponData* StaffDA = Cast<UDA_StaffWeaponData>(EquippedWeapon))
	{
		OutWeaponActorClass = StaffDA->WeaponActorClass;
		OutAttachSocketName = StaffDA->AttachSocketName;
		OutAttachOffset = StaffDA->AttachOffset;
		return true;
	}

	return false;
}

bool USpellRiseWeaponComponent::GetWeaponGrantedAbilities(TArray<TSubclassOf<UGameplayAbility>>& OutAbilities) const
{
	OutAbilities.Reset();

	if (!EquippedWeapon)
	{
		return false;
	}

	if (const UDA_MeleeWeaponData* MeleeDA = Cast<UDA_MeleeWeaponData>(EquippedWeapon))
	{
		OutAbilities = MeleeDA->GrantAbilities;
		return true;
	}

	if (const UDA_StaffWeaponData* StaffDA = Cast<UDA_StaffWeaponData>(EquippedWeapon))
	{
		OutAbilities = StaffDA->GrantAbilities;
		return true;
	}

	return false;
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

	// Melee init ONLY if melee weapon equipped
	if (Cast<UDA_MeleeWeaponData>(EquippedWeapon))
	{
		ResolveMeleeComponentOnOwner();
		InitializeMeleeComponent();
	}
}

void USpellRiseWeaponComponent::InitializeMeleeComponent()
{
	ResolveMeleeComponentOnOwner();

	if (bMeleeComponentInitialized) return;

	const UDA_MeleeWeaponData* MeleeDA = Cast<UDA_MeleeWeaponData>(EquippedWeapon);
	if (!MeleeDA) return;

	if (!MeleeComponent || !EquippedWeaponActor)
	{
		return;
	}

	UStaticMeshComponent* WeaponStaticMesh = EquippedWeaponActor->Mesh;
	if (!WeaponStaticMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponComponent] ❌ EquippedWeaponActor->Mesh is NULL."));
		return;
	}

	USceneComponent* WeaponMeshAsScene = WeaponStaticMesh;
	MeleeComponent->InitializeWithWeaponData(MeleeDA, WeaponMeshAsScene);

	bMeleeComponentInitialized = true;

	UE_LOG(LogTemp, Log, TEXT("[WeaponComponent] ✅ Melee initialized. Weapon=%s Mesh=%s"),
		*GetNameSafe(MeleeDA), *GetNameSafe(WeaponStaticMesh));
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

	TArray<TSubclassOf<UGameplayAbility>> Abilities;
	if (!GetWeaponGrantedAbilities(Abilities))
	{
		return;
	}

	for (TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		if (!AbilityClass) continue;

		FGameplayAbilitySpec Spec(AbilityClass, 1);
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

void USpellRiseWeaponComponent::EquipMeleeWeapon(const UDA_MeleeWeaponData* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void USpellRiseWeaponComponent::EquipStaffWeapon(const UDA_StaffWeaponData* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void USpellRiseWeaponComponent::EquipWeapon(const UPrimaryDataAsset* NewWeapon)
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

void USpellRiseWeaponComponent::ServerEquipWeapon_Implementation(const UPrimaryDataAsset* NewWeapon)
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
	if (!EquippedWeapon) return;

	TSubclassOf<AActor> WeaponActorClass = nullptr;
	FName AttachSocketName = NAME_None;
	FTransform AttachOffset = FTransform::Identity;

	if (!GetWeaponVisualData(WeaponActorClass, AttachSocketName, AttachOffset))
	{
		return;
	}

	if (!WeaponActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	USkeletalMeshComponent* SkelMesh = GetOwnerSkeletalMesh();
	if (!SkelMesh) return;

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Spawned = World->SpawnActor<AActor>(WeaponActorClass, FTransform::Identity, Params);
	if (!Spawned) return;

	EquippedWeaponVisualActor = Spawned;
	EquippedWeaponActor = Cast<ASpellRiseWeaponActor>(Spawned);

	Spawned->AttachToComponent(
		SkelMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		AttachSocketName
	);

	Spawned->SetActorRelativeTransform(AttachOffset);
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

	// Server: ensure ability handles reflect EquippedWeapon at begin play
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ClearWeaponAbilities_Server();
		if (EquippedWeapon)
		{
			GrantWeaponAbilities_Server();
		}
	}

	// Client: EquippedWeapon arrives via OnRep (but if pre-set in defaults, handle it)
	if (EquippedWeapon)
	{
		SpawnAndAttachWeaponActor();

		// Only melee weapons init melee runtime
		if (Cast<UDA_MeleeWeaponData>(EquippedWeapon))
		{
			InitializeMeleeComponent();
		}
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
