# Backlog

## Progressao
- Feito, pendente de validacao formal: XP, level, `CraftPoints` e `TalentPoints` centralizados no `USpellRiseProgressionComponent` autoritativo do `PlayerState`.
- Corrigido, pendente de validacao formal: cooldown por nivel de ability no fluxo GAS; manter configuracao data-driven e decisao final no servidor.
- Pendente BP: no `TalentTreeComponent`, chamar `EnsureWeaponSkillLevelFromTalent_Server` no `USpellRiseProgressionComponent` do `PlayerState` quando talento de arma for concedido/restaurado.
- Pendente BP: no `TalentTreeComponent`, trocar o vinculo manual de arma por `EnsureProgressionLevelFromAbilityDefinitionTalent_Server` quando houver `USpellRiseAbilityDefinition`, cobrindo arma e escola com o mesmo fluxo.
- Pendente migração: avaliar renomear `bGrantTalentPointsOnDeath`/`MinDamageContributionForTalentReward` para semântica de XP depois que os Blueprints forem resalvos com redirect seguro.
- Pendente BP: criar/configurar asset `USpellRiseProgressionBalanceData` e atribuir no `ProgressionComponent` do `ASpellRisePlayerState`; ate la o servidor usa fallback C++ seguro.
- Pendente BP: migrar `TalentTreeComponent` para ler `TalentPoints` do `USpellRiseProgressionComponent` e deixar de manter pontos como fonte de verdade.
- Pendente migração: remover do BP `PDA_AbilityDefinition` os campos legados duplicados (`Type`, `Name`, icons, descrição, regras, GAS e progressão) após copiar os valores para os campos C++ finais de `USpellRiseAbilityDefinition`.
- Pendente hardening: validar no servidor se a `AbilityDefinition` enviada para hotbar pertence ao catálogo/desbloqueios do jogador antes de aceitar o slot.

## Foco atual
1. Fechar persistência de produção.
2. Corrigir overflow de replicação no `PlayerController`.
3. Fechar gating de auth/session para fluxos sensíveis de DS.
4. Consolidar building mode com budget e validação de RPC.
5. Avançar automação multiplayer em gate contínuo/CI.

## Blockers de alta prioridade
### 0. DS 100 load gate / login bootstrap
- Severidade: Critical
- Owner: Multiplayer/Core
- Alvo: substituir carga baseada em 100 processos Editor por cliente/bot headless leve e fechar timeout de bootstrap/login observado em 32 clientes locais.
- Evidencia: `Saved\Logs\LoadNoSteam\2026-06-10_16-59-51` falhou com 15/32 conectados, timeout de conexoes aceitas e zero `FBitReader::SetOverflowed`.
- Automacao local deve limitar rodadas baseadas em processos Editor a 12 clientes salvo override explicito; 16 clientes conectou 16/16, mas gerou OOM/pagefile em `Saved\Logs\LoadNoSteam\2026-06-10_19-17-57`.
- Binarios `Binaries\Win64\SpellRiseClient.exe`/`SpellRiseServer.exe` sem cook nao servem como carga leve: falham carregando asset registry/pacotes uncooked. Usar build staged/cooked para `-UseProjectBinaries`.
- Build staged/cooked em `Saved\LoadTestBuild` permite subir `DS+2` com `SpellRiseServer.exe`/`SpellRiseClient.exe`.
- Evidencia limpa: `Saved\Logs\LoadNoSteam\2026-06-10_20-59-26` conectou 2/2, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `BitReaderOverflowSignals=0`, sem erro `FocalLength/FieldOfView` e sem spam `DLWE_Interaction`.
- Evidencia de escala: `Saved\Logs\LoadNoSteam\2026-06-10_21-02-52` conectou 12/12 com zero timeout, zero prelogin failure, zero network failure e zero `FBitReader::SetOverflowed`, mas ficou instavel por 8 ensures client-side de Gameplay Cameras (`FocalLength/FieldOfView`).
- Evidencia apos mitigacao C++: `Saved\Logs\LoadNoSteam\2026-06-10_21-38-14` conectou 12/12 com zero timeout/overflow, mas ainda ficou instavel por 4 ensures de `CameraRigAsset_2147482084`.
- Evidencia limpa apos ajuste de CameraAsset/CameraDirector: `Saved\Logs\LoadNoSteam\2026-06-11_08-49-18` conectou 12/12, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia de escala local: `Saved\Logs\LoadNoSteam\2026-06-11_09-42-59` conectou 16/16, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia com lag/loss Perfil A: `Saved\Logs\LoadNoSteam\2026-06-11_09-55-56` conectou 16/16 com `PktLag=120/PktLoss=1`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia com lag/loss Perfil B: `Saved\Logs\LoadNoSteam\2026-06-11_10-00-58` conectou 16/16 com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia `DS+24`: primeira tentativa `Saved\Logs\LoadNoSteam\2026-06-11_10-05-01` falhou por `Server full.`; runner agora injeta `?MaxPlayers=100` no servidor de carga. Nova tentativa `Saved\Logs\LoadNoSteam\2026-06-11_10-17-12` conectou 24/24, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia `DS+24` com lag/loss Perfil A: `Saved\Logs\LoadNoSteam\2026-06-11_10-30-45` conectou 24/24 com `PktLag=120/PktLoss=1`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`, sem `Server full.`.
- Evidencia `DS+24` com lag/loss Perfil B: `Saved\Logs\LoadNoSteam\2026-06-11_11-05-33` conectou 24/24 com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`, sem `Server full.`.
- Divida visual separada: ainda ha 1 warning `Both FocalLength and FieldOfView` por cliente no perfil headless; nao bloqueia gate de rede, mas deve ser removido antes de validar cliente visual real.
- Proximo criterio de saida: executar `DS+32` curto e monitorado com cleanup/kill por arvore de processo; 24 processos cooked ja exigiram intervencao manual no pos-run em Perfil A/B antes desse hardening.

