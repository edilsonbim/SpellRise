#pragma once

// Componente autoritativo de progressao persistente do jogador.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRiseProgressionComponent.generated.h"

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRiseProgressionLevelEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	FGameplayTag ProgressionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression", meta=(ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	int32 Level = 1;
};

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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetWeaponSkillLevel_Server(FGameplayTag WeaponSkillTag, int32 NewLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression", meta=(ClampMin="1", ClampMax="100", UIMin="1", UIMax="100"))
	bool EnsureWeaponSkillLevelFromTalent_Server(
		UPARAM(meta=(Categories="Progression.Weapon")) FGameplayTag WeaponSkillTag,
		int32 TalentLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Progression")
	bool SetSchoolLevel_Server(FGameplayTag SchoolTag, int32 NewLevel);

protected:
	UPROPERTY(ReplicatedUsing=OnRep_WeaponSkillLevels, EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	TArray<FSpellRiseProgressionLevelEntry> WeaponSkillLevels;

	UPROPERTY(ReplicatedUsing=OnRep_SchoolLevels, EditAnywhere, BlueprintReadOnly, Category="SpellRise|Progression")
	TArray<FSpellRiseProgressionLevelEntry> SchoolLevels;

	UFUNCTION()
	void OnRep_WeaponSkillLevels();

	UFUNCTION()
	void OnRep_SchoolLevels();

private:
	static constexpr int32 MinProgressionLevel = 1;
	static constexpr int32 MaxProgressionLevel = 100;

	static int32 ClampProgressionLevel(int32 Level);
	static int32 FindLevel(const TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag);
	static bool IsChildOfTag(FGameplayTag Tag, const TCHAR* ParentTagName);
	static bool SetLevel(TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag, int32 NewLevel);
	static bool EnsureLevelAtLeast(TArray<FSpellRiseProgressionLevelEntry>& Entries, FGameplayTag Tag, int32 NewLevel);
};
