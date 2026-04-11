#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SpellRiseGameState.generated.h"

class USpellRiseChatComponent;

UCLASS()
class SPELLRISE_API ASpellRiseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASpellRiseGameState();

	UFUNCTION(BlueprintPure, Category="SpellRise|Chat")
	USpellRiseChatComponent* GetChatComponent() const { return ChatComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Chat")
	TObjectPtr<USpellRiseChatComponent> ChatComponent = nullptr;
};
