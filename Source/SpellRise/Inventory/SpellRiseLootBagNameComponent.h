#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellRiseLootBagNameComponent.generated.h"

class UNarrativeInteractableComponent;

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseLootBagNameComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseLootBagNameComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetDeadPlayerDisplayName_Server(const FString& InName);
	const FString& GetDeadPlayerDisplayName() const { return DeadPlayerDisplayName; }

protected:
	UFUNCTION()
	void OnRep_DeadPlayerDisplayName();

	void ApplyDisplayNameToInteractable();
	UNarrativeInteractableComponent* ResolveLootInteractable();

	UPROPERTY(ReplicatedUsing=OnRep_DeadPlayerDisplayName, VisibleInstanceOnly, Category="SpellRise|Loot")
	FString DeadPlayerDisplayName;

	UPROPERTY(Transient)
	TObjectPtr<UNarrativeInteractableComponent> CachedLootInteractable = nullptr;
};
