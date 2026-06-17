// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "ExecCalc_Damage.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Equipment/SpellRiseWeaponComponent.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "HAL/IConsoleManager.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseDamage, Log, All);

static TAutoConsoleVariable<int32> CVarSpellRiseDamageDebug(
	TEXT("sr.Damage.Debug"),
	0,
	TEXT("Logs server-side SpellRise damage calculation details when set to 1."),
	ECVF_Default);

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

	inline const FGameplayTag& Data_Damage()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.Damage"), false);
		return Tag;
	}
	inline const FGameplayTag& Data_FallSeverity()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.FallSeverity"), false);
		return Tag;
	}
	inline const FGameplayTag& Data_DamageType_Fall()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageType.Fall"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageChannel_Melee()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageChannel.Melee"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageChannel_Bow()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageChannel.Bow"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageChannel_Spell()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageChannel.Spell"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageChannel_Environment()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageChannel.Environment"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageType_Spell()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Bow()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Bow"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageRule_TrueDamage()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageRule.TrueDamage"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageType_Physical_Slashing()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Physical.Slashing"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Physical_Bashing()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Physical.Bashing"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Physical_Piercing()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Physical.Piercing"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Physical_Impact()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Physical.Impact"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageType_Spell_Fire()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell.Fire"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Spell_Cold()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell.Cold"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Spell_Acid()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell.Acid"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Spell_Shock()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Spell.Shock"), false);
		return Tag;
	}

	inline const FGameplayTag& DamageType_Divine()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Divine"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Curse()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Curse"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Almighty()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Almighty"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Generic()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Generic"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Bleed()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Bleed"), false);
		return Tag;
	}
	inline const FGameplayTag& DamageType_Poison()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("DamageType.Poison"), false);
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

namespace SpellRiseDamageProgression
{
	constexpr float MinimumDamageContribution = 0.50f;
	constexpr float MaximumProgressionBonusContribution = 0.50f;

	static const USpellRiseGameplayAbility* ResolveSpellRiseAbilityFromSpec(const FGameplayEffectSpec& Spec)
	{
		const FGameplayEffectContextHandle& Context = Spec.GetContext();

		if (const UGameplayAbility* AbilityInstance = Context.GetAbilityInstance_NotReplicated())
		{
			if (const USpellRiseGameplayAbility* SpellRiseAbility = Cast<USpellRiseGameplayAbility>(AbilityInstance))
			{
				return SpellRiseAbility;
			}
		}

		if (const UGameplayAbility* AbilityCDO = Context.GetAbility())
		{
			return Cast<USpellRiseGameplayAbility>(AbilityCDO);
		}

		return nullptr;
	}

	static USpellRiseProgressionComponent* ResolveSourceProgressionComponent(const UAbilitySystemComponent* SourceASC)
	{
		if (!SourceASC)
		{
			return nullptr;
		}

		AActor* OwnerActor = SourceASC->GetOwnerActor();
		if (ASpellRisePlayerState* SourcePlayerState = Cast<ASpellRisePlayerState>(OwnerActor))
		{
			return SourcePlayerState->GetProgressionComponent();
		}

		if (OwnerActor)
		{
			if (USpellRiseProgressionComponent* OwnerProgression = OwnerActor->FindComponentByClass<USpellRiseProgressionComponent>())
			{
				return OwnerProgression;
			}
		}

		if (AActor* AvatarActor = SourceASC->GetAvatarActor())
		{
			if (AvatarActor != OwnerActor)
			{
				if (USpellRiseProgressionComponent* AvatarProgression = AvatarActor->FindComponentByClass<USpellRiseProgressionComponent>())
				{
					return AvatarProgression;
				}
			}
		}

		return nullptr;
	}

	static int32 GetProgressionLevel(
		const USpellRiseProgressionComponent* ProgressionComponent,
		const FGameplayTag& ProgressionTag,
		bool bWeapon)
	{
		if (!ProgressionComponent || !ProgressionTag.IsValid())
		{
			return 0;
		}

		const int32 Level = bWeapon
			? ProgressionComponent->GetWeaponSkillLevel(ProgressionTag)
			: ProgressionComponent->GetSchoolLevel(ProgressionTag);

		return FMath::Clamp(Level, 0, 100);
	}

