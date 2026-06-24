# Architecture Overview

## Topologia principal
- `ASpellRiseGameModeBase` define `GameState`, `PlayerController` e `PlayerState` custom.
- O owner autoritativo do GAS é `ASpellRisePlayerState`.
- `ASpellRiseCharacterBase` consome `ASC` e `AttributeSets` do `PlayerState`.
- `USpellRiseInputRouterComponent`, subobjeto local e não replicado do `ASpellRisePlayerController`, gerencia exclusivamente cinco contextos: `IMC_Global`, `IMC_Movement`, `IMC_Combat`, `IMC_Building` e `IMC_UI`. As `Input Actions` são descobertas nos próprios IMCs por nomes canônicos e não são declaradas como propriedades no componente. O `PlayerController` mantém somente handlers/roteamento de intenção; o `Character` não adiciona mapping contexts.

## Fonte de verdade de gameplay (única)
- Estado autoritativo de gameplay: `USpellRiseAbilitySystemComponent` + `GameplayEffects` + `GameplayTags`.
- O servidor é a única autoridade que pode mutar esse estado.
- Variáveis fora do ASC (ex.: caches/UI/espelhos locais) não podem decidir resultado final de gameplay.
- `USpellRiseEquipmentComponent` no `PlayerState` é a nova fonte autoritativa de slots, grants GAS e identidade equipada. `USpellRiseWeaponComponent` no `Character` continua responsável pela execução/apresentação de armas, mas sua migração do contrato Narrative ainda está pendente.

## GAS Layout
### Ability System
- `USpellRiseAbilitySystemComponent` no `PlayerState`.
- Replication mode: `Mixed`.
- `USpellRiseAbilityHotbarComponent` vive no `PlayerState` e mapeia 16 slots lógicos (`0-7` Weapon, `8-15` Common) para `InputTag`/classe de ability já concedida.
- A seleção atual da hotbar (`SelectedAbilityInputTag`) vive no `PlayerState`, replica `OwnerOnly` e reconcilia o handle selecionado no ASC; o `Character` apenas encaminha a intenção de input. A UI reage pelo `USpellRisePlayerHUDViewModelComponent::OnSelectedAbilityChanged`, sem depender do ciclo de vida do Pawn.
- Hotbar de abilities é persistida no snapshot de personagem como slots owner-only do `PlayerState`; restore roda no servidor e revalida payload de slot/definition.
- Requisitos/contribuicao de arma ficam em `USpellRiseGameplayAbility::WeaponProgressionTag`; Blueprints de ability não devem implementar checagem estrutural de arma por classe.
- `USpellRiseGameplayAbility` declara `DamageChannelTag`, `DamageTypeTag`, `SchoolProgressionTag` e `bUsesEquippedWeaponDamage` para o pipeline de dano.
- A base `USpellRiseGameplayAbility` bloqueia ativacao por `State.Dead`; `State.Downed` é bloqueado por padrao em `CanActivateAbility` e só é liberado quando a ability marca `bAllowWhileDowned`.
- A tag raiz `GameplayAbility` não deve ser usada como `ActivationBlockedTags` genérica na classe base, pois tags filhas como `GameplayAbility.FireBall` bloqueiam a propria ativacao pelo matching hierarquico.
- Grants de abilities persistentes/startup/progressão são responsabilidade do `ASpellRisePlayerState`; o `Character` apenas inicializa `ActorInfo` como avatar atual.
- `SourceObject` de grants persistentes deve ser estável (`PlayerState`, item, instância de equipamento ou asset persistente), nunca o pawn quando a ability deve sobreviver morte/respawn.

### Inventario e loot
- O `ASpellRisePlayerState` cria em paralelo `USpellRiseInventoryComponent`, `USpellRiseEquipmentComponent`, `USpellRiseInventoryViewModelComponent` e o legado `UNarrativeInventoryComponent`.
- O inventário próprio usa `USpellRiseItemDefinition : UPrimaryDataAsset`, identidade por `FGuid ItemInstanceId` + `FPrimaryAssetId DefinitionId`, capacidade por slots/peso e `FFastArraySerializer` privado `OwnerOnly`.
- O equipamento próprio mantém nove slots e FastArray privado `OwnerOnly`. O descriptor público no Character permanece pendente; não há fan-out visual pelo PlayerState.
- Equipar/desequipar é uma transferência server-side pelo bridge de inventário. Grants e efeitos GAS são registrados por `ItemInstanceId` com source estável no `PlayerState`; `OnRep` nunca aplica grants.
- A persistência corrente permanece em `14/2`. Os contratos `15/3` com GUID e `PrimaryAssetId` estão implementados, mas só poderão ser ativados após dual-read e full loot nativo.
- Narrative continua ativo para compatibilidade de assets e para fluxos ainda não migrados: full loot, loot bags, vendor/containers, peso/movimento e integração atual do `WeaponComponent`.
- O `Character` não é owner nem fonte autoritativa de inventário/equipamento. A apresentação no Pawn deve ser reconstruível após respawn a partir do estado estável no `PlayerState`.
- UI nova deve consumir `USpellRiseInventoryViewModelComponent`, ser event-driven e enviar apenas intenções; widgets/assets UMG ainda precisam ser ligados e migrados manualmente.

