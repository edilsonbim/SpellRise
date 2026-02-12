#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

// ---------------------------------------------------------
// Gameplay Tags (LAZY INIT - evita static init no load do módulo)
// OBS: getters retornam const ref para evitar cópias.
// ---------------------------------------------------------
namespace SpellRiseTags
{
	inline const FGameplayTag& Cue_DamageNumber()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Cue.DamageNumber"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline const FGameplayTag& Cue_ShieldUp()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldUp"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline const FGameplayTag& Cue_ShieldDown()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldDown"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline const FGameplayTag& Event_Interrupt_Cast()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Interrupt.Cast"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline const FGameplayTag& Data_WeaponImpact()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.WeaponImpact"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline const FGameplayTag& Data_StabilityScalar()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.StabilityScalar"), /*ErrorIfNotFound=*/false);
		return Tag;
	}
}

static void AdjustForMaxChange(
	UAbilitySystemComponent* ASC,
	const FGameplayAttributeData& Affected,
	const FGameplayAttributeData& MaxAttr,
	float NewMaxValue,
	const FGameplayAttribute& AffectedProperty)
{
	if (!ASC)
	{
		return;
	}

	const float CurrentMax = MaxAttr.GetCurrentValue();
	if (CurrentMax > 0.f && !FMath::IsNearlyEqual(CurrentMax, NewMaxValue))
	{
		const float CurrentValue = Affected.GetCurrentValue();
		const float NewDelta = (CurrentValue * NewMaxValue / CurrentMax) - CurrentValue;
		ASC->ApplyModToAttributeUnsafe(AffectedProperty, EGameplayModOp::Additive, NewDelta);
	}
}

static void ApplyShieldCuePolicy(UAbilitySystemComponent* ASC, float OldShield, float NewShield)
{
	if (!ASC || !ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	const bool bWasActive = (OldShield > 0.f);
	const bool bIsActive  = (NewShield > 0.f);

	auto IsCueActive = [&](const FGameplayTag& CueTag) -> bool
	{
		return CueTag.IsValid() && ASC->HasMatchingGameplayTag(CueTag);
	};

	if (bIsActive)
	{
		if (!IsCueActive(SpellRiseTags::Cue_ShieldUp()))
		{
			ASC->AddGameplayCue(SpellRiseTags::Cue_ShieldUp());
		}

		if (IsCueActive(SpellRiseTags::Cue_ShieldDown()))
		{
			ASC->RemoveGameplayCue(SpellRiseTags::Cue_ShieldDown());
		}

		return;
	}

	if (IsCueActive(SpellRiseTags::Cue_ShieldUp()))
	{
		ASC->RemoveGameplayCue(SpellRiseTags::Cue_ShieldUp());
	}

	if (bWasActive && SpellRiseTags::Cue_ShieldDown().IsValid())
	{
		ASC->ExecuteGameplayCue(SpellRiseTags::Cue_ShieldDown());
	}
}

// ---------------------------------------------------------
// Ctor
// ---------------------------------------------------------
UResourceAttributeSet::UResourceAttributeSet()
{
	Health = 100.f;   MaxHealth = 100.f;
	Mana = 80.f;      MaxMana = 80.f;
	Stamina = 100.f;  MaxStamina = 100.f;

	Shield = 0.f;     MaxShield = 0.f;

	HealthRegen = 1.f;
	ManaRegen = 1.f;
	StaminaRegen = 1.f;

	Damage = 0.f;
}

// ---------------------------------------------------------
// Replication
// ---------------------------------------------------------
void UResourceAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Health,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxHealth,   COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Mana,        COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxMana,     COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Stamina,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxStamina,  COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Shield,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxShield,   COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, HealthRegen,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, ManaRegen,    COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, StaminaRegen, COND_None, REPNOTIFY_Always);
}

// ---------------------------------------------------------
// PreAttributeChange
// ---------------------------------------------------------
void UResourceAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Shield, MaxShield, NewValue, GetShieldAttribute());
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxShield());
	}
	else if (Attribute == GetHealthRegenAttribute() ||
	         Attribute == GetManaRegenAttribute() ||
	         Attribute == GetStaminaRegenAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
	}
}

// ---------------------------------------------------------
// PostGameplayEffectExecute
// ---------------------------------------------------------
void UResourceAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	if (Attr == GetDamageAttribute())
	{
		const float TotalDamage = GetDamage();
		SetDamage(0.f);

		if (TotalDamage <= 0.f)
		{
			return;
		}

		UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
		float Remaining = TotalDamage;

		{
			const float OldShield = GetShield();

			if (GetMaxShield() > 0.f && OldShield > 0.f && Remaining > 0.f)
			{
				const float NewShield = FMath::Clamp(OldShield - Remaining, 0.f, GetMaxShield());
				Remaining -= (OldShield - NewShield);
				SetShield(NewShield);
			}

			const float NewShield = GetShield();
			if (TargetASC)
			{
				ApplyShieldCuePolicy(TargetASC, OldShield, NewShield);
			}
			// -------------------------
			// Catalyst: gain charge when receiving damage (server only)
			// -------------------------
			if (TargetASC && TargetASC->IsOwnerActorAuthoritative() && GE_Catalyst_AddCharge)
			{
				// Use the original effect context if possible (keeps instigator/source data)
				FGameplayEffectContextHandle Ctx = Data.EffectSpec.GetEffectContext();
				if (!Ctx.IsValid())
				{
					Ctx = TargetASC->MakeEffectContext();
				}

				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GE_Catalyst_AddCharge, 1.f, Ctx);
				if (SpecHandle.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}

			// Damage number cue (optional)
			if (TargetASC && TargetASC->IsOwnerActorAuthoritative() && SpellRiseTags::Cue_DamageNumber().IsValid())
			{
				FGameplayCueParameters CueParams;
				CueParams.RawMagnitude = TotalDamage;
				TargetASC->ExecuteGameplayCue(SpellRiseTags::Cue_DamageNumber(), CueParams);
			}

		}

		if (Remaining > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - Remaining, 0.f, GetMaxHealth()));
		}

		return;
	}

	if (Attr == GetShieldAttribute())
	{
		UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();

		const float OldShield = GetShield() - Data.EvaluatedData.Magnitude;

		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
		const float NewShield = GetShield();

		if (TargetASC)
		{
			ApplyShieldCuePolicy(TargetASC, OldShield, NewShield);
		}
	}
	else if (Attr == GetMaxShieldAttribute())
	{
		UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();

		const float OldShield = GetShield();

		SetMaxShield(FMath::Max(0.f, GetMaxShield()));
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));

		const float NewShield = GetShield();

		if (TargetASC)
		{
			ApplyShieldCuePolicy(TargetASC, OldShield, NewShield);
		}
	}
}

// ---------------------------------------------------------
// OnRep
// ---------------------------------------------------------
void UResourceAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Health, OldValue);
}

void UResourceAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxHealth, OldValue);
}

void UResourceAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Mana, OldValue);
}

void UResourceAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxMana, OldValue);
}

void UResourceAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Stamina, OldValue);
}

void UResourceAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxStamina, OldValue);
}

void UResourceAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Shield, OldValue);
}

void UResourceAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxShield, OldValue);
}

void UResourceAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, HealthRegen, OldValue);
}

void UResourceAttributeSet::OnRep_ManaRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, ManaRegen, OldValue);
}

void UResourceAttributeSet::OnRep_StaminaRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, StaminaRegen, OldValue);
}
