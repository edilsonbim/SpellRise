# Bug Log

## Template
- ID:
- Date:
- Severity: Critical | High | Medium | Low
- Status: Open | In Progress | Fixed | Verified | Closed
- Area:
- Issue:
- Reproduction:
- Expected:
- Actual:
- Root Cause:
- Fix:
- Tested On:
- Standalone:
- Listen Server:
- Dedicated Server:
- Owner:

## Template complementar (incidente de replicação)
- Net Scope: combate | GAS | projétil | atributos | morte/loot | building | controller
- Authority Boundary: quem decide estado final no servidor
- Prediction Path (Client): input local / predição usada
- Server Validation: checks de ownership/contexto/payload/rate-limit
- RPCs afetados:
- OnRep afetados:
- Replication Condition envolvida: None | OwnerOnly | SkipOwner | Other
- FailureTags observadas (GA):
- Overflow (`FBitReader::SetOverflowed`): sim/nao + contagem
- Cenário com lag/loss: Perfil A (120/1) | Perfil B (180/3) | ambos | nenhum
- Evidência de log (paths):

## Open Issues
### BUG-2026-06-13-043
- Date: 2026-06-13
- Severity: High
- Status: Open
- Area: Character Creation / MetaHuman / Mutable / Animation
- Issue: editor crash durante PIE apos fluxo de morte/equip, com assert em avaliacao paralela de skeletal mesh.
- Reproduction: em PIE, testar personagem com visual MetaHuman/Mutable, equipar/desequipar staff, matar personagem por fall damage ou fluxo equivalente de death/respawn; crash ocorreu durante um dos ciclos.
- Expected: troca/atualizacao de visual, morte, equip e animacao devem completar sem crash; death/respawn nao deve avaliar mesh com skeleton/transforms inconsistentes.
- Actual: editor fechou com assert `GetRefSkeleton().GetNum() == OutComponentSpaceTransforms.Num()` em `SkinnedAsset.cpp:462`.
- Root Cause: pendente. Causa provavel: `CO_MetaHuman`/Mutable atualizando skeletal mesh async enquanto `USkeletalMeshComponent` executa `ParallelAnimationEvaluation`, ou asset de pose/animacao desatualizado causando mismatch de ref skeleton/transforms.
- Fix: pendente. Investigar pipeline de criacao de personagem/visual override para bloquear ou sincronizar troca de skeletal mesh durante avaliacao de animacao, especialmente perto de death/ragdoll/equip.
- Tested On: `Saved\Logs\SpellRise.log`, crash `Saved\Crashes\UECC-Windows-5EE5E20B4EFC910BCF9437888DC92DF7_0000`.
- Standalone: pendente
- Listen Server: pendente
- Dedicated Server: provavelmente nao aplicavel ao crash visual, mas validar que DS nao depende de MetaHuman/Mutable.
- Owner: Character Creation/Animation

### BUG-2026-06-12-041
- Date: 2026-06-12
- Severity: High
- Status: Fixed
- Area: Inventory / Loot / Narrative Interaction / Dedicated Server
- Issue: containers/loot Narrative nao abriam de forma autoritativa em Dedicated Server.
- Reproduction: interagir com chest/loot bag em client conectado ao DS usando interacao Narrative instantanea.
- Expected: servidor valida trace/interactable, executa `OnInteracted`, chama `SetLootSource` no inventario do jogador e replica `LootSource` `OwnerOnly` para abrir UI no client dono.
- Actual: client executava apenas o caminho local/remoto do Blueprint; o ramo `Authority` que seta `LootSource` nao completava no DS.
- Root Cause: `UNarrativeInteractionComponent` desliga tick continuo no Dedicated Server por padrao; `ServerBeginInteract` fazia o trace sob demanda, mas a conclusao de interacao instantanea (`InteractionTime <= 0`) dependia do proximo tick, que estava desabilitado.
- Fix: `BeginInteract` agora completa interacoes instantaneas imediatamente apos `CanInteract`; hold-interact no DS reativa tick apenas enquanto houver tempo restante e volta ao estado idle depois.
- Net Scope: morte/loot
- Authority Boundary: servidor decide a interacao final e o `LootSource`; client apenas inicia input/RPC e apresenta UI via `OnRep_LootSource`.
- Prediction Path (Client): interacao local pode apresentar feedback, mas nao decide loot final.
- Server Validation: trace server-side em `ServerBeginInteract`, `CanInteract` do interactable e validacoes existentes de `SetLootSource`.
- RPCs afetados: `ServerBeginInteract`, `ServerEndInteract`.
- OnRep afetados: `UNarrativeInventoryComponent::OnRep_LootSource`.
- Replication Condition envolvida: `OwnerOnly`.
- Overflow (`FBitReader::SetOverflowed`): nao observado neste ciclo; smoke pendente.
- Cenário com lag/loss: pendente.
- Tested On: 2026-06-12, build `SpellRiseEditor Win64 Development` PASS via `C:\UnrealSource\UnrealEngine`.
- Standalone: pendente
- Listen Server: pendente
- Dedicated Server: pendente
- Owner: Multiplayer/Inventory

### BUG-2026-06-13-042
- Date: 2026-06-13
- Severity: High
- Status: Fixed
- Area: Inventory / UI Input / Narrative Inventory
- Issue: ao fechar janela de loot/container, o input de gameplay nao voltava e `OnKeyDown` com ESC nao era chamado de forma confiavel.
- Reproduction: abrir inventory/loot Narrative e tentar sair pelo ESC ou fechar a janela; cursor/UI permanecem capturando input e movimento/look nao retomam.
- Expected: ESC/clear fecha looting, chama `StopLooting`, fecha a UI e restaura `GameOnly` no client local mesmo se o BP/widget nao receber foco.
- Actual: `UInventoryWidget` apenas logava foco; `OnKeyDown` dependia de foco/visibilidade do BP e `OnClearSelectionPressed` retornava cedo quando input de gameplay estava bloqueado pela UI.
- Root Cause: contrato de UI estava dividido entre BP e C++ sem fallback nativo; `WB_Inventory`/`WB_Storage` sao focusable, mas usam visibilidade `SelfHitTestInvisible`, entao o caminho de `OnKeyDown` em BP/C++ nao e confiavel quando foco cai em widget filho ou input mode fica travado em UI.
- Fix: `UInventoryWidget` agora e focusable, seta foco/input `GameAndUI` ao abrir, trata ESC em `NativeOnKeyDown`, chama `StopLooting` no inventario do owning player e restaura `GameOnly`/cursor/input no fechamento. Alem disso, `ASpellRisePlayerController` trata ESC/clear antes do bloqueio de gameplay: se houver `LootSource`, chama `UNarrativeInventoryComponent::RequestStopLootingFromUI()` e restaura input local.
- Net Scope: morte/loot
- Authority Boundary: client apenas solicita `StopLooting`; servidor limpa `LootSource` autoritativo via RPC existente.
- Prediction Path (Client): fechamento da UI e restore de input sao locais; loot final segue server-authoritative.
- Server Validation: `ServerStopLooting` usa rate-limit/validação existente do `UNarrativeInventoryComponent`.
- RPCs afetados: `ServerStopLooting`.
- OnRep afetados: `UNarrativeInventoryComponent::OnRep_LootSource`.
- Replication Condition envolvida: `OwnerOnly`.
- Overflow (`FBitReader::SetOverflowed`): nao observado neste ciclo.
- Cenário com lag/loss: pendente.
- Tested On: 2026-06-13, build `SpellRiseEditor Win64 Development` PASS via `C:\UnrealSource\UnrealEngine`.
- Standalone: pendente
- Listen Server: pendente
- Dedicated Server: pendente
- Owner: UI/Inventory

