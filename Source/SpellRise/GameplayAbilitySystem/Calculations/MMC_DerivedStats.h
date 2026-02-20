#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_DerivedStats.generated.h"

UCLASS(Abstract)
class SPELLRISE_API UMMC_PrimaryBase : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

protected:
	UMMC_PrimaryBase();

	float GetStrength(const FGameplayEffectSpec& Spec) const;
	float GetAgility(const FGameplayEffectSpec& Spec) const;
	float GetIntelligence(const FGameplayEffectSpec& Spec) const;
	float GetWisdom(const FGameplayEffectSpec& Spec) const;

	float Normalize01_FromBaseline(float PrimaryValue) const;

private:
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition AgilityDef;
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	FGameplayEffectAttributeCaptureDefinition WisdomDef;
};

UCLASS()
class SPELLRISE_API UMMC_MeleeDamageMultiplier : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_MeleeDamageMultiplier();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_BowDamageMultiplier : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_BowDamageMultiplier();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_SpellDamageMultiplier : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_SpellDamageMultiplier();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_HealingMultiplier : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_HealingMultiplier();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_CastTimeReduction : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_CastTimeReduction();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_CritChance : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_CritChance();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_CritDamage : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_CritDamage();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_ArmorPenetration : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_ArmorPenetration();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_ManaCostReduction : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_ManaCostReduction();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_MaxHealthFromPrimaries : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_MaxHealthFromPrimaries();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_MaxManaFromPrimaries : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_MaxManaFromPrimaries();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class SPELLRISE_API UMMC_MaxStaminaFromPrimaries : public UMMC_PrimaryBase
{
	GENERATED_BODY()
public:
	UMMC_MaxStaminaFromPrimaries();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
