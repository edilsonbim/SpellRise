#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRiseConstructionModeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSpellRiseConstructionModeChangedNative, bool );

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseConstructionModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseConstructionModeComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	bool IsConstructionModeEnabled() const { return bConstructionModeEnabled; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void RequestSetConstructionMode(bool bEnableConstructionMode);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void RequestToggleConstructionMode();

	FSpellRiseConstructionModeChangedNative& OnConstructionModeChanged() { return ConstructionModeChangedNative; }

protected:
	UPROPERTY(ReplicatedUsing=OnRep_ConstructionModeEnabled, BlueprintReadOnly, Category="SpellRise|Construction")
	bool bConstructionModeEnabled = false;

	UFUNCTION()
	void OnRep_ConstructionModeEnabled();

	UFUNCTION(Server, Reliable)
	void ServerSetConstructionMode(bool bEnableConstructionMode);

private:
	void ApplyConstructionModeInternal(bool bEnableConstructionMode, bool bLog);
	bool CanAcceptConstructionToggle(bool bEnableConstructionMode, FString& OutRejectReason) const;
	bool IsBlockedForConstructionEntry(FString& OutRejectReason) const;

	FSpellRiseConstructionModeChangedNative ConstructionModeChangedNative;
	double LastConstructionToggleServerTimeSeconds = -1000.0;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Construction|Validation")
	FGameplayTagContainer BlockedEntryStateTags;
};
