// Implementacao do componente de progressao persistente do jogador.
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"

#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/GameplayAbilitySystem/Data/SpellRiseAbilityDefinition.h"
#include "SpellRise/Progression/SpellRiseProgressionBalanceData.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseProgressionComponent, Log, All);

USpellRiseProgressionComponent::USpellRiseProgressionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USpellRiseProgressionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, WeaponSkillLevels, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, SchoolLevels, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, CharacterLevel, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, Experience, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, TalentPoints, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, CraftPoints, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, AttributePoints, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

int32 USpellRiseProgressionComponent::GetWeaponSkillLevel(FGameplayTag WeaponSkillTag) const
{
	return FindLevel(WeaponSkillLevels, WeaponSkillTag);
}

int32 USpellRiseProgressionComponent::GetSchoolLevel(FGameplayTag SchoolTag) const
{
	return FindLevel(SchoolLevels, SchoolTag);
}

int32 USpellRiseProgressionComponent::GetExperienceRequiredForLevel(int32 TargetLevel) const
{
	return CalculateCumulativeExperienceForLevel(ClampCharacterLevel(TargetLevel));
}

int32 USpellRiseProgressionComponent::GetExperienceRequiredForNextLevel() const
{
	if (CharacterLevel >= ClampCharacterLevel(MaxCharacterLevel))
	{
		return 0;
	}

	return GetExperienceRequiredForLevel(CharacterLevel + 1);
}

FSpellRiseCharacterProgressionSnapshot USpellRiseProgressionComponent::GetCharacterProgressionSnapshot() const
{
	FSpellRiseCharacterProgressionSnapshot Snapshot;
	Snapshot.CharacterLevel = CharacterLevel;
	Snapshot.Experience = Experience;
	Snapshot.ExperienceRequiredForNextLevel = GetExperienceRequiredForNextLevel();
	Snapshot.TalentPoints = TalentPoints;
	Snapshot.CraftPoints = CraftPoints;
	Snapshot.AttributePoints = AttributePoints;
	return Snapshot;
}

bool USpellRiseProgressionComponent::InitializeCharacterProgressionDefaults_Server(bool bForceReset)
{
	if (!HasAuthorityOwner())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][DefaultInitRejected] Reason=not_authority Owner=%s"),
			*GetNameSafe(GetOwner()));
		return false;
	}

	if (!bForceReset
		&& CharacterLevel == DefaultCharacterLevel
		&& Experience == DefaultExperience
		&& TalentPoints >= DefaultTalentPoints
		&& CraftPoints >= DefaultCraftPoints
		&& AttributePoints >= DefaultAttributePoints)
	{
		return false;
	}

	CharacterLevel = DefaultCharacterLevel;
	Experience = DefaultExperience;
	TalentPoints = DefaultTalentPoints;
	CraftPoints = DefaultCraftPoints;
	AttributePoints = DefaultAttributePoints;
	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();

	UE_LOG(LogSpellRiseProgressionComponent, Log,
		TEXT("[Progression][DefaultsInitialized] Owner=%s Level=%d Experience=%d TalentPoints=%d CraftPoints=%d AttributePoints=%d"),
		*GetNameSafe(GetOwner()),
		CharacterLevel,
		Experience,
		TalentPoints,
		CraftPoints,
		AttributePoints);
	return true;
}

