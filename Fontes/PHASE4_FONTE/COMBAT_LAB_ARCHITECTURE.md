# Combat Lab Architecture

## Objective
Establish a production-grade internal combat lab for SpellRise to validate combat balance, authority, prediction, replication, and regression safety before feature merge or release.

The combat lab exists to answer four questions quickly and repeatably:
1. Is the runtime still server-authoritative?
2. Are combat outcomes stable under normal and degraded network conditions?
3. Is the current balance envelope still inside the target TTK and scaling rules?
4. Did a change introduce regression in projectile, damage, attributes, death, loot, or presentation?

## Scope
The combat lab covers:
- damage formulas and combat scaling
- projectile authority and replicated target-data flow
- melee/spell/bow combat validation
- resource cost/cooldown validation
- death, loot, respawn, and combat log verification when touched by combat changes
- owner vs remote visual parity checks
- lag/loss reconciliation checks for predicted combat flows

It is not a replacement for full gameplay QA. It is a controlled validation environment for combat and network correctness.

## Source of truth
Use this order inside the lab workflow:
1. PROJECT_STATE.md
2. ARCHITECTURE.md
3. ATTRIBUTE_MATRIX.md
4. MULTIPLAYER_RULES.md
5. SECURITY_MODEL.md
6. BACKLOG.md
7. BUG_LOG.md
8. COMBAT_DESIGN_PHILOSOPHY.md

If current behavior conflicts with documented runtime, flag the conflict explicitly before recording a baseline.

Runtime truth rule:
- Gameplay state authority is only `ASC + GameplayEffects + GameplayTags` on server.
- Any mirrored state outside ASC is observational/presentation only.

## Non-negotiable runtime contract
- Dedicated Server hosts the authoritative ASC runtime.
- Combat remains server-authoritative.
- GAS owner remains PlayerState.
- ASC and AttributeSets remain on PlayerState.
- The client can predict for UX but never decides final damage, death, loot, or final hit result.
- Projectile flow remains: local aim trace -> target data -> server validate/consume -> server commit -> server spawn -> server GE on hit.

## Combat lab pillars

### 1. Authority validation
Every lab scenario must prove that:
- damage is resolved on server
- resource mutation is authoritative
- cooldown/commit are authoritative
- projectile spawn authority remains on server
- death and respawn remain authoritative

### 2. Balance validation
Every balance scenario must measure against current project targets:
- Light armor TTK: 5-8s
- Medium armor TTK: 8-12s
- Heavy armor TTK: 12-18s
- No uncontrolled power creep beyond current clamps and soft limits

### 3. Network validation
Every net-critical scenario must be repeatable in:
- Standalone
- Listen Server
- Dedicated Server + 2 clients
- Dedicated Server + 2 clients with lag/loss

### 4. Regression validation
Every combat change should be mapped to at least one regression bucket:
- formulas / attributes
- ability activation / commit / cancel
- projectile path
- RPC / OnRep / ownership
- combat log / feedback
- death / loot / respawn

## Recommended environment

### Maps
Use one controlled combat map for lab validation with:
- isolated duel lanes
- fixed spawn points
- fixed line-of-sight checkpoints
- no unrelated AI/system noise
- optional test stations for projectile, melee, spell, fall damage, and respawn validation

### Characters / presets
Use controlled preset profiles instead of ad hoc manual setups:
- LightArmor_Melee
- MediumArmor_Bow
- HeavyArmor_Melee
- Mage_GlassCannon
- Mage_Sustain
- Baseline_20_All
- Cap_120_STR
- Cap_120_AGI
- Cap_120_INT
- Cap_120_WIS

Each preset should be reproducible from canonical STR/AGI/INT/WIS data.

### Test instrumentation
Minimum instrumentation expected from the lab:
- ability activation
- commit success/failure
- server validation rejection reason
- projectile spawn and hit authority
- applied damage and blocked damage
- death/loot/respawn transitions when in scope
- scenario metadata: build, map, net mode, lag/loss, preset pair, timestamp

## Core scenario groups

### Group A - Formula sanity
Validate current runtime math against canonical formulas:
- melee scaling from STR
- bow scaling from AGI
- spell scaling from INT
- crit chance from AGI
- crit damage from WIS
- mana reduction from WIS
- armor penetration from STR
- resistance clamps

### Group B - TTK envelope
Run controlled 1v1 tests for:
- light target
- medium target
- heavy target
Measure average and spread across repeated runs.

### Group C - Projectile authority
Validate:
- client local aim trace
- target-data send
- server validation
- authoritative commit
- replicated projectile spawn
- server-side GE application
- no double fire / double damage

### Group D - Ability prediction and reconciliation
For predicted abilities, validate:
- immediate local UX feedback
- authoritative confirmation
- visual reconciliation under lag/loss
- no duplicated commit or damage

### Group E - Death loop safety
When combat touches death pipeline, validate:
- health reaches death on authority
- death state presentation remains correct for owner and remote
- loot processing remains server-authoritative
- respawn scheduling remains authoritative

### Group F - Presentation parity
Validate owner vs remote consistency for:
- hit feedback
- cue timing
- montage timing when relevant
- combat log and damage feedback in scope

## Roles in the combat lab pipeline

### Combat owner
Defines scenario intent, expected gameplay result, and acceptable TTK/range.

### Systems owner
Verifies formulas, attributes, ExecCalc/MMC/GE impact, and canonical stat sources.

### Network owner
Verifies authority, RPC paths, OnRep, target-data validation, lag/loss behavior, and DS stability.

### QA owner
Runs the matrix, archives evidence, and records PASS/FAIL with reproducible steps.

## Output contract
Every combat lab run should emit:
- scenario id
- build id / package id
- map
- net mode
- lag/loss settings
- preset A / preset B
- expected result
- actual result
- PASS / FAIL
- primary log evidence
- suspected root cause when failed

## Promotion rule
A combat or network change should not be treated as safe until:
- compile/build/package is green in the approved pipeline
- relevant combat lab scenarios pass
- DS+2 lag/loss cases pass for net-critical changes
- balance impact is recorded when formulas or attributes are touched

## Current priority alignment
The combat lab should first target active project risk areas:
1. balance pass for resist/penetration/crit
2. automated gameplay validation for damage, fall damage, and projectile authority
3. regressions involving PlayerController-driven multiplayer instability when combat presentation or local UX paths are touched
4. reconnect and persistent state consistency when combat interacts with save/load or death pipeline
