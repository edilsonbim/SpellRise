#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellRiseWeaponBase.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class USpellRiseWeaponDefinition;

UCLASS(Blueprintable)
class SPELLRISE_API ASpellRiseWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ASpellRiseWeaponBase();

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	USpellRiseWeaponDefinition* GetWeaponDefinition() const { return WeaponDefinition; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Weapon")
	USceneComponent* GetWeaponSpawnPointComponent() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SpellRise|Weapon")
	TObjectPtr<USceneComponent> DefaultSceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SpellRise|Weapon")
	TObjectPtr<USceneComponent> SpawnPoint = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SpellRise|Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SpellRise|Weapon")
	TObjectPtr<UStaticMeshComponent> StaticWeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Weapon")
	TObjectPtr<USpellRiseWeaponDefinition> WeaponDefinition = nullptr;
};
