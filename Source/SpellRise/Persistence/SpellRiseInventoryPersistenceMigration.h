#pragma once

#include "CoreMinimal.h"
#include "SpellRise/Persistence/SpellRisePersistenceTypes.h"

/**
 * Resultado puro da conversão legacy Narrative para o schema de inventário 3.
 * O chamador decide se os avisos são aceitáveis antes de persistir o resultado.
 */
struct FSpellRiseInventoryMigrationResult
{
	bool bSucceeded = false;
	FSpellRiseInventorySaveDataV3 Data;
	TArray<FString> Warnings;
	TArray<FString> Errors;
};

struct FSpellRiseInventoryPersistenceMigration
{
	/**
	 * Converte snapshots legacy sem carregar assets ou alterar estado externo.
	 *
	 * @param LegacyInventory Snapshot de inventário schema 2.
	 * @param LegacyCharacter Snapshot de personagem schema 14, usado para equipamento.
	 * @param DefinitionIdsByLegacyClassPath Registry explícito classe Narrative -> PrimaryAssetId.
	 */
	static FSpellRiseInventoryMigrationResult ConvertLegacyToV3(
		const FSpellRiseInventorySaveData& LegacyInventory,
		const FSpellRiseCharacterSaveData& LegacyCharacter,
		const TMap<FString, FPrimaryAssetId>& DefinitionIdsByLegacyClassPath);
};
