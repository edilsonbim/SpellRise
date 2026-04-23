#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_AuthoritativeProjectile.h"
#include "SpellRiseGA_AuthoritativeProjectileVariants.generated.h"

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_ShootProjectile : public USpellRiseGameplayAbility_AuthoritativeProjectile
{
	GENERATED_BODY()

public:
	USpellRiseGA_ShootProjectile();
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_ShootProjectileAOE : public USpellRiseGameplayAbility_AuthoritativeProjectile
{
	GENERATED_BODY()

public:
	USpellRiseGA_ShootProjectileAOE();
};
