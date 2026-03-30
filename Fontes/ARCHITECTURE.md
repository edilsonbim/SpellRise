# Architecture Overview (Current Runtime)

## High-Level Topology
- ASpellRiseGameModeBase sets custom GameState, PlayerController, and PlayerState classes.
- GAS authoritative owner is ASpellRisePlayerState.
- ASpellRiseCharacterBase consumes ASC/AttributeSets from PlayerState and handles avatar-side behavior.

## GAS Layout
- Ability System Component:
  - USpellRiseAbilitySystemComponent on PlayerState
  - Replication mode: Mixed
- Attribute Sets:
  - UBasicAttributeSet (primary attributes)
  - UCombatAttributeSet (resistances, movement, compatibility helpers)
  - UResourceAttributeSet (health/mana/stamina/meta damage)
  - UCatalystAttributeSet (charge/xp/level/meta delta)
  - UDerivedStatsAttributeSet (combat multipliers and reductions)

## Combat Data Flow
1. Ability prepares GameplayEffect spec (SetByCaller + damage tags).
2. ExecCalc_Damage resolves channel, scaling, resistance, crit, and drains.
3. Output writes meta Damage in UResourceAttributeSet.
4. UResourceAttributeSet::PostGameplayEffectExecute applies health loss and side effects:
   - catalyst charge path
   - damage number multicast/cue path
   - resource clamps

## Projectile Flow
1. PlayerController builds local aim trace result.
2. Projectile ability sends target data to server.
3. Server consumes target data, commits ability, spawns replicated projectile.
4. Projectile overlap/hit applies GE damage on authority.

## Additional Combat Components
- UFallDamageComponent:
  - server-only tracking and application
  - computes severity and applies GE-driven fall damage
- UCatalystComponent:
  - server-side charge listener and proc trigger
  - integrates with damage events
- Number pops:
  - multicast/UI pathway plus gameplay cue support

## Controller Runtime Modules
- `ASpellRisePlayerController` hosts gameplay input and local UX glue.
- Building mode is isolated in `USpellRiseConstructionModeComponent` (controller component), keeping gameplay input gating outside of ability core code.
- Dedicated server flow must remain independent of HUD/widget logic.

## Current Technical Debt
- Legacy attribute aliases/MMCs still coexist with new derived formulas.
- Some runtime debug logs/effects remain in gameplay path.
- Documentation now reflects runtime state and should be kept as source-of-truth baseline.
- Narrative inventory migration is active; integration hardening for full-loot + persistence remains under ongoing validation.
