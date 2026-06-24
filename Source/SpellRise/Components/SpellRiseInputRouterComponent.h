#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRiseInputRouterComponent.generated.h"

class ASpellRisePlayerController;
class UInputAction;
class UInputComponent;
class UInputMappingContext;

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseInputRouterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseInputRouterComponent();

	void InitializeLocalInput();
	void RefreshContexts();
	void BindInput(UInputComponent* InputComponent);
	FText GetAbilitySlotInputText(int32 PhysicalSlotIndex) const;
	bool HasMappedAction(FName ActionName) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Input")
	void SetBuildingInputActive(bool bActive);

	UFUNCTION(BlueprintPure, Category="SpellRise|Input")
	bool IsBuildingInputActive() const { return bBuildingInputActive; }

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input|Contexts")
	TObjectPtr<UInputMappingContext> IMC_Global = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Contexts")
	TObjectPtr<UInputMappingContext> IMC_Movement = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Contexts")
	TObjectPtr<UInputMappingContext> IMC_Combat = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Contexts")
	TObjectPtr<UInputMappingContext> IMC_Building = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Contexts")
	TObjectPtr<UInputMappingContext> IMC_UI = nullptr;

private:
	ASpellRisePlayerController* GetSpellRiseController() const;
	const UInputAction* FindAction(FName ActionName) const;
	void UpdateContext(UInputMappingContext* Context, int32 Priority, bool bShouldBeActive) const;

	UPROPERTY(Transient)
	bool bBuildingInputActive = false;
};
