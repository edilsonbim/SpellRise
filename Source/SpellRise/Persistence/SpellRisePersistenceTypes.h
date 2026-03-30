#pragma once

#include "CoreMinimal.h"
#include "SpellRisePersistenceTypes.generated.h"

UENUM()
enum class ESpellRiseSaveOwnerScope : uint8
{
	Unknown = 0,
	Character = 1,
	PlayerState = 2
};

UENUM()
enum class ESpellRiseSaveContainerRole : uint8
{
	Unknown = 0,
	Inventory = 1,
	Hotbar = 2,
	Equipment = 3,
	Storage = 4
};

USTRUCT()
struct FSpellRiseSavedNarrativeItem
{
	GENERATED_BODY()

	UPROPERTY()
	FString ItemClassPath;

	UPROPERTY()
	int32 Quantity = 0;
};

USTRUCT()
struct FSpellRiseSavedInventoryComponent
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 OwnerScope = static_cast<uint8>(ESpellRiseSaveOwnerScope::Unknown);

	UPROPERTY()
	uint8 ContainerRole = static_cast<uint8>(ESpellRiseSaveContainerRole::Unknown);

	UPROPERTY()
	FString ComponentName;

	UPROPERTY()
	FString ComponentClassPath;

	UPROPERTY()
	FString InventoryFriendlyName;

	UPROPERTY()
	float WeightCapacity = 0.0f;

	UPROPERTY()
	int32 Capacity = 0;

	UPROPERTY()
	int32 Currency = 0;

	UPROPERTY()
	TArray<FSpellRiseSavedNarrativeItem> Items;
};

USTRUCT()
struct FSpellRiseCharacterSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SchemaVersion = 5;

	UPROPERTY()
	FString SteamId64;

	UPROPERTY()
	FString PlayerDisplayName;

	UPROPERTY()
	FTransform CharacterTransform = FTransform::Identity;

	UPROPERTY()
	FString RespawnBedActorName;

	UPROPERTY()
	FString RespawnBedClassPath;

	UPROPERTY()
	FVector RespawnBedLocation = FVector::ZeroVector;

	UPROPERTY()
	uint8 ArchetypeValue = 0;

	UPROPERTY()
	float Strength = 20.0f;

	UPROPERTY()
	float Agility = 20.0f;

	UPROPERTY()
	float Intelligence = 20.0f;

	UPROPERTY()
	float Wisdom = 20.0f;

	UPROPERTY()
	float Health = 180.0f;

	UPROPERTY()
	float Mana = 180.0f;

	UPROPERTY()
	float Stamina = 180.0f;

	UPROPERTY()
	float CatalystCharge = 0.0f;

	UPROPERTY()
	float CatalystXP = 0.0f;

	UPROPERTY()
	float CatalystLevel = 1.0f;

	UPROPERTY()
	TArray<FSpellRiseSavedInventoryComponent> InventoryComponents;
};

USTRUCT()
struct FSpellRiseInventorySaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SchemaVersion = 2;

	UPROPERTY()
	FString SteamId64;

	UPROPERTY()
	TArray<FSpellRiseSavedInventoryComponent> InventoryComponents;
};

USTRUCT()
struct FSpellRiseWorldActorSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FString ActorName;

	UPROPERTY()
	FString ActorClassPath;

	UPROPERTY()
	FTransform ActorTransform = FTransform::Identity;

	UPROPERTY()
	TArray<FString> Tags;
};

USTRUCT()
struct FSpellRiseWorldSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SchemaVersion = 1;

	UPROPERTY()
	FString WorldId;

	UPROPERTY()
	TArray<FSpellRiseWorldActorSaveData> BuildingActors;
};
