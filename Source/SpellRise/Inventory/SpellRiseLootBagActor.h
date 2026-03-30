#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellRiseLootBagActor.generated.h"

class UNarrativeInventoryComponent;
class UNarrativeInteractableComponent;

UCLASS(Blueprintable)
class SPELLRISE_API ASpellRiseLootBagActor : public AActor
{
	GENERATED_BODY()

public:
	ASpellRiseLootBagActor();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Loot")
	void SetDeadPlayerDisplayName(const FString& NewDeadPlayerDisplayName);

	UFUNCTION(BlueprintPure, Category="SpellRise|Loot")
	const FString& GetDeadPlayerDisplayName() const { return DeadPlayerDisplayName; }

protected:
	UFUNCTION()
	void OnRep_DeadPlayerDisplayName();

	void ApplyDeadPlayerNameToInteractable();
	FString SanitizeDisplayNameForNet(const FString& InDisplayName) const;
	UNarrativeInteractableComponent* ResolveLootInteractable();

	UPROPERTY(ReplicatedUsing=OnRep_DeadPlayerDisplayName, VisibleInstanceOnly, BlueprintReadOnly, Category="SpellRise|Loot")
	FString DeadPlayerDisplayName;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Network", meta=(ClampMin="16", ClampMax="256"))
	int32 DeadPlayerNameMaxUtf8Bytes = 64;

	UPROPERTY(Transient)
	TObjectPtr<UNarrativeInteractableComponent> CachedLootInteractable = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UNarrativeInventoryComponent> CachedInventoryComponent = nullptr;
};
