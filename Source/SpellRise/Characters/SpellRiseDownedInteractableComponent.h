#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "SpellRiseDownedInteractableComponent.generated.h"

UENUM(BlueprintType)
enum class ESpellRiseDownedInteractionMode : uint8
{
	Revive UMETA(DisplayName="Revive"),
	Finish UMETA(DisplayName="Finish")
};

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseDownedInteractableComponent : public UNarrativeInteractableComponent
{
	GENERATED_BODY()

public:
	USpellRiseDownedInteractableComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SpellRise|Downed")
	ESpellRiseDownedInteractionMode InteractionMode = ESpellRiseDownedInteractionMode::Revive;

protected:
	virtual void OnRegister() override;
	virtual bool CanInteract_Implementation(APawn* Interactor, UNarrativeInteractionComponent* InteractionComp, FText& OutErrorText) override;
	virtual void OnInteract_Implementation(APawn* Interactor, UNarrativeInteractionComponent* InteractionComp) override;
};
