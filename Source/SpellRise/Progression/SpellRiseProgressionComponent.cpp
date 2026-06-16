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
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][WeaponLevelRejected] Reason=not_authority Owner=%s Tag=%s Level=%d"),
			*GetNameSafe(GetOwner()),
			*WeaponSkillTag.ToString(),
			NewLevel);
		return false;
	}

	if (!IsChildOfTag(WeaponSkillTag, TEXT("Progression.Weapon")))
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][WeaponLevelRejected] Reason=invalid_weapon_tag Owner=%s Tag=%s Level=%d"),
			*GetNameSafe(GetOwner()),
			*WeaponSkillTag.ToString(),
			NewLevel);
		return false;
	}

	const bool bChanged = SetLevel(WeaponSkillLevels, WeaponSkillTag, NewLevel);
	if (bChanged)
	{
		GetOwner()->ForceNetUpdate();
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::EnsureWeaponSkillLevelFromTalent_Server(FGameplayTag WeaponSkillTag, int32 TalentLevel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][WeaponTalentLevelRejected] Reason=not_authority Owner=%s Tag=%s TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*WeaponSkillTag.ToString(),
			TalentLevel);
		return false;
	}

	if (!IsChildOfTag(WeaponSkillTag, TEXT("Progression.Weapon")))
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][WeaponTalentLevelRejected] Reason=invalid_weapon_tag Owner=%s Tag=%s TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*WeaponSkillTag.ToString(),
			TalentLevel);
		return false;
	}

	const int32 PreviousLevel = GetWeaponSkillLevel(WeaponSkillTag);
	const bool bChanged = EnsureLevelAtLeast(WeaponSkillLevels, WeaponSkillTag, TalentLevel);
	if (bChanged)
	{
		GetOwner()->ForceNetUpdate();
		UE_LOG(LogSpellRiseProgressionComponent, Log,
			TEXT("[Progression][WeaponTalentLevelApplied] Owner=%s Tag=%s Previous=%d New=%d TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*WeaponSkillTag.ToString(),
			PreviousLevel,
			GetWeaponSkillLevel(WeaponSkillTag),
			TalentLevel);
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::SetSchoolLevel_Server(FGameplayTag SchoolTag, int32 NewLevel)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][SchoolLevelRejected] Reason=not_authority Owner=%s Tag=%s Level=%d"),
			*GetNameSafe(GetOwner()),
			*SchoolTag.ToString(),
			NewLevel);
		return false;
	}

	if (!IsChildOfTag(SchoolTag, TEXT("Progression.School")))
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][SchoolLevelRejected] Reason=invalid_school_tag Owner=%s Tag=%s Level=%d"),
			*GetNameSafe(GetOwner()),
			*SchoolTag.ToString(),
			NewLevel);
		return false;
	}

	const bool bChanged = SetLevel(SchoolLevels, SchoolTag, NewLevel);
	if (bChanged)
	{
		GetOwner()->ForceNetUpdate();
	}
	return bChanged;
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

bool USpellRiseProgressionComponent::IsChildOfTag(FGameplayTag Tag, const TCHAR* ParentTagName)
{
	if (!Tag.IsValid() || !ParentTagName)
	{
		return false;
	}

	const FGameplayTag ParentTag = FGameplayTag::RequestGameplayTag(FName(ParentTagName), false);
	return ParentTag.IsValid() && Tag.MatchesTag(ParentTag);
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

bool USpellRiseProgressionComponent::EnsureLevelAtLeast(TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag, int32 NewLevel)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning, TEXT("Progression minimum level rejeitado: tag invalida."));
		return false;
	}

	const int32 ClampedLevel = ClampProgressionLevel(NewLevel);
	for (FSpellRiseProgressionLevelEntry& Entry : Entries)
	{
		if (Entry.ProgressionTag == Tag)
		{
			if (Entry.Level >= ClampedLevel)
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
