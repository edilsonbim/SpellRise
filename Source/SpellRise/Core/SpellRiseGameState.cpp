// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRiseGameState.h"

#include "SpellRise/Components/SpellRiseChatComponent.h"

ASpellRiseGameState::ASpellRiseGameState()
{
	ChatComponent = CreateDefaultSubobject<USpellRiseChatComponent>(TEXT("ChatComponent"));
}
