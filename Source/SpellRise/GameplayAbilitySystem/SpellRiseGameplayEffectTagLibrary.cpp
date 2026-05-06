#include "SpellRise/GameplayAbilitySystem/SpellRiseGameplayEffectTagLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

bool USpellRiseGameplayEffectTagLibrary::GameplayEffectHasGrantedTag(const UGameplayEffect* GameplayEffect, FGameplayTag Tag, bool bExactMatch)
{
	if (!GameplayEffect || !Tag.IsValid())
	{
		return false;
	}

	const FGameplayTagContainer& GrantedTags = GameplayEffect->GetGrantedTags();
	return bExactMatch ? GrantedTags.HasTagExact(Tag) : GrantedTags.HasTag(Tag);
}

bool USpellRiseGameplayEffectTagLibrary::GameplayEffectClassHasGrantedTag(TSubclassOf<UGameplayEffect> GameplayEffectClass, FGameplayTag Tag, bool bExactMatch)
{
	const UGameplayEffect* GameplayEffect = GameplayEffectClass ? GameplayEffectClass.GetDefaultObject() : nullptr;
	return GameplayEffectHasGrantedTag(GameplayEffect, Tag, bExactMatch);
}

FGameplayTagContainer USpellRiseGameplayEffectTagLibrary::GetGameplayEffectGrantedTags(const UGameplayEffect* GameplayEffect)
{
	return GameplayEffect ? GameplayEffect->GetGrantedTags() : FGameplayTagContainer();
}

const UGameplayEffect* USpellRiseGameplayEffectTagLibrary::GetGameplayEffectFromActiveHandleSafe(
	FActiveGameplayEffectHandle ActiveHandle,
	bool& bIsActive)
{
	bIsActive = false;

	if (!ActiveHandle.IsValid())
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = ActiveHandle.GetOwningAbilitySystemComponent();
	if (!ASC || !ASC->GetActiveGameplayEffect(ActiveHandle))
	{
		return nullptr;
	}

	bIsActive = true;
	return ASC->GetGameplayEffectCDO(ActiveHandle);
}
