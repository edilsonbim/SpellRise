#include "SpellRiseGameModeBase.h"

#include "SpellRiseGameState.h"
#include "SpellRisePlayerController.h"
#include "SpellRisePlayerState.h"

ASpellRiseGameModeBase::ASpellRiseGameModeBase()
{
	GameStateClass = ASpellRiseGameState::StaticClass();
	PlayerControllerClass = ASpellRisePlayerController::StaticClass();
	PlayerStateClass = ASpellRisePlayerState::StaticClass();
}
