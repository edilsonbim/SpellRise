#include "SpellRise/Persistence/SpellRiseInventoryPersistenceMigration.h"

#include "Misc/SecureHash.h"

namespace
{
	FString NormalizeLegacyClassPath(const FString& ClassPath)
	{
		FString Result = ClassPath;
		Result.TrimStartAndEndInline();
		return Result.ToLower();
	}

	FGuid MakeDeterministicGuid(const FString& Seed)
	{
		const FString Hash = FMD5::HashAnsiString(*Seed);
		FGuid Result;
		FGuid::ParseExact(Hash, EGuidFormats::Digits, Result);
		return Result;
	}

	const FPrimaryAssetId* FindDefinitionId(
		const TMap<FString, FPrimaryAssetId>& Registry,
		const FString& LegacyClassPath)
	{
		if (const FPrimaryAssetId* Exact = Registry.Find(LegacyClassPath))
		{
			return Exact;
		}

		const FString NormalizedPath = NormalizeLegacyClassPath(LegacyClassPath);
		for (const TPair<FString, FPrimaryAssetId>& Pair : Registry)
		{
			if (NormalizeLegacyClassPath(Pair.Key) == NormalizedPath)
			{
				return &Pair.Value;
			}
		}

		return nullptr;
	}

	FString MakeContainerSeed(
		const FString& SteamId64,
		const FSpellRiseSavedInventoryComponent& LegacyContainer,
		const int32 ContainerIndex)
	{
		return FString::Printf(
			TEXT("spellrise:v3:container:%s:%d:%d:%s:%d"),
			*SteamId64,
			static_cast<int32>(LegacyContainer.OwnerScope),
			static_cast<int32>(LegacyContainer.ContainerRole),
			*LegacyContainer.ComponentName,
			ContainerIndex);
	}
}

