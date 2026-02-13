#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Combat/Melee/Components/SpellRiseMeleeComponent.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"   // ✅ caminho real
#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

USpellRiseWeaponComponent::USpellRiseWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    MeleeComponent = CreateDefaultSubobject<USpellRiseMeleeComponent>(TEXT("MeleeComponent"));
}

void USpellRiseWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(USpellRiseWeaponComponent, EquippedWeapon);
}

void USpellRiseWeaponComponent::OnRep_EquippedWeapon()
{
    if (!EquippedWeapon)
    {
        DestroyEquippedWeaponActor();
        return;
    }

    DestroyEquippedWeaponActor();
    SpawnAndAttachWeaponActor();

    if (MeleeComponent && EquippedWeaponActor)
    {
        InitializeMeleeComponent();
    }
}

void USpellRiseWeaponComponent::InitializeMeleeComponent()
{
    if (bMeleeComponentInitialized) return;
    if (!MeleeComponent || !EquippedWeaponActor || !EquippedWeapon) return;

    USceneComponent* WeaponMesh = Cast<USceneComponent>(EquippedWeaponActor->Mesh);
    if (WeaponMesh)
    {
        MeleeComponent->InitializeWithWeaponData(EquippedWeapon, WeaponMesh);
        bMeleeComponentInitialized = true;
    }
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

    EquippedWeapon = NewWeapon;
    // Reset da flag de inicialização para que a nova arma seja configurada corretamente
    bMeleeComponentInitialized = false;

    if (bRebuildVisualOnServer) OnRep_EquippedWeapon();
    Owner->ForceNetUpdate();
}

void USpellRiseWeaponComponent::UnequipWeapon()
{
    EquipWeapon(nullptr);
}

// ============================================
// 🟢 RPCs – IMPLEMENTAÇÃO COM _Implementation (CONST)
// ============================================
void USpellRiseWeaponComponent::ServerEquipWeapon_Implementation(const UDA_MeleeWeaponData* NewWeapon)
{
    EquipWeapon(NewWeapon);
}

void USpellRiseWeaponComponent::ServerUnequipWeapon_Implementation()
{
    UnequipWeapon();
}

// ============================================
// VISUAL
// ============================================
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

    if (EquippedWeapon)
    {
        SpawnAndAttachWeaponActor();
        if (EquippedWeapon) InitializeMeleeComponent();
    }
}

void USpellRiseWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DestroyEquippedWeaponActor();
    Super::EndPlay(EndPlayReason);
}

void USpellRiseWeaponComponent::HandleOwnerDeath()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    if (Owner->HasAuthority())
    {
        EquippedWeapon = nullptr;
        bMeleeComponentInitialized = false;
        if (bRebuildVisualOnServer) OnRep_EquippedWeapon();
        Owner->ForceNetUpdate();
        return;
    }

    DestroyEquippedWeaponActor();
}