bool USpellRiseProgressionComponent::SetCharacterProgression_Server(
	int32 NewLevel,
	int32 NewExperience,
	int32 NewTalentPoints,
	int32 NewCraftPoints,
	int32 NewAttributePoints)
{
	if (!HasAuthorityOwner())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][CharacterProgressionSetRejected] Reason=not_authority Owner=%s Level=%d Experience=%d TalentPoints=%d CraftPoints=%d AttributePoints=%d"),
			*GetNameSafe(GetOwner()),
			NewLevel,
			NewExperience,
			NewTalentPoints,
			NewCraftPoints,
			NewAttributePoints);
		return false;
	}

	const int32 ClampedLevel = ClampCharacterLevel(NewLevel);
	const int32 ClampedExperience = FMath::Max(0, NewExperience);
	const int32 ClampedTalentPoints = ClampProgressionCurrency(NewTalentPoints);
	const int32 ClampedCraftPoints = ClampProgressionCurrency(NewCraftPoints);
	const int32 ClampedAttributePoints = ClampProgressionCurrency(NewAttributePoints);
	const bool bChanged = CharacterLevel != ClampedLevel
		|| Experience != ClampedExperience
		|| TalentPoints != ClampedTalentPoints
		|| CraftPoints != ClampedCraftPoints
		|| AttributePoints != ClampedAttributePoints;

	CharacterLevel = ClampedLevel;
	Experience = ClampedExperience;
	TalentPoints = ClampedTalentPoints;
	CraftPoints = ClampedCraftPoints;
	AttributePoints = ClampedAttributePoints;
	if (bChanged)
	{
		ForceOwnerNetUpdate();
		BroadcastCharacterProgressionChanged();
	}

	return bChanged;
}

bool USpellRiseProgressionComponent::AddExperience_Server(int32 ExperienceAmount)
{
	if (!HasAuthorityOwner())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][ExperienceRejected] Reason=not_authority Owner=%s Amount=%d"),
			*GetNameSafe(GetOwner()),
			ExperienceAmount);
		return false;
	}

	if (ExperienceAmount <= 0)
	{
		return false;
	}

	const int32 PreviousLevel = CharacterLevel;
	const int32 PreviousExperience = Experience;
	Experience = static_cast<int32>(FMath::Clamp<int64>(
		static_cast<int64>(Experience) + static_cast<int64>(ExperienceAmount),
		0,
		TNumericLimits<int32>::Max()));

	const int32 EffectiveMaxLevel = ClampCharacterLevel(MaxCharacterLevel);
	while (CharacterLevel < EffectiveMaxLevel)
	{
		const int32 RequiredExperience = GetExperienceRequiredForLevel(CharacterLevel + 1);
		if (RequiredExperience <= 0 || Experience < RequiredExperience)
		{
			break;
		}

		++CharacterLevel;
		int32 LevelTalentPoints = 0;
		int32 LevelCraftPoints = 0;
		int32 LevelAttributePoints = 0;
		GetLevelRewards(CharacterLevel, LevelTalentPoints, LevelCraftPoints, LevelAttributePoints);
		TalentPoints = ClampProgressionCurrency(TalentPoints + LevelTalentPoints);
		CraftPoints = ClampProgressionCurrency(CraftPoints + LevelCraftPoints);
		AttributePoints = ClampProgressionCurrency(AttributePoints + LevelAttributePoints);
	}

	if (CharacterLevel != PreviousLevel)
	{
		ForceOwnerNetUpdate();
	}
	BroadcastCharacterProgressionChanged();
	UE_LOG(LogSpellRiseProgressionComponent, Log,
		TEXT("[Progression][ExperienceGranted] Owner=%s Added=%d Experience=%d PreviousExperience=%d Level=%d PreviousLevel=%d TalentPoints=%d CraftPoints=%d AttributePoints=%d"),
		*GetNameSafe(GetOwner()),
		ExperienceAmount,
		Experience,
		PreviousExperience,
		CharacterLevel,
		PreviousLevel,
		TalentPoints,
		CraftPoints,
		AttributePoints);
	return true;
}

bool USpellRiseProgressionComponent::SetTalentPoints_Server(int32 NewAmount)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	const int32 ClampedPoints = ClampProgressionCurrency(NewAmount);
	const bool bChanged = TalentPoints != ClampedPoints;
	TalentPoints = ClampedPoints;
	if (bChanged)
	{
		ForceOwnerNetUpdate();
		BroadcastCharacterProgressionChanged();
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::AddTalentPoints_Server(int32 Amount)
{
	if (!HasAuthorityOwner() || Amount <= 0)
	{
		return false;
	}

	TalentPoints = ClampProgressionCurrency(TalentPoints + Amount);
	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();
	return true;
}

bool USpellRiseProgressionComponent::AddCraftPoints_Server(int32 Amount)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	if (Amount <= 0)
	{
		return false;
	}

	CraftPoints = ClampProgressionCurrency(CraftPoints + Amount);
	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();
	return true;
}

