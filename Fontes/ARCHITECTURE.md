# Architecture Overview

## Topologia principal
- `ASpellRiseGameModeBase` define `GameState`, `PlayerController` e `PlayerState` custom.
- O owner autoritativo do GAS é `ASpellRisePlayerState`.
- `ASpellRiseCharacterBase` consome `ASC` e `AttributeSets` do `PlayerState`.

## Fonte de verdade de gameplay (única)
- Estado autoritativo de gameplay: `USpellRiseAbilitySystemComponent` + `GameplayEffects` + `GameplayTags`.
- O servidor é a única autoridade que pode mutar esse estado.
- Variáveis fora do ASC (ex.: caches/UI/espelhos locais) não podem decidir resultado final de gameplay.

## GAS Layout
### Ability System
- `USpellRiseAbilitySystemComponent` no `PlayerState`.
- Replication mode: `Mixed`.
- `USpellRiseAbilityHotbarComponent` vive no `PlayerState` e mapeia 16 slots lógicos (`0-7` Weapon, `8-15` Common) para `InputTag`/classe de ability já concedida.
- Requisitos/contribuicao de arma ficam em `USpellRiseGameplayAbility::WeaponProgressionTag`; Blueprints de ability não devem implementar checagem estrutural de arma por classe.
- `USpellRiseGameplayAbility` declara `DamageChannelTag`, `DamageTypeTag`, `SchoolProgressionTag` e `bUsesEquippedWeaponDamage` para o pipeline de dano.
- Grants de abilities persistentes/startup/progressão são responsabilidade do `ASpellRisePlayerState`; o `Character` apenas inicializa `ActorInfo` como avatar atual.
- `SourceObject` de grants persistentes deve ser estável (`PlayerState`, item, instância de equipamento ou asset persistente), nunca o pawn quando a ability deve sobreviver morte/respawn.

### Inventario e loot
- O inventario de player vive no `ASpellRisePlayerState` via `UNarrativeInventoryComponent`.
- O `Character` pode consumir o inventario do `PlayerState` para capacidade/peso/movimento, mas nao e owner nem source autoritativo do inventario.
- Resolucao de inventario para pawn/controller deve priorizar `PlayerState`; componentes legados no pawn nao devem ser usados como fonte de verdade de player.
- Full loot, persistencia e UI devem coletar o inventario de player pelo `PlayerState`, mantendo o source estavel durante morte/respawn.

### Attribute Sets
- `UBasicAttributeSet`
- `UCombatAttributeSet`
- `UResourceAttributeSet`
- `UCatalystAttributeSet`
- `UDerivedStatsAttributeSet`

### Progressao de combate
- `USpellRiseProgressionComponent` vive no `ASpellRisePlayerState`.
- Guarda niveis autoritativos de arma e escola/familia por `GameplayTag`.
- Replicacao owner-only para UI/UX.
- `Character` nao e fonte de verdade para progressao.
- `TalentTreeComponent` em Blueprint deve vincular talentos chamando `EnsureProgressionLevelFromAbilityDefinitionTalent_Server` no `USpellRiseProgressionComponent` do `PlayerState` quando houver `USpellRiseAbilityDefinition`; para fluxos especificos, pode chamar `EnsureWeaponSkillLevelFromTalent_Server` ou `EnsureSchoolLevelFromTalent_Server`. O BP nao deve mutar arrays de progressao diretamente.
- `ExecCalc`/helpers server-side devem consultar este componente para nivel de arma/escola quando o novo pipeline de dano for consolidado.
- AI pode receber `USpellRiseProgressionComponent` opcionalmente direto no Blueprint do actor quando precisar de progressao propria; `EnemyBase` nao deve ser obrigado a carregar este componente.
- `ExecCalc_Damage` resolve progressao em ordem: `PlayerState`, componente no owner do ASC, componente no avatar do ASC, fallback sem componente.

### Definicao de abilities
- `USpellRiseAbilityDefinition` é a fonte tipada de dados editáveis de UI, hotbar, grants, efeitos declarativos e progressão para talents/abilities.
- Campos finais de identificação/UI da definition: `DefinitionType` para categoria técnica e `DisplayName` para nome exibido; campos BP legados duplicados devem ser removidos após migração dos assets.
- `USpellRiseGameplayAbility` continua sendo a fonte de comportamento runtime: ativação, commit, prediction, target data, aplicação de GE, cast/channel e validação server-side.
- A hotbar deve ler `USpellRiseAbilityDefinition` de forma tipada primeiro e manter fallback por reflexão apenas para assets legados durante a migração.
- O cliente pode enviar intenção de slot/definition, mas o servidor deve validar grants, slot group e futuramente catálogo/desbloqueio antes de aceitar.

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

## Fluxo de morte / agonizando
1. Player com `Health <= 0` entra em `State.Downed`, trava `Health=1`, cancela abilities, para regen e bloqueia input.
2. O servidor inicia timer de 60s; ragdoll/tela de morte ficam suprimidos temporariamente para debug da mecanica.
3. Revive usa Narrative Interaction hold de 5s e chama `ReviveFromDowned_Server`, restaurando `20% MaxHealth`, `0 Mana` e `0 Stamina`.
4. Finish usa ação server-side `FinishDownedByInteractor_Server`; quando exposta por Blueprint/Narrative, deve validar alvo downed, interactor vivo e distância/trace do Narrative.
5. Aceitar morte usa `ServerAcceptDeath` chamado pela UI do jogador downed.
6. Full loot, `State.Dead`, corpse despawn e respawn só acontecem em `FinalizeDeath_Server`.
7. AI não entra em downed no v1; continua no fluxo de morte direta.

## Fluxo de combate
1. Ability prepara spec/SetByCaller.
2. `ExecCalc_Damage` resolve canal, scaling, resistência, crit e drains; multiplicadores derivados por canal de arma nao fazem parte do runtime.
3. Resultado escreve meta damage em `UResourceAttributeSet`.
4. `PostGameplayEffectExecute` aplica perda de vida e efeitos colaterais autorizados.

## Fluxo de cura
1. Ability de cura inicia por input local, evento autorizado ou efeito server-side.
2. Cliente pode prever apenas UX/VFX/SFX, sem decidir alvo ou valor final curado.
3. Servidor valida contexto, ownership, alvo, alcance/LOS quando aplicável, tags, custo e cooldown.
4. Servidor commita a ability e aplica `GameplayEffect` autoritativo de cura pelo `ASC` do `PlayerState`.
5. `ExecCalc_Healing` calcula a cura final usando `Data.BaseHeal`/`Data.Heal` e `Data.HealingScaling`.
6. Resultado escreve meta healing em `UResourceAttributeSet`.
7. `PostGameplayEffectExecute` aplica `Health += Healing` com clamp em `0..MaxHealth`.
8. UI/VFX/SFX reagem por replicação, `GameplayCue` ou espelho local sem autoridade de gameplay.

## Fluxo de projétil
1. `PlayerController` gera aim local.
2. Ability envia target data.
3. Servidor valida target data.
4. Servidor commita e spawna projétil replicado.
5. Projétil aplica GE em authority.

## Módulos de runtime relevantes
- `ASpellRisePlayerController`: input de gameplay e glue local de UX.
- `USpellRiseAbilityHotbarComponent`: loadout owner-only de hotbar, validado no servidor e consumido pelo input local.
- `USpellRiseConstructionModeComponent`: isola o building mode no controller.
- Chat/combat feed: transporte nativo em C++ com autoridade no servidor.

## Regras estruturais
- dedicated server deve funcionar sem HUD/widget/câmera;
- dedicated server não deve forçar tick de AnimBP/bones nem trace visual contínuo quando o fluxo puder ser validado sob demanda pelo servidor;
- lógica obrigatória de gameplay não deve depender de Blueprint de UI;
- features novas devem preferir data-driven sobre hardcode quando isso não enfraquecer a arquitetura.

## Pipeline de animacao
- O padrao canonico de locomocao runtime e Game Animation Sample / UE5 Manny-Quinn.
- MetaHuman entra como visual final via `VisualOverride`, nao como fonte de gameplay.
- Animacoes UEFN devem ser tratadas como fonte secundaria e retargetadas antes de uso em producao.
- Detalhes operacionais ficam em `ANIMATION_PIPELINE.md`.
