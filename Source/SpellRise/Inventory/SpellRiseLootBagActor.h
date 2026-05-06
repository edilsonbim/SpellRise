#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellRiseLootBagActor.generated.h"

class UNarrativeInventoryComponent;
class UNarrativeInteractableComponent;
class UNarrativeItem;

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

	UFUNCTION()
	void HandleInventoryUpdated();

	UFUNCTION()
	void HandleInventoryItemRemoved(UNarrativeItem* Item, int32 Amount);

	void BindInventoryEmptyDespawn_Server();
	void ScheduleEmptyDespawnIfNeeded_Server();
	void ExecuteEmptyDespawn_Server();
	bool IsLootInventoryEmpty() const;
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

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Lifetime", meta=(ClampMin="0.0"))
	float EmptyDespawnDelaySeconds = 0.0f;

	FTimerHandle EmptyDespawnTimerHandle;
};
