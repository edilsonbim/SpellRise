// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Persistence/SpellRiseBuildPersistenceAdapter.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePersistenceBuild, Log, All);

namespace
{
	bool IsEasyBuildingClassPath(const FString& ClassPath)
	{
		if (!ClassPath.Contains(TEXT("/EasyBuildingSystem/"), ESearchCase::IgnoreCase))
		{
			return false;
		}

		return ClassPath.Contains(TEXT("BP_EBS_Building"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT("BuildingObjects"), ESearchCase::IgnoreCase);
	}
}

bool FSpellRiseBuildPersistenceAdapter::IsManagedBuildingActor(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	return IsEasyBuildingClassPath(Actor->GetClass()->GetPathName());
}

void FSpellRiseBuildPersistenceAdapter::GatherWorldBuildingActors(UWorld* World, TArray<FSpellRiseWorldActorSaveData>& OutActors)
{
	OutActors.Reset();

	if (!World)
	{
		return;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsManagedBuildingActor(Actor))
		{
			continue;
		}

		FSpellRiseWorldActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.ActorClassPath = Actor->GetClass()->GetPathName();
		ActorData.ActorTransform = Actor->GetActorTransform();

		for (const FName Tag : Actor->Tags)
		{
			ActorData.Tags.Add(Tag.ToString());
		}

		OutActors.Add(MoveTemp(ActorData));
	}

}

void FSpellRiseBuildPersistenceAdapter::SpawnMissingBuildingActors(UWorld* World, const TArray<FSpellRiseWorldActorSaveData>& SavedActors, double LocationMatchToleranceSq)
{
	if (!World)
	{
		return;
	}

	int32 SpawnedActors = 0;
	int32 SkippedInvalidClassPath = 0;
	int32 SkippedLoadFailure = 0;
	int32 SkippedExistingActors = 0;

	for (const FSpellRiseWorldActorSaveData& SavedActor : SavedActors)
	{
		if (SavedActor.ActorClassPath.IsEmpty() || !IsEasyBuildingClassPath(SavedActor.ActorClassPath))
		{
			++SkippedInvalidClassPath;
			continue;
		}

		UClass* ActorClass = LoadObject<UClass>(nullptr, *SavedActor.ActorClassPath);
		if (!ActorClass)
		{
			++SkippedLoadFailure;
			continue;
		}

		bool bAlreadyExists = false;
		for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
		{
			AActor* ExistingActor = *It;
			if (!ExistingActor)
			{
				continue;
			}

			const bool bNameMatch = !SavedActor.ActorName.IsEmpty() && ExistingActor->GetName().Equals(SavedActor.ActorName, ESearchCase::IgnoreCase);
			const bool bLocationMatch = FVector::DistSquared(ExistingActor->GetActorLocation(), SavedActor.ActorTransform.GetLocation()) <= LocationMatchToleranceSq;
			if (bNameMatch || bLocationMatch)
			{
				bAlreadyExists = true;
				break;
			}
		}

		if (bAlreadyExists)
		{
			++SkippedExistingActors;
			continue;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, SavedActor.ActorTransform, SpawnParams);
		if (!SpawnedActor)
		{
			continue;
		}

		for (const FString& TagString : SavedActor.Tags)
		{
			if (!TagString.IsEmpty())
			{
				SpawnedActor->Tags.AddUnique(FName(*TagString));
			}
		}

		++SpawnedActors;
	}

}
