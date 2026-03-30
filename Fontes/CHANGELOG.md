# Changelog

## Unreleased (2026-03-25)
### Build / Tooling
- Fixed plugin metadata warning by declaring `CommonUI` dependency in `Plugins/NarrativeInventory/NarrativeInventory.uplugin`.
- Updated deprecated `FString::LeftInline` call in `SpellRiseGameModeBase.cpp` to UE 5.7-compatible `EAllowShrinking::No`.
- Revalidated `SpellRiseEditor Win64 Development` build (succeeded).

## Unreleased (2026-03-16)
### Gameplay / Networking
- Added authoritative combat log dispatch in `ResourceAttributeSet` to source and target player controllers on each valid damage event.
- Added `ASpellRisePlayerController::ClientReceiveCombatLogEntry` for per-player combat messages (`damage dealt` / `damage received`).
- Added chat channel bridge in `ASpellRisePlayerController` that appends combat entries into chat channel `Combat` using reflective payload fill (`Name`, `Text`, `TimeText`, `Channel`).
- Added server-side guard in `ASpellRisePlayerController::ProcessEvent` to block manual client chat sends to `Combat` via `SendChatToSERVER`.
- Migrated core chat transport to native `USpellRiseChatComponent` on `ASpellRiseGameState` with server authority.
- Added targeted combat-feed API `SendCombatToPlayer` (private combat messages per player).
- Combat chat now includes damage type labels (fire/cold/shock/slashing/piercing/impact and others).
- Combat chat now emits death messages (`Voce morreu.` for victim and kill notice for attacker).
- Fixed projectile ability delegate binding crash by marking release callback with `UFUNCTION()`.
- Fixed damage pop regression by making pop dispatch target-driven and hardening Niagara spawn path in C++.

### Documentation
- Synchronized all design/architecture docs in Fontes with current runtime implementation.
- Updated architecture to reflect ASC and AttributeSets on PlayerState.
- Updated network model with authority + prediction + replicated target data flow.
- Updated attribute matrix and project state to STR/AGI/INT/WIS derived formulas.
- Added structured backlog and bug log entries for current technical risks.

### Known Technical Risks
- Legacy MMC formulas still coexist with newer derived-stat pipeline.
- Legacy reflection chat routing remains available only as optional fallback (`bEnableLegacyReflectionChatRouting`).

## v0.2 (Historical)
- Attribute scaling normalized.
- Soft caps introduced.

## v0.1 (Historical)
- Initial GAS structure.
- Damage ExecCalc implemented.
- Derived stats added.
