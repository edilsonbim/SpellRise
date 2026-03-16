// SpellRiseAbilityTagRelationshipMapping.cpp

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h"

// Iterate through the relationship definitions and collect tags to block and cancel
void USpellRiseAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
    for (const FSpellRiseAbilityTagRelationship& Tags : AbilityTagRelationships)
    {
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            if (OutTagsToBlock)
            {
                OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
            }
            if (OutTagsToCancel)
            {
                OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
            }
        }
    }
}

// Iterate through the relationship definitions and collect activation requirements and blocked tags
void USpellRiseAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
    for (const FSpellRiseAbilityTagRelationship& Tags : AbilityTagRelationships)
    {
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            if (OutActivationRequired)
            {
                OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
            }
            if (OutActivationBlocked)
            {
                OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
            }
        }
    }
}

// Determine if any of the ability tags would be cancelled by the action tag according to the mapping
bool USpellRiseAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
    for (const FSpellRiseAbilityTagRelationship& Tags : AbilityTagRelationships)
    {
        // If the ability trying to activate matches the action tag and any of the tags to cancel match the ability tags
        if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
        {
            return true;
        }
    }

    return false;
}