### BUG-2026-06-10-038
- Date: 2026-06-10
- Severity: High
- Status: Open
- Area: Dedicated Server / Load Test / Login Bootstrap
- Issue: escala local NoSteam com 32 clientes nao estabiliza; conexoes aceitas entram em timeout durante bootstrap/login antes de formar sessao completa.
- Reproduction: `Scripts/Run-Load-NoSteam-Scale.ps1 -ClientCounts 32 -ServerBootSeconds 65 -ClientJoinGapMs 2000 -RoundDurationSeconds 90 -InitialConnectTimeoutSeconds 90 -ConnectionTimeoutSeconds 90 -WithPerfStats -StopOnFirstUnstable`.
- Expected: 32/32 clientes conectados sem timeout, overflow ou failure.
- Actual: 15/32 clientes conectados; `TimeoutSignals=64`, `BitReaderOverflowSignals=0`; servidor registra `PreLoginAccept` e depois `UNetConnection::Tick: Connection TIMED OUT` em conexoes ainda sem PC/Owner.
- Root Cause: causa provavel identificada em 2026-06-10: pending id de PreLogin usava chave de IP normalizada sem porta (`127.0.0.1`), fazendo clientes locais sobrescreverem o id uns dos outros antes do registro do controller.
- Fix: usar chave de conexao com porta para `PendingPersistentIdByAddress`; manter chave sem porta apenas para ban por IP.
- Tested On: `Saved\Logs\LoadNoSteam\2026-06-10_16-59-51`
- Additional Evidence: `Saved\Logs\LoadNoSteam\2026-06-10_19-17-57` conectou 16/16 com zero overflow e zero timeout apos timeout via URL, mas clientes locais falharam por OOM/pagefile.
- Additional Evidence: `Saved\Logs\LoadNoSteam\2026-06-10_19-43-11` mostrou que binarios soltos `Binaries\Win64` sem cook crasham em `FBufferReaderBase::Serialize`/asset registry; carga por binario precisa de build staged/cooked.
- Standalone: nao aplicavel
- Listen Server: pendente
- Dedicated Server: falha em 32 clientes locais; 16 clientes locais estavel sem overflow
- Owner: Multiplayer/Core

### BUG-2026-06-05-037
- Date: 2026-06-05
- Severity: High
- Status: Fixed
- Area: Animation / Traversal / Networking
- Issue: traversal movements do Character deixaram de replicar para outros clients.
- Reproduction: executar traversal GASP em sessão multiplayer com player, enemy e `ABP_Sandbox`; nenhum caminho replica/apresenta traversal corretamente.
- Expected: input local dispara intenção no pawn possuído; servidor valida/decide movimento final; apresentação de traversal chega aos demais clients pelo fluxo padrão GASP/CharacterMovement.
- Actual: falha não está isolada no player Character; também aparece em enemy/base diferente e no `ABP_Sandbox`.
- Root Cause: em investigação. Causa provável atual é quebra global no caminho `AC_TraversalLogic`/RPC ownership ou no pipeline AnimBP/linked layer compartilhado; logs antigos mostram `No owning connection` para `PerformTraversalAction_Server` em enemy, e logs recentes mostram histórico de falha em `LinkAnimClassLayers`.
- Fix: pendente. Não alterar estruturalmente o GASP até confirmar o ponto exato; validar primeiro se o componente está chamando RPC Server em ator sem owning connection e se o AnimBP ainda tem slot/layer de traversal válido.
- Tested On: 2026-06-05
- Standalone: pendente
- Listen Server: pendente
- Dedicated Server: pendente
- Owner: Animation/Network

### BUG-2026-05-27-036
- Date: 2026-05-27
- Severity: High
- Status: Fixed
- Area: Animation / AnimBlueprint / Editor Preview
- Issue: `SpellRiseCharacter_CMC_ABP` nao compilava pelo fluxo normal do editor/Persona ao conectar o linked graph `OverlayStates`.
- Reproduction: abrir `SpellRiseCharacter_CMC_ABP`, conectar `OverlayStates` no fluxo do AnimGraph e compilar com preview ativo.
- Expected: AnimBP deve compilar e inicializar preview sem assert em `AnimNodeData`.
- Actual: editor assertava em `AnimNodeData.cpp` (`Entries.IsValidIndex(InId.Index)`) durante inicializacao de linked anim graph/layer.
- Root Cause: assinatura do linked graph `OverlayStates` estava inconsistente com o input pose esperado (`InPose`), causando falha de dynamic link do root `OverlayStates`.
- Fix: recriado/corrigido o `Linked Input Pose` do graph `OverlayStates` com nome `InPose` e reconectado no fluxo esperado.
- Tested On: 2026-05-27
- Standalone: nao validado neste ciclo
- Listen Server: nao validado neste ciclo
- Dedicated Server: nao aplicavel ao preview/editor; sem impacto server-authoritative
- Owner: Animation/Presentation

