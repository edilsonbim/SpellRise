# Attribute Matrix

## Primários canônicos
- `STR`
- `AGI`
- `INT`
- `WIS`

## Faixa efetiva
- Baseline: `20`
- Clamp de runtime: `0..120`
- Normalização derivada: bonus acima de `20`, limitado a `100` pontos efetivos

## Normalização
`T = clamp((Primary - 20) / 100, 0, 1)`

## Progressão autoritativa
- Valor inicial do `AttributeSet`: `0`
- Baseline de `20`: aplicado fora do `AttributeSet` por BP/GE autoritativo
- Progressão por talentos autoritativos: até `100`
- Booster de escolha do jogador: até `+20` em um atributo primário
- Cap final por primário após baseline/talentos + booster: `120`

## Derivados
### STR
- `MeleeDamageMultiplier = 1.00 + 0.50 * T`
- `ArmorPenetration = 0.00 + 0.30 * T`

### AGI
- `BowDamageMultiplier = 1.00 + 0.50 * T`
- `CritChance = 0.05 + 0.20 * T`

### INT
- `SpellDamageMultiplier = 1.00 + 0.50 * T`

### WIS
- `HealingMultiplier = 1.00 + 0.40 * T`
- `CritDamageMultiplier = 1.50 + 0.50 * T`

## Caps de recurso
Usando bônus sobre baseline:
- `MaxHealth = 180 + 2 * STR_bonus + 1 * WIS_bonus`
- `MaxMana = 180 + 2 * INT_bonus + 1 * WIS_bonus`
- `MaxStamina = 180 + 1 * STR_bonus + 2 * AGI_bonus`

## Regras de uso
- Novo código e novos assets devem usar apenas `STR/AGI/INT/WIS`.
- Aliases legados só podem existir como compatibilidade passiva durante migração.
- Rework de fórmula deve respeitar `COMBAT_DESIGN_PHILOSOPHY.md` antes de alterar balance live.
