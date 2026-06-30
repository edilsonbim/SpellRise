#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SpellRiseFullLootSubsystem.generated.h"

class ASpellRiseCharacterBase;
class USpellRiseStorageComponent;

UCLASS()
class SPELLRISE_API USpellRiseFullLootSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void HandleCharacterDeath(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride);
	void HandleCharacterCorpseDespawn(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride, const FVector& CorpseLocation);
	void RegisterTrackedLootBag(AActor* BagActor, USpellRiseStorageComponent* StorageComponent);
	void RegisterTrackedLootBag(AActor* BagActor); // despawn por timeout apenas, sem check de esvaziamento

private:
	struct FTrackedLootBag
	{
		TWeakObjectPtr<AActor> BagActor;
		TWeakObjectPtr<USpellRiseStorageComponent> StorageComponent;
		double ExpireAtServerTimeSeconds = 0.0;
		double EmptySinceServerTimeSeconds = -1.0;
	};

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot")
	float LootBagLifetimeSeconds = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot")
	float LootBagMonitorIntervalSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot", meta=(ClampMin="0.0"))
	float LootBagSpawnDelaySeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot", meta=(ClampMin="0.0"))
	float LootBagEmptyDespawnDelaySeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Network", meta=(ClampMin="1.0"))
	float LootBagNetUpdateFrequency = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Loot|Network", meta=(ClampMin="16", ClampMax="256"))
	int32 DeadPlayerNameMaxUtf8Bytes = 64;

	FTimerHandle LootBagMonitorTimerHandle;
	TArray<FTrackedLootBag> TrackedLootBags;

	void ProcessCharacterDeathNow(ASpellRiseCharacterBase* DeadCharacter, TSubclassOf<AActor> LootBagClassOverride, const FVector& DeathLocation);
	void EnsureMonitorTimer();
	void StopMonitorTimerIfIdle();
	void TickLootBags();
};
