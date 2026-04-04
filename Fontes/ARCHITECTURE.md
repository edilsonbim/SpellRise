# Architecture Overview

## Topologia principal
- `ASpellRiseGameModeBase` define `GameState`, `PlayerController` e `PlayerState` custom.
- O owner autoritativo do GAS é `ASpellRisePlayerState`.
- `ASpellRisePawnBase` consome `ASC` e `AttributeSets` do `PlayerState`.

## GAS Layout
### Ability System
- `USpellRiseAbilitySystemComponent` no `PlayerState`.
- Replication mode: `Mixed`.

### Attribute Sets
- `UBasicAttributeSet`
- `UCombatAttributeSet`
- `UResourceAttributeSet`
- `UCatalystAttributeSet`
- `UDerivedStatsAttributeSet`

## Contrato de authority
### Servidor
- resolve dano e mutação de vida/recursos;
- valida target data e RPC;
- aplica `GameplayEffects` autoritativos;
- decide morte, loot e respawn;
- executa fall damage e catalyst.

### Cliente
- gera input;
- prevê apenas UX;
- faz aim trace local quando necessário;
- apresenta UI/VFX/SFX.

## Fluxo de combate
1. Ability prepara spec/SetByCaller.
2. `ExecCalc_Damage` resolve canal, scaling, resistência, crit e drains.
3. Resultado escreve meta damage em `UResourceAttributeSet`.
4. `PostGameplayEffectExecute` aplica perda de vida e efeitos colaterais autorizados.

## Fluxo de projétil
1. `PlayerController` gera aim local.
2. Ability envia target data.
3. Servidor valida target data.
4. Servidor commita e spawna projétil replicado.
5. Projétil aplica GE em authority.

## Módulos de runtime relevantes
- `ASpellRisePlayerController`: input de gameplay e glue local de UX.
- `USpellRiseConstructionModeComponent`: isola o building mode no controller.
- Chat/combat feed: transporte nativo em C++ com autoridade no servidor.

## Regras estruturais
- dedicated server deve funcionar sem HUD/widget/câmera;
- lógica obrigatória de gameplay não deve depender de Blueprint de UI;
- features novas devem preferir data-driven sobre hardcode quando isso não enfraquecer a arquitetura.
