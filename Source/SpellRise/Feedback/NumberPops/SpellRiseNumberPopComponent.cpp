#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SpellRiseNumberPopComponent)

USpellRiseNumberPopComponent::USpellRiseNumberPopComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USpellRiseNumberPopComponent::AddNumberPop(const FSpellRiseNumberPopRequest& NewRequest)
{
	// Base abstrata: subclasses implementam.
}