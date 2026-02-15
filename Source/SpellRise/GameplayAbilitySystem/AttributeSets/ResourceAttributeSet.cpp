#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"

// ---------------------------------------------------------
// Gameplay Tags (lazy init)
// ---------------------------------------------------------
namespace SpellRiseTags
{
	inline const FGameplayTag& Cue_DamageNumber()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Cue.DamageNumber"), false);
		return Tag;
	}

	inline const FGameplayTag& Cue_ShieldUp()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldUp"), false);
		return Tag;
	}

	inline const FGameplayTag& Cue_ShieldDown()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.ShieldDown"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_StabilityScalar()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.StabilityScalar"), false);
		return Tag;
	}

	// Optional: set-by-caller for catalyst charge amount (if your GE supports)
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

static void ApplyCatalystChargeIfConfigured(
	UAbilitySystemComponent* ASC,
	const FGameplayEffectContextHandle& Context,
	TSubclassOf<UGameplayEffect> GE_Catalyst_AddCharge,
	float ChargeAmount /*can be 1*/)
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

	// Optional set-by-caller (só funciona se o GE ler esse tag)
	if (SpellRiseTags::Data_CatalystCharge().IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(SpellRiseTags::Data_CatalystCharge(), FMath::Max(0.f, ChargeAmount));
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Health,       COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxHealth,    COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Mana,         COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxMana,      COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Stamina,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxStamina,   COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Shield,       COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxShield,    COND_None, REPNOTIFY_Always);

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

	UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	// HARD RULE: recursos/dano só são aplicados no servidor
	if (!TargetASC->IsOwnerActorAuthoritative())
	{
		// clientes só recebem replicação dos atributos
		if (Data.EvaluatedData.Attribute == GetDamageAttribute())
		{
			// garante que não acumule “Damage” em client (segurança)
			SetDamage(0.f);
		}
		return;
	}

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	// -------------------------
	// DAMAGE META -> apply to Shield/Health (server only)
	// -------------------------
	if (Attr == GetDamageAttribute())
	{
		const float TotalDamage = GetDamage();
		SetDamage(0.f);

		if (TotalDamage <= 0.f)
		{
			return;
		}

		float Remaining = TotalDamage;

		// context (para identificar instigator/source e reutilizar para catalyst)
		const FGameplayEffectContextHandle Ctx = Data.EffectSpec.GetEffectContext();

		// scalar opcional para catalyst (p.ex. estabilidade/impacto)
		float CatalystScalar = 1.f;
		if (SpellRiseTags::Data_StabilityScalar().IsValid())
		{
			CatalystScalar = Data.EffectSpec.GetSetByCallerMagnitude(SpellRiseTags::Data_StabilityScalar(), false, 1.f);
			CatalystScalar = FMath::Clamp(CatalystScalar, 0.f, 10.f);
		}

		// 1) Shield first
		const float OldShield = GetShield();

		if (GetMaxShield() > 0.f && OldShield > 0.f && Remaining > 0.f)
		{
			const float NewShield = FMath::Clamp(OldShield - Remaining, 0.f, GetMaxShield());
			Remaining -= (OldShield - NewShield);
			SetShield(NewShield);
		}

		ApplyShieldCuePolicy(TargetASC, OldShield, GetShield());

		// 2) Health
		if (Remaining > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - Remaining, 0.f, GetMaxHealth()));
		}

		// 3) Catalyst: receiving damage (target)
		ApplyCatalystChargeIfConfigured(TargetASC, Ctx, GE_Catalyst_AddCharge, TotalDamage * CatalystScalar);

		// 4) Catalyst: dealing damage (source) - usa instigator ASC do contexto
		if (UAbilitySystemComponent* SourceASC = Ctx.GetOriginalInstigatorAbilitySystemComponent())
		{
			// evita auto-hit duplicado
			if (SourceASC != TargetASC)
			{
				ApplyCatalystChargeIfConfigured(SourceASC, Ctx, GE_Catalyst_AddCharge, TotalDamage * CatalystScalar);
			}
		}

		// 5) Damage number cue (server triggers; cue replicates)
		if (SpellRiseTags::Cue_DamageNumber().IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.RawMagnitude = TotalDamage;
			TargetASC->ExecuteGameplayCue(SpellRiseTags::Cue_DamageNumber(), CueParams);
		}

		return;
	}

	// -------------------------
	// Clamp shield changes + cues (server only)
	// -------------------------
	if (Attr == GetShieldAttribute())
	{
		// Estimar OldShield pelo delta avaliado (magnitude pode ser negativa/positiva)
		const float OldShield = GetShield() - Data.EvaluatedData.Magnitude;

		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
		ApplyShieldCuePolicy(TargetASC, OldShield, GetShield());
	}
	else if (Attr == GetMaxShieldAttribute())
	{
		const float OldShield = GetShield();

		SetMaxShield(FMath::Max(0.f, GetMaxShield()));
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));

		ApplyShieldCuePolicy(TargetASC, OldShield, GetShield());
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
