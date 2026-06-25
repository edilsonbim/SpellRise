# Changelog

- Documentado o fechamento operacional da migração em 2026-06-25: branch de migração pronto para virar `develop`, `main` permanece como linha estável e bugs restantes são tratados como herdados da versão anterior.
- Registrado que o ruído de áudio pós-migração foi causado pelo Ultra Dynamic Sky e resolvido pelo operador ao remover/adicionar novamente o plugin/asset; workarounds globais de AudioMixer não devem permanecer por esse incidente.
- Registrado que o bug de travar o personagem no downed foi resolvido; abilities após renascer/sair de downed e bug de câmera permanecem como pendências herdadas até validação/correção específica.

- Corrigido hardening pós-downed: revive/reset agora cancela timers pendentes de morte/respawn e `ExecuteRespawn_Server` ignora execução se o Character não estiver em `Dead`, evitando `UnPossess/Destroy/RestartPlayer` após voltar para `Alive`.
- Removido o bloqueio genérico `ActivationBlockedTags=GameplayAbility` do construtor base de `USpellRiseGameplayAbility`; o bloqueio por morte permanece via `State.Dead` e downed continua controlado por `bAllowWhileDowned`/`State.Downed`.
- Adicionados `USpellRiseInventoryComponent`, `USpellRiseEquipmentComponent` e `USpellRiseInventoryViewModelComponent` ao `ASpellRisePlayerState`, em paralelo ao inventário Narrative durante a migração.
- Adicionado `USpellRiseItemDefinition` e especializações data-driven; instâncias próprias usam `FGuid ItemInstanceId`, `FPrimaryAssetId DefinitionId`, quantidade, slot, durabilidade e revisão.
- Inventário próprio replica por FastArray `OwnerOnly`, sem Tick, com operações server-side de add/remove/move/split/merge/drop e RPCs de intenção limitados por payload, request id e rate-limit.
- Equipamento próprio adiciona nove slots, FastArray privado `OwnerOnly`, bloqueio 2H/OffHand e grants GAS por `ItemInstanceId`, atrás de flag default-off.
- Contratos de persistência `15/3` e conversor legacy foram adicionados, mas os schemas ativos permanecem `14/2` até dual-read, full loot e rollback serem fechados.
- Criado ViewModel event-driven para snapshots e intenções de UI. Layout UMG lateral, bindings, migração de assets e smoke DS+2 normal/lag/loss permanecem pendentes.

- Removido o legado `USpellRiseEquipmentManagerComponent` e sua `USpellRiseEquipmentInstance`; GA, persistência de loadout e ativação Narrative de armas agora usam `USpellRiseWeaponComponent`. Itens não-arma mantêm fallback visual em `NarrativeEquipment`.

- Removida `USpellRiseGA_DownedCrawl`; `USpellRiseLifeStateComponent::DownedAbilityClass` aceita uma ability Blueprint opcional. `USpellRiseGameplayAbility` ganhou `bAllowWhileDowned` para liberar somente abilities explicitamente permitidas.
- Corrigido input pós-respawn: possession restaura explicitamente `GameOnly`, cursor e flags de input pelo PlayerController.
- Interact tenta localizar player downed antes do bloqueio genérico de UI e registra `[DownedActionTrace][Input]`, permitindo diagnosticar o caminho em Dedicated Server client.
- Seleção do player downed no client deixou de depender de colisão/sweep do visual: Interact escolhe deterministicamente o melhor alvo downed no cone da câmera e alcance configurado; servidor continua revalidando distância e LOS.
- `State.Downed` deixou de alterar movimento e input: não limita `MaxWalkSpeed`, não muda `MovementMode`, não bloqueia `AddMovementInput` e não aplica `EnableInput`/`DisableInput` ou `SetIgnoreMoveInput`.
- Client local downed recebe pós-processo em escala de cinza e borda vermelha persistente configurável; efeitos são removidos no revive, morte final ou respawn.
- Configurações de downed/revive foram centralizadas e expostas no `USpellRiseLifeStateComponent`: duração, resultado ao expirar (`FinalDeath`/`AutomaticRevive`), alcance, cooldown e percentuais independentes de Health/Mana/Stamina por WIS 100..140.
- Adicionado `USpellRiseLifeStateComponent` replicado como orquestrador autoritativo dos estados `Alive`, `Downed`, `ReviveRecovery` e `Dead`, incluindo timers, cooldown, recovery, revive, gank e lifecycle da GA de crawl.
- `SpellRiseDeathScreenWidget` agora usa explicitamente o Blueprint `/Game/UI/Widgets/SpellRiseDeathScreenWidget` e expõe `OnShowDownedActions`, `OnHideDownedActions` e `OnLifeStateChanged`.
- Trace de interação downed passou a usar object sweep `Pawn` e logs categorizados para deproject, ausência de hit, alvo encontrado e rejeição.
- Build `SpellRiseEditor Win64 Development` aprovada em 2026-06-22 após integração do `USpellRiseLifeStateComponent`, Blueprint do DeathScreen e trace endurecido.
- Removidos `USpellRiseDownedInteractableComponent` e os dois interactables Narrative de downed.
- Interact agora prioriza trace nativo de player downed e abre `SpellRiseDeathScreenWidget`; o widget envia Revive/Gank por `ServerResolveDownedAction`, validado no servidor por owner, estado, distância e LOS.
- Adicionada `USpellRiseGA_DownedCrawl`, server-initiated, para permitir locomoção lenta enquanto downed; revive cancela a GA e levanta, gank finaliza morte e full loot.
- Build `SpellRiseEditor Win64 Development` aprovada com UHT em 2026-06-22 após remoção dos interactables Narrative e integração do widget/GA.

