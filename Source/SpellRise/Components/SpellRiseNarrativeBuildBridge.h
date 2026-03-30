#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRiseNarrativeBuildBridge.generated.h"

class UNarrativeInteractionComponent;

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseNarrativeBuildBridge : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseNarrativeBuildBridge();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction|Narrative")
	bool TryBuildInteract(APawn* InteractingPawn, bool bPressed, AActor*& OutTargetActor);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction|Narrative")
	bool TryOpenMalletMenu(UActorComponent* BuildingComponent, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Construction|Narrative")
	bool TryCloseMalletMenu(UActorComponent* BuildingComponent);

private:
	UNarrativeInteractionComponent* ResolveNarrativeInteractionComponent(APawn* InteractingPawn) const;
	AActor* ResolveCurrentNarrativeTargetActor(const UNarrativeInteractionComponent* InteractionComponent) const;
};

