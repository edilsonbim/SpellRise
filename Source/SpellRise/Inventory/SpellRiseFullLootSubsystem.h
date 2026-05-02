#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SpellRiseFullLootSubsystem.generated.h"

class ASpellRiseCharacterBase;
class UNarrativeInventoryComponent;

UCLASS()
class SPELLRISE_API USpellRiseFullLootSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void HandleCharacterDeath(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride);
	void HandleCharacterCorpseDespawn(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride, const FVector& CorpseLocation);

private:
	struct FTrackedLootBag
	{
		TWeakObjectPtr<AActor> BagActor;
		TWeakObjectPtr<UNarrativeInventoryComponent> InventoryComponent;
		double ExpireAtServerTimeSeconds = 0.0;
		double EmptySinceServerTimeSeconds = -1.0;
	};

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot")
	float LootBagLifetimeSeconds = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot")
	float LootBagMonitorIntervalSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot", meta=(ClampMin="0.0"))
	float LootBagSpawnDelaySeconds = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot", meta=(ClampMin="0.0"))
	float LootBagEmptyDespawnDelaySeconds = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Policy")
	bool bUseStrictDeathLootInventoryPolicy = false;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Policy")
	TArray<FName> AllowedCharacterInventoryComponentNames
	{
		TEXT("NarrativeInventoryComponent"),
		TEXT("InventoryComponent"),
		TEXT("HotbarComponent")
	};

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Policy")
	TArray<FName> AllowedPlayerStateInventoryComponentNames
	{
		TEXT("NarrativeInventoryComponent"),
		TEXT("InventoryComponent"),
		TEXT("HotbarComponent")
	};

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Policy")
	TArray<FName> DeniedDeathLootInventoryComponentNames
	{
		TEXT("StorageInventory")
	};

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Policy", meta=(ClampMin="1"))
	int32 MaxCollectedInventoryComponentsPerOwner = 8;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Network", meta=(ClampMin="1.0"))
	float LootBagNetUpdateFrequency = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Network", meta=(ClampMin="16", ClampMax="256"))
	int32 DeadPlayerNameMaxUtf8Bytes = 64;

	FTimerHandle LootBagMonitorTimerHandle;
	TArray<FTrackedLootBag> TrackedLootBags;

	void RegisterTrackedLootBag(AActor* BagActor, UNarrativeInventoryComponent* InventoryComponent);
	void ProcessCharacterDeathNow(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride, const FVector& DeathLocation);
	void GatherEligibleInventoryComponents(AActor* OwnerActor, bool bOwnerIsPlayerState, TArray<UNarrativeInventoryComponent*>& OutInventoryComponents) const;
	bool IsInventoryComponentEligibleForDeathLoot(const UNarrativeInventoryComponent* InventoryComponent, bool bOwnerIsPlayerState, FString& OutRejectReason) const;
	void EnsureMonitorTimer();
	void StopMonitorTimerIfIdle();
	void TickLootBags();
};
