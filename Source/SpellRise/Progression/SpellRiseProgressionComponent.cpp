// Implementacao do componente de progressao persistente do jogador.
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"

#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
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
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, MeleeBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, BowBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, SpellBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, DivineBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, ActiveMeleeBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, ActiveBowBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, ActiveSpellBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellRiseProgressionComponent, ActiveDivineBoosterCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

int32 USpellRiseProgressionComponent::GetWeaponSkillLevel(FGameplayTag WeaponSkillTag) const
{
	return FindLevel(WeaponSkillLevels, WeaponSkillTag);
}

int32 USpellRiseProgressionComponent::GetSchoolLevel(FGameplayTag SchoolTag) const
{
	return FindLevel(SchoolLevels, SchoolTag);
}

int32 USpellRiseProgressionComponent::GetCombatBoosterCount(ESpellRiseCombatBooster Booster) const
{
	switch (Booster)
	{
	case ESpellRiseCombatBooster::Melee: return MeleeBoosterCount;
	case ESpellRiseCombatBooster::Bow: return BowBoosterCount;
	case ESpellRiseCombatBooster::Spell: return SpellBoosterCount;
	case ESpellRiseCombatBooster::Divine: return DivineBoosterCount;
	default: return 0;
	}
}

int32 USpellRiseProgressionComponent::GetTotalCombatBoosterCount() const
{
	return MeleeBoosterCount + BowBoosterCount + SpellBoosterCount + DivineBoosterCount;
}

int32 USpellRiseProgressionComponent::GetActiveCombatBoosterCount(ESpellRiseCombatBooster Booster) const
{
	switch (Booster)
	{
	case ESpellRiseCombatBooster::Melee: return ActiveMeleeBoosterCount;
	case ESpellRiseCombatBooster::Bow: return ActiveBowBoosterCount;
	case ESpellRiseCombatBooster::Spell: return ActiveSpellBoosterCount;
	case ESpellRiseCombatBooster::Divine: return ActiveDivineBoosterCount;
	default: return 0;
	}
}

int32 USpellRiseProgressionComponent::GetTotalActiveCombatBoosterCount() const
{
	return ActiveMeleeBoosterCount + ActiveBowBoosterCount + ActiveSpellBoosterCount + ActiveDivineBoosterCount;
}

int32 USpellRiseProgressionComponent::GetNextCombatBoosterCost(ESpellRiseCombatBooster Booster) const
{
	static constexpr int32 Costs[MaxCombatBoosters] = { 200, 400, 800, 1600 };
	const int32 OwnedCount = GetCombatBoosterCount(Booster);
	return OwnedCount >= 0 && OwnedCount < MaxCombatBoosters ? Costs[OwnedCount] : 0;
}

float USpellRiseProgressionComponent::GetCombatBoosterDamageMultiplier(ESpellRiseCombatBooster Booster) const
{
	return 1.0f + static_cast<float>(GetActiveCombatBoosterCount(Booster)) * DamageBonusPerBooster;
}

float USpellRiseProgressionComponent::GetDivineBoosterHealingMultiplier() const
{
	return 1.0f + static_cast<float>(ActiveDivineBoosterCount) * DivineHealingBonusPerBooster;
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
	HighestRewardedCharacterLevel = DefaultCharacterLevel;
	MeleeBoosterCount = 0;
	BowBoosterCount = 0;
	SpellBoosterCount = 0;
	DivineBoosterCount = 0;
	ActiveMeleeBoosterCount = 0;
	ActiveBowBoosterCount = 0;
	ActiveSpellBoosterCount = 0;
	ActiveDivineBoosterCount = 0;
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

bool USpellRiseProgressionComponent::SetHighestRewardedCharacterLevel_Server(int32 NewLevel)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	HighestRewardedCharacterLevel = FMath::Max(
		HighestRewardedCharacterLevel,
		ClampCharacterLevel(NewLevel));
	return true;
}

void USpellRiseProgressionComponent::GetCumulativeLevelRewards(
	int32 RewardedThroughLevel,
	int32& OutTalentPoints,
	int32& OutCraftPoints,
	int32& OutAttributePoints) const
{
	OutTalentPoints = DefaultTalentPoints;
	OutCraftPoints = DefaultCraftPoints;
	OutAttributePoints = DefaultAttributePoints;
	const int32 FinalLevel = ClampCharacterLevel(RewardedThroughLevel);
	for (int32 Level = 2; Level <= FinalLevel; ++Level)
	{
		int32 TalentReward = 0;
		int32 CraftReward = 0;
		int32 AttributeReward = 0;
		GetLevelRewards(Level, TalentReward, CraftReward, AttributeReward);
		OutTalentPoints = ClampProgressionCurrency(OutTalentPoints + TalentReward);
		OutCraftPoints = ClampProgressionCurrency(OutCraftPoints + CraftReward);
		OutAttributePoints = ClampProgressionCurrency(OutAttributePoints + AttributeReward);
	}
}

