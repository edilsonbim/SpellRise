#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Engine/AssetManagerTypes.h"
#include "GameplayTagContainer.h"
#include "SpellRisePersistenceTypes.generated.h"

namespace SpellRisePersistenceSchema
{
	inline constexpr int32 CharacterLegacy = 14;
	inline constexpr int32 CharacterInventoryV3 = 15;
	inline constexpr int32 InventoryLegacy = 2;
	inline constexpr int32 InventoryItemInstances = 3;
}

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

/**
 * Estado persistente de uma instância de item no schema de inventário 3.
 * LegacyItemClassPath permanece durante uma janela de rollback/migração e não
 * deve ser usado como identidade pelo runtime novo.
 */
USTRUCT()
struct FSpellRiseSavedItemInstanceV3
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid ItemInstanceId;

	UPROPERTY()
	FPrimaryAssetId DefinitionId;

	UPROPERTY()
	FString LegacyItemClassPath;

	UPROPERTY()
	FGuid ContainerId;

	UPROPERTY()
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY()
	int32 Quantity = 0;

	UPROPERTY()
	int32 Durability = 0;

	UPROPERTY()
	uint8 Flags = 0;

	UPROPERTY()
	TMap<FName, FString> ControlledProperties;
};

USTRUCT()
struct FSpellRiseSavedInventoryContainerV3
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid ContainerId;

	UPROPERTY()
	uint8 OwnerScope = static_cast<uint8>(ESpellRiseSaveOwnerScope::Unknown);

	UPROPERTY()
	uint8 ContainerRole = static_cast<uint8>(ESpellRiseSaveContainerRole::Unknown);

	UPROPERTY()
	FName ComponentName;

	UPROPERTY()
	int32 Capacity = 0;

	UPROPERTY()
	float WeightCapacity = 0.0f;

	UPROPERTY()
	int32 Currency = 0;

	UPROPERTY()
	TArray<FSpellRiseSavedItemInstanceV3> Items;
};

USTRUCT()
struct FSpellRiseSavedEquipmentEntryV3
{
	GENERATED_BODY()

	UPROPERTY()
	FName SlotName;

	UPROPERTY()
	FGuid ItemInstanceId;
};

USTRUCT()
struct FSpellRiseInventorySaveDataV3
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SchemaVersion = SpellRisePersistenceSchema::InventoryItemInstances;

	UPROPERTY()
	FString SteamId64;

	UPROPERTY()
	TArray<FSpellRiseSavedInventoryContainerV3> Containers;

	UPROPERTY()
	TArray<FSpellRiseSavedEquipmentEntryV3> EquippedItems;

	UPROPERTY()
	int32 ActiveWeaponQuickSlotIndex = INDEX_NONE;
};

USTRUCT()
struct FSpellRiseSavedTalent
{
	GENERATED_BODY()

	UPROPERTY()
	FString TalentAssetPath;

	UPROPERTY()
	int32 Level = 0;

	UPROPERTY()
	int32 AbilityLevel = 0;
};

USTRUCT()
struct FSpellRiseSavedProgressionLevel
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag ProgressionTag;

	UPROPERTY()
	int32 Level = 0;
};

USTRUCT()
struct FSpellRiseSavedAbilityHotbarSlot
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY()
	FGameplayTag AbilityInputTag;

	UPROPERTY()
	FString AbilityClassPath;

	UPROPERTY()
	FString AbilityDefinitionPath;
};

USTRUCT()
struct FSpellRiseSavedEquippedItem
{
	GENERATED_BODY()

	UPROPERTY()
	FString SlotName;

	UPROPERTY()
	FString ItemClassPath;
};

USTRUCT()
struct FSpellRiseSavedWeaponLoadoutSlot
{
	GENERATED_BODY()

	UPROPERTY()
	int32 QuickSlotIndex = INDEX_NONE;

	UPROPERTY()
	FString ItemClassPath;
};

USTRUCT()
struct FSpellRiseCharacterSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SchemaVersion = 14;

	UPROPERTY()
	FString SteamId64;

	UPROPERTY()
	FString PlayerDisplayName;

	UPROPERTY()
	bool bCharacterCreated = false;

	UPROPERTY()
	FString VisualConfigurationJson;

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
	int32 TalentPoints = 0;

	UPROPERTY()
	int32 CharacterLevel = 1;

	UPROPERTY()
	int32 Experience = 0;

	UPROPERTY()
	int32 CraftPoints = 100;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 HighestRewardedCharacterLevel = 1;

	UPROPERTY()
	int32 MeleeBoosterCount = 0;

	UPROPERTY()
	int32 BowBoosterCount = 0;

	UPROPERTY()
	int32 SpellBoosterCount = 0;

	UPROPERTY()
	int32 DivineBoosterCount = 0;

	UPROPERTY()
	int32 ActiveMeleeBoosterCount = 0;

	UPROPERTY()
	int32 ActiveBowBoosterCount = 0;

	UPROPERTY()
	int32 ActiveSpellBoosterCount = 0;

	UPROPERTY()
	int32 ActiveDivineBoosterCount = 0;

	UPROPERTY()
	TArray<FSpellRiseSavedTalent> Talents;

	UPROPERTY()
	TArray<FSpellRiseSavedProgressionLevel> WeaponSkillLevels;

	UPROPERTY()
	TArray<FSpellRiseSavedProgressionLevel> SchoolLevels;

	UPROPERTY()
	TArray<FSpellRiseSavedAbilityHotbarSlot> AbilityHotbarSlots;

	UPROPERTY()
	TArray<FSpellRiseSavedEquippedItem> EquippedItems;

	UPROPERTY()
	TArray<FSpellRiseSavedWeaponLoadoutSlot> WeaponLoadoutSlots;

	UPROPERTY()
	int32 ActiveWeaponQuickSlotIndex = INDEX_NONE;

	UPROPERTY()
	FString OffHandItemClassPath;

	UPROPERTY()
	TArray<FSpellRiseSavedInventoryComponent> InventoryComponents;

	UPROPERTY()
	FSpellRiseInventorySaveDataV3 NativeInventory;
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

	UPROPERTY()
	FSpellRiseInventorySaveDataV3 NativeInventory;
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

USTRUCT()
struct FSpellRiseDeathParticipantData
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlayerId;

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	FString CauseType;

	UPROPERTY()
	float DamageAmount = 0.0f;
};

USTRUCT()
struct FSpellRiseDeathEventData
{
	GENERATED_BODY()

	UPROPERTY()
	FString OccurredAtUtcIso8601;

	UPROPERTY()
	FString WorldId;

	UPROPERTY()
	FString VictimPlayerId;

	UPROPERTY()
	FString VictimName;

	UPROPERTY()
	int32 VictimLevel = 0;

	UPROPERTY()
	FSpellRiseDeathParticipantData TopDamage;

	UPROPERTY()
	FSpellRiseDeathParticipantData Fatal;

	UPROPERTY()
	FString DamageType;

	UPROPERTY()
	FVector DeathLocation = FVector::ZeroVector;

	UPROPERTY()
	FString Message;
};
