// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h" // ApplyModToAttributeUnsafe
#include "GameplayTagContainer.h"
#include "GameplayCueNotifyTypes.h" // FGameplayCueParameters
#include "GameplayCueNotifyTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CombatAttributeSet.h"



static void AdjustAttributeForMaxChange(
	UAbilitySystemComponent* ASC,
	const FGameplayAttributeData& Affected,
	const FGameplayAttributeData& MaxAttr,
	float NewMaxValue,
	const FGameplayAttribute& AffectedProperty)
{
	if (!ASC) return;

	const float CurrentMax = MaxAttr.GetCurrentValue();
	if (CurrentMax > 0.f && !FMath::IsNearlyEqual(CurrentMax, NewMaxValue))
	{
		const float CurrentValue = Affected.GetCurrentValue();
		const float NewDelta = (CurrentValue * NewMaxValue / CurrentMax) - CurrentValue;
		ASC->ApplyModToAttributeUnsafe(AffectedProperty, EGameplayModOp::Additive, NewDelta);
	}
}

UBasicAttributeSet::UBasicAttributeSet()
{
	Health = 100.f;
	MaxHealth = 100.f;

	Stamina = 100.f;
	MaxStamina = 100.f;

	Mana = 80.f;
	MaxMana = 80.f;

	Damage = 0.f;

	Shield = 0.f;
	MaxShield = 100.f;

	Carga = 30.f;

	// Resistências agora em PERCENTUAL (0..75)
	PhysicalResistance = 0.f;
	MagicResistance = 0.f;

	Armor = 0.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Carga, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(ASC, Health, MaxHealth, NewValue, GetHealthAttribute());
		NewValue = FMath::Max(1.f, NewValue);
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(ASC, Mana, MaxMana, NewValue, GetManaAttribute());
		NewValue = FMath::Max(0.f, NewValue);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(ASC, Stamina, MaxStamina, NewValue, GetStaminaAttribute());
		NewValue = FMath::Max(0.f, NewValue);
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		AdjustAttributeForMaxChange(ASC, Shield, MaxShield, NewValue, GetShieldAttribute());
		NewValue = FMath::Max(0.f, NewValue);
	}

	// clamps
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxShield());
	}
	else if (Attribute == GetCargaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetPhysicalResistanceAttribute())
	{
		// ✅ agora é percentual
		NewValue = FMath::Clamp(NewValue, 0.f, 75.f);
	}
	else if (Attribute == GetMagicResistanceAttribute())
	{
		// ✅ agora é percentual
		NewValue = FMath::Clamp(NewValue, 0.f, 75.f);
	}
	else if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
	}
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	// Damage meta -> aplica no shield e health
	if (Attr == GetDamageAttribute())
	{
		const float TotalDamage = GetDamage();
		SetDamage(0.f);

		// DamageNumber cue
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			if (ASC && TotalDamage > 0.f)
			{
				FGameplayCueParameters CueParams;
				CueParams.RawMagnitude = TotalDamage;
				CueParams.EffectCauser = Data.EffectSpec.GetEffectContext().GetEffectCauser();
				CueParams.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();

				ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("Cue.DamageNumber")), CueParams);
			}
		}

		float RemainingDamage = TotalDamage;

		// =======================
		// Darkfall cast stability interrupt event
		// =======================
		if (TotalDamage > 0.f)
		{
			AActor* TargetActor = GetOwningActor();
			AActor* InstigatorActor = Data.EffectSpec.GetEffectContext().GetOriginalInstigator();

			// Attacker stats
			float AttackerVigor = 0.f;
			float AttackerAgility = 0.f;
			float BreakBonus = 0.f;

			if (UAbilitySystemComponent* InstASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor))
			{
				AttackerVigor = InstASC->GetNumericAttribute(UCombatAttributeSet::GetVigorAttribute());
				AttackerAgility = InstASC->GetNumericAttribute(UCombatAttributeSet::GetAgilityAttribute());
				BreakBonus = InstASC->GetNumericAttribute(UCombatAttributeSet::GetBreakPowerAttribute()); // optional bonus
			}

			// Per-hit tunables via SetByCaller on the damage GE
			const float WeaponImpact = Data.EffectSpec.GetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag(TEXT("Data.WeaponImpact")),
				false,
				0.f
			);

			const float StabilityScalar = Data.EffectSpec.GetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag(TEXT("Data.StabilityScalar")),
				false,
				0.15f // default
			);

			// Break formula
			const float Break = (AttackerAgility * 0.40f) + (AttackerVigor * 0.30f) + WeaponImpact + BreakBonus;

			// Stability damage
			const float StabilityDamage = (TotalDamage * FMath::Max(0.f, StabilityScalar)) + FMath::Max(0.f, Break);

			if (TargetActor && StabilityDamage > 0.f)
			{
				FGameplayEventData EventData;
				EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Interrupt.Cast"));
				EventData.EventMagnitude = StabilityDamage;
				EventData.Instigator = InstigatorActor;
				EventData.Target = TargetActor;

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					TargetActor,
					EventData.EventTag,
					EventData
				);
			}
		}

		if (GetShield() > 0.f && RemainingDamage > 0.f)
		{
			const float OldShield = GetShield();
			const float NewShield = FMath::Clamp(OldShield - RemainingDamage, 0.f, GetMaxShield());
			RemainingDamage -= (OldShield - NewShield);
			SetShield(NewShield);
		}

		if (RemainingDamage > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - RemainingDamage, 0.f, GetMaxHealth()));
		}

		return;
	}

	// clamps pós
	if (Attr == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Attr == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Attr == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	}
	else if (Attr == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
	}
	else if (Attr == GetPhysicalResistanceAttribute())
	{
		SetPhysicalResistance(FMath::Clamp(GetPhysicalResistance(), 0.f, 75.f));
	}
	else if (Attr == GetMagicResistanceAttribute())
	{
		SetMagicResistance(FMath::Clamp(GetMagicResistance(), 0.f, 75.f));
	}
	else if (Attr == GetArmorAttribute())
	{
		SetArmor(FMath::Max(0.f, GetArmor()));
	}
}

void UBasicAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Health, OldValue);
}
void UBasicAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxHealth, OldValue);
}
void UBasicAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Stamina, OldValue);
}
void UBasicAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxStamina, OldValue);
}
void UBasicAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Mana, OldValue);
}
void UBasicAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxMana, OldValue);
}
void UBasicAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Shield, OldValue);
}
void UBasicAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxShield, OldValue);
}
void UBasicAttributeSet::OnRep_Carga(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Carga, OldValue);
}
void UBasicAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, PhysicalResistance, OldValue);
}
void UBasicAttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MagicResistance, OldValue);
}
void UBasicAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Armor, OldValue);
}