bool USpellRiseProgressionComponent::SetCharacterLevelByAdmin_Server(int32 NewLevel)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	const int32 ClampedLevel = ClampCharacterLevel(NewLevel);
	if (ClampedLevel > HighestRewardedCharacterLevel)
	{
		for (int32 Level = HighestRewardedCharacterLevel + 1; Level <= ClampedLevel; ++Level)
		{
			int32 TalentReward = 0;
			int32 CraftReward = 0;
			int32 AttributeReward = 0;
			GetLevelRewards(Level, TalentReward, CraftReward, AttributeReward);
			TalentPoints = ClampProgressionCurrency(TalentPoints + TalentReward);
			CraftPoints = ClampProgressionCurrency(CraftPoints + CraftReward);
			AttributePoints = ClampProgressionCurrency(AttributePoints + AttributeReward);
		}
		HighestRewardedCharacterLevel = ClampedLevel;
	}

	CharacterLevel = ClampedLevel;
	Experience = GetExperienceRequiredForLevel(ClampedLevel);
	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();
	return true;
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
		if (CharacterLevel > HighestRewardedCharacterLevel)
		{
			int32 LevelTalentPoints = 0;
			int32 LevelCraftPoints = 0;
			int32 LevelAttributePoints = 0;
			GetLevelRewards(CharacterLevel, LevelTalentPoints, LevelCraftPoints, LevelAttributePoints);
			TalentPoints = ClampProgressionCurrency(TalentPoints + LevelTalentPoints);
			CraftPoints = ClampProgressionCurrency(CraftPoints + LevelCraftPoints);
			AttributePoints = ClampProgressionCurrency(AttributePoints + LevelAttributePoints);
			HighestRewardedCharacterLevel = CharacterLevel;
		}
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

bool USpellRiseProgressionComponent::SetAttributePoints_Server(int32 NewAmount)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	const int32 ClampedPoints = ClampProgressionCurrency(NewAmount);
	const bool bChanged = AttributePoints != ClampedPoints;
	AttributePoints = ClampedPoints;
	if (bChanged)
	{
		ForceOwnerNetUpdate();
		BroadcastCharacterProgressionChanged();
	}
	return bChanged;
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

bool USpellRiseProgressionComponent::PurchaseCombatBooster_Server(ESpellRiseCombatBooster Booster)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	uint8* TargetCount = nullptr;
	switch (Booster)
	{
	case ESpellRiseCombatBooster::Melee: TargetCount = &MeleeBoosterCount; break;
	case ESpellRiseCombatBooster::Bow: TargetCount = &BowBoosterCount; break;
	case ESpellRiseCombatBooster::Spell: TargetCount = &SpellBoosterCount; break;
	case ESpellRiseCombatBooster::Divine: TargetCount = &DivineBoosterCount; break;
	default: return false;
	}

	const int32 TalentPointCost = GetNextCombatBoosterCost(Booster);
	if (TalentPointCost <= 0 || *TargetCount >= MaxCombatBoosters)
	{
		return false;
	}

	if (TalentPoints < TalentPointCost)
	{
		return false;
	}

	TalentPoints -= TalentPointCost;
	++(*TargetCount);
	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();
	return true;
}

