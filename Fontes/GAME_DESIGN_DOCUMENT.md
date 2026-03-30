# Spell Rise - Game Design Document (Core)

## Genre
- Hardcore PvP MMORPG
- Skill-based real-time combat

## Core Identity
- No safe zones outside onboarding/tutorial flow.
- Full loot on death (design target, not fully implemented yet).
- Player-driven economy (design target, persistence phase pending).
- Precision combat with manual aiming.

## Combat Model
- No tab target.
- No aim assist.
- Projectile + melee hybrid.
- Server authoritative damage with client-side aiming/prediction support.

## Risk / Reward
- Death has meaningful consequence.
- Gear value should be tied to player economy and circulation.
- No artificial permanent item binding in the long-term loop.

## Progression
- Horizontal first, controlled vertical gain.
- Attribute-driven builds (STR/AGI/INT/WIS).
- Power growth bounded by clamps and soft limits.

## Current Playable Technical Loop
- Spawn -> abilities granted -> combat exchanges -> damage/death states.
- Projectile abilities support client aim trace + server validation/spawn.
- Fall damage and catalyst charge systems are integrated into combat runtime.

## Not Yet Fully Implemented
- Complete death + loot + respawn economy loop.
- Persistent character/inventory/economy storage.
- Replicated combat log for audit and UX history.
