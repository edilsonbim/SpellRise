# Pipeline - Magias de Cura

## GE base
- Usar `GE_ApplyHealing`.
- `Duration Policy`: `Instant`.
- `Executions`: `ExecCalc_Healing`.
- `Calculation Modifiers`: vazio.

## SetByCaller obrigatorio
- Cura base: `Data.BaseHeal`.
- Compatibilidade legado: `Data.Heal`.
- Multiplicador opcional: `Data.HealingScaling`.

## Tags de tipo
- `HealingType.Direct`: cura direta.
- `HealingType.Area`: cura em area.
- `HealingType.Lifesteal`: retorno por roubo de vida.
- `HealingType.Transfer`: transferencia/doacao de recurso.

## Lifesteal por tempo
- GE duration no atacante concede `Status.Lifesteal` para estado/UX.
- O mesmo GE modifica `LifestealPercent`.
- Exemplo: `0.20` = cura 20% do dano real aplicado.
- O pos-dano do servidor aplica `GE_Lifesteal_Healing`.
- `GE_Lifesteal_Healing` deve usar `ExecCalc_Healing`.
- Configurar `GE_Lifesteal_Healing` no `ResourceAttributeSet`.
- Lifesteal usa `HealingType.Lifesteal`.

## Fluxo
1. Cliente coleta input/target.
2. Servidor valida alvo, range, LOS, custo e cooldown.
3. Servidor commita.
4. Servidor aplica `GE_ApplyHealing`.
5. `ExecCalc_Healing` calcula cura final.
6. `ResourceAttributeSet` aplica `Health += Healing`.
7. Clamp final: `Health <= MaxHealth`.

## Regra
- Cliente nunca envia cura final.
- Usar apenas atributos canonicos: `STR`, `AGI`, `INT`, `WIS`.
- Cura nao deve disparar fluxo de dano, morte, catalyst ou hit reaction.
