// SpellRiseAbilityTagRelationshipMapping.h

/*
 * This data asset defines the relationships between gameplay ability tags for SpellRise.
 *
 * Based on Lyra's ULyraAbilityTagRelationshipMapping, it allows a designer to specify which ability tags
 * implicitly block or cancel other tags and which additional tags are required or blocked when an
 * ability with a given tag activates.  Centralising these rules makes it far easier to reason
 * about how abilities interact instead of burying special cases inside each ability class.
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "SpellRiseAbilityTagRelationshipMapping.generated.h"

/**
 * Struct that defines the relationship between a single ability tag and other ability tags.
 * A single ability can have multiple tags; each tag that appears in the AbilityTagRelationships
 * array below will contribute to the final set of tags to block, cancel, require and block
 * activation.
 */
USTRUCT(BlueprintType)
struct FSpellRiseAbilityTagRelationship
{
    GENERATED_BODY()

    /** The tag that this relationship applies to.  Abilities can have multiple of these. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability, meta = (Categories = "Gameplay.Action"))
    FGameplayTag AbilityTag;

    /** Other ability tags that will be blocked by any ability using this tag. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer AbilityTagsToBlock;

    /** Other ability tags that will be canceled by any ability using this tag. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer AbilityTagsToCancel;

    /** Additional tags that will be implicitly added to the activation required tags of the ability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer ActivationRequiredTags;

    /** Additional tags that will be implicitly added to the activation blocked tags of the ability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer ActivationBlockedTags;
};

/**
 * Data asset that defines how ability tags interact with one another.  It can be set on
 * the SpellRiseAbilitySystemComponent via SetTagRelationshipMapping or via the editor on the
 * component.  When an ability with any of the tags specified in AbilityTagRelationships
 * activates, the relationships are consulted to determine which other tags should be blocked
 * or cancelled and which additional activation tags are implicitly required or blocked.
 */
UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseAbilityTagRelationshipMapping : public UDataAsset
{
    GENERATED_BODY()

private:
    /**
     * List of relationship definitions.  Each entry defines how a particular ability tag interacts
     * with other tags in terms of blocking, cancelling and activation tag requirements.
     */
    UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
    TArray<FSpellRiseAbilityTagRelationship> AbilityTagRelationships;

public:
    /**
     * Given a set of ability tags, accumulate tags to block and cancel into the provided containers.
     *
     * @param AbilityTags         Tags of the ability trying to activate
     * @param OutTagsToBlock      Optional container to append ability tags that should be blocked
     * @param OutTagsToCancel     Optional container to append ability tags that should be cancelled
     */
    void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

    /**
     * Given a set of ability tags, append additional required and blocked activation tags.
     *
     * @param AbilityTags             Tags of the ability trying to activate
     * @param OutActivationRequired   Optional container to append additional required tags
     * @param OutActivationBlocked    Optional container to append additional blocked tags
     */
    void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

    /**
     * Returns true if any of the specified ability tags would be cancelled by the passed-in action tag.
     *
     * @param AbilityTags     Tags of the ability to check
     * @param ActionTag       Tag representing another ability trying to activate
     */
    bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};