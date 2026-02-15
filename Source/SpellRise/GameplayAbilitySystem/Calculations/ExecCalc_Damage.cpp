#include "ExecCalc_Damage.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "Math/UnrealMathUtility.h"

// ---------------------------------------------------------
// Tags (lazy init)
// ---------------------------------------------------------
namespace SpellRiseTags
{
	inline const FGameplayTag& Data_BaseWeaponDamage()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseWeaponDamage"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_BaseSpellDamage()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseSpellDamage"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_DamageScaling()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageScaling"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_ForceCrit()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.ForceCrit"), false);
		return Tag;
	}

	// Legacy fallbacks
	inline const FGameplayTag& Data_BaseDamage_Legacy()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseDamage"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_DamageMultiplier_Legacy()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageMultiplier"), false);
		return Tag;
	}

	// Damage types (prefer Spec dynamic asset tags)
	inline const FGameplayTag& DamageType_Spell()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageType_Melee()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Melee"), false);
		return Tag;
	}

	// Elements
	inline const FGameplayTag& Damage_Element_Fire()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Element.Fire"), false);
		return Tag;
	}
	inline const FGameplayTag& Damage_Element_Frost()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Element.Frost"), false);
		return Tag;
	}
	inline const FGameplayTag& Damage_Element_Lightning()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Element.Lightning"), false);
		return Tag;
	}
	inline const FGameplayTag& Damage_Element_Elemental()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Element.Elemental"), false);
		return Tag;
	}

	inline float GetSBC_WithFallback(
		const FGameplayEffectSpec& Spec,
		const FGameplayTag& PrimaryTag,
		const FGameplayTag& FallbackTag,
		bool bWarnIfNotFound,
		float DefaultValue)
	{
		if (PrimaryTag.IsValid())
		{
			float Value = Spec.GetSetByCallerMagnitude(PrimaryTag, bWarnIfNotFound, DefaultValue);
			if (FallbackTag.IsValid() && PrimaryTag != FallbackTag && FMath::IsNearlyEqual(Value, DefaultValue))
			{
				Value = Spec.GetSetByCallerMagnitude(FallbackTag, false, DefaultValue);
			}
			return Value;
		}

		if (FallbackTag.IsValid())
		{
			return Spec.GetSetByCallerMagnitude(FallbackTag, false, DefaultValue);
		}

		return DefaultValue;
	}
}