	static float GetProgressionDamageContribution(float RawDamage, int32 ProgressionLevel)
	{
		const float ClampedRawDamage = FMath::Max(0.f, RawDamage);
		const float BaseContribution = ClampedRawDamage * MinimumDamageContribution;
		const float ProgressionAlpha = FMath::Clamp(static_cast<float>(ProgressionLevel) / 100.f, 0.f, 1.f);
		const float ProgressionBonus = FMath::Clamp(
			ClampedRawDamage * MaximumProgressionBonusContribution * ProgressionAlpha,
			0.f,
			ClampedRawDamage * MaximumProgressionBonusContribution);

		return BaseContribution + ProgressionBonus;
	}

	static float ApplyPrimaryAttributeContribution(float Damage, float PrimaryAttribute)
	{
		const float ClampedDamage = FMath::Max(0.f, Damage);
		const float PrimaryAlpha = FMath::Clamp(PrimaryAttribute, 0.f, 100.f) / 100.f;
		return (ClampedDamage * MinimumDamageContribution)
			+ (ClampedDamage * MaximumProgressionBonusContribution * PrimaryAlpha);
	}

	static FGameplayTag ResolveEquippedWeaponProgressionTag(const UAbilitySystemComponent* SourceASC)
	{
		if (!SourceASC)
		{
			return FGameplayTag();
		}

		if (AActor* AvatarActor = SourceASC->GetAvatarActor())
		{
			if (const USpellRiseWeaponComponent* WeaponComponent = AvatarActor->FindComponentByClass<USpellRiseWeaponComponent>())
			{
				return WeaponComponent->GetActiveWeaponProgressionTag();
			}
		}

		return FGameplayTag();
	}
}

