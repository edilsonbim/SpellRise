#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpellRiseGameInstance.generated.h"

UCLASS()
class SPELLRISE_API USpellRiseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
