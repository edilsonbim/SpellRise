#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "SpellRiseAbilityTagRelationshipMapping.generated.h"

USTRUCT(BlueprintType)
struct FSpellRiseAbilityTagRelationship
{
    GENERATED_BODY()


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability, meta = (Categories = "Gameplay.Action"))
    FGameplayTag AbilityTag;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer AbilityTagsToBlock;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer AbilityTagsToCancel;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer ActivationRequiredTags;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer ActivationBlockedTags;
};

UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseAbilityTagRelationshipMapping : public UDataAsset
{
    GENERATED_BODY()

private:


    UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
    TArray<FSpellRiseAbilityTagRelationship> AbilityTagRelationships;

public:


    void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;



    void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;



    bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};