UExecCalc_Damage::FCaptureDefs::FCaptureDefs()
{
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

	EquippedWeaponBaseDamageDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetEquippedWeaponBaseDamageAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	StrengthDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetStrengthAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	AgilityDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetAgilityAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetIntelligenceAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	WisdomDef = FGameplayEffectAttributeCaptureDefinition(
		UBasicAttributeSet::GetWisdomAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,
		false);

	SlashingResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetSlashingResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	BashingResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetBashingResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	PiercingResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetPiercingResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	ImpactResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetImpactResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	FireResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetFireResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	ColdResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetColdResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	AcidResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetAcidResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	ShockResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetShockResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	DivineResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetDivineResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	CurseResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetCurseResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	AlmightyResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetAlmightyResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	GenericResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetGenericResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	BleedResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetBleedResAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false);

	PoisonResDef = FGameplayEffectAttributeCaptureDefinition(
		UCombatAttributeSet::GetPoisonResAttribute(),
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

	RelevantAttributesToCapture.Add(C.CritChanceDef);
	RelevantAttributesToCapture.Add(C.CritDamageDef);
	RelevantAttributesToCapture.Add(C.ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(C.EquippedWeaponBaseDamageDef);
	RelevantAttributesToCapture.Add(C.StrengthDef);
	RelevantAttributesToCapture.Add(C.AgilityDef);
	RelevantAttributesToCapture.Add(C.IntelligenceDef);
	RelevantAttributesToCapture.Add(C.WisdomDef);

	RelevantAttributesToCapture.Add(C.SlashingResDef);
	RelevantAttributesToCapture.Add(C.BashingResDef);
	RelevantAttributesToCapture.Add(C.PiercingResDef);
	RelevantAttributesToCapture.Add(C.ImpactResDef);

	RelevantAttributesToCapture.Add(C.FireResDef);
	RelevantAttributesToCapture.Add(C.ColdResDef);
	RelevantAttributesToCapture.Add(C.AcidResDef);
	RelevantAttributesToCapture.Add(C.ShockResDef);

	RelevantAttributesToCapture.Add(C.DivineResDef);
	RelevantAttributesToCapture.Add(C.CurseResDef);
	RelevantAttributesToCapture.Add(C.AlmightyResDef);

	RelevantAttributesToCapture.Add(C.GenericResDef);
	RelevantAttributesToCapture.Add(C.BleedResDef);
	RelevantAttributesToCapture.Add(C.PoisonResDef);
}

static bool SR_RollChancePct(float ChancePct)
{
	ChancePct = FMath::Clamp(ChancePct, 0.f, 100.f);
	const float Roll = FMath::FRandRange(0.f, 100.f);
	return Roll <= ChancePct;
}

static void SR_GetDrainPercentsForDamageTypeTag(const FGameplayTagContainer& SpecDynTags, float& OutStaminaPct, float& OutManaPct)
{
	OutStaminaPct = 0.f;
	OutManaPct = 0.f;

	auto HasExact = [&](const FGameplayTag& T) -> bool
	{
		return T.IsValid() && SpecDynTags.HasTagExact(T);
	};

	if (HasExact(SpellRiseTags::DamageType_Physical_Slashing())) { OutStaminaPct = 15.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Physical_Bashing())) { OutStaminaPct = 20.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Physical_Piercing())) { OutStaminaPct = 20.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Spell_Fire())) { OutStaminaPct = 15.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Spell_Cold())) { OutStaminaPct = 30.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Spell_Acid())) { OutStaminaPct = 20.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Spell_Shock())) { OutStaminaPct = 25.f; return; }

	if (HasExact(SpellRiseTags::DamageType_Divine())) { OutStaminaPct = 15.f; OutManaPct = 20.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Curse())) { OutStaminaPct = 20.f; OutManaPct = 15.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Almighty())) { OutStaminaPct = 0.f; OutManaPct = 0.f; return; }

	if (HasExact(SpellRiseTags::DamageType_Generic())) { OutStaminaPct = 15.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Bleed())) { OutStaminaPct = 20.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Poison())) { OutStaminaPct = 20.f; return; }
	if (HasExact(SpellRiseTags::DamageType_Physical_Impact())) { OutStaminaPct = 15.f; return; }

	OutStaminaPct = 15.f;
	OutManaPct = 0.f;
}

static bool SR_IsPhysicalDamageType(const FGameplayTagContainer& SpecDynTags)
{
	auto HasExact = [&](const FGameplayTag& T) -> bool
	{
		return T.IsValid() && SpecDynTags.HasTagExact(T);
	};

	return HasExact(SpellRiseTags::DamageType_Physical_Slashing())
		|| HasExact(SpellRiseTags::DamageType_Physical_Bashing())
		|| HasExact(SpellRiseTags::DamageType_Physical_Piercing())
		|| HasExact(SpellRiseTags::DamageType_Physical_Impact());
}

static float SR_GetResistPctForDamageType(
	const FGameplayTagContainer& SpecDynTags,
	const UExecCalc_Damage::FCaptureDefs& C,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& Params)
{
	auto HasExact = [&](const FGameplayTag& T) -> bool
	{
		return T.IsValid() && SpecDynTags.HasTagExact(T);
	};

	float ResistPct = 0.f;

	if (HasExact(SpellRiseTags::DamageType_Physical_Slashing()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.SlashingResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Physical_Bashing()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.BashingResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Physical_Piercing()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.PiercingResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Physical_Impact()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ImpactResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Spell_Fire()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.FireResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Spell_Cold()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ColdResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Spell_Acid()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.AcidResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Spell_Shock()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ShockResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Divine()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.DivineResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Curse()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.CurseResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Almighty()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.AlmightyResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Bleed()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.BleedResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Poison()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.PoisonResDef, Params, ResistPct);
	else if (HasExact(SpellRiseTags::DamageType_Generic()))
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.GenericResDef, Params, ResistPct);
	else
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.GenericResDef, Params, ResistPct);

	return FMath::Clamp(ResistPct, 0.f, 75.f);
}