FSpellRiseInventoryMigrationResult FSpellRiseInventoryPersistenceMigration::ConvertLegacyToV3(
	const FSpellRiseInventorySaveData& LegacyInventory,
	const FSpellRiseCharacterSaveData& LegacyCharacter,
	const TMap<FString, FPrimaryAssetId>& DefinitionIdsByLegacyClassPath)
{
	FSpellRiseInventoryMigrationResult Result;
	Result.Data.SchemaVersion = SpellRisePersistenceSchema::InventoryItemInstances;
	Result.Data.SteamId64 = !LegacyInventory.SteamId64.IsEmpty()
		? LegacyInventory.SteamId64
		: LegacyCharacter.SteamId64;
	Result.Data.ActiveWeaponQuickSlotIndex = LegacyCharacter.ActiveWeaponQuickSlotIndex;

	if (Result.Data.SteamId64.IsEmpty())
	{
		Result.Errors.Add(TEXT("missing_steam_id"));
		return Result;
	}

	TMap<FString, TArray<FGuid>> ItemIdsByLegacyClassPath;
	for (int32 ContainerIndex = 0; ContainerIndex < LegacyInventory.InventoryComponents.Num(); ++ContainerIndex)
	{
		const FSpellRiseSavedInventoryComponent& LegacyContainer = LegacyInventory.InventoryComponents[ContainerIndex];
		FSpellRiseSavedInventoryContainerV3& Container = Result.Data.Containers.AddDefaulted_GetRef();
		Container.ContainerId = MakeDeterministicGuid(MakeContainerSeed(Result.Data.SteamId64, LegacyContainer, ContainerIndex));
		Container.OwnerScope = LegacyContainer.OwnerScope;
		Container.ContainerRole = LegacyContainer.ContainerRole;
		Container.ComponentName = FName(*LegacyContainer.ComponentName);
		Container.Capacity = LegacyContainer.Capacity;
		Container.WeightCapacity = LegacyContainer.WeightCapacity;
		Container.Currency = LegacyContainer.Currency;

		for (int32 ItemIndex = 0; ItemIndex < LegacyContainer.Items.Num(); ++ItemIndex)
		{
			const FSpellRiseSavedNarrativeItem& LegacyItem = LegacyContainer.Items[ItemIndex];
			if (LegacyItem.ItemClassPath.IsEmpty() || LegacyItem.Quantity <= 0)
			{
				Result.Warnings.Add(FString::Printf(
					TEXT("invalid_legacy_item:container=%d:item=%d"),
					ContainerIndex,
					ItemIndex));
				continue;
			}

			const FPrimaryAssetId* DefinitionId = FindDefinitionId(
				DefinitionIdsByLegacyClassPath,
				LegacyItem.ItemClassPath);
			if (!DefinitionId || !DefinitionId->IsValid())
			{
				Result.Errors.Add(FString::Printf(
					TEXT("unmapped_item_class:%s"),
					*LegacyItem.ItemClassPath));
				continue;
			}

			FSpellRiseSavedItemInstanceV3& Item = Container.Items.AddDefaulted_GetRef();
			Item.ItemInstanceId = MakeDeterministicGuid(FString::Printf(
				TEXT("spellrise:v3:item:%s:%s:%d"),
				*Result.Data.SteamId64,
				*Container.ContainerId.ToString(EGuidFormats::Digits),
				ItemIndex));
			Item.DefinitionId = *DefinitionId;
			Item.LegacyItemClassPath = LegacyItem.ItemClassPath;
			Item.ContainerId = Container.ContainerId;
			Item.SlotIndex = ItemIndex;
			Item.Quantity = LegacyItem.Quantity;
			Item.Durability = 0;

			ItemIdsByLegacyClassPath.FindOrAdd(NormalizeLegacyClassPath(LegacyItem.ItemClassPath))
				.Add(Item.ItemInstanceId);
		}
	}

	TMap<FString, int32> NextEquipmentCandidateByClassPath;
	for (const FSpellRiseSavedEquippedItem& LegacyEquippedItem : LegacyCharacter.EquippedItems)
	{
		if (LegacyEquippedItem.SlotName.IsEmpty() || LegacyEquippedItem.ItemClassPath.IsEmpty())
		{
			Result.Warnings.Add(TEXT("invalid_legacy_equipment_entry"));
			continue;
		}

		const FString NormalizedPath = NormalizeLegacyClassPath(LegacyEquippedItem.ItemClassPath);
		const TArray<FGuid>* Candidates = ItemIdsByLegacyClassPath.Find(NormalizedPath);
		if (!Candidates || Candidates->IsEmpty())
		{
			Result.Errors.Add(FString::Printf(
				TEXT("equipment_item_not_found:%s:%s"),
				*LegacyEquippedItem.SlotName,
				*LegacyEquippedItem.ItemClassPath));
			continue;
		}

		int32& CandidateIndex = NextEquipmentCandidateByClassPath.FindOrAdd(NormalizedPath);
		if (!Candidates->IsValidIndex(CandidateIndex))
		{
			Result.Errors.Add(FString::Printf(
				TEXT("equipment_item_ambiguous_exhausted:%s:%s"),
				*LegacyEquippedItem.SlotName,
				*LegacyEquippedItem.ItemClassPath));
			continue;
		}

		if (Candidates->Num() > 1)
		{
			Result.Warnings.Add(FString::Printf(
				TEXT("equipment_item_ambiguous_deterministic_choice:%s:%s:%d"),
				*LegacyEquippedItem.SlotName,
				*LegacyEquippedItem.ItemClassPath,
				CandidateIndex));
		}

		FSpellRiseSavedEquipmentEntryV3& EquippedItem = Result.Data.EquippedItems.AddDefaulted_GetRef();
		EquippedItem.SlotName = FName(*LegacyEquippedItem.SlotName);
		EquippedItem.ItemInstanceId = (*Candidates)[CandidateIndex++];
	}

	Result.bSucceeded = Result.Errors.IsEmpty();
	return Result;
}
