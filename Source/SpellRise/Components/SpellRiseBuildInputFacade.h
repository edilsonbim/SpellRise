#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <initializer_list>
#include "SpellRiseBuildInputFacade.generated.h"

class USpellRiseNarrativeBuildBridge;

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseBuildInputFacade : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseBuildInputFacade();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void HandleOwningPawnReady();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void SetConstructionModeActive(bool bInConstructionModeActive);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void SetConstructionModifierHeld(bool bIsHeld);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildMenuPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildMenuReleased();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildCancelPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildCycleMenuPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildToggleNearSnappingPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildToggleGridPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildAdjustWheel(float AxisValue);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildFloorUpPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildFloorDownPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildInteractPressed();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction")
	void OnBuildInteractReleased();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	float WheelRotationStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	float WheelHeightStep = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	int32 FloorStep = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	bool bRequireConstructionModeForCommands = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	bool bEnableDisplayedFloorCommands = false;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	bool bEnableLegacyEBSMenuCalls = false;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction")
	bool bEnableLegacyEBSAxisCalls = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Construction|Validation")
	bool bRequireBuildMaterialValidation = true;

private:
	UActorComponent* ResolveEBSBuildingComponent();
	bool CanProcessConstructionCommand() const;

	bool CallNoParamFunction(UObject* Target, const FName FunctionName) const;
	bool CallNoParamBoolFunction(UObject* Target, const FName FunctionName, bool& OutResult) const;
	bool CallFirstAvailableNoParamFunction(UObject* Target, std::initializer_list<FName> FunctionNames) const;
	bool GetBooleanProperty(UObject* Target, const TCHAR* PropertyName, bool& OutValue) const;
	bool SetBooleanProperty(UObject* Target, const TCHAR* PropertyName, bool bValue) const;
	bool ToggleBooleanPropertyAndSubmit(UObject* Target, const TCHAR* PropertyName, const FName FunctionName, const TCHAR* ParamName) const;
	bool CallOperationFunction(UObject* Target, const FName FunctionName, bool bIncrease, float Value) const;
	bool CallDisplayedFloorFunction(bool bIncrease);
	bool HandleMalletInteraction(bool bPressed);
	bool CanConfirmBuild(UObject* BuildingComponent, FString& OutRejectReason) const;

	UPROPERTY(Transient)
	TObjectPtr<UActorComponent> CachedEBSBuildingComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USpellRiseNarrativeBuildBridge> NarrativeBuildBridge = nullptr;

	UPROPERTY(Transient)
	bool bConstructionModeActive = false;

	UPROPERTY(Transient)
	bool bConstructionModifierHeld = false;

	UPROPERTY(Transient)
	bool bCachedClickEvents = false;

	UPROPERTY(Transient)
	bool bCachedMouseOverEvents = false;

	UPROPERTY(Transient)
	bool bCachedTraceToMouseMode = false;

	UPROPERTY(Transient)
	bool bHasCachedTraceToMouseMode = false;

	UPROPERTY(Transient)
	bool bStartupMenuSyncDone = false;
};
