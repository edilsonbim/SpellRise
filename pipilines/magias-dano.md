# Pipeline - Magias de Dano

## GE base
- Usar `GE_ApplyDamage`.
- `Duration Policy`: `Instant`.
- `Executions`: `ExecCalc_Damage`.
- `Calculation Modifiers`: vazio, salvo caso especifico validado em C++.

## Campos na GA
- `DamageChannelTag`: canal do dano.
- `DamageTypeTag`: tipo do dano.
- `SchoolProgressionTag`: escola/familia.
- `WeaponProgressionTag`: arma exigida/contribuidora.
- `bUsesEquippedWeaponDamage`: true quando a arma equipada entra no calculo.

## SetByCaller obrigatorio
- Spell: `Data.BaseSpellDamage`.
- Arma/melee/bow: `Data.BaseWeaponDamage`.
- Arma equipada: ler `EquippedWeaponBaseDamage` quando `bUsesEquippedWeaponDamage=true`.
- Multiplicador opcional: `Data.DamageScaling`.

## Formula atual
- `Base = Data.Base* + EquippedWeaponBaseDamage` quando marcado na GA.
- `Progressao = 1 + AbilityLevel*0.005 + WeaponLevel*0.0025 + SchoolLevel*0.0025`.
- `Final antes de mitigacao = Base * Progressao * Data.DamageScaling`.
- Depois entram resistencia, penetracao, critico e drains.

## Tags de canal
- `DamageChannel.Spell`
- `DamageChannel.Melee`
- `DamageChannel.Bow`
- `DamageChannel.Environment`

## Tags de tipo
- `DamageType.Spell.Fire`
- `DamageType.Spell.Cold`
- `DamageType.Spell.Acid`
- `DamageType.Spell.Shock`
- `DamageType.Physical.Slashing`
- `DamageType.Physical.Piercing`
- `DamageType.Physical.Bashing`
- `DamageType.Physical.Impact`
- `DamageType.Poison`
- `DamageType.Bleed`
- `DamageType.Divine`
- `DamageType.Curse`
- `DamageType.Generic`

## Fluxo
1. Cliente coleta input/aim.
2. Ability envia intencao/target data.
3. Servidor valida alvo, range, LOS, custo e cooldown.
4. Servidor commita.
5. Servidor aplica `GE_ApplyDamage`.
6. `ExecCalc_Damage` calcula dano final.
7. `ResourceAttributeSet` aplica `Health -= Damage`.

## Regra
- Cliente nunca envia dano final.
- GA nao calcula progressao de arma/escola.
- Multicast apenas para VFX/SFX/UI.
- Projetil: aim local -> target data -> validacao server -> commit -> spawn replicado -> hit/GE no servidor.
