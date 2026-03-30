#pragma once

#include "CoreMinimal.h"
#include "SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "USpellRiseGA_CancelCast.generated.h"

UCLASS()
class SPELLRISE_API USpellRiseGA_CancelCast : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	USpellRiseGA_CancelCast();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
