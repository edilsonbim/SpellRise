#include "SpellRise/Inventory/SpellRiseItemDefinitionResolver.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Modules/ModuleManager.h"
#include "SpellRise/Inventory/SpellRiseItemDefinition.h"

namespace SpellRiseItemDefinitionResolver
{
	USpellRiseItemDefinition* ResolveItemDefinition(const FPrimaryAssetId& DefinitionId)
	{
		if (!DefinitionId.IsValid())
		{
			return nullptr;
		}

		UAssetManager& Manager = UAssetManager::Get();
		if (UObject* LoadedObject = Manager.GetPrimaryAssetObject(DefinitionId))
		{
			return Cast<USpellRiseItemDefinition>(LoadedObject);
		}

		const FSoftObjectPath PrimaryAssetPath = Manager.GetPrimaryAssetPath(DefinitionId);
		if (PrimaryAssetPath.IsValid())
		{
			if (USpellRiseItemDefinition* LoadedDefinition = Cast<USpellRiseItemDefinition>(PrimaryAssetPath.TryLoad()))
			{
				return LoadedDefinition;
			}
		}

		const FName AssetName = DefinitionId.PrimaryAssetName;
		if (AssetName.IsNone())
		{
			return nullptr;
		}

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter Filter;
		Filter.ClassPaths.Add(USpellRiseItemDefinition::StaticClass()->GetClassPathName());
		Filter.bRecursiveClasses = true;
		Filter.PackagePaths.Add(FName(TEXT("/Game")));
		Filter.bRecursivePaths = true;

		TArray<FAssetData> Assets;
		AssetRegistryModule.Get().GetAssets(Filter, Assets);
		for (const FAssetData& AssetData : Assets)
		{
			if (AssetData.AssetName != AssetName)
			{
				continue;
			}

			return Cast<USpellRiseItemDefinition>(AssetData.GetAsset());
		}

		return nullptr;
	}
}