### 1. PlayerController replication overflow
- Severidade: Critical
- Owner: Multiplayer/Core
- Alvo: eliminar `FBitReader::SetOverflowed` em `Standalone`, `Listen`, `DS+2` e `DS+2 lag/loss`.

### 2. Steam DS bootstrap / auth gating
- Severidade: Critical / High
- Owner: Backend/Online
- Alvo: DS com Steam válido, sem fallback indevido para `NULL`, e gating seguro de identidade.

### 3. Persistência de produção
- Severidade: S1
- Owner: Backend/Persistence
- Alvo: economia + DB operacional + safeguards + reconciliação determinística.
- Progresso: tabela `spellrise_death_events`, gravação server-side de morte e snapshot de personagem schema 7 com `CharacterCreated`/`VisualConfigurationJson` adicionados; falta validar criação/reconexão em DS+2 normal e lag/loss.

### 4. Building mode network contract
- Severidade: S2
- Owner: Gameplay/Network
- Alvo: budget de rede, matriz RPC e validação server-side de contexto/alcance/LOS.

## Gameplay — P0 bloqueante
- Corrigido, pendente de validacao formal: persistir ability hotbar, armas e itens equipados no snapshot server-side; validar restore em DS+2 e reconexao sem criar item ausente no inventario restaurado.
- Implementar/fechar fluxo de dead e revive (em progresso): estado `Downed`, aceitar morte, finalizar, levantar/reviver, full loot apenas na morte final, respawn e reconciliação.
- Corrigir inventario que nao equipa item: revisar fluxo Narrative Inventory -> equipamento -> grant/remove GAS -> attach visual, mantendo decisão no servidor.

## Gameplay — P1 alta
- Implementado e com build `SpellRiseEditor Win64 Development` aprovada em 2026-06-19: gasto autoritativo de `AttributePoints` em STR/AGI/INT/WIS via RPC owner-bound no `PlayerState`, com validacao de perfil, owner, saldo, atributo canonico, quantidade `1..10`, cap `100`, rate-limit `5/s`, mutacao pelo ASC e dirty-save server-side. Pendente ligar BP/UI e validar DS+2 normal + lag/loss + reconexao.
- Recriar projetil de flecha: restaurar pipeline free-target `aim local -> target data -> validação server -> spawn replicado -> hit/GE no servidor`.
- Refazer cálculo de dano para melhorar TTK (em progresso): consolidar fórmula com dano base da ability, arma equipada, nível da arma, escola, resist/penetration/crit e curvas de balanceamento; `AbilityLevel` permanece restrito a custo/cooldown.
- Corrigir socket de arma no `VisualOverride`: garantir attach correto quando o visual do personagem troca mesh/skeleton, sem depender de lógica de UI.
- Corrigir sockets de equipamento/projeteis: revisar attach points usados por equip/unequip, arma equipada/stowed e spawn de projeteis.
- Melhorar talent tree: permitir nível setável por talento, curva de gasto de talent points e validação server-side do custo.
- balance pass de resist / penetration / crit sob TTK alvo.

## Gameplay — P2 média
- Incluir durabilidade de itens: desgaste e reparo decididos no servidor, persistidos e auditáveis, com UI apenas apresentando o estado replicado.
- Refazer bônus derivados dos atributos base: revisar o que STR/AGI/INT/WIS concedem e documentar impacto em AttributeSet/MMC/ExecCalc/UI.
- Feito em C++, pendente UI/smoke: ate quatro boosters por categoria com custos `200/400/800/1600 TalentPoints`, todos compraveis e loadout limitado a quatro ativos; Melee/Bow/Spell dão `+5%` de dano e Divine dá `+5%` de dano e `+10%` de cura por ativo.
- Criar menu de settings para hotkeys: permitir remapeamento de inputs/hotbar sem mover estado autoritativo para UI/PlayerController.
- Melhorar ragdoll: separar decisão de morte server-side da apresentação client-side e evitar dependência em Dedicated Server.

## Gameplay — P3 baixa
- Criar spell de retorno de dano: aplicar retorno via GAS/GameplayEffect no servidor com limites anti-loop, sem confiar em dano informado pelo cliente.
- Refazer visual da talent tree com animações interativas por escolas, mantendo UI client-only e sem estado autoritativo no widget.
- Revisar luzes: ajustar iluminação/custo visual sem impactar DS/headless.

## Engenharia contínua
- automação multiplayer em CI;
- revisão da base GAS para fronteiras explícitas de activation / commit / prediction / replication.

## Média prioridade
- painel de stats e fórmulas derivadas;
- tooltip com valores live;
- refatorar `WBP_TalentTree` para distribuir `TalentTreeComponent` aos `WBP_Talent` por array/loop de filhos, removendo chamadas manuais widget-a-widget e a resolução própria no `WBP_Talent`;
- testes automatizados de dano, fall damage e autoridade de projétil.

## Regra de governança
Toda dívida nova ou alterada deve registrar:
- severidade;
- owner;
- data alvo;
- critério objetivo de saída.

## Hotfix em andamento (2026-04-06)
- Fechar BUG-2026-04-06-035: garantir spawn replicado de pickup no drop de cliente em DS com `ItemClass` valido no `BP_BasicItemPickup`.
- Revalidar fluxo completo de drop em Standalone, Listen e DS+Client apos ajuste final.