- Refeito o fluxo de downed/revive: removidos seletor `Gank/Revive`, aceite da vítima, `PendingReviver`, `ServerRespondToRevive` e `ServerChooseDownedAction`.
- Revive agora conclui após hold Narrative server-side de 5s; execução usa hold de 3s. Abilities C++ passam a bloquear ativação também por `State.Downed`.
- Revive escala linearmente por WIS 100..140: vida `20%..30%`, mana/stamina `5%..10%` e `State.ReviveRecovery` `3s..5s`.
- Após revive, `State.Downed.Cooldown` é aplicado por 300s configuráveis; dano letal durante o cooldown causa morte final direta.
- Build `SpellRiseEditor Win64 Development` com UHT aprovada em 2026-06-22 para o rework de downed/revive; smoke DS+2 normal e lag/loss permanece pendente.

- Fluxo downed atualizado: apresentação simples por animação Blueprint sem física/ragdoll, opções `Gank` e `Revive` habilitadas, revive pendente owner-only com expiração e aceite/rejeição `Y/N`; servidor revalida estado e distância antes de reviver ou finalizar.
- `SpellRiseDeathScreenWidget` Blueprint passa a ser instanciado pela classe C++ do Character; título, ações, teclas e foco da confirmação são configurados no C++, mantendo o Graph apenas para layout/apresentação.
- Corrigida apresentação owner-client do downed/morte com RPC client-only confiável; removido desenho nativo legado que sobrepunha o layout Blueprint e Gank/morte final agora troca o título para `Voce morreu`.
- A interação Narrative do alvo downed foi consolidada em um único seletor usando `SpellRiseDeathScreenWidget`: `Y=Gank` e `N=Revive`; o servidor recebe alvo + escolha mínima e revalida contexto/distância antes de executar.
- Corrigida visibilidade remota de `State.Downed`: `bIsDowned` agora replica como espelho público do estado autoritativo da ASC, permitindo que clientes reconheçam o alvo downed antes de abrir o seletor `Gank/Revive`.

- Adicionados `/clear` local por aba e `/invite Player` server-side, com PartyId no PlayerState, roteamento do canal PARTY e NavigationMarker visível entre membros.
- Party passou a exigir aceite `Y/N`, ganhou leader autoritativo, `/remove Player`, `/leader Player` e `/leave`; NavigationMarker agora exibe o nome do player.
- Adicionado `/party`, saída automática no logout e reconciliação global local dos NavigationMarkers para corrigir marker visível apenas ao leader.
- Chat passou a exibir horário `HH:mm`; markers de outros membros da Party ficam verdes e o próprio permanece branco; adicionada API local para abrir whisper por duplo clique no nome.

## Unreleased
### Documentacao / Status
- Sincronizado o recorte reportado pelo operador em 2026-06-22: ability bar ativa corrigida; indicador da ability selecionada, remake do chat com funcoes de player/admin e whisper, e Party v1 feitos. `Dead`/`revive` e clamp de atributos continuam em progresso.
- Registrados como pendentes: inventario sem equipar, drag and drop e remake geral do inventario, block 2H, cue do tornado, mapa incompleto, projetil de flecha, settings de hotkeys, socket de arma no `VisualOverride`, ragdoll, luzes, evolucao da talent tree, durabilidade e spell de retorno de dano.
- Status informados pelo operador nao foram promovidos a `Verified`; build e smoke nao foram executados nesta atualizacao documental.

### Build / Tooling
- Build policy consolidada em Unreal Source.
- Adicionado plugin Editor-only `CodexBlueprintBridge` para inspecao segura de Blueprints via commandlet JSON; validado com `ListBlueprints` sem alteracao de assets.
- Adicionado `CHAT_BLUEPRINT_MAP.md` como mapa vivo do fluxo C++/Blueprint do chat, incluindo assets, graphs, IDs de abas, unread, ordem da primeira mensagem whisper e inspeção via commandlet sem screenshots.
- Compatibilidade UE 5.7 ajustada em pontos críticos de build.
- Dependência de `CommonUI` declarada corretamente no plugin Narrative.

