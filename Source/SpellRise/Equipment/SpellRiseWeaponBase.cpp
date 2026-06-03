#include "SpellRise/Equipment/SpellRiseWeaponBase.h"

#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

ASpellRiseWeaponBase::ASpellRiseWeaponBase()
{
	bReplicates = true;
	SetReplicateMovement(false);
	bNetUseOwnerRelevancy = true;
	SetNetUpdateFrequency(15.0f);

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->bEditableWhenInherited = true;
	SetRootComponent(DefaultSceneRoot);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->bEditableWhenInherited = true;
	WeaponMesh->SetupAttachment(DefaultSceneRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticWeaponMesh"));
	StaticWeaponMesh->bEditableWhenInherited = true;
	StaticWeaponMesh->SetupAttachment(DefaultSceneRoot);
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->bEditableWhenInherited = true;
	SpawnPoint->SetupAttachment(DefaultSceneRoot);
}

USceneComponent* ASpellRiseWeaponBase::GetWeaponSpawnPointComponent() const
{
	return SpawnPoint ? SpawnPoint.Get() : GetRootComponent();
}
