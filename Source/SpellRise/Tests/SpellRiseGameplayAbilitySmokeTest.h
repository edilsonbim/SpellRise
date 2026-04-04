#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRiseGameplayAbilitySmokeTest.generated.h"

UCLASS()
class SPELLRISE_API USpellRiseAbilityBroadcastSmokeTestAbility : public USpellRiseGameplayAbility
{
	GENERATED_BODY()

public:
	bool IsInputEventAllowedForActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const
	{
		const AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
		return AvatarActor && AvatarActor->GetLocalRole() == ROLE_AutonomousProxy;
	}
};

UCLASS()
class SPELLRISE_API ASpellRiseAbilityBroadcastSmokeTestAvatar : public AActor
{
	GENERATED_BODY()

public:
	ASpellRiseAbilityBroadcastSmokeTestAvatar()
	{
		bReplicates = true;
	}

	UPROPERTY()
	bool bIsLocallyControlledOverride = false;

	virtual bool IsLocallyControlled() const
	{
		return bIsLocallyControlledOverride;
	}
};
