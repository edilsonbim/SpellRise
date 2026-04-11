// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h"

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

bool USpellRiseAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
    for (const FSpellRiseAbilityTagRelationship& Tags : AbilityTagRelationships)
    {

        if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
        {
            return true;
        }
    }

    return false;
}
