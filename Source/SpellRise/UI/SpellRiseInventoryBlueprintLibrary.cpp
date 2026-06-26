#include "SpellRise/UI/SpellRiseInventoryBlueprintLibrary.h"

#include "SpellRise/Inventory/SpellRiseItemDefinitionResolver.h"
#include "SpellRise/Inventory/SpellRiseItemDefinition.h"

USpellRiseItemDefinition* USpellRiseInventoryBlueprintLibrary::ResolveItemDefinition(const FPrimaryAssetId DefinitionId)
{
	if (!DefinitionId.IsValid())
	{
		return nullptr;
	}

	return SpellRiseItemDefinitionResolver::ResolveItemDefinition(DefinitionId);
}
