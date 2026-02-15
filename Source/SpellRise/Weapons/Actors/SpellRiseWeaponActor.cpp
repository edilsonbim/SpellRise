// SpellRiseWeaponActor.cpp

#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ASpellRiseWeaponActor::ASpellRiseWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Visual-only
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);

	// Keep replication cheap; weapon visual is spawned/attached on both sides by WeaponComponent
	bReplicates = false;
	SetReplicateMovement(false);

	// Attachment-friendly defaults
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->bReceivesDecals = false;
}

void ASpellRiseWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	ValidateTraceSockets();
}

void ASpellRiseWeaponActor::ValidateTraceSockets() const
{
	// These must match your DA defaults unless overridden per weapon:
	static const FName SocketBase(TEXT("Melee_Base"));
	static const FName SocketTip(TEXT("Melee_Tip"));

	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("[WeaponActor] Mesh is NULL (%s)"), *GetName());
		return;
	}

	// If no mesh assigned in BP/DA class, sockets obviously won't exist
	if (!Mesh->GetStaticMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponActor] No StaticMesh assigned on %s. Trace sockets cannot exist."),
			   *GetName());
		return;
	}

	const bool bHasBase = Mesh->DoesSocketExist(SocketBase);
	const bool bHasTip  = Mesh->DoesSocketExist(SocketTip);

	if (!bHasBase || !bHasTip)
	{
		UE_LOG(LogTemp, Warning,
			   TEXT("[WeaponActor] Missing trace sockets on %s (Mesh=%s). Required: %s=%d %s=%d"),
			   *GetName(),
			   *GetNameSafe(Mesh->GetStaticMesh()),
			   *SocketBase.ToString(), bHasBase ? 1 : 0,
			   *SocketTip.ToString(), bHasTip ? 1 : 0);
	}
}