### BUG-2026-04-02-034
- Date: 2026-04-02
- Severity: High
- Status: Fixed
- Area: Full Loot / Death / Networking
- Issue: bag de full loot podia spawnar imediatamente após a morte e permanecer suspensa em desnível/queda.
- Reproduction: matar personagem em terreno irregular, borda ou durante queda e observar bag fora do chão.
- Expected: bag deve spawnar após janela de morte e ajustar no piso validado pelo servidor.
- Actual: bag era criada no mesmo frame da morte e ocasionalmente ficava presa no ar.
- Root Cause: `LootBagSpawnDelaySeconds` estava sendo ignorado no fluxo de morte e o cálculo de posição usava contexto imediato da morte.
- Fix: ativado delay server-side de 3s no `HandleCharacterDeath`, captura de `DeathLocation` na morte e ground trace aplicado no momento real do spawn.
- Tested On: pendente de validação manual pós-patch
- Standalone: pendente
- Listen Server: pendente
- Dedicated Server: pendente
- Owner: Multiplayer/Inventory

### BUG-2026-03-30-033
- Date: 2026-03-30
- Severity: Critical
- Status: Fixed
- Area: Networking / Replication / PlayerController
- Issue: `FBitReader::SetOverflowed` em tráfego replicado do `BP_SpellRisePlayerController_C`.
- Reproduction: conectar cliente em DS e observar overflow em payloads como `ClientSetHUD`, `ClientRestart` e `ClientSetCameraMode`.
- Tested On: baseline de gate executado em 2026-04-02; reconfirmado em 2026-05-27
- Standalone: PASS (gate baseline)
- Listen Server: PASS (gate baseline)
- Dedicated Server: PASS em `DS+2` com reconnect + lag/loss Perfil A (120/1), sem overflow no recorte local
- Evidência:
  - `C:\Users\biM\Documents\Unreal Projects\SpellRise\Saved\Logs\SmokeAuto\2026-04-02_21-03-10\Smoke_Summary.txt`
  - `C:\Users\biM\Documents\Unreal Projects\SpellRise\Saved\Logs\SmokeAuto\2026-05-27_23-57-01\Smoke_Summary.txt`
  - `ReplicationOverflowTotalCount=0`
- Observação: corrigido no recorte local DS+2; validacao AWS/staging ainda pendente antes de `Verified`.
- Owner: Multiplayer/Core

### BUG-2026-03-28-032
- Severity: Critical
- Status: In Progress
- Area: Online / Dedicated Server / Steam
- Issue: DS sobe com `OnlineSubsystem=NULL` e rejeita cliente Steam por `incompatible_unique_net_id`.
- Reproduction: subir DS em ambiente Steam e observar fallback para `NULL` + `PreLoginFailure`.
- Root Cause: bootstrap do `GameInstance` permitia continuar silenciosamente com default subsystem degradado para `NULL` antes do gate de `PreLogin`; alem disso, o projeto nao habilitava `SteamAuthComponentModuleInterface`, entao o fluxo validava tipo de `UniqueNetId` mas nao autenticava ticket Steam no handshake.
- Fix: adicionada observabilidade `[Auth][Bootstrap]` e fail-fast no Dedicated Server quando Steam e obrigatorio e o default subsystem nao e `STEAM`; `SteamAuthComponentModuleInterface` habilitado em `PacketHandlerComponents` e bootstrap agora falha cedo quando SteamAuth esta desabilitado; `-nosteam` continua permitido apenas no modo de teste configurado.
- Tested On: 2026-06-09
- Standalone: nao aplicavel
- Listen Server: nao aplicavel
- Dedicated Server: build pendente; falta smoke Steam real com cliente empacotado autenticado.
- Observação: nao marcar `Fixed` ate build/link passar e smoke Steam/NoSteam confirmar o comportamento.
- Owner: Backend/Online

