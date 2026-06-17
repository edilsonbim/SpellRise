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
- `ArmorPenetration = 0.00 + 0.30 * T`
- Escala dano melee: `Damage = Damage * 0.50 + Damage * 0.50 * clamp(STR, 0, 100) / 100`

### AGI
- `CritChance = 0.05 + 0.20 * T`
- Escala dano bow: `Damage = Damage * 0.50 + Damage * 0.50 * clamp(AGI, 0, 100) / 100`

### INT
- Reservado para progressão de magia/escolas e recursos derivados.
- Escala dano spell: `Damage = Damage * 0.50 + Damage * 0.50 * clamp(INT, 0, 100) / 100`

### WIS
- `CritDamageMultiplier = 1.50 + 0.50 * T`
- Escala dano divine e cura: `Value = Value * 0.50 + Value * 0.50 * clamp(WIS, 0, 100) / 100`

## Dano e progressao
- Multiplicadores derivados por canal de arma foram removidos do runtime.
- Dano deve vir do dano base da ability, dano base da arma equipada, nivel da arma, nivel da escola e atributo primario aplicavel.
- `EquippedWeaponBaseDamage` vive em `UCombatAttributeSet`, replica `OwnerOnly` e deve ser setado/removido por GE aplicado pelo `WeaponComponent` a partir da `WeaponDefinition`.
- O SetByCaller padrao para o GE de arma e `Data.EquippedWeaponBaseDamage`.
- Formula atual do `ExecCalc_Damage`: `((BaseDaGA * 0.50 + clamp(BaseDaGA * 0.50 * SchoolLevel/100, 0, BaseDaGA * 0.50)) + (EquippedWeaponBaseDamage * 0.50 + clamp(EquippedWeaponBaseDamage * 0.50 * WeaponLevel/100, 0, EquippedWeaponBaseDamage * 0.50) quando habilitado)) * Data.DamageScaling`, seguido pela escala do atributo primario aplicavel.
- `AbilityLevel` nao escala mais dano; nivel da ability deve afetar apenas custo e cooldown.
- `DamageChannel.*` classifica o fluxo de dano, mas nao aplica multiplicador por si so.
- `ExecCalc_Damage` continua responsavel por resistencia, penetracao, critico e drains.
- Para players, `WeaponLevel` e `SchoolLevel` vêm do `USpellRiseProgressionComponent` no `PlayerState`.
- Para AI especial, o mesmo componente pode ser adicionado manualmente no Blueprint do actor; AI sem componente usa fallback seguro sem progressao propria.

## Cura
- Cura é mutação autoritativa de `Health` e deve ser aplicada pelo servidor via `GameplayEffect`.
- `ExecCalc_Healing` usa `Data.BaseHeal` com fallback compatível para `Data.Heal`, multiplicador opcional `Data.HealingScaling` e escala final por WIS.
- Cliente pode exibir previsão/feedback, mas não envia valor final de cura confiável.
- Cura é clampada contra `MaxHealth` no `AttributeSet`.
- Novo código de cura deve usar apenas STR, AGI, INT e WIS conforme a matriz canônica.

## Lifesteal
- `LifestealPercent` é atributo temporário de sustain em `UCombatAttributeSet`.
- Faixa efetiva: `0.0..1.0`.
- GEs duration podem modificar `LifestealPercent` e conceder `Status.Lifesteal` para estado/UX.
- O servidor calcula cura de lifesteal após dano real aplicado: `ActualDamageApplied * LifestealPercent`.
- Cura de lifesteal usa `GE_Lifesteal_Healing` e `HealingType.Lifesteal`.
- A tag `Status.Lifesteal` sozinha não aplica cura.

## Caps de recurso
Usando bônus sobre baseline:
- `MaxHealth = 180 + 2 * STR_bonus + 1 * WIS_bonus`
- `MaxMana = 180 + 2 * INT_bonus + 1 * WIS_bonus`
- `MaxStamina = 180 + 1 * STR_bonus + 2 * AGI_bonus`

## Regeneração de recursos
- `HealthRegen`, `ManaRegen` e `StaminaRegen` são atributos de valor final, replicados `OwnerOnly`.
- O valor base de regen deve ser concedido por GameplayEffect aplicado no servidor, normalmente infinite.
- `GE` instant também é válido para definir o valor base final de regen, desde que seja aplicado no servidor e represente valor por tick.
- O `AttributeSet` inicia regen em `0`; sem GE/configuração ativa não há recuperação automática.
- O servidor aplica o tick real de recuperação usando os atributos de regen; cliente apenas apresenta UI.
- GEs de regen de bootstrap devem ser idempotentes e não acumular a cada morte/respawn.
- Tick padrão de regen: `2s`; `HealthRegen`, `ManaRegen` e `StaminaRegen` são aplicados uma vez por tick, sem multiplicar pelo intervalo.
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
