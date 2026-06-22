#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

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

	bool HandleClientMessage(
		ASpellRisePlayerController* SenderController,
		const FString& RawText,
		uint8 RequestedChannel);

	UE_DEPRECATED(5.7, "Use ASpellRisePlayerController::SubmitChatMessage. Legacy Blueprint adapter only.")
	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SendToMULTICAST(FName Name, const FText& Text, const FText& TimeText, uint8 Channel);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Chat")
	void SendCombatToPlayer(ASpellRisePlayerController* TargetPlayerController, const FText& Text, const FText& TimeText);

private:
	struct FChatRateState
	{
		double WindowStartSeconds = 0.0;
		int32 AcceptedInWindow = 0;
		int32 RejectedInWindow = 0;
	};

	bool CheckRateLimit(ASpellRisePlayerController* SenderController, uint8 Channel, FString& OutRejectReason);
	bool HandleNameCommand(ASpellRisePlayerController* SenderController, const FString& RawText);
	void BroadcastPublicMessage(const FSpellRiseChatMessage& Message);
	void SendPrivateSystemMessage(ASpellRisePlayerController* TargetController, const FString& MessageText, uint8 Channel = 0);
	FSpellRiseChatMessage BuildPublicMessage(ASpellRisePlayerController* SenderController, const FString& Text, uint8 Channel) const;

	TMap<TWeakObjectPtr<ASpellRisePlayerController>, FChatRateState> PublicRateStates;
};
