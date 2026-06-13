# Attribute Matrix

## Primários canônicos
- `STR`
- `AGI`
- `INT`
- `WIS`

## Faixa efetiva
- Baseline: `20`
- Clamp de runtime: `0..120`
- Talentos persistidos: nivel `1..100`
- Normalização derivada: bonus acima de `20`, limitado a `100` pontos efetivos

## Normalização
`T = clamp((Primary - 20) / 100, 0, 1)`

## Progressão autoritativa
- Valor inicial do `AttributeSet`: `20`
- Progressão por talentos autoritativos: até `100`
- Booster de escolha do jogador: até `+20` em um atributo primário
- Cap final por primário após baseline/talentos + booster: `120`
- Pontos de talento não são `AttributeSet`; são estado persistido de progressão do personagem.
- Talentos aplicados por login devem vir da persistência, não de `Default Talents` em `BeginPlay`.

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

## Regeneração de recursos
- `HealthRegen`, `ManaRegen` e `StaminaRegen` são atributos de valor final, replicados `OwnerOnly`.
- O valor base de regen deve ser concedido por GameplayEffect aplicado no servidor, normalmente infinite.
- `GE` instant também é válido para definir o valor base final de regen, desde que seja aplicado no servidor.
- O `AttributeSet` inicia regen em `0`; sem GE/configuração ativa não há recuperação automática.
- O servidor aplica o tick real de recuperação usando os atributos de regen; cliente apenas apresenta UI.
- GEs de regen de bootstrap devem ser idempotentes e não acumular a cada morte/respawn.
- Tick padrão de regen: `2s`.
- Morto não regenera e o timer server-side é parado antes de full loot/respawn.
- Durante combat lock, regen usa multiplicadores de combate configuráveis no `Character`.
- `Status.Bleeding` bloqueia regen natural de vida.
- `State.Resource.StaminaRegenPaused`, `State.Casting`, `State.Drawing` e `State.Sprinting` pausam regen de stamina.
- `Debuff.ManaRegenPenalty` aplica multiplicador configurável sobre regen de mana.
- Snapshot de persistência deve ler recursos já reconciliados no ASC; cliente não envia valor final confiável.

## Movimento
- `MoveSpeed`: override opcional de velocidade base. Valor `0` usa `BaseWalkSpeed` do Character.
- `MoveSpeedMultiplier`: multiplicador autoritativo para buffs/debuffs de movimento, incluindo congelamento.
- Velocidade efetiva do Character: `BaseOrMoveSpeed * MoveSpeedMultiplier * InventoryWeightMultiplier`.
- `Status.Frozen` ou `Debuff.Ice` aplica fallback autoritativo de slow usando `FrozenStatusSpeedMultiplier` quando o GE/cue de congelamento nao modifica diretamente `MoveSpeedMultiplier`.
- O servidor aplica o valor final em `CharacterMovement->MaxWalkSpeed`; cliente usa replicação/reconciliação para UX.
- Novo slow/freeze deve usar GameplayEffect/tag GAS, nao RPC direto nem decisao client-side.

## Regras de uso
- Novo código e novos assets devem usar apenas `STR/AGI/INT/WIS`.
- Aliases legados só podem existir como compatibilidade passiva durante migração.
- Rework de fórmula deve respeitar `COMBAT_DESIGN_PHILOSOPHY.md` antes de alterar balance live.
