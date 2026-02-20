#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

namespace SpellRiseTags
{
	inline const FGameplayTag& Cue_DamageNumber()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Cue.DamageNumber"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_StabilityScalar()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.StabilityScalar"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_CatalystCharge()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.CatalystCharge"), false);
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
	if (!ASC) return;

	const float CurrentMax = MaxAttr.GetCurrentValue();
	if (CurrentMax > 0.f && !FMath::IsNearlyEqual(CurrentMax, NewMaxValue))
	{
		const float CurrentValue = Affected.GetCurrentValue();
		const float NewDelta = (CurrentValue * NewMaxValue / CurrentMax) - CurrentValue;
		ASC->ApplyModToAttributeUnsafe(AffectedProperty, EGameplayModOp::Additive, NewDelta);
	}
}

static void ApplyCatalystChargeIfConfigured(
	UAbilitySystemComponent* ASC,
	const FGameplayEffectContextHandle& Context,
	TSubclassOf<UGameplayEffect> GE_Catalyst_AddCharge,
	float ChargeAmount)
{
	if (!ASC || !ASC->IsOwnerActorAuthoritative() || !GE_Catalyst_AddCharge)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_Catalyst_AddCharge, 1.f, Context.IsValid() ? Context : ASC->MakeEffectContext());
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	if (SpellRiseTags::Data_CatalystCharge().IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(SpellRiseTags::Data_CatalystCharge(), FMath::Max(0.f, ChargeAmount));
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

UResourceAttributeSet::UResourceAttributeSet()
{
	Health = 180.f;
	MaxHealth = 180.f;

	Mana = 180.f;
	MaxMana = 180.f;

	Stamina = 180.f;
	MaxStamina = 180.f;

	HealthRegen = 1.f;
	ManaRegen = 1.f;
	StaminaRegen = 1.f;

	Damage = 0.f;
}

void UResourceAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, CarryWeight, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, ManaRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, StaminaRegen, COND_None, REPNOTIFY_Always);
}

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
	else if (Attribute == GetHealthRegenAttribute() ||
	         Attribute == GetManaRegenAttribute() ||
	         Attribute == GetStaminaRegenAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
	}
}

void UResourceAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	if (!TargetASC->IsOwnerActorAuthoritative())
	{
		if (Data.EvaluatedData.Attribute == GetDamageAttribute())
		{
			SetDamage(0.f);
		}
		return;
	}

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	if (Attr == GetDamageAttribute())
	{
		const float TotalDamage = GetDamage();
		SetDamage(0.f);

		if (TotalDamage <= 0.f)
		{
			return;
		}

		const FGameplayEffectContextHandle Ctx = Data.EffectSpec.GetEffectContext();

		float CatalystScalar = 1.f;
		if (SpellRiseTags::Data_StabilityScalar().IsValid())
		{
			CatalystScalar = Data.EffectSpec.GetSetByCallerMagnitude(SpellRiseTags::Data_StabilityScalar(), false, 1.f);
			CatalystScalar = FMath::Clamp(CatalystScalar, 0.f, 10.f);
		}

		SetHealth(FMath::Clamp(GetHealth() - TotalDamage, 0.f, GetMaxHealth()));

		ApplyCatalystChargeIfConfigured(TargetASC, Ctx, GE_Catalyst_AddCharge, TotalDamage * CatalystScalar);

		if (UAbilitySystemComponent* SourceASC = Ctx.GetOriginalInstigatorAbilitySystemComponent())
		{
			if (SourceASC != TargetASC)
			{
				ApplyCatalystChargeIfConfigured(SourceASC, Ctx, GE_Catalyst_AddCharge, TotalDamage * CatalystScalar);
			}
		}

		if (SpellRiseTags::Cue_DamageNumber().IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.RawMagnitude = TotalDamage;
			TargetASC->ExecuteGameplayCue(SpellRiseTags::Cue_DamageNumber(), CueParams);
		}

		return;
	}
}
void UResourceAttributeSet::OnRep_CarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, CarryWeight, OldValue);
}

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