### Gameplay / Networking
- Unread do chat foi generalizado em C++ para todas as abas (`GLOBAL`, `PARTY`, `GUILD`, `COMBAT` e whisper por `ConversationId`), mantendo contadores exclusivamente locais e sem replicação/RPC adicional.
- Chat/whisper foi consolidado em C++ no `PlayerController`: `SubmitChatMessageForConversation` recebe explicitamente `Text + Channel + ConversationId`, roteia `Global` via `ServerSubmitChatMessage` e `Whisper` via `SendWhisperToConversation`; `SubmitChatMessage` permanece como adaptador compatível.
- Movida a seleção atual de ability do `Character` para o `PlayerState`: `SelectedAbilityInputTag` agora replica `OwnerOnly`, valida slot/contexto/rate-limit no servidor, corrige prediction rejeitada no owner e expõe `OnSelectedAbilityChanged` pelo HUD ViewModel para UI sobreviver a morte/respawn sem bind no Pawn.
- Sincronizado o status reportado pelo operador em 2026-06-21: calculo de dano/TTK, bonus dos atributos base e vendor corrigidos; boosters de atributos base feitos; `dead`/`revive` e clamp de atributos em progresso. Permanecem pendentes barra ativa, equip de inventario, block 2H, drag and drop, indicador de ability selecionada, mapa, projetil de flecha, hotkeys, socket de arma, ragdoll, luzes, evolucao visual/funcional da talent tree, durabilidade, retorno de dano e decisao sobre o `TalentTreeComponent`. Este registro nao substitui validacao formal.
- Contribuicao de mana invertida para reforcar a identidade dos primarios: `INT` concede `+2 MaxMana` por ponto e `WIS` concede `+1 MaxMana`; o baseline `20/20` continua produzindo `160 MaxMana`.
- Ativacao de booster passa a enviar o nivel `1..4`; o servidor rejeita desativar níveis intermediarios e exige ordem sequencial para ativar/desativar.
- Boosters ativos passam a conceder também `+10` ao primario correspondente (`STR/AGI/INT/WIS`), além do bônus ofensivo. O bônus ativo não é gravado como atributo permanente e é removido ao desativar.
- Boosters ofensivos agora separam posse de ativacao: cada categoria permite quatro compras por `200/400/800/1600 TalentPoints`, todos os 16 podem ser possuidos e apenas quatro ficam ativos simultaneamente. Adicionado RPC autoritativo de ativacao/desativacao e persistencia schema 13.
- Atributos primarios deixam de escalar dano/cura e passam a servir apenas a recursos/status. Adicionados quatro slots persistentes de boosters ofensivos comprados server-side por `TalentPoints`: Melee/Bow/Spell concedem `+5%` de dano por booster; Divine concede `+5%` de dano e `+10%` de cura. O cap permanente de STR/AGI/INT/WIS passa a `100`.
- Removida a progressao de `ArmorPenetration` por STR, `CritChance` por AGI e `CritDamage` por WIS nos MMCs derivados; os valores base permanecem fixos em `0`, `5%` e `1.5x`, respectivamente.
- Implementado o fluxo C++ autoritativo de gasto de `AttributePoints`: UI envia apenas atributo canonico e quantidade ao `PlayerState`; servidor valida owner, perfil persistente aplicado, saldo, quantidade `1..10`, cap `100` e rate-limit `5/s`, aplica a base pelo ASC, replica pelos atributos/saldo owner-only existentes e marca o personagem dirty para persistencia. Build `SpellRiseEditor Win64 Development` aprovada em 2026-06-19; BP/UI e validacao multiplayer continuam pendentes.
- Registrada como prioridade alta a mecanica de gastar `AttributePoints` em STR/AGI/INT/WIS, com validacao server-side de saldo, atributo canonico, quantidade, rate-limit e cap antes da aplicacao autoritativa via GAS e persistencia.
- Documentado o recorte de gameplay de 2026-06-18: persistencia de ability hotbar, armas e itens equipados; progressao de XP/level/Craft Points/Talent Points; e cooldown por nivel de ability foram reportados como corrigidos/feitos pelo operador, ainda sem nova validacao formal neste registro.
- Registradas pendencias de inventario/equipamento, projetil de flecha, hotkeys, socket no `VisualOverride`, ragdoll, luzes, talent tree, boosters/bonus de atributos, durabilidade de itens e spell de retorno de dano; `dead`/`revive` e rework de TTK permanecem em progresso.
- Recursos derivados passam a usar base `100` e o valor total dos primarios: `MaxHealth = 100 + 3*STR`, `MaxMana = 100 + 2*INT + WIS`, `MaxStamina = 100 + 3*AGI` e `CarryWeight = 2*STR`. Com os quatro primarios iniciais em `20`, o personagem novo inicia com caps de `160` para vida, mana e stamina.
- `ExecCalc_Damage` passa a resolver metadados de dano uma unica vez por execucao, priorizar cache/ponte via `PlayerState` para progressao, capturar apenas o atributo primario necessario ao canal e expor `QUICK_SCOPE_CYCLE_COUNTER` para perfil em Unreal Insights.
- `ExecCalc_Damage` passa a aplicar escala PvP por level efetivo capado em `65` antes de mitigacao/crit. Levels iguais mantem dano `1:1`; `65 vs 10` usa `Damage * 0.5 + Damage * 0.5 / 65 * 10`; acima de `65`, o level efetivo continua `65`.
- `USpellRiseProgressionComponent` passa a guardar `Experience`, `CharacterLevel`, `TalentPoints`, `CraftPoints` e `AttributePoints` owner-only no `PlayerState`; personagem novo inicia level 1 com 100 Talent Points, 100 Craft Points, 0 Attribute Points e primarios STR/AGI/INT/WIS em 20. Cada level ate 999 concede +100 Craft Points e +100 Talent Points; Attribute Points concedem +5 apenas ate o level 65, totalizando 320 pontos.
- `USpellRiseProgressionComponent` passa a expor dispatchers `OnCharacterProgressionChanged`, `OnWeaponSkillLevelsChanged` e `OnSchoolLevelsChanged` para HUD/UI atualizar por evento replicado sem Tick.
- Progressão/XP foi ajustada para escala 100+ players: RepNotify owner-only agora dispara apenas quando há mudança real, XP comum não força net update imediato e reward de inimigo marca o personagem como dirty para autosave periódico em vez de salvar no storage a cada kill/contributor.
- Reward de morte de inimigo renomeia `TalentPointsOnKill` para `ExperiencePointOnKill`, com CoreRedirect para preservar valores de assets antigos.
- Adicionado `USpellRiseProgressionBalanceData` como `PrimaryDataAsset` C++ para tabela data-driven de XP total e rewards por level, com fallback server-side seguro quando o asset nao estiver configurado.
- Reward de morte de inimigo passa a conceder XP proporcional por contribuicao de dano; `ExperiencePointOnKill` alimenta XP e nao Talent Points diretos.
- Contrato anterior de escala final por atributo primario foi substituido pelos boosters ofensivos; STR/AGI/INT/WIS permanecem apenas como fonte de recursos/status.
- `ExecCalc_Damage` passa a escalar dano da ability por nivel da escola e dano da arma por nivel da arma, cada um com 50% fixo + ate 50% por progressao; `AbilityLevel` deixa de afetar dano e permanece restrito a custo/cooldown. O debug `sr.Damage.Debug` agora registra as contribuicoes separadas de ability e arma.
- `USpellRiseProgressionComponent` passa a expor `EnsureSchoolLevelFromTalent_Server` e `EnsureProgressionLevelFromAbilityDefinitionTalent_Server`, permitindo que talentos de escola como `DA_Talent_Fire` atualizem `SchoolLevels` autoritativos no `PlayerState` em vez de cair no fluxo de arma com tag vazia.
- Implementado fluxo player-only de `State.Downed`: ao zerar vida, o player agoniza por 60s com vida travada em 1 e regen parada; morte final, full loot e respawn foram movidos para `FinalizeDeath_Server`.
- Adicionado `USpellRiseDownedInteractableComponent` para interações Narrative de revive/finish em alvo agonizando; revive server-side retorna com `20% MaxHealth`, `0 Mana` e `0 Stamina`.
- `USpellRiseDeathScreenWidget` passa a expor `RequestAcceptDeath`, permitindo UI chamar `ServerAcceptDeath` enquanto o player está em `State.Downed`.
- Ragdoll, fade/camera e tela de morte foram suprimidos temporariamente no fluxo de morte/downed para testar a mecanica server-side limpa.
- Adicionado atributo `EquippedWeaponBaseDamage` em `UCombatAttributeSet` para o dano base da arma equipada, alimentado por GE de equipamento via `Data.EquippedWeaponBaseDamage`.
- `ExecCalc_Damage` passa a consumir `EquippedWeaponBaseDamage`, `AbilityLevel`, nivel de arma e nivel de escola quando a ability declara os metadados de progressao.
- `ExecCalc_Damage` passa a aceitar `USpellRiseProgressionComponent` opcional em actors de AI, configuravel por Blueprint, sem exigir o componente em `EnemyBase`.
- Progressao de arma passa a usar `Progression.Weapon.TwoHandSword` no lugar de `Progression.Weapon.Katana`; o mapeamento C++ ainda aceita tag legada `.Katana` como compatibilidade temporaria para assets antigos.
- `USpellRiseGameplayAbility` passa a declarar metadados de dano/progressao (`DamageChannelTag`, `DamageTypeTag`, `SchoolProgressionTag`, `WeaponProgressionTag`, `bUsesEquippedWeaponDamage`) e remove os campos antigos de requisito/bloqueio por `WeaponTag`.
- Adicionado `USpellRiseAbilityDefinition` como data asset C++ tipado para consolidar dados editaveis de UI/hotbar/grants/effects/progressao de talents e abilities; hotbar passa a ler a classe tipada primeiro e preserva fallback legado por reflection. `DefinitionType` e `DisplayName` passam a ser os campos finais para tipo técnico e nome exibido.
- Adicionadas tags de progressao `Progression.Weapon.*` e `Progression.School.*` para o novo componente de progressao no `PlayerState`.
- Adicionado `USpellRiseProgressionComponent` no `ASpellRisePlayerState` para armazenar niveis owner-only de arma e escola por `GameplayTag`.
- `ASpellRisePlayerState` passa a expor `GetProgressionComponent` para Blueprint, e `USpellRiseProgressionComponent` passa a expor `EnsureWeaponSkillLevelFromTalent_Server` para o `TalentTreeComponent` BP vincular talentos de arma aos niveis autoritativos de progressao.
- Removido `HealingMultiplier` do runtime C++; cura passa a depender de base/scaling explicito e futura progressao de escola/GA.
- Removidos multiplicadores derivados de dano por canal (`MeleeDamageMultiplier`, `BowDamageMultiplier`, `SpellDamageMultiplier`) do runtime C++ para abrir caminho ao modelo baseado em nivel da GA, arma equipada, nivel de arma e escola.
- Adicionado suporte base de lifesteal server-authoritative: `LifestealPercent` em `UCombatAttributeSet`, `Status.Lifesteal` para estado/UX e cura pós-dano real via `GE_Lifesteal_Healing`.
- Adicionado contrato de cura GAS server-authoritative: `ExecCalc_Healing` calcula cura por `Data.BaseHeal`/`Data.Heal` e `Data.HealingScaling`; `UResourceAttributeSet` consome meta `Healing` e aplica clamp em `Health <= MaxHealth`.
- Corrigido input da ability hotbar para respeitar `USpellRiseGameplayAbility::bFireOnAbilityInput`: slots que apontam direto para `AbilityClass` agora apenas armam abilities configuradas para disparar pelo `Primary`, em vez de ativá-las imediatamente.
- Documentado recorte de correcoes reportadas em 2026-06-14: inventario/vendor/loot fechando aceitavelmente, barra de ability ao morrer, regen de atributos, animacao equip/unequip, camera sem arma, AoE sem dano, `Blizzard` nao entrando mais no solo e `shoot arrow` corrigido. Validacao formal ainda pendente quando nao houver evidencia de build/smoke registrada.
- Registrados como pendencias ativas: sockets, melhoria de ragdoll, fluxo `dead`/`revive` e revisao de luzes.
- Movido o contrato de grant/remove/evento de abilities de player para `ASpellRisePlayerState`, mantendo o `Character` apenas como avatar do ASC.
- Grants persistentes/startup passam a usar source estável no `PlayerState`; grants de equipamento continuam usando item/instância como source.
- Inventario de player movido para `ASpellRisePlayerState`; resolucao de inventario para pawn/controller passa a priorizar o `PlayerState`, e full loot/persistencia/UI deixam de coletar inventario do `Character` como fonte de verdade.
- Interacao Narrative instantanea em Dedicated Server passa a completar no proprio `BeginInteract` apos trace/`CanInteract` server-side; hold-interact reativa tick no DS apenas enquanto necessario. Corrige containers/loot que nao abriam porque `SetLootSource` nunca era executado no ramo autoritativo.
- `UInventoryWidget` passa a capturar foco de teclado, tratar ESC nativamente, chamar `StopLooting` no inventario do owning player e restaurar input `GameOnly` ao fechar inventory/loot; `ASpellRisePlayerController` tambem fecha looting via ESC/clear antes do bloqueio de gameplay, cobrindo BP/widget sem foco confiavel.
- Auditoria P0 de performance DS 100: tick local do `CharacterBase` passa a ligar somente para pawn local vivo; Dedicated Server não força refresh de bones/AnimBP por padrão; trace contínuo do `NarrativeInteractionComponent` foi desativado no DS com validação sob demanda no RPC de interação.
- Chat público deixa de usar multicast reliable; RPC genérico de gameplay event passa a rejeitar target data, objetos opcionais, context handle e tag containers enviados pelo cliente.
- Multicasts visuais de morte, corpse despawn e refresh visual de equipment deixam de usar reliable; a decisão autoritativa continua em GE/tag/estado do servidor.
- Bootstrap NoSteam em escala local passa a manter pending persistent id por endereco de conexao com porta, evitando colisao entre varios clientes `127.0.0.1`.
- `Run-Load-NoSteam-Scale.ps1` passa a bloquear cargas locais acima de 12 clientes sem `-AllowHighLocalClientCount`, evitando saturar RAM/pagefile da maquina de desenvolvimento durante auditoria.
- `Run-Load-NoSteam-Scale.ps1` passa a usar `-nosound` e a contar apenas timeouts reais de rede, evitando falso positivo causado pelo parametro `ConnectionTimeout`.
- `Run-Load-NoSteam-Scale.ps1` passa a enviar `InitialConnectTimeout` e `ConnectionTimeout` tambem na URL de conexao do cliente, que e o caminho lido pelo `PendingNetDriver`.
- `Run-Load-NoSteam-Scale.ps1` passa a suportar `-UseProjectBinaries` para usar `SpellRiseServer.exe`/`SpellRiseClient.exe` em vez de `UnrealEditor-Cmd`, reduzindo custo local da carga.
- Adicionado `Scripts/Build-Cooked-Load-NoSteam.ps1` para preparar build staged/cooked usado por carga com binarios de projeto.
- `Scripts/Build-Cooked-Load-NoSteam.ps1` passa a gerar client e server em etapas separadas (`-TargetSet Client|Server|Both`), garantindo que `SpellRiseServer.exe` entre no archive/stage.
- Primeiro `DS+2` com binarios staged/cooked conectou 2/2 sem timeout e sem `FBitReader::SetOverflowed`; o run inicial ficou bloqueado por ensure client-side de Gameplay Cameras e warnings Blueprint de `DLWE_Interaction`.
- Gate `DS+2` staged/cooked revalidado apos ajustes de assets: `Saved\Logs\LoadNoSteam\2026-06-10_20-59-26` passou com 2/2 clientes, `OverallUnstable=False`, zero ensure, zero timeout, zero overflow, sem erro `FocalLength/FieldOfView` e sem spam `DLWE_Interaction`.
- Gate staged/cooked com 12 clientes (`Saved\Logs\LoadNoSteam\2026-06-10_21-02-52`) conectou 12/12 com zero timeout, zero prelogin/network failure e zero overflow, mas reabriu o bloqueio de Gameplay Cameras por 8 ensures `FocalLength/FieldOfView`.
- `Run-Load-NoSteam-Scale.ps1` passa a desligar Gameplay Cameras por padrao apenas em clientes sinteticos `-unattended/-nullrhi`; usar `-EnableGameplayCameras` para reproduzir o fluxo visual real.
- `ASpellRisePlayerController` e `ASpellRiseCharacterBase` passam a respeitar `-SpellRiseSkipLocalHUDFlow` para carga headless, evitando parte do fluxo visual local; `Saved\Logs\LoadNoSteam\2026-06-10_21-38-14` ainda falha por 4 ensures no `CameraRigAsset_2147482084`, entao a correcao definitiva continua no asset/runtime de camera.
- Ajuste de CameraAsset/CameraDirector removeu o bloqueio de Gameplay Cameras no gate: `Saved\Logs\LoadNoSteam\2026-06-11_08-49-18` passou com 12/12 clientes, zero ensure, zero timeout e zero overflow; warnings residuais `FocalLength/FieldOfView` sem ensure permanecem como follow-up visual.
- Gate staged/cooked local escalado para 16 clientes em `Saved\Logs\LoadNoSteam\2026-06-11_09-42-59`: 16/16 conectados, `OverallUnstable=False`, zero ensure, zero timeout, zero prelogin/network failure e zero overflow; warnings de pose mista de camera seguem como divida visual.
- `Run-Load-NoSteam-Scale.ps1` passa a aplicar emulacao de rede via `NetEmulation.PktLag/PktLoss` e argumentos `PktLag/PktLoss`; Perfil A `DS+16` em `Saved\Logs\LoadNoSteam\2026-06-11_09-55-56` passou com `LagLossApplied=True`, 16/16 conectados, zero ensure, timeout e overflow.
- Perfil B `DS+16` em `Saved\Logs\LoadNoSteam\2026-06-11_10-00-58` passou com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, 16/16 conectados, zero ensure, timeout, prelogin/network failure e overflow.
- `Run-Load-NoSteam-Scale.ps1` passa a iniciar o servidor de carga com `?MaxPlayers=100` por padrao (`-LoadTestMaxPlayers`), evitando falso bloqueio `Server full.` em cenarios acima de 16 clientes; `DS+24` em `Saved\Logs\LoadNoSteam\2026-06-11_10-17-12` passou com 24/24 conectados, zero ensure, timeout, network failure e overflow.
- Perfil A `DS+24` em `Saved\Logs\LoadNoSteam\2026-06-11_10-30-45` passou com `PktLag=120/PktLoss=1`, `LagLossApplied=True`, 24/24 conectados, zero ensure, timeout, prelogin/network failure, `Server full.` e overflow.
- Perfil B `DS+24` em `Saved\Logs\LoadNoSteam\2026-06-11_11-05-33` passou com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, 24/24 conectados, zero ensure, timeout, prelogin/network failure, `Server full.` e overflow.
- Cleanup do `Run-Load-NoSteam-Scale.ps1` passa a aguardar encerramento por `ProcessExitGraceSeconds` e usar `taskkill /PID /T /F` como fallback por arvore de processo, evitando clientes cooked residuais apos rodadas pesadas.
- Atributos privados de recurso (`Mana`, `Stamina`, regens e `CarryWeight`) passam a replicar `OwnerOnly`; `Health`/`MaxHealth` permanecem públicos.
- RPCs de inventário Narrative receberam rate-limit server-side e limite de quantidade por chamada, com log `InventoryRpcRejected`.
- `Run-Load-NoSteam-Scale.ps1` passa a suportar `-WithInsightsTrace` e `-WithPerfStats` para gerar `.utrace` e coletar stats de rede/gameplay nos cenários de escala.
- Base C++ da hotbar de abilities 8+8 adicionada: slots `0-7` para weapon abilities e `8-15` para common abilities em componente owner-only no `PlayerState`, com RPC server-side validado/rate-limitado para edição de slots.
- `USpellRiseGameplayAbility` passou a suportar validacao C++ de arma por tags; contrato atual usa `WeaponProgressionTag`.
- `USpellRiseWeaponComponent` remove animação/notify de equip/unequip e visual stowed; apenas arma equipada fica visível, anexada ao socket equipado, priorizando `VisualOverride` válido sem modificar o mesh.
- Combat log autoritativo com transporte nativo em C++.
- Fall damage agora resolve causer/instigator como causa ambiental determinística quando o GE é aplicado pelo ASC do próprio alvo, evitando self-causer em chat/combat log/death event.
- Combat log owner-only limitado para reduzir burst de replicação: buffer replicado padrão reduzido para 50 entradas, snapshot inicial limitado às 30 últimas e `ForceNetUpdate` rate-limitado em spam de dano.
- Hardening de RPCs críticos de gameplay e respawn.
- Bootstrap/auth de Dedicated Server agora registra estado do OnlineSubsystem, registra `SteamAuthEnabled`, falha cedo quando Steam e obrigatorio mas o processo degradou para `NULL`, e exige o packet handler `OnlineSubsystemSteam.SteamAuthComponentModuleInterface` para autenticar ticket Steam no handshake.
- Validação server-side para target data de projétil.
- Removido `ManaCostReduction` dos atributos derivados; redução/custo de mana passa a ser responsabilidade de GE/ability level.
- Removido `CastTimeReduction` dos atributos derivados; duração/cast time passa a ser responsabilidade de GE/ability level.
- `MoveSpeed`/`MoveSpeedMultiplier` agora alimentam `CharacterMovement->MaxWalkSpeed` via Character, permitindo slow/freeze autoritativo por GameplayEffect.
- Observabilidade de slow/freeze passa a registrar mudanças reais de `MaxWalkSpeed`; logs de persistência com ID `DEV_` em PIE foram reduzidos para `Verbose`.
- Reward de morte de inimigo passa a usar o maior contribuidor como fallback de `KillerPS` quando o contexto fatal nao resolve PlayerState.
- Clamp anterior dos primários havia sido ajustado para `0..120`; schema 8 eleva o teto runtime para `0..140`.
- Regen de `Health/Mana/Stamina` passa a usar atributos finais modificados por GE server-side, com tick autoritativo de 2s no `Character`, aplicação idempotente dos GEs de bootstrap, bloqueio por morte/sangramento, multiplicadores de combate, pausa de stamina por ação e penalidade de mana por debuff.
- Regen de bootstrap agora zera os atributos `HealthRegen/ManaRegen/StaminaRegen` antes de aplicar GE instant/infinite e o tick de 2s aplica o valor do atributo uma vez por tick, evitando acúmulo e escala indevida por segundo.
- Contrato de grant GAS ajustado: `FSpellRiseGrantedAbility` não carrega mais level; o level editável fica no Blueprint callable do `CharacterBase`, enquanto grants por source/inimigo usam default server-side seguro.
- Fluxo de death -> full loot -> respawn fechado no recorte atual.
- Full loot da morte ajustado para spawn com delay de 3s e verificação de piso no servidor (evita bag presa no ar).
- Hook determinístico de QA para morte em smoke multiplayer.
- Building mode isolado em componente dedicado no `PlayerController`.
- Gate multiplayer validado em `DS+2` com reconnect + perfis de lag/loss A (120/1) e B (180/3), sem overflow de replicação no run baseline `2026-04-02_21-03-10`.

