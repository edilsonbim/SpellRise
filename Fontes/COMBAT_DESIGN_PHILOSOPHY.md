# Combat Design Philosophy

## Target TTK (1v1)
- Light armor: 5-8s
- Medium armor: 8-12s
- Heavy armor: 12-18s

## Core Priorities
- Skill > Gear
- Reaction > RNG
- Positioning > Burst
- Free target, sem auto-aim e sem tab target.

## Runtime Damage Order
1. Base damage from SetByCaller (weapon or spell channel).
2. Channel multiplier (melee/bow/spell).
3. Generic damage scaling multiplier.
4. Resistance mitigation (with physical penetration interaction).
5. Critical roll and critical multiplier.
6. Resource drains (stamina/mana) by damage type profile.

## Scaling Rules (Current)
- Crit chance clamped to 25%.
- Crit damage multiplier clamped to 2.0x.
- Armor penetration clamped to 30%.
- Resistances clamped to 75%.
- No exponential primary scaling in current derived formulas.

## Power Creep Controls
- Baseline-driven formulas (primary baseline = 20).
- Linear bonus growth to derived multipliers.
- Hard clamps on crit, penetration, and resist to avoid runaway scaling.

## Current Balance Constraints
- Primary runtime clamp is 20..120.
- Derived normalization code targets 20..120.
- Runtime clamp and normalization are aligned.
