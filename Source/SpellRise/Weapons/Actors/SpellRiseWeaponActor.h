#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellRiseWeaponActor.generated.h"

class UStaticMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class SPELLRISE_API ASpellRiseWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	ASpellRiseWeaponActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