### Persistence / Observability
- Adicionado `USpellRisePlayerHUDViewModelComponent` no `PlayerState`: Widgets podem consumir um snapshot unico com display name, level, XP/progresso, STR/AGI/INT/WIS e Health/Mana/Stamina, atualizado por delegates de GAS/progressao sem Tick ou replicacao nova.
- Adicionado override seguro `-SRTestPersistentId=<SteamID64>` para reutilizar o mesmo personagem em testes de progressao; aceita somente ID numerico de 17 digitos em PIE ou NoSteam de teste e e ignorado em contexto de producao.
- Snapshot de personagem schema 11 passa a persistir hotbar de abilities, itens equipados e loadout de armas/offhand; restore roda server-side depois do inventario ser recriado e só reaplica itens existentes no inventario do `PlayerState`.
- Snapshot de personagem schema 10 passa a persistir `CharacterLevel`, `Experience`, `TalentPoints`, `CraftPoints` e `AttributePoints`, mantendo schemas 8/9 carregaveis com defaults seguros de progressao.
- Snapshot de personagem schema 8 passa a persistir `WeaponSkillLevels`, `SchoolLevels` e `AbilityLevel` por talento, mantendo load antigo compativel via fallback para `Level`.
- Clamp persistente dos primarios ajustado para `0..100`; boosters ofensivos nao elevam STR/AGI/INT/WIS.
- Persistence foundation v1 ativa com snapshots de personagem e inventário.
- Snapshot de personagem schema 7 passa a persistir `CharacterCreated` e `VisualConfigurationJson`, com colunas auxiliares no PostgreSQL para checagem rápida de criação de personagem por player.
- Snapshots de personagem/inventário agora exigem SteamID64 real; IDs `DEV_`/`NULL` ficam sem gravação em DB e recebem defaults runtime.
- `PreLogin` passa a consultar denylist opcional por IP em `spellrise_portal_ip_bans`, além do ban existente por `steam_id64`.
- Snapshot de personagem evoluído para persistir talentos e pontos disponíveis; talentos deixam de depender de grant padrão em login.
- Persistência de talentos endurecida por conta SteamId, com nível de talento salvo/reaplicado clampado em `1..100`.
- Eventos de morte passam a ser persistidos em `spellrise_death_events`, com vítima, data/hora, maior causador de dano, golpe fatal, causa e mensagem auditável server-side.
- Combat log durável owner-centric em `PlayerState`.
- Gate local de smoke multiplayer com reconnect e lag/loss.

