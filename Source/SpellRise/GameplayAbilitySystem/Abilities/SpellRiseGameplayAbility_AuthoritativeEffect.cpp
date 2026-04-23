// Cabeçalho de implementação: executa efeitos autoritativos no servidor sem depender de eventos cosméticos.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility_AuthoritativeEffect.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

USpellRiseGameplayAbility_AuthoritativeEffect::USpellRiseGameplayAbility_AuthoritativeEffect()
{
}

void USpellRiseGameplayAbility_AuthoritativeEffect::NativeOnSpellExecuted()
{
	const bool bShouldRunAuthorityEffects = !bRunAuthorityEffectsOnServerOnly || HasServerAuthority();
	if (bShouldRunAuthorityEffects)
	{
		ApplyConfiguredAuthorityEffectsToSelf();
		K2_OnAuthoritySpellExecuted();
	}

	Super::NativeOnSpellExecuted();
}

bool USpellRiseGameplayAbility_AuthoritativeEffect::ApplyConfiguredAuthorityEffectsToSelf()
{
	bool bAppliedAnyEffect = false;

	for (const FSpellRiseAuthoritativeSelfEffectEntry& EffectEntry : SelfAuthorityEffects)
	{
		bAppliedAnyEffect |= ApplyAuthorityEffectToSelf(EffectEntry.GameplayEffectClass, EffectEntry.LevelMultiplier);
	}

	return bAppliedAnyEffect;
}

bool USpellRiseGameplayAbility_AuthoritativeEffect::ApplyAuthorityEffectToSelf(
	TSubclassOf<UGameplayEffect> GameplayEffectClass,
	float LevelMultiplier)
{
	if (!GameplayEffectClass)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	const float AbilityLevel = FMath::Max(1.0f, static_cast<float>(GetAbilityLevel()));
	const float EffectLevel = FMath::Max(0.0f, LevelMultiplier) * AbilityLevel;
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffectClass, EffectLevel);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return false;
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}
