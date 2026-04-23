#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "SpellRiseGameplayAbility.h"
#include "SpellRiseGameplayAbility_AuthoritativeEffect.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FSpellRiseAuthoritativeSelfEffectEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability", meta=(ClampMin="0.0"))
	float LevelMultiplier = 1.0f;
};

UCLASS(Blueprintable, Abstract)
class SPELLRISE_API USpellRiseGameplayAbility_AuthoritativeEffect : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGameplayAbility_AuthoritativeEffect();

protected:
	virtual void NativeOnSpellExecuted() override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability|Authority")
	bool ApplyConfiguredAuthorityEffectsToSelf();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Ability|Authority")
	bool ApplyAuthorityEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float LevelMultiplier = 1.0f);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|Ability", DisplayName="OnAuthoritySpellExecuted")
	void K2_OnAuthoritySpellExecuted();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Authority")
	TArray<FSpellRiseAuthoritativeSelfEffectEntry> SelfAuthorityEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Ability|Authority")
	bool bRunAuthorityEffectsOnServerOnly = true;
};