### Attribute Sets
- `UBasicAttributeSet`
- `UCombatAttributeSet`
- `UResourceAttributeSet`
- `UCatalystAttributeSet`
- `UDerivedStatsAttributeSet`

### Progressao de combate
- `USpellRiseProgressionComponent` vive no `ASpellRisePlayerState`.
- Guarda `CharacterLevel`, `Experience`, `TalentPoints`, `CraftPoints` e `AttributePoints` autoritativos do personagem.
- Personagem novo inicia em level `1`, `Experience=0`, `CraftPoints=100`, `AttributePoints=0` e `TalentPoints=100`.
- Personagem novo inicia com STR, AGI, INT e WIS em `20` no `UCombatAttributeSet`.
- Tabela progressiva de XP e level-up ficam no servidor ate o level `999`; ao subir um level, o servidor concede `+100 CraftPoints` e `+100 TalentPoints`. `AttributePoints` concede `+5` apenas ate o level `65`, totalizando `320` pontos, e depois concede `0`.
- `USpellRiseProgressionBalanceData` pode substituir a tabela fallback C++ de XP/rewards por level; Dedicated Server usa fallback seguro se o asset nao estiver configurado.
- `OnCharacterProgressionChanged`, `OnWeaponSkillLevelsChanged` e `OnSchoolLevelsChanged` sao dispatchers de apresentacao para HUD/UI reagirem a replicacao sem Tick.
- Gasto de `AttributePoints` parte da UI apenas como intencao minima (`atributo enum + quantidade`) para RPC owner-bound no `PlayerState`; o servidor valida perfil carregado, owner, atributo canonico, quantidade `1..10`, rate-limit, saldo e cap permanente `100`, aplica a base de STR/AGI/INT/WIS pelo ASC, debita o saldo e marca o snapshot persistente como dirty.
- Compra de booster ofensivo parte da UI como enum minimo (`Melee`, `Bow`, `Spell` ou `Divine`) para RPC owner-bound no `PlayerState`; o servidor valida perfil, owner, rate-limit, saldo e nivel comprado na categoria, cobrando `200/400/800/1600 TalentPoints`. Cada categoria pode possuir quatro níveis.
- Ativacao/desativacao usa enum + booleano; o servidor valida ownership, booster comprado e limite global de quatro ativos. Contadores comprados e ativos sao persistidos e replicados owner-only.
- Cada booster ativo adiciona `+10` ao primario correspondente, mantendo o primario permanente limitado a `100` e o valor efetivo limitado a `140`. Também concede `+5%` de dano da categoria; Divine concede adicionalmente `+10%` de cura.
- XP comum marca o personagem como dirty para autosave periódico; level-up força atualização de rede owner-only imediata, evitando save/replication burst por kill em cenários 100+ players.
- Guarda niveis autoritativos de arma e escola/familia por `GameplayTag`.
- Replicacao owner-only para UI/UX.
- Persistencia de personagem schema 15 salva progressão, hotbar e o contrato paralelo de inventário/equipamento próprio; inventário separado usa schema 3. Schemas 14/2 continuam legíveis para migração.
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
1. `USpellRiseLifeStateComponent`, replicado no Character, é o orquestrador autoritativo de `Alive`, `Downed`, `ReviveRecovery` e `Dead`; Character apenas encaminha dano letal e executa apresentação/serviços de loot e respawn.
2. Player com `Health <= 0` entra em `State.Downed` somente se não possuir `State.Downed.Cooldown`; durante cooldown, dano letal chama morte final diretamente.
3. Downed trava `Health=1`, cancela abilities ofensivas, para regen e expira server-side após 60s. `bIsDowned` permanece apenas como espelho legado público.
4. O input Interact faz sweep local por objeto `Pawn`, registra diagnóstico e abre explicitamente o Blueprint `/Game/UI/Widgets/SpellRiseDeathScreenWidget` com `Reviver/Gankar`.
5. A escolha envia `TargetCharacter + bool bRevive` por `ServerResolveDownedAction`; o componente do alvo revalida owner, estados, distância e LOS antes de reviver ou finalizar.
5. O revive restaura recursos por WIS: em WIS 100, `20% Health`, `5% Mana` e `5% Stamina`; em WIS 140, `30% Health`, `10% Mana` e `10% Stamina`; valores intermediários usam interpolação linear clampada.
7. Enquanto downed, o componente não altera `CharacterMovement` nem flags de input; pode ativar uma `DownedAbilityClass` configurada em Blueprint somente para comportamento/apresentação explicitamente desejado. Não existe mais GA C++ específica.
7. Após revive, o ASC recebe `State.ReviveRecovery` por 3s em WIS 100 até 5s em WIS 140 e `State.Downed.Cooldown` por 300s configuráveis.
8. Aceitar morte usa `ServerAcceptDeath`; full loot, `State.Dead`, corpse despawn e respawn só acontecem em `FinalizeDeath_Server`.
9. Revive/reset cancela timers pendentes de morte/respawn; `ExecuteRespawn_Server` deve abortar se o Character nao estiver em `Dead`.
10. O componente expõe `OnShowDownedActions(Target)`, `OnHideDownedActions` e `OnLifeStateChanged`; eventos são apresentação, nunca autoridade.
11. AI não entra em downed; continua no fluxo de morte direta.

