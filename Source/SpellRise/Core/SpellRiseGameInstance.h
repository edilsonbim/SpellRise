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

	UFUNCTION(BlueprintPure, Category="SpellRise|Online")
	bool IsSteamSubsystemActive() const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Online")
	bool HasLocalSessionIdentity(int32 LocalUserNum = 0) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Online")
	FString GetLocalSteamId64(int32 LocalUserNum = 0) const;

	UFUNCTION(BlueprintPure, Category="SpellRise|Online")
	FString GetLocalSessionIdentity(int32 LocalUserNum = 0) const;

private:
	mutable bool bLoggedLocalSessionFallback = false;
};