static float SR_GetFallResistPct(
	const UExecCalc_Damage::FCaptureDefs& C,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& Params)
{
	float ResistPct = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ImpactResDef, Params, ResistPct);
	return FMath::Clamp(ResistPct, 0.f, 75.f);
}

static const TCHAR* SR_ChannelToText(int32 Channel)
{
	switch (Channel)
	{
	case 1: return TEXT("Melee");
	case 2: return TEXT("Bow");
	case 3: return TEXT("Spell");
	case 4: return TEXT("Environment");
	default: return TEXT("Unknown");
	}
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
	const USpellRiseGameplayAbility* SourceAbility = SpellRiseDamageProgression::ResolveSpellRiseAbilityFromSpec(Spec);
	FGameplayTagContainer EffectiveDamageTags = SpecDynTags;
	if (SourceAbility)
	{
		if (SourceAbility->DamageChannelTag.IsValid())
		{
			EffectiveDamageTags.AddTag(SourceAbility->DamageChannelTag);
		}
		if (SourceAbility->DamageTypeTag.IsValid())
		{
			EffectiveDamageTags.AddTag(SourceAbility->DamageTypeTag);
		}
	}

	const float FallTagMagnitude = SpellRiseTags::Data_DamageType_Fall().IsValid()
		? Spec.GetSetByCallerMagnitude(SpellRiseTags::Data_DamageType_Fall(), false, -1.f)
		: -1.f;

	const float FallSeverity = SpellRiseTags::Data_FallSeverity().IsValid()
		? Spec.GetSetByCallerMagnitude(SpellRiseTags::Data_FallSeverity(), false, -1.f)
		: -1.f;

	const float FallBaseDamage = SpellRiseTags::Data_Damage().IsValid()
		? Spec.GetSetByCallerMagnitude(SpellRiseTags::Data_Damage(), false, -1.f)
		: -1.f;

	const bool bIsFallDamage = FallTagMagnitude > 0.f;

	const bool bChanMelee = SpellRiseTags::DamageChannel_Melee().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageChannel_Melee());
	const bool bChanBow = SpellRiseTags::DamageChannel_Bow().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageChannel_Bow());
	const bool bChanSpell = SpellRiseTags::DamageChannel_Spell().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageChannel_Spell());
	const bool bChanEnv = SpellRiseTags::DamageChannel_Environment().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageChannel_Environment());

	const bool bLegacySpell = SpellRiseTags::DamageType_Spell().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageType_Spell());
	const bool bLegacyBow = SpellRiseTags::DamageType_Bow().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageType_Bow());

	const bool bTrueDamage = SpellRiseTags::DamageRule_TrueDamage().IsValid()
		&& EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageRule_TrueDamage());

	int32 Channel = 0;
	if (bIsFallDamage) Channel = 4;
	else if (bChanSpell) Channel = 3;
	else if (bChanBow) Channel = 2;
	else if (bChanMelee) Channel = 1;
	else if (bChanEnv) Channel = 4;
	else if (bLegacySpell) Channel = 3;
	else if (bLegacyBow) Channel = 2;

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float CritChance01 = 0.05f;
	float CritDamageMult = 1.5f;
	float ArmorPenPct = 0.f;
	float EquippedWeaponBaseDamage = 0.f;
	float Strength = 20.f;
	float Agility = 20.f;
	float Intelligence = 20.f;
	float Wisdom = 20.f;

	{
		float Tmp = 0.f;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.CritChanceDef, Params, Tmp)) CritChance01 = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.CritDamageDef, Params, Tmp)) CritDamageMult = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.ArmorPenetrationDef, Params, Tmp)) ArmorPenPct = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.EquippedWeaponBaseDamageDef, Params, Tmp)) EquippedWeaponBaseDamage = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.StrengthDef, Params, Tmp)) Strength = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.AgilityDef, Params, Tmp)) Agility = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.IntelligenceDef, Params, Tmp)) Intelligence = Tmp;
		if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(C.WisdomDef, Params, Tmp)) Wisdom = Tmp;
	}

	CritChance01 = FMath::Clamp(CritChance01, 0.f, 0.25f);
	CritDamageMult = FMath::Clamp(CritDamageMult, 1.f, 2.0f);
	EquippedWeaponBaseDamage = FMath::Clamp(EquippedWeaponBaseDamage, 0.f, 10000.f);


	const float ArmorPen01 = FMath::Clamp(ArmorPenPct, 0.f, 0.30f);

	float BaseDamage = 0.f;
	float DamageScaling = 1.f;
	float FinalDamage = 0.f;
	float PreMitigationDamage = 0.f;
	float AppliedResist01 = 0.f;
	bool bAppliedArmorPenetration = false;
	int32 AbilityLevelForDebug = 0;
	int32 WeaponLevelForDebug = 0;
	int32 SchoolLevelForDebug = 0;
	float AbilityDamageContribution = 0.f;
	float WeaponDamageContribution = 0.f;
	float PreAttributeDamage = 0.f;
	float PrimaryAttributeRawForDebug = 0.f;
	float PrimaryAttributeClampedForDebug = 0.f;
	const TCHAR* PrimaryAttributeNameForDebug = TEXT("None");
	FGameplayTag EffectiveWeaponProgressionTag;

	if (bIsFallDamage)
	{
		BaseDamage = FMath::Max(0.f, FallBaseDamage);
		FinalDamage = BaseDamage;

		if (FinalDamage <= 0.f)
		{
			return;
		}

		float ResistPct = SR_GetFallResistPct(C, ExecutionParams, Params);
		float Resist01 = FMath::Clamp(ResistPct / 100.f, 0.f, 0.75f);
		AppliedResist01 = Resist01;
		PreMitigationDamage = FinalDamage;

		FinalDamage *= (1.f - Resist01);
		if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f)
		{
			FinalDamage = 0.f;
		}

		if (FinalDamage <= 0.f)
		{
			return;
		}

		if (CVarSpellRiseDamageDebug.GetValueOnAnyThread() != 0)
		{
			UE_LOG(
				LogSpellRiseDamage,
				Log,
				TEXT("[DamageCalc] Source=%s Target=%s Channel=%s FallBase=%.2f PreMitigation=%.2f Resist=%.2f Final=%.2f"),
				*GetNameSafe(SourceASC->GetOwnerActor()),
				*GetNameSafe(TargetASC->GetOwnerActor()),
				SR_ChannelToText(Channel),
				BaseDamage,
				PreMitigationDamage,
				AppliedResist01,
				FinalDamage);
		}

		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UResourceAttributeSet::GetDamageAttribute(),
				EGameplayModOp::Additive,
				FinalDamage));

		return;
	}

	const bool bUseSpellBase = (Channel == 3);
	const bool bUsesEquippedWeaponDamage = SourceAbility && SourceAbility->bUsesEquippedWeaponDamage;
	const float SetByCallerBaseDamage = FMath::Max(
		0.f,
		bUseSpellBase
			? SpellRiseTags::GetSBC_WithFallback(Spec, SpellRiseTags::Data_BaseSpellDamage(), SpellRiseTags::Data_BaseDamage_Legacy(), false, 0.f)
			: SpellRiseTags::GetSBC_WithFallback(Spec, SpellRiseTags::Data_BaseWeaponDamage(), SpellRiseTags::Data_BaseDamage_Legacy(), false, 0.f)
	);

	BaseDamage = SetByCallerBaseDamage;

	DamageScaling = FMath::Max(
		0.f,
		SpellRiseTags::GetSBC_WithFallback(Spec, SpellRiseTags::Data_DamageScaling(), SpellRiseTags::Data_DamageMultiplier_Legacy(), false, 1.f)
	);

	const float ForceCrit = SpellRiseTags::Data_ForceCrit().IsValid()
		? Spec.GetSetByCallerMagnitude(SpellRiseTags::Data_ForceCrit(), false, 0.f)
		: 0.f;

	if (SourceAbility)
	{
		const int32 AbilityLevel = FMath::Clamp(FMath::RoundToInt(Spec.GetLevel()), 1, 100);
		const USpellRiseProgressionComponent* ProgressionComponent =
			SpellRiseDamageProgression::ResolveSourceProgressionComponent(SourceASC);
		EffectiveWeaponProgressionTag = bUsesEquippedWeaponDamage
			? SpellRiseDamageProgression::ResolveEquippedWeaponProgressionTag(SourceASC)
			: SourceAbility->WeaponProgressionTag;
		const int32 WeaponLevel = SpellRiseDamageProgression::GetProgressionLevel(
			ProgressionComponent,
			EffectiveWeaponProgressionTag,
			true);
		const int32 SchoolLevel = SpellRiseDamageProgression::GetProgressionLevel(
			ProgressionComponent,
			SourceAbility->SchoolProgressionTag,
			false);
		AbilityLevelForDebug = AbilityLevel;
		WeaponLevelForDebug = WeaponLevel;
		SchoolLevelForDebug = SchoolLevel;

		AbilityDamageContribution = SpellRiseDamageProgression::GetProgressionDamageContribution(SetByCallerBaseDamage, SchoolLevel);
		if (bUsesEquippedWeaponDamage)
		{
			WeaponDamageContribution = SpellRiseDamageProgression::GetProgressionDamageContribution(EquippedWeaponBaseDamage, WeaponLevel);
		}

		BaseDamage = AbilityDamageContribution + WeaponDamageContribution;
	}
	else
	{
		AbilityDamageContribution = SetByCallerBaseDamage;
		WeaponDamageContribution = bUsesEquippedWeaponDamage ? EquippedWeaponBaseDamage : 0.f;
		BaseDamage = AbilityDamageContribution + WeaponDamageContribution;
	}

	FinalDamage = BaseDamage * DamageScaling;
	PreAttributeDamage = FinalDamage;

	if (SpellRiseTags::DamageType_Divine().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageType_Divine()))
	{
		PrimaryAttributeRawForDebug = Wisdom;
		PrimaryAttributeNameForDebug = TEXT("WIS");
	}
	else if (Channel == 1)
	{
		PrimaryAttributeRawForDebug = Strength;
		PrimaryAttributeNameForDebug = TEXT("STR");
	}
	else if (Channel == 2)
	{
		PrimaryAttributeRawForDebug = Agility;
		PrimaryAttributeNameForDebug = TEXT("AGI");
	}
	else if (Channel == 3)
	{
		PrimaryAttributeRawForDebug = Intelligence;
		PrimaryAttributeNameForDebug = TEXT("INT");
	}

	PrimaryAttributeClampedForDebug = FMath::Clamp(PrimaryAttributeRawForDebug, 0.f, 100.f);
	if (Channel == 1 || Channel == 2 || Channel == 3
		|| (SpellRiseTags::DamageType_Divine().IsValid() && EffectiveDamageTags.HasTagExact(SpellRiseTags::DamageType_Divine())))
	{
		FinalDamage = SpellRiseDamageProgression::ApplyPrimaryAttributeContribution(FinalDamage, PrimaryAttributeRawForDebug);
	}

	PreMitigationDamage = FinalDamage;
	if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f) FinalDamage = 0.f;

	if (FinalDamage <= 0.f)
	{
		return;
	}

	if (!bTrueDamage)
	{
		float ResistPct = SR_GetResistPctForDamageType(EffectiveDamageTags, C, ExecutionParams, Params);
		float Resist01 = FMath::Clamp(ResistPct / 100.f, 0.f, 0.75f);

		if (SR_IsPhysicalDamageType(EffectiveDamageTags))
		{
			Resist01 = Resist01 * (1.f - ArmorPen01);
			Resist01 = FMath::Clamp(Resist01, 0.f, 0.75f);
			bAppliedArmorPenetration = true;
		}
		AppliedResist01 = Resist01;

		FinalDamage *= (1.f - Resist01);
		if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f) FinalDamage = 0.f;

		if (FinalDamage <= 0.f)
		{
			return;
		}
	}

	bool bCrit = false;
	if (!bTrueDamage && FinalDamage > 0.f)
	{
		const float CritChancePct = CritChance01 * 100.f;
		bCrit = (ForceCrit > 0.f) ? true : SR_RollChancePct(CritChancePct);

		if (bCrit)
		{
			FinalDamage *= CritDamageMult;
			if (!FMath::IsFinite(FinalDamage) || FinalDamage < 0.f) FinalDamage = 0.f;
		}
	}

	if (FinalDamage <= 0.f)
	{
		return;
	}

	if (CVarSpellRiseDamageDebug.GetValueOnAnyThread() != 0)
	{
		UE_LOG(
			LogSpellRiseDamage,
			Log,
			TEXT("[DamageCalc] Source=%s Target=%s Ability=%s Channel=%s AbilityRaw=%.2f AbilityContribution=%.2f UsesWeapon=%s WeaponRaw=%.2f WeaponContribution=%.2f Base=%.2f AbilityLevel=%d AbilityLevelAffectsDamage=false WeaponTag=%s WeaponLevel=%d SchoolTag=%s SchoolLevel=%d Scaling=%.3f PreAttribute=%.2f PrimaryAttribute=%s PrimaryRaw=%.2f PrimaryClamped=%.2f PreMitigation=%.2f TrueDamage=%s Resist=%.2f ArmorPen=%.2f ArmorPenApplied=%s Crit=%s CritMult=%.2f Final=%.2f"),
			*GetNameSafe(SourceASC->GetOwnerActor()),
			*GetNameSafe(TargetASC->GetOwnerActor()),
			*GetNameSafe(SourceAbility),
			SR_ChannelToText(Channel),
			SetByCallerBaseDamage,
			AbilityDamageContribution,
			bUsesEquippedWeaponDamage ? TEXT("true") : TEXT("false"),
			EquippedWeaponBaseDamage,
			WeaponDamageContribution,
			BaseDamage,
			AbilityLevelForDebug,
			*EffectiveWeaponProgressionTag.ToString(),
			WeaponLevelForDebug,
			SourceAbility ? *SourceAbility->SchoolProgressionTag.ToString() : TEXT("None"),
			SchoolLevelForDebug,
			DamageScaling,
			PreAttributeDamage,
			PrimaryAttributeNameForDebug,
			PrimaryAttributeRawForDebug,
			PrimaryAttributeClampedForDebug,
			PreMitigationDamage,
			bTrueDamage ? TEXT("true") : TEXT("false"),
			AppliedResist01,
			ArmorPen01,
			bAppliedArmorPenetration ? TEXT("true") : TEXT("false"),
			bCrit ? TEXT("true") : TEXT("false"),
			CritDamageMult,
			FinalDamage);
	}

	if (bCrit)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UResourceAttributeSet::GetDamageWasCriticalAttribute(),
				EGameplayModOp::Additive,
				1.f));
	}

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UResourceAttributeSet::GetDamageAttribute(),
			EGameplayModOp::Additive,
			FinalDamage));

	float StaminaDrainPct = 0.f;
	float ManaDrainPct = 0.f;
	SR_GetDrainPercentsForDamageTypeTag(EffectiveDamageTags, StaminaDrainPct, ManaDrainPct);

	const float StaminaDrain = (StaminaDrainPct > 0.f) ? (FinalDamage * (StaminaDrainPct / 100.f)) : 0.f;
	const float ManaDrain = (ManaDrainPct > 0.f) ? (FinalDamage * (ManaDrainPct / 100.f)) : 0.f;

	if (StaminaDrain > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UResourceAttributeSet::GetStaminaAttribute(),
				EGameplayModOp::Additive,
				-StaminaDrain));
	}

	if (ManaDrain > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				UResourceAttributeSet::GetManaAttribute(),
				EGameplayModOp::Additive,
				-ManaDrain));
	}
}