### Configuração do Life State
- Todas as regras editáveis ficam no `USpellRiseLifeStateComponent` do Character Blueprint.
- `DownedDurationSeconds`: tempo máximo agonizando.
- `DownedExpirationResult`: `FinalDeath` ou `AutomaticRevive`.
- `DownedCooldownSeconds`: intervalo após revive no qual novo dano letal causa morte final.
- `InteractionMaxDistance`: alcance de interação Revive/Gank.
- `State.Downed` não altera `CharacterMovement.MaxWalkSpeed`, `MovementMode`, `AddMovementInput`, `EnableInput`, `DisableInput` ou flags `SetIgnoreMoveInput/SetIgnoreLookInput`.
- `DownedRedEdgeAlpha`: intensidade persistente da borda vermelha enquanto downed; o client local também aplica saturação zero na câmera.
- Percentuais de Health/Mana/Stamina e duração de recovery são configuráveis separadamente para WIS 100 e WIS 140, com interpolação linear.

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
- `USpellRisePlayerHUDViewModelComponent`: componente local/event-driven no `PlayerState` que agrega nome, progressao, primarios e recursos ja replicados para Widgets, sem Tick, RPC ou estado autoritativo duplicado.
- `USpellRiseInventoryViewModelComponent`: projeção local/event-driven de inventário/equipamento para UMG; não replica, não persiste e não decide operações.
- `USpellRiseConstructionModeComponent`: isola o building mode no controller.
- Chat/combat feed: transporte nativo em C++ com autoridade no servidor.
- Chat runtime: `ASpellRisePlayerController::SubmitChatMessageForConversation` recebe da UI texto/canal/conversation ID e roteia para RPCs owner-bound; `USpellRiseChatComponent` no `GameState` sanitiza, aplica rate-limit e roteia Global/System. Whisper usa identidade estável, entrega client-only e block list server-side. Histórico/unread são exclusivamente locais e o mapa Blueprint detalhado vive em `CHAT_BLUEPRINT_MAP.md`.
- Interação de nome no chat: duplo clique é UX local, resolve o `PlayerState` replicado por nome e abre a conversa whisper sem RPC; o primeiro RPC ocorre apenas ao enviar texto.
- Party básica: `PartyId` e `PartyLeaderId` autoritativos vivem no `ASpellRisePlayerState`; convite pendente é aceito/recusado por `Y/N` no servidor; `/party`, `/invite`, `/remove`, `/leader` e `/leave` são validados pelo `USpellRiseChatComponent`. Logout remove o membro e transfere liderança quando necessário. O canal PARTY é entregue apenas a PlayerStates com o mesmo ID e o `NavigationMarker` usa o nome do PlayerState, reconcilia todos os markers locais após mudança de Party e é filtrado para owner ou membros da mesma party.

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