// ---------------------------------------------------------
// Captures
// ---------------------------------------------------------
UExecCalc_Damage::FCaptureDefs::FCaptureDefs()
{
	WeaponDamageMultiplierDef = FGameplayEffectAttributeCaptureDefinition(
		UDerivedStatsAttributeSet::GetWeaponDamageMultiplierAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	SpellPowerMultiplierDef = FGameplayEffectAttributeCaptureDefinition(
		UDerivedStatsAttributeSet::GetSpellPowerMultiplierAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	CritChanceDef = FGameplayEffectAttributeCaptureDefinition(
		UDerivedStatsAttributeSet::GetCritChanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	CritDamageDef = FGameplayEffectAttributeCaptureDefinition(
		UDerivedStatsAttributeSet::GetCritDamageAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	ArmorPenetrationDef = FGameplayEffectAttributeCaptureDefinition(
		UDerivedStatsAttributeSet::GetArmorPenetrationAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	ElementPenetrationDef = FGameplayEffectAttributeCaptureDefinition(
		UDerivedStatsAttributeSet::GetElementPenetrationAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	PhysicalResistDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetPhysicalResistanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	MagicResistDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetMagicResistanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	ElementalResistDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetElementalResistanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	FireResistDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetFireResistanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	FrostResistDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetFrostResistanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	LightningResistDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetLightningResistanceAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	ArmorDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetArmorAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);
}

const UExecCalc_Damage::FCaptureDefs& UExecCalc_Damage::Captures()
{
	static FCaptureDefs CaptureDefs;
	return CaptureDefs;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	const FCaptureDefs& C = Captures();

	RelevantAttributesToCapture.Add(C.WeaponDamageMultiplierDef);
	RelevantAttributesToCapture.Add(C.SpellPowerMultiplierDef);

	RelevantAttributesToCapture.Add(C.CritChanceDef);
	RelevantAttributesToCapture.Add(C.CritDamageDef);
	RelevantAttributesToCapture.Add(C.ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(C.ElementPenetrationDef);

	RelevantAttributesToCapture.Add(C.PhysicalResistDef);
	RelevantAttributesToCapture.Add(C.MagicResistDef);
	RelevantAttributesToCapture.Add(C.ElementalResistDef);
	RelevantAttributesToCapture.Add(C.FireResistDef);
	RelevantAttributesToCapture.Add(C.FrostResistDef);
	RelevantAttributesToCapture.Add(C.LightningResistDef);
	RelevantAttributesToCapture.Add(C.ArmorDef);
}

// ---------------------------------------------------------
// Random (server-only roll is fine)
// ---------------------------------------------------------
static bool SR_RollChancePct(float ChancePct)
{
	ChancePct = FMath::Clamp(ChancePct, 0.f, 100.f);
	const float Roll = FMath::FRandRange(0.f, 100.f);
	return Roll <= ChancePct;
}

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FCaptureDefs& C = Captures();
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!SourceASC || !TargetASC)
	{
		return;
	}

	const FGameplayTagContainer& SpecDynTags = Spec.GetDynamicAssetTags();

	// Prefer spec dynamic tags for type
	const bool bIsSpell = SpellRiseTags::DamageType_Spell().IsValid() && SpecDynTags.HasTagExact(SpellRiseTags::DamageType_Spell());
	const bool bIsMelee = (!bIsSpell) && SpellRiseTags::DamageType_Melee().IsValid() && SpecDynTags.HasTagExact(SpellRiseTags::DamageType_Melee());

	const TCHAR* TypeStr =
		bIsSpell ? TEXT("Spell") :
		(bIsMelee ? TEXT("Melee") : TEXT("Weapon"));

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Captured source stats
	float WeaponDamageMult = 1.f;
	float SpellPowerMult = 1.f;

	float CritChance01 = 0.f;     // 0..1
	float CritDamageMult = 1.5f;  // multiplier (1.5..2.0)
	float ArmorPenPct = 0.f;      // percent 0..100
	float ElemPenPct = 0.f;       // percent 0..100

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.WeaponDamageMultiplierDef, Params, WeaponDamageMult);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.SpellPowerMultiplierDef, Params, SpellPowerMult);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.CritChanceDef, Params, CritChance01);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.CritDamageDef, Params, CritDamageMult);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ArmorPenetrationDef, Params, ArmorPenPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ElementPenetrationDef, Params, ElemPenPct);

	WeaponDamageMult = FMath::Clamp(WeaponDamageMult, 0.f, 10.f);
	SpellPowerMult   = FMath::Clamp(SpellPowerMult, 0.f, 10.f);

	// DESIGN: soft cap 25% crit chance, 200% crit damage
	CritChance01     = FMath::Clamp(CritChance01, 0.f, 0.25f);
	CritDamageMult   = FMath::Clamp(CritDamageMult, 1.f, 2.0f);

	ArmorPenPct      = FMath::Clamp(ArmorPenPct, 0.f, 75.f);
	ElemPenPct       = FMath::Clamp(ElemPenPct, 0.f, 75.f);

	// Base + optional scaling
	const float BaseDamage = FMath::Max(
		0.f,
		bIsSpell
			? SpellRiseTags::GetSBC_WithFallback(Spec, SpellRiseTags::Data_BaseSpellDamage(), SpellRiseTags::Data_BaseDamage_Legacy(), false, 0.f)
			: SpellRiseTags::GetSBC_WithFallback(Spec, SpellRiseTags::Data_BaseWeaponDamage(), SpellRiseTags::Data_BaseDamage_Legacy(), false, 0.f)
	);

	const float DamageScaling = FMath::Max(
		0.f,
		SpellRiseTags::GetSBC_WithFallback(Spec, SpellRiseTags::Data_DamageScaling(), SpellRiseTags::Data_DamageMultiplier_Legacy(), false, 1.f)
	);

	const float ForceCrit = SpellRiseTags::Data_ForceCrit().IsValid()
		? Spec.GetSetByCallerMagnitude(SpellRiseTags::Data_ForceCrit(), false, 0.f)
		: 0.f;

	const float PowerMultiplier = bIsSpell ? SpellPowerMult : WeaponDamageMult;

	float FinalDamage = BaseDamage * PowerMultiplier * DamageScaling;
	if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f) FinalDamage = 0.f;

	// Target defenses
	float PhysicalResistPct = 0.f, MagicResistPct = 0.f, Armor = 0.f;
	float ElementalResistBasePct = 0.f;
	float FireResistPct = 0.f;
	float FrostResistPct = 0.f;
	float LightningResistPct = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.PhysicalResistDef, Params, PhysicalResistPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.MagicResistDef, Params, MagicResistPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ElementalResistDef, Params, ElementalResistBasePct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.FireResistDef, Params, FireResistPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.FrostResistDef, Params, FrostResistPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.LightningResistDef, Params, LightningResistPct);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ArmorDef, Params, Armor);

	PhysicalResistPct = FMath::Clamp(PhysicalResistPct, 0.f, 75.f);
	MagicResistPct = FMath::Clamp(MagicResistPct, 0.f, 75.f);
	ElementalResistBasePct = FMath::Clamp(ElementalResistBasePct, 0.f, 75.f);
	FireResistPct = FMath::Clamp(FireResistPct, 0.f, 75.f);
	FrostResistPct = FMath::Clamp(FrostResistPct, 0.f, 75.f);
	LightningResistPct = FMath::Clamp(LightningResistPct, 0.f, 75.f);
	Armor = FMath::Max(0.f, Armor);

	const float ArmorPen01 = FMath::Clamp(ArmorPenPct / 100.f, 0.f, 0.75f);
	const float ElemPen01  = FMath::Clamp(ElemPenPct  / 100.f, 0.f, 0.75f);

	const float EffectiveArmor = Armor * (1.f - ArmorPen01);

	// Mitigation
	float Mitigation = 0.f;

	if (bIsSpell)
	{
		float SelectedElemResistPct = -1.f;

		if (SpellRiseTags::Damage_Element_Fire().IsValid() && SpecDynTags.HasTagExact(SpellRiseTags::Damage_Element_Fire()))
			SelectedElemResistPct = FireResistPct;
		else if (SpellRiseTags::Damage_Element_Frost().IsValid() && SpecDynTags.HasTagExact(SpellRiseTags::Damage_Element_Frost()))
			SelectedElemResistPct = FrostResistPct;
		else if (SpellRiseTags::Damage_Element_Lightning().IsValid() && SpecDynTags.HasTagExact(SpellRiseTags::Damage_Element_Lightning()))
			SelectedElemResistPct = LightningResistPct;
		else if (SpellRiseTags::Damage_Element_Elemental().IsValid() && SpecDynTags.HasTagExact(SpellRiseTags::Damage_Element_Elemental()))
			SelectedElemResistPct = ElementalResistBasePct;

		const float BasePct =
			(SelectedElemResistPct >= 0.f) ? SelectedElemResistPct :
			(ElementalResistBasePct > 0.f ? ElementalResistBasePct : MagicResistPct);

		float SpellResist01 = FMath::Clamp(BasePct / 100.f, 0.f, 0.75f);
		SpellResist01 *= (1.f - ElemPen01);
		SpellResist01 = FMath::Clamp(SpellResist01, 0.f, 0.75f);

		Mitigation = SpellResist01;
	}
	else
	{
		const float PhysicalResist01 = PhysicalResistPct / 100.f;

		// Armor curve: Armor / (Armor + 200)
		const float ArmorDenom = FMath::Max(1.f, EffectiveArmor + 200.f);
		const float ArmorMitigation = EffectiveArmor / ArmorDenom;

		Mitigation = (ArmorMitigation * 0.70f) + (PhysicalResist01 * 0.30f);
	}

	Mitigation = FMath::Clamp(Mitigation, 0.f, 0.80f);
	FinalDamage *= (1.f - Mitigation);

	if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f) FinalDamage = 0.f;

	// Crit
	bool bCrit = false;
	float CritMult = 1.f;

	if (FinalDamage > 0.f)
	{
		const float CritChancePct = CritChance01 * 100.f;
		bCrit = (ForceCrit > 0.f) ? true : SR_RollChancePct(CritChancePct);

		if (bCrit)
		{
			CritMult = CritDamageMult;
			FinalDamage *= CritMult;
			if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f) FinalDamage = 0.f;
		}
	}

	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UResourceAttributeSet::GetDamageAttribute(),
				EGameplayModOp::Additive,
				FinalDamage
			)
		);
	}
}
