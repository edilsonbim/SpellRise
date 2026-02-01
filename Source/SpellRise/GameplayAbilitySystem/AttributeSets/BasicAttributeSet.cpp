// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UBasicAttributeSet::UBasicAttributeSet()
{
	// Inventory / Weight
	CarryWeight = 30.f;

	// Defense
	PhysicalResistance = 0.f; // percent 0..75
	MagicResistance = 0.f;    // percent 0..75
	Armor = 0.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, CarryWeight, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCarryWeightAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetPhysicalResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 75.f);
	}
	else if (Attribute == GetMagicResistanceAttribute())
	{
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

	// Post clamps
	if (Attr == GetCarryWeightAttribute())
	{
		SetCarryWeight(FMath::Max(0.f, GetCarryWeight()));
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

/* =======================
   OnRep
   ======================= */

void UBasicAttributeSet::OnRep_CarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, CarryWeight, OldValue);
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
