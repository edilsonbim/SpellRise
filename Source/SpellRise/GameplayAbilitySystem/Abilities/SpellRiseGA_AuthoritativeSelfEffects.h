#pragma once

#include "CoreMinimal.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_AuthoritativeEffect.h"
#include "SpellRiseGA_AuthoritativeSelfEffects.generated.h"

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_HealSelf : public USpellRiseGameplayAbility_AuthoritativeEffect
{
	GENERATED_BODY()

public:
	USpellRiseGA_HealSelf();
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_TransferToHealth : public USpellRiseGameplayAbility_AuthoritativeEffect
{
	GENERATED_BODY()

public:
	USpellRiseGA_TransferToHealth();
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_TransferToMana : public USpellRiseGameplayAbility_AuthoritativeEffect
{
	GENERATED_BODY()

public:
	USpellRiseGA_TransferToMana();
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_TransferToStamina : public USpellRiseGameplayAbility_AuthoritativeEffect
{
	GENERATED_BODY()

public:
	USpellRiseGA_TransferToStamina();
};

UCLASS(Blueprintable)
class SPELLRISE_API USpellRiseGA_ShieldSelf : public USpellRiseGameplayAbility_AuthoritativeEffect
{
	GENERATED_BODY()

public:
	USpellRiseGA_ShieldSelf();
};
