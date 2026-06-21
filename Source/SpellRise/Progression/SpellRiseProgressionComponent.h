#pragma once

// Componente autoritativo de progressao persistente do jogador.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRiseProgressionComponent.generated.h"

class USpellRiseAbilityDefinition;
class USpellRiseProgressionBalanceData;

UENUM(BlueprintType)
enum class ESpellRiseCombatBooster : uint8
{
	Melee,
	Bow,
	Spell,
	Divine
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseProgressionLevelEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	FGameplayTag ProgressionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	int32 Level = 1;
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseCharacterProgressionSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Progression")
	int32 CharacterLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Progression")
	int32 Experience = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Progression")
	int32 ExperienceRequiredForNextLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Progression")
	int32 TalentPoints = 100;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Progression")
	int32 CraftPoints = 100;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|Progression")
	int32 AttributePoints = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellRiseCharacterProgressionChangedSignature, const FSpellRiseCharacterProgressionSnapshot&, Snapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpellRiseProgressionLevelsChangedSignature);

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseProgressionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetWeaponSkillLevel(FGameplayTag WeaponSkillTag) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetSchoolLevel(FGameplayTag SchoolTag) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetCharacterLevel() const { return CharacterLevel; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetExperience() const { return Experience; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetTalentPoints() const { return TalentPoints; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetCraftPoints() const { return CraftPoints; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetAttributePoints() const { return AttributePoints; }
	int32 GetHighestRewardedCharacterLevel() const { return HighestRewardedCharacterLevel; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	int32 GetCombatBoosterCount(ESpellRiseCombatBooster Booster) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	int32 GetTotalCombatBoosterCount() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	int32 GetActiveCombatBoosterCount(ESpellRiseCombatBooster Booster) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	int32 GetTotalActiveCombatBoosterCount() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	int32 GetNextCombatBoosterCost(ESpellRiseCombatBooster Booster) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	float GetCombatBoosterDamageMultiplier(ESpellRiseCombatBooster Booster) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression|Boosters")
	float GetDivineBoosterHealingMultiplier() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetExperienceRequiredForLevel(int32 TargetLevel) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	int32 GetExperienceRequiredForNextLevel() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Progression")
	FSpellRiseCharacterProgressionSnapshot GetCharacterProgressionSnapshot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool InitializeCharacterProgressionDefaults_Server(bool bForceReset);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetCharacterProgression_Server(int32 NewLevel, int32 NewExperience, int32 NewTalentPoints, int32 NewCraftPoints, int32 NewAttributePoints);

	bool SetCharacterLevelByAdmin_Server(int32 NewLevel);
	bool SetHighestRewardedCharacterLevel_Server(int32 NewLevel);
	void GetCumulativeLevelRewards(int32 RewardedThroughLevel, int32& OutTalentPoints, int32& OutCraftPoints, int32& OutAttributePoints) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool AddExperience_Server(int32 ExperienceAmount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetTalentPoints_Server(int32 NewAmount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool AddTalentPoints_Server(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool AddCraftPoints_Server(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetAttributePoints_Server(int32 NewAmount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool AddAttributePoints_Server(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression|Boosters")
	bool PurchaseCombatBooster_Server(ESpellRiseCombatBooster Booster);

	bool SetCombatBoosterCounts_Server(
		int32 MeleeOwned,
		int32 BowOwned,
		int32 SpellOwned,
		int32 DivineOwned,
		int32 MeleeActive,
		int32 BowActive,
		int32 SpellActive,
		int32 DivineActive);

	bool SetCombatBoosterActive_Server(ESpellRiseCombatBooster Booster, int32 BoosterLevel, bool bActivate);

	const TArray<FSpellRiseProgressionLevelEntry>& GetWeaponSkillLevels() const { return WeaponSkillLevels; }
	const TArray<FSpellRiseProgressionLevelEntry>& GetSchoolLevels() const { return SchoolLevels; }

	void ResetProgressionLevels_Server();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetWeaponSkillLevel_Server(FGameplayTag WeaponSkillTag, int32 NewLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression", meta=(ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	bool EnsureWeaponSkillLevelFromTalent_Server(
		UPARAM(meta=(Categories="Progression.Weapon")) FGameplayTag WeaponSkillTag,
		int32 TalentLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetSchoolLevel_Server(FGameplayTag SchoolTag, int32 NewLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression", meta=(ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	bool EnsureSchoolLevelFromTalent_Server(
		UPARAM(meta=(Categories="Progression.School")) FGameplayTag SchoolTag,
		int32 TalentLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression", meta=(ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	bool EnsureProgressionLevelFromAbilityDefinitionTalent_Server(
		const USpellRiseAbilityDefinition* AbilityDefinition,
		int32 TalentLevel);

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Progression")
	FSpellRiseCharacterProgressionChangedSignature OnCharacterProgressionChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Progression")
	FSpellRiseProgressionLevelsChangedSignature OnWeaponSkillLevelsChanged;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|Progression")
	FSpellRiseProgressionLevelsChangedSignature OnSchoolLevelsChanged;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_WeaponSkillLevels, EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	TArray<FSpellRiseProgressionLevelEntry> WeaponSkillLevels;

	UPROPERTY(ReplicatedUsing=OnRep_SchoolLevels, EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	TArray<FSpellRiseProgressionLevelEntry> SchoolLevels;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	int32 CharacterLevel = 1;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	int32 Experience = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	int32 TalentPoints = 100;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	int32 CraftPoints = 100;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 HighestRewardedCharacterLevel = 1;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 MeleeBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 BowBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 SpellBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 DivineBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 ActiveMeleeBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 ActiveBowBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 ActiveSpellBoosterCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterProgression, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Progression|Boosters")
	uint8 ActiveDivineBoosterCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="2", UIMin="2"))
	int32 MaxCharacterLevel = 999;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Progression")
	TObjectPtr<USpellRiseProgressionBalanceData> ProgressionBalance = nullptr;

	UFUNCTION()
	void OnRep_WeaponSkillLevels();

	UFUNCTION()
	void OnRep_SchoolLevels();

	UFUNCTION()
	void OnRep_CharacterProgression();

private:
	static constexpr int32 MinProgressionLevel = 1;
	static constexpr int32 MaxProgressionLevel = 100;
	static constexpr int32 MaxFallbackCharacterLevel = 999;
	static constexpr int32 DefaultCharacterLevel = 1;
	static constexpr int32 DefaultExperience = 0;
	static constexpr int32 DefaultTalentPoints = 100;
	static constexpr int32 DefaultCraftPoints = 100;
	static constexpr int32 DefaultAttributePoints = 0;
	static constexpr int32 TalentPointsPerLevel = 100;
	static constexpr int32 CraftPointsPerLevel = 100;
	static constexpr int32 AttributePointsPerLevel = 5;
	static constexpr int32 MaxAttributePointGrantLevel = 65;
	static constexpr int32 MaxProgressionCurrency = 1000000;
	static constexpr int32 MaxCombatBoosters = 4;
	static constexpr float DamageBonusPerBooster = 0.05f;
	static constexpr float DivineHealingBonusPerBooster = 0.10f;
	static constexpr float PrimaryAttributeBonusPerActiveBooster = 10.0f;

	static int32 ClampProgressionLevel(int32 Level);
	int32 ClampCharacterLevel(int32 Level) const;
	static int32 ClampProgressionCurrency(int32 Value);
	int32 CalculateCumulativeExperienceForLevel(int32 TargetLevel) const;
	void GetLevelRewards(int32 TargetLevel, int32& OutTalentPoints, int32& OutCraftPoints, int32& OutAttributePoints) const;
	bool HasAuthorityOwner() const;
	void ForceOwnerNetUpdate() const;
	void BroadcastCharacterProgressionChanged();
	static int32 FindLevel(const TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag);
	static bool IsChildOfTag(FGameplayTag Tag, const TCHAR* ParentTagName);
	static bool SetLevel(TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag, int32 NewLevel);
	static bool EnsureLevelAtLeast(TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag, int32 NewLevel);
	void ApplyPrimaryAttributeBoosterDelta(ESpellRiseCombatBooster Booster, int32 CountDelta) const;
};
