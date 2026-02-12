#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"
#include "Components/StaticMeshComponent.h"



ASpellRiseWeaponActor::ASpellRiseWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
}
