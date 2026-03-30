#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRise/Core/SpellRiseChatTypes.h"
#include "SpellRiseChatComponent.generated.h"

class ASpellRisePlayerController;

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseChatComponent();

	// Keep this signature compatible with existing BP graphs.
	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SendToMULTICAST(FName Name, const FText& Text, const FText& TimeText, uint8 Channel);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SendCombatToPlayer(ASpellRisePlayerController* TargetPlayerController, const FText& Text, const FText& TimeText);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReceivePublicMessage(const FSpellRiseChatMessage& Message);
};

