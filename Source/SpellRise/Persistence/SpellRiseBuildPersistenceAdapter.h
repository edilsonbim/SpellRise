#pragma once

#include "CoreMinimal.h"
#include "SpellRise/Persistence/SpellRisePersistenceTypes.h"

class AActor;
class UWorld;

struct FSpellRiseBuildPersistenceAdapter
{
	static bool IsManagedBuildingActor(const AActor* Actor);
	static void GatherWorldBuildingActors(UWorld* World, TArray<FSpellRiseWorldActorSaveData>& OutActors);
	static void SpawnMissingBuildingActors(UWorld* World, const TArray<FSpellRiseWorldActorSaveData>& SavedActors, double LocationMatchToleranceSq);
};