bool USpellRiseProgressionComponent::SetCombatBoosterCounts_Server(
	int32 MeleeOwned,
	int32 BowOwned,
	int32 SpellOwned,
	int32 DivineOwned,
	int32 MeleeActive,
	int32 BowActive,
	int32 SpellActive,
	int32 DivineActive)
{
	if (!HasAuthorityOwner())
	{
		return false;
	}

	const int32 SafeMelee = FMath::Clamp(MeleeOwned, 0, MaxCombatBoosters);
	const int32 SafeBow = FMath::Clamp(BowOwned, 0, MaxCombatBoosters);
	const int32 SafeSpell = FMath::Clamp(SpellOwned, 0, MaxCombatBoosters);
	const int32 SafeDivine = FMath::Clamp(DivineOwned, 0, MaxCombatBoosters);
	const int32 SafeActiveMelee = FMath::Clamp(MeleeActive, 0, SafeMelee);
	const int32 SafeActiveBow = FMath::Clamp(BowActive, 0, SafeBow);
	const int32 SafeActiveSpell = FMath::Clamp(SpellActive, 0, SafeSpell);
	const int32 SafeActiveDivine = FMath::Clamp(DivineActive, 0, SafeDivine);
	if (SafeActiveMelee + SafeActiveBow + SafeActiveSpell + SafeActiveDivine > MaxCombatBoosters)
	{
		return false;
	}

	const bool bChanged = MeleeBoosterCount != SafeMelee
		|| BowBoosterCount != SafeBow
		|| SpellBoosterCount != SafeSpell
		|| DivineBoosterCount != SafeDivine
		|| ActiveMeleeBoosterCount != SafeActiveMelee
		|| ActiveBowBoosterCount != SafeActiveBow
		|| ActiveSpellBoosterCount != SafeActiveSpell
		|| ActiveDivineBoosterCount != SafeActiveDivine;
	const int32 MeleeActiveDelta = SafeActiveMelee - ActiveMeleeBoosterCount;
	const int32 BowActiveDelta = SafeActiveBow - ActiveBowBoosterCount;
	const int32 SpellActiveDelta = SafeActiveSpell - ActiveSpellBoosterCount;
	const int32 DivineActiveDelta = SafeActiveDivine - ActiveDivineBoosterCount;
	MeleeBoosterCount = static_cast<uint8>(SafeMelee);
	BowBoosterCount = static_cast<uint8>(SafeBow);
	SpellBoosterCount = static_cast<uint8>(SafeSpell);
	DivineBoosterCount = static_cast<uint8>(SafeDivine);
	ActiveMeleeBoosterCount = static_cast<uint8>(SafeActiveMelee);
	ActiveBowBoosterCount = static_cast<uint8>(SafeActiveBow);
	ActiveSpellBoosterCount = static_cast<uint8>(SafeActiveSpell);
	ActiveDivineBoosterCount = static_cast<uint8>(SafeActiveDivine);
	ApplyPrimaryAttributeBoosterDelta(ESpellRiseCombatBooster::Melee, MeleeActiveDelta);
	ApplyPrimaryAttributeBoosterDelta(ESpellRiseCombatBooster::Bow, BowActiveDelta);
	ApplyPrimaryAttributeBoosterDelta(ESpellRiseCombatBooster::Spell, SpellActiveDelta);
	ApplyPrimaryAttributeBoosterDelta(ESpellRiseCombatBooster::Divine, DivineActiveDelta);
	if (bChanged)
	{
		ForceOwnerNetUpdate();
		BroadcastCharacterProgressionChanged();
	}
	return bChanged;
}

bool USpellRiseProgressionComponent::SetCombatBoosterActive_Server(
	ESpellRiseCombatBooster Booster,
	int32 BoosterLevel,
	bool bActivate)
{
	if (!HasAuthorityOwner() || BoosterLevel < 1 || BoosterLevel > MaxCombatBoosters)
	{
		return false;
	}

	uint8* ActiveCount = nullptr;
	switch (Booster)
	{
	case ESpellRiseCombatBooster::Melee: ActiveCount = &ActiveMeleeBoosterCount; break;
	case ESpellRiseCombatBooster::Bow: ActiveCount = &ActiveBowBoosterCount; break;
	case ESpellRiseCombatBooster::Spell: ActiveCount = &ActiveSpellBoosterCount; break;
	case ESpellRiseCombatBooster::Divine: ActiveCount = &ActiveDivineBoosterCount; break;
	default: return false;
	}

	if (bActivate)
	{
		if (BoosterLevel != static_cast<int32>(*ActiveCount) + 1
			|| BoosterLevel > GetCombatBoosterCount(Booster)
			|| GetTotalActiveCombatBoosterCount() >= MaxCombatBoosters)
		{
			return false;
		}
		++(*ActiveCount);
		ApplyPrimaryAttributeBoosterDelta(Booster, 1);
	}
	else
	{
		if (BoosterLevel != static_cast<int32>(*ActiveCount))
		{
			return false;
		}
		--(*ActiveCount);
		ApplyPrimaryAttributeBoosterDelta(Booster, -1);
	}

	ForceOwnerNetUpdate();
	BroadcastCharacterProgressionChanged();
	return true;
}

void USpellRiseProgressionComponent::ApplyPrimaryAttributeBoosterDelta(
	ESpellRiseCombatBooster Booster,
	int32 CountDelta) const
{
	if (CountDelta == 0)
	{
		return;
	}

	const ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner());
	USpellRiseAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetSpellRiseASC() : nullptr;
	if (!ASC)
	{
		return;
	}

	FGameplayAttribute Attribute;
	switch (Booster)
	{
	case ESpellRiseCombatBooster::Melee: Attribute = UCombatAttributeSet::GetStrengthAttribute(); break;
	case ESpellRiseCombatBooster::Bow: Attribute = UCombatAttributeSet::GetAgilityAttribute(); break;
	case ESpellRiseCombatBooster::Spell: Attribute = UCombatAttributeSet::GetIntelligenceAttribute(); break;
	case ESpellRiseCombatBooster::Divine: Attribute = UCombatAttributeSet::GetWisdomAttribute(); break;
	default: return;
	}

	const float CurrentBase = ASC->GetNumericAttributeBase(Attribute);
	ASC->SetNumericAttributeBase(
		Attribute,
		FMath::Clamp(
			CurrentBase + static_cast<float>(CountDelta) * PrimaryAttributeBonusPerActiveBooster,
			0.0f,
			140.0f));
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