### Known Active Risks
- bootstrap/auth Steam em DS pendente de build/link e validação Steam real;
- persistência de produção ainda incompleta;
- contrato final de rede do building mode ainda pendente.

### Inventory / Equipment / Drop (2026-04-06)
- Fluxo de equip/unequip via inventory + quick slots consolidado com `SpellRiseEquipmentManagerComponent`.
- Ajustes de sync visual entre preview (`WBP_PlayerPreview`) e estado de quick slots.
- Implementado caminho C++ para request de drop com limpeza de vínculos de equipamento antes da remoção do item.
- Incluídas tentativas de robustez para spawn de pickup no servidor (fallback de classe + inicialização refletida).
- Pendente: validação final do spawn do pickup em cliente DS no fluxo de drop (ver BUG-2026-04-06-035).
# 2026-06-23

- Extraído o gerenciamento de Enhanced Input para `USpellRiseInputRouterComponent`, local e não replicado.
- Removidos do `ASpellRisePlayerController` os IMCs, Input Actions, prioridades e fallback legado `DefaultMappingContext`.
- Removido do `ASpellRiseCharacterBase` o fallback legado `IMC_Default` e sua adição direta ao subsystem.
- O router foi consolidado em cinco contextos (`Global`, `Movement`, `Combat`, `Building`, `UI`), com prioridades internas fixas e descoberta das actions diretamente nos IMCs por nomes canônicos.
- Removidas todas as propriedades editáveis de `Input Action` e prioridades do componente; `IA_ToggleUIInteraction` e `IA_DebugGrantExperience` também são resolvidas pelo `IMC_Global`.
- Build `SpellRiseEditor Win64 Development` aprovada com UHT usando `C:\UnrealSource\UnrealEngine`.
