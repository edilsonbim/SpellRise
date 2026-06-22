#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "SpellRiseChatTypes.generated.h"

USTRUCT(BlueprintType)
struct FSpellRiseChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat")
	FName Name = NAME_None;

	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat")
	FText Text;

	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat")
	FText TimeText;



	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat")
	uint8 Channel = 0;

	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat|Whisper")
	FString ConversationId;

	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat|Whisper")
	FString ConversationName;

	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat|Whisper")
	bool bOutgoing = false;
};

namespace SpellRiseChatChannel
{
	static constexpr uint8 Global = 0;
	static constexpr uint8 Party = 1;
	static constexpr uint8 Guild = 2;
	static constexpr uint8 Combat = 3;
	static constexpr uint8 Whisper = 4;
}