bool USpellRiseProgressionComponent::AddAttributePoints_Server(int32 Amount)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	if (Amount <= 0)
	{
		return false;
	}

	AttributePoints = ClampProgressionCurrency(AttributePoints + Amount);
	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();
	return true;
}

void USpellRiseProgressionComponent::ResetProgressionLevels_Server()
{
	if (!HasAuthorityOwner())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][ResetRejected] Reason=not_authority Owner=%s"),
			*GetNameSafe(GetOwner()));
		return;
	}

	const bool bHadLevels = WeaponSkillLevels.Num() > 0 || SchoolLevels.Num() > 0;
	WeaponSkillLevels.Reset();
	SchoolLevels.Reset();
	if (bHadLevels)
	{
		ForceOwnerNetUpdate();
		OnWeaponSkillLevelsChanged.Broadcast();
		OnSchoolLevelsChanged.Broadcast();
	}
}

bool USpellRiseProgressionComponent::SetWeaponSkillLevel_Server(FGameplayTag WeaponSkillTag, int32 NewLevel)
{
	if (!HasAuthorityOwner())
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
		ForceOwnerNetUpdate();
		OnWeaponSkillLevelsChanged.Broadcast();
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::EnsureWeaponSkillLevelFromTalent_Server(FGameplayTag WeaponSkillTag, int32 TalentLevel)
{
	if (!HasAuthorityOwner())
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
		ForceOwnerNetUpdate();
		OnWeaponSkillLevelsChanged.Broadcast();
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
	if (!HasAuthorityOwner())
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
		ForceOwnerNetUpdate();
		OnSchoolLevelsChanged.Broadcast();
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::EnsureSchoolLevelFromTalent_Server(FGameplayTag SchoolTag, int32 TalentLevel)
{
	if (!HasAuthorityOwner())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][SchoolTalentLevelRejected] Reason=not_authority Owner=%s Tag=%s TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*SchoolTag.ToString(),
			TalentLevel);
		return false;
	}

	if (!IsChildOfTag(SchoolTag, TEXT("Progression.School")))
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][SchoolTalentLevelRejected] Reason=invalid_school_tag Owner=%s Tag=%s TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*SchoolTag.ToString(),
			TalentLevel);
		return false;
	}

	const int32 PreviousLevel = GetSchoolLevel(SchoolTag);
	const bool bChanged = EnsureLevelAtLeast(SchoolLevels, SchoolTag, TalentLevel);
	if (bChanged)
	{
		ForceOwnerNetUpdate();
		OnSchoolLevelsChanged.Broadcast();
		UE_LOG(LogSpellRiseProgressionComponent, Log,
			TEXT("[Progression][SchoolTalentLevelApplied] Owner=%s Tag=%s Previous=%d New=%d TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*SchoolTag.ToString(),
			PreviousLevel,
			GetSchoolLevel(SchoolTag),
			TalentLevel);
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::EnsureProgressionLevelFromAbilityDefinitionTalent_Server(
	const USpellRiseAbilityDefinition* AbilityDefinition,
	int32 TalentLevel)
{
	if (!AbilityDefinition)
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][DefinitionTalentLevelRejected] Reason=missing_definition Owner=%s TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			TalentLevel);
		return false;
	}

	bool bChanged = false;
	if (AbilityDefinition->WeaponProgressionTag.IsValid())
	{
		bChanged |= EnsureWeaponSkillLevelFromTalent_Server(AbilityDefinition->WeaponProgressionTag, TalentLevel);
	}

	if (AbilityDefinition->SchoolProgressionTag.IsValid())
	{
		bChanged |= EnsureSchoolLevelFromTalent_Server(AbilityDefinition->SchoolProgressionTag, TalentLevel);
	}

	if (!AbilityDefinition->WeaponProgressionTag.IsValid() && !AbilityDefinition->SchoolProgressionTag.IsValid())
	{
		UE_LOG(LogSpellRiseProgressionComponent, Warning,
			TEXT("[Progression][DefinitionTalentLevelRejected] Reason=missing_progression_tag Owner=%s Definition=%s TalentLevel=%d"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(AbilityDefinition),
			TalentLevel);
	}

	return bChanged;
}

