#include "CatalystAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

// 🔥 para chamar o TryProcCatalystIfReady no seu ASC
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

namespace SpellRiseCatalyst
{
	constexpr float CHARGE_MIN = 0.f;
	constexpr float CHARGE_MAX = 100.f;

	constexpr float XP_MIN = 0.f;
	constexpr float LEVEL_MIN = 1.f;
	constexpr float LEVEL_MAX = 3.f;
}

UCatalystAttributeSet::UCatalystAttributeSet()
{
	InitCatalystCharge(0.f);
	InitCatalystXP(0.f);
	InitCatalystLevel(SpellRiseCatalyst::LEVEL_MIN);

	InitCatalystChargeDelta(0.f);
}

void UCatalystAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCatalystAttributeSet, CatalystCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCatalystAttributeSet, CatalystXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCatalystAttributeSet, CatalystLevel, COND_None, REPNOTIFY_Always);
}

void UCatalystAttributeSet::OnRep_CatalystCharge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCatalystAttributeSet, CatalystCharge, OldValue);
}

void UCatalystAttributeSet::OnRep_CatalystXP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCatalystAttributeSet, CatalystXP, OldValue);
}

void UCatalystAttributeSet::OnRep_CatalystLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCatalystAttributeSet, CatalystLevel, OldValue);
}

void UCatalystAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCatalystChargeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseCatalyst::CHARGE_MIN, SpellRiseCatalyst::CHARGE_MAX);
	}
	else if (Attribute == GetCatalystLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, SpellRiseCatalyst::LEVEL_MIN, SpellRiseCatalyst::LEVEL_MAX);
	}
	else if (Attribute == GetCatalystXPAttribute())
	{
		NewValue = FMath::Max(SpellRiseCatalyst::XP_MIN, NewValue);
	}
	else if (Attribute == GetCatalystChargeDeltaAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
	}
}

void UCatalystAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	// -----------------------------
	// META: Delta -> Charge
	// -----------------------------
	if (Attr == GetCatalystChargeDeltaAttribute())
	{
		const float Delta = GetCatalystChargeDelta();
		SetCatalystChargeDelta(0.f);

		if (Delta > 0.f)
		{
			const float NewCharge = FMath::Clamp(
				GetCatalystCharge() + Delta,
				SpellRiseCatalyst::CHARGE_MIN,
				SpellRiseCatalyst::CHARGE_MAX
			);
			SetCatalystCharge(NewCharge);
		}
	}

	// -----------------------------
	// Clamp Charge sempre
	// -----------------------------
	if (Attr == GetCatalystChargeAttribute() || Attr == GetCatalystChargeDeltaAttribute())
	{
		const float Clamped = FMath::Clamp(GetCatalystCharge(), SpellRiseCatalyst::CHARGE_MIN, SpellRiseCatalyst::CHARGE_MAX);
		if (!FMath::IsNearlyEqual(GetCatalystCharge(), Clamped))
		{
			SetCatalystCharge(Clamped);
		}

		// -----------------------------
		// PROC (SERVER ONLY)
		// quando chega em 100 -> pede pro ASC ativar GA
		// -----------------------------
		if (ASC->IsOwnerActorAuthoritative() && Clamped >= (SpellRiseCatalyst::CHARGE_MAX - KINDA_SMALL_NUMBER))
		{
			if (USpellRiseAbilitySystemComponent* SRASC = Cast<USpellRiseAbilitySystemComponent>(ASC))
			{
				SRASC->TryProcCatalystIfReady();
			}
		}

		return;
	}

	// -----------------------------
	// clamps restantes
	// -----------------------------
	if (Attr == GetCatalystLevelAttribute())
	{
		SetCatalystLevel(FMath::Clamp(GetCatalystLevel(), SpellRiseCatalyst::LEVEL_MIN, SpellRiseCatalyst::LEVEL_MAX));
	}
	else if (Attr == GetCatalystXPAttribute())
	{
		SetCatalystXP(FMath::Max(SpellRiseCatalyst::XP_MIN, GetCatalystXP()));
	}
}
