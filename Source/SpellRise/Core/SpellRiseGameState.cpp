#include "SpellRiseGameState.h"

#include "SpellRise/Components/SpellRiseChatComponent.h"

ASpellRiseGameState::ASpellRiseGameState()
{
	ChatComponent = CreateDefaultSubobject<USpellRiseChatComponent>(TEXT("ChatComponent"));
}

