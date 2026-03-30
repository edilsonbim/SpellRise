# Attribute Matrix (Current Runtime)

## Primary Attributes
- Strength (STR)
- Agility (AGI)
- Intelligence (INT)
- Wisdom (WIS)

## Baseline and Effective Range
- Baseline: 20
- Runtime clamp (BasicAttributeSet): 20..120
- Derived normalization code target: 20..120
- Status: runtime clamp and normalization are aligned.

## Derived Stats Formula (from MMC_DerivedStats)
T = clamp((Primary - 20) / 100, 0, 1)

STR:
- MeleeDamageMultiplier = 1.00 + 0.50 * T
- ArmorPenetration = 0.00 + 0.30 * T (0%..30%)

AGI:
- BowDamageMultiplier = 1.00 + 0.50 * T
- CritChance = 0.05 + 0.20 * T (5%..25%)

INT:
- SpellDamageMultiplier = 1.00 + 0.50 * T
- CastTimeReduction = 0.00 + 0.096 * T (0%..9.6%)

WIS:
- HealingMultiplier = 1.00 + 0.40 * T
- CritDamageMultiplier = 1.50 + 0.50 * T (150%..200%)
- ManaCostReduction = 0.00 + 0.20 * T (0%..20%)

## Resource Caps (from MMC_DerivedStats)
Using bonuses over baseline (Primary - 20):
- MaxHealth = 180 + 2 * STR_bonus + 1 * WIS_bonus
- MaxMana = 180 + 2 * INT_bonus + 1 * WIS_bonus
- MaxStamina = 180 + 1 * STR_bonus + 2 * AGI_bonus

## Legacy Alias Compatibility
- Runtime C++ canonical access is STR/AGI/INT/WIS only.
- Legacy names `VIG/FOC` may still appear only in old assets under migration.
- New assets and new implementations must not introduce legacy aliases.