### BUG-2026-06-10-039
- Severity: High
- Status: In Progress
- Area: Client / Gameplay Cameras / Load Gate
- Issue: cliente cooked em carga headless dispara ensure de Gameplay Cameras ao possuir personagem.
- Reproduction: rodar `Scripts\Run-Load-NoSteam-Scale.ps1 -UseProjectBinaries` com build staged/cooked e `ClientCounts=2`; ver `Saved\Logs\LoadNoSteam\2026-06-10_20-07-20\Round_2Clients\Client_1.stdout.log`.
- Actual: `Ensure condition failed: (FocalLength <= 0 || FieldOfView <= 0) && ...` e mensagem `Can't specify both FocalLength and FieldOfView on a camera pose!`.
- Expected: cliente headless `-nullrhi` nao deve gerar ensure nem hitch de camera durante gate multiplayer.
- Root Cause: asset/config de `GameplayCamera`/`CameraRigAsset_2147482084` ainda pode emitir pose invalida com `FocalLength` e `FieldOfView` simultaneamente em carga headless; o problema nao apareceu em `DS+2`, mas reapareceu em escala 12.
- Fix: assets de camera revisados no Editor; rigs ativos mantidos em `FieldOfView` sem `Lens Parameters/FocalLength`; recook/stage executado. Mitigacao parcial: carga sintetica headless passa a desligar fluxo local de HUD/Gameplay Cameras por flag e desativar componentes de Gameplay Camera no Character, mas o asset ainda e avaliado em alguns clientes.
- Tested On: 2026-06-10
- Dedicated Server: PASS em `Saved\Logs\LoadNoSteam\2026-06-10_20-59-26`; `JoinedClients=2/2`, `EnsureSignals=0`, sem `FocalLength/FieldOfView`, sem `FBitReader::SetOverflowed`.
- Additional Evidence: `Saved\Logs\LoadNoSteam\2026-06-10_21-02-52` conectou 12/12 com zero timeout, zero prelogin failure, zero network failure e zero `FBitReader::SetOverflowed`, mas marcou `OverallUnstable=True` por 8 ensures de camera em clientes.
- Additional Evidence: `Saved\Logs\LoadNoSteam\2026-06-10_21-38-14` apos build/cook com mitigacao C++ conectou 12/12 com zero timeout, zero prelogin failure, zero network failure e zero `FBitReader::SetOverflowed`, mas ainda marcou `OverallUnstable=True` por 4 ensures de `CameraRigAsset_2147482084`.
- Additional Evidence: `Saved\Logs\LoadNoSteam\2026-06-11_08-49-18` apos ajuste do `CameraAsset_SandboxCharacter`/`CameraDirector_SandboxCharacter` conectou 12/12, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Additional Evidence: `Saved\Logs\LoadNoSteam\2026-06-11_09-42-59` conectou 16/16, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`; houve 16 warnings `Both FocalLength and FieldOfView`, sem ensure.
- Impact: gate `DS+16` cooked ficou limpo para rede/performance; warning visual de pose mista permanece como follow-up nao bloqueante do gate headless.
- Owner: Client/Camera

### BUG-2026-06-10-040
- Severity: Medium
- Status: Fixed
- Area: Blueprint / Weather Effects / Load Gate
- Issue: `DLWE_Interaction_C` gera spam de `Accessed None` lendo `Interaction Sound` em cliente cooked.
- Reproduction: mesmo run `Saved\Logs\LoadNoSteam\2026-06-10_20-07-20`, apos possess de `BP_SpellRiseCharacter`.
- Actual: warnings repetidos em `/Game/UltraDynamicSky/Blueprints/Weather_Effects/DLWE_Interaction.DLWE_Interaction_C:Make Sound Component`.
- Expected: Blueprint deve validar nulo ou desativar esse fluxo em headless/load test sem gerar spam de log.
- Fix: `Interaction Sound` validado no Blueprint pelo Editor.
- Tested On: 2026-06-10
- Dedicated Server: PASS em `Saved\Logs\LoadNoSteam\2026-06-10_20-59-26`; busca por `Interaction Sound`, `DLWE_Interaction` e `Accessed None` nao retornou ocorrencias.
- Impact: log do gate `DS+2` cooked ficou limpo para esses sinais; escala maior ainda pendente.
- Owner: Client/Blueprint

### BUG-2026-03-28-031
- Severity: High
- Status: In Progress
- Area: Online / Steam Auth Gating
- Issue: flows sensíveis ainda precisam separar bootstrap offline seguro de auth Steam obrigatória.
- Reproduction: iniciar DS com identidade degradada e validar se paths sensíveis ainda aceitam fallback indevido.
- Fix: persistência de personagem/inventário endurecida para aceitar apenas SteamID64 real; `DEV_`/`NULL` não gravam snapshot em DB. `PreLogin` passa a consultar ban por IP em tabela separada `spellrise_portal_ip_bans`; ban por MAC não é implementado porque o DS não recebe MAC confiável do cliente em ambiente Steam/Internet.
- Tested On: pendente de build/link e validação Steam real.
- Owner: Backend/Online

### BUG-2026-03-28-029
- Severity: High
- Status: Open
- Area: Death / Respawn / Presentation
- Issue: client pode permanecer em tela preta durante janela de respawn longa sem reconciliação visual suficiente.
- Reproduction: morrer em sessão multiplayer e observar delay de respawn.
- Owner: Multiplayer/Respawn

### BUG-2026-03-28-028
- Severity: Medium
- Status: Open
- Area: Input / Interact / Dedicated Server Client
- Issue: `Interact` ainda não fecha de forma confiável em alguns caminhos de DS client.
- Reproduction: usar interação narrativa em DS client e verificar fallback/rejeição inconsistente.
- Owner: Gameplay/Input

### BUG-2026-03-28-030
- Severity: Medium
- Status: Fixed
- Area: Combat / Chat / Fall Damage
- Issue: chat de fall damage ainda pode resolver causer/instigator de forma ambígua.
- Reproduction: sofrer fall damage em multiplayer e comparar mensagem de chat com evento real.
- Root Cause: fall damage usa o ASC do próprio alvo para aplicar GE server-side, então o `PostGameplayEffectExecute` podia interpretar o alvo como source/causer e gravar combat log/death event como self-caused.
- Fix: `Data.DamageType.Fall` agora normaliza fonte self/unknown como ambiente; não replica entrada outgoing duplicada no combat log do próprio jogador; death participant de queda não herda `PlayerId` da vítima e a mensagem persistida usa causa explícita de fall damage.
- Tested On: 2026-06-05
- Standalone: não validado neste ciclo
- Listen Server: não validado neste ciclo
- Dedicated Server: build `SpellRiseEditor Win64 Development` PASS; smoke DS+2 pendente
- Owner: Combat/Feedback

### BUG-2026-04-06-035
- Date: 2026-04-06
- Severity: High
- Status: Fixed
- Area: Inventory / Drop / Dedicated Server
- Issue: ao dropar item por UI no cliente conectado em DS, o item e removido do inventario, mas pickup nao aparece no mundo.
- Reproduction: abrir `WBP_ItemInspector`, executar `Drop Item` em sessao DS+Client.
- Expected: remocao autoritativa + spawn do `BP_BasicItemPickup` replicado com `ItemClass` e `QuantityToGive` validos.
- Actual: item removido, mas sem pickup visivel/funcional em cliente; logs apontam `Item Class` nulo no pickup.
- Root Cause: inicializacao do `BP_BasicItemPickup` ainda recebe classe nula em parte do fluxo (construction/event graph), apesar do request autoritativo estar ativo.
- Fix: server request em C++ + fallback de classe + inicializacao refletida do pickup; validado manualmente com drop por um client e pickup por outro client.
- Tested On: 2026-05-27
- Standalone: Parcial
- Listen Server: Nao validado neste ciclo
- Dedicated Server: PASS manual em DS com dois clients; item dropado por um client apareceu no mundo e foi coletado por outro personagem em modo client
- Owner: Multiplayer/Inventory
