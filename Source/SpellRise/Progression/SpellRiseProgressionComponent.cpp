// Implementacao do componente de progressao persistente do jogador.
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseProgressionComponent, Log, All);

USpellRiseProgressionComponent::USpellRiseProgressionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USpellRiseProgressionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, WeaponSkillLevels, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, SchoolLevels, COND_OwnerOnly, REPNOTIFY_Always);
}

int32 USpellRiseProgressionComponent::GetWeaponSkillLevel(FGameplayTag WeaponSkillTag) const
{
	return FindLevel(WeaponSkillLevels, WeaponSkillTag);
}

int32 USpellRiseProgressionComponent::GetSchoolLevel(FGameplayTag SchoolTag) const
{
	return FindLevel(SchoolLevels, SchoolTag);
}

bool USpellRiseProgressionComponent::SetWeaponSkillLevel_Server(FGameplayTag WeaponSkillTag, int32 NewLevel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	return SetLevel(WeaponSkillLevels, WeaponSkillTag, NewLevel);
}

bool USpellRiseProgressionComponent::SetSchoolLevel_Server(FGameplayTag SchoolTag, int32 NewLevel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	return SetLevel(SchoolLevels, SchoolTag, NewLevel);
}

void USpellRiseProgressionComponent::OnRep_WeaponSkillLevels()
{
}

void USpellRiseProgressionComponent::OnRep_SchoolLevels()
{
}

int32 USpellRiseProgressionComponent::ClampProgressionLevel(int32 Level)
{
	return FMath::Clamp(Level, MinProgressionLevel, MaxProgressionLevel);
}

int32 USpellRiseProgressionComponent::FindLevel(const TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag)
{
	if (!Tag.IsValid())
	{
		return MinProgressionLevel;
	}

	for (const FSpellRiseProgressionLevelEntry& Entry : Entries)
	{
		if (Entry.ProgressionTag == Tag)
		{
			return ClampProgressionLevel(Entry.Level);
		}
	}

	return MinProgressionLevel;
}

bool USpellRiseProgressionComponent::SetLevel(TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag, int32 NewLevel)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning, TEXT("Progression level rejeitado: tag invalida."));
		return false;
	}

	const int32 ClampedLevel = ClampProgressionLevel(NewLevel);
	for (FSpellRiseProgressionLevelEntry& Entry : Entries)
	{
		if (Entry.ProgressionTag == Tag)
		{
			if (Entry.Level == ClampedLevel)
			{
				return false;
			}

			Entry.Level = ClampedLevel;
			return true;
		}
	}

	FSpellRiseProgressionLevelEntry NewEntry;
	NewEntry.ProgressionTag = Tag;
	NewEntry.Level = ClampedLevel;
	Entries.Add(NewEntry);
	return true;
}
