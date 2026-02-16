// SpellRiseWeaponActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellRiseWeaponActor.generated.h"

class UStaticMeshComponent;

/**
 * Visual-only weapon actor.
 * - No collision / no overlap (server-authoritative melee uses traces against targets).
 * - Provides a StaticMeshComponent that MUST contain sockets used by combat (melee trace, staff muzzle, etc).
 */
UCLASS(BlueprintType, Blueprintable)
class SPELLRISE_API ASpellRiseWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	ASpellRiseWeaponActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> Mesh;

protected:
	virtual void BeginPlay() override;

private:
	// Melee sockets are REQUIRED for melee weapons (kept as warnings).
	void ValidateMeleeTraceSockets() const;

	// Spell sockets are OPTIONAL (only verbose logs if missing).
	void ValidateSpellSockets_Optional() const;
};