void USpellRiseProgressionComponent::OnRep_WeaponSkillLevels()
{
	OnWeaponSkillLevelsChanged.Broadcast();
}

void USpellRiseProgressionComponent::OnRep_SchoolLevels()
{
	OnSchoolLevelsChanged.Broadcast();
}

void USpellRiseProgressionComponent::OnRep_CharacterProgression()
{
	BroadcastCharacterProgressionChanged();
}

int32 USpellRiseProgressionComponent::ClampProgressionLevel(int32 Level)
{
	return FMath::Clamp(Level, MinProgressionLevel, MaxProgressionLevel);
}

int32 USpellRiseProgressionComponent::ClampCharacterLevel(int32 Level) const
{
	return FMath::Clamp(Level, DefaultCharacterLevel, FMath::Clamp(MaxCharacterLevel, DefaultCharacterLevel, MaxFallbackCharacterLevel));
}

int32 USpellRiseProgressionComponent::ClampProgressionCurrency(int32 Value)
{
	return FMath::Clamp(Value, 0, MaxProgressionCurrency);
}

int32 USpellRiseProgressionComponent::CalculateCumulativeExperienceForLevel(int32 TargetLevel) const
{
	const int32 ClampedTargetLevel = FMath::Max(DefaultCharacterLevel, TargetLevel);
	if (ProgressionBalance)
	{
		FSpellRiseLevelProgressionRow Row;
		if (ProgressionBalance->GetLevelRow(ClampedTargetLevel, Row))
		{
			return FMath::Max(0, Row.RequiredTotalExperience);
		}
	}

	const int32 CompletedLevelSteps = ClampedTargetLevel - DefaultCharacterLevel;
	return 50 * CompletedLevelSteps * (CompletedLevelSteps + 1);
}

void USpellRiseProgressionComponent::GetLevelRewards(
	int32 TargetLevel,
	int32& OutTalentPoints,
	int32& OutCraftPoints,
	int32& OutAttributePoints) const
{
	OutTalentPoints = TalentPointsPerLevel;
	OutCraftPoints = CraftPointsPerLevel;
	OutAttributePoints = TargetLevel <= MaxAttributePointGrantLevel ? AttributePointsPerLevel : 0;

	if (ProgressionBalance)
	{
		FSpellRiseLevelProgressionRow Row;
		if (ProgressionBalance->GetLevelRow(TargetLevel, Row))
		{
			OutTalentPoints = ClampProgressionCurrency(Row.TalentPointsOnLevel);
			OutCraftPoints = ClampProgressionCurrency(Row.CraftPointsOnLevel);
			OutAttributePoints = ClampProgressionCurrency(Row.AttributePointsOnLevel);
		}
	}
}

bool USpellRiseProgressionComponent::HasAuthorityOwner() const
{
	return GetOwner() && GetOwner()->HasAuthority();
}

void USpellRiseProgressionComponent::ForceOwnerNetUpdate() const
{
	if (AActor* Owner = GetOwner())
	{
		Owner->ForceNetUpdate();
	}
}

void USpellRiseProgressionComponent::BroadcastCharacterProgressionChanged()
{
	OnCharacterProgressionChanged.Broadcast(GetCharacterProgressionSnapshot());
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
