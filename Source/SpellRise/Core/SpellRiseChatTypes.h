#pragma once

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

	// Mirrors the existing BP enum ordering:
	// 0 Global, 1 Party, 2 Guild, 3 Combat.
	UPROPERTY(BlueprintReadWrite, Category="SpellRise|Chat")
	uint8 Channel = 0;
};

namespace SpellRiseChatChannel
{
	static constexpr uint8 Global = 0;
	static constexpr uint8 Party = 1;
	static constexpr uint8 Guild = 2;
	static constexpr uint8 Combat = 3;
}

