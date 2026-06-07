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
### BUG-2026-06-05-037
- Date: 2026-06-05
- Severity: High
- Status: In Progress
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
- Root Cause: bootstrap do `GameInstance` permitia continuar silenciosamente com default subsystem degradado para `NULL` antes do gate de `PreLogin`.
- Fix: adicionada observabilidade `[Auth][Bootstrap]` e fail-fast no Dedicated Server quando Steam e obrigatorio e o default subsystem nao e `STEAM`; `-nosteam` continua permitido apenas no modo de teste configurado.
- Tested On: 2026-05-27
- Standalone: nao aplicavel
- Listen Server: nao aplicavel
- Dedicated Server: build pendente; link bloqueado por `UnrealEditor-SpellRise.dll` em uso pelo editor aberto
- Observação: nao marcar `Fixed` ate build/link passar e smoke Steam/NoSteam confirmar o comportamento.
- Owner: Backend/Online

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
