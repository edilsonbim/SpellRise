#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManagerTypes.h"

class USpellRiseItemDefinition;

namespace SpellRiseItemDefinitionResolver
{
	SPELLRISE_API USpellRiseItemDefinition* ResolveItemDefinition(const FPrimaryAssetId& DefinitionId);
}
