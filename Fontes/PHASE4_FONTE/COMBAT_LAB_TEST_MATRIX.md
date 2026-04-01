# Combat Lab Test Matrix

## Objective
Define the minimum repeatable combat validation matrix for SpellRise.

## Execution modes
Run the relevant scenarios in:
1. Standalone
2. Listen Server
3. Dedicated Server + 2 clients
4. Dedicated Server + 2 clients with:
   - Net PktLag=120
   - Net PktLoss=5

## Preset matrix
Minimum preset coverage:
- Baseline_20_All vs Baseline_20_All
- LightArmor_Melee vs Mage_GlassCannon
- MediumArmor_Bow vs MediumArmor_Bow
- HeavyArmor_Melee vs Mage_Sustain
- Cap_120_STR vs HeavyArmor_Melee
- Cap_120_AGI vs MediumArmor_Bow
- Cap_120_INT vs HeavyArmor_Melee
- Cap_120_WIS vs Mage_Sustain

## Scenario catalog

### CL-001 Formula sanity - melee
Goal:
- verify STR melee scaling and penetration behavior
Expected:
- damage follows canonical multiplier and penetration clamp

### CL-002 Formula sanity - bow
Goal:
- verify AGI bow scaling and crit chance behavior
Expected:
- damage and crit rate stay inside documented clamp envelope

### CL-003 Formula sanity - spell
Goal:
- verify INT spell scaling and WIS mana reduction interaction
Expected:
- spell damage and mana efficiency remain within canonical formulas

### CL-004 TTK envelope - light armor
Goal:
- verify target TTK against light target
Expected:
- 5-8s average envelope under controlled duel conditions

### CL-005 TTK envelope - medium armor
Goal:
- verify target TTK against medium target
Expected:
- 8-12s average envelope under controlled duel conditions

### CL-006 TTK envelope - heavy armor
Goal:
- verify target TTK against heavy target
Expected:
- 12-18s average envelope under controlled duel conditions

### CL-007 Projectile authority
Goal:
- validate full projectile runtime contract
Expected:
- local aim trace input only
- server validates target data
- server commits ability
- server spawns projectile
- hit applies GE on authority
- no double fire / no double damage

### CL-008 Predicted ability reconciliation
Goal:
- validate a representative predicted ability under normal and degraded network
Expected:
- responsive local UX
- server confirmation
- acceptable visual reconciliation
- no duplicate commit/damage

### CL-009 Resource and cooldown integrity
Goal:
- verify authoritative cost/cooldown behavior
Expected:
- insufficient resource blocks authoritative commit
- client cannot bypass cost/cooldown

### CL-010 Fall damage
Goal:
- validate server-only fall damage path
Expected:
- fall severity and health mutation occur only on authority
- owner and remote presentation remain coherent

### CL-011 Death, loot, respawn
Goal:
- validate combat interaction with death pipeline when touched
Expected:
- death remains authoritative
- loot remains authoritative
- respawn remains authoritative
- no client-trusted shortcuts

### CL-012 Owner vs remote presentation parity
Goal:
- verify owner and remote clients observe compatible combat state
Expected:
- no major divergence in hit feedback, cues, combat log, or death presentation

### CL-013 Reconnect after combat state change
Goal:
- validate reconnect stability after recent combat/death events
Expected:
- reconnect does not restore invalid combat state
- combat-related replicated state remains coherent

## Run policy

### Required by change type
#### Formula / attribute / balance changes
Run at minimum:
- CL-001
- CL-002
- CL-003
- one relevant TTK scenario
- one DS+2 lag/loss scenario when combat outcome timing is affected

#### Projectile / RPC / replication / OnRep changes
Run at minimum:
- CL-007
- CL-008 when prediction is involved
- CL-012
- CL-013 if reconnect-sensitive state is touched

#### Death / loot / respawn changes
Run at minimum:
- CL-011
- CL-012
- CL-013

#### PlayerController / local UX / camera/HUD-adjacent combat changes
Run at minimum:
- CL-007 if projectile flow is involved
- CL-008 if prediction or local UX is involved
- CL-012
- DS+2 lag/loss mandatory

## Evidence template
Use this table per scenario:

| Scenario | Mode | Presets | Expected | Actual | Evidence | Status |
|---|---|---|---|---|---|---|
| CL-001 |  |  |  |  |  |  |

## Failure classification
- F0: build/package/runtime blocker
- F1: authority/security violation
- F2: network regression or DS instability
- F3: balance out of envelope
- F4: presentation parity issue

## Merge recommendation
- PASS: all required scenarios passed
- PASS WITH RISK: non-blocking presentation issue only, documented and accepted
- FAIL: any authority, DS stability, duplicate damage/commit, or critical TTK violation
