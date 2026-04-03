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
- Status: Open
- Area: Networking / Replication / PlayerController
- Issue: `FBitReader::SetOverflowed` em tráfego replicado do `BP_SpellRisePlayerController_C`.
- Reproduction: conectar cliente em DS e observar overflow em payloads como `ClientSetHUD`, `ClientRestart` e `ClientSetCameraMode`.
- Tested On: baseline de gate executado em 2026-04-02
- Standalone: PASS (gate baseline)
- Listen Server: PASS (gate baseline)
- Dedicated Server: PASS em `DS+2` com reconnect + lag/loss Perfil A (120/1) e Perfil B (180/3), sem overflow no recorte
- Evidência:
  - `C:\Users\biM\Documents\Unreal Projects\SpellRise\Saved\Logs\SmokeAuto\2026-04-02_21-03-10\Smoke_Summary.txt`
  - `ReplicationOverflowTotalCount=0`
- Observação: manter `Open` até repetibilidade em múltiplos runs e validação em ambiente AWS/staging.
- Owner: Multiplayer/Core

### BUG-2026-03-28-032
- Severity: Critical
- Status: Open
- Area: Online / Dedicated Server / Steam
- Issue: DS sobe com `OnlineSubsystem=NULL` e rejeita cliente Steam por `incompatible_unique_net_id`.
- Reproduction: subir DS em ambiente Steam e observar fallback para `NULL` + `PreLoginFailure`.
- Owner: Backend/Online

### BUG-2026-03-28-031
- Severity: High
- Status: Open
- Area: Online / Steam Auth Gating
- Issue: flows sensíveis ainda precisam separar bootstrap offline seguro de auth Steam obrigatória.
- Reproduction: iniciar DS com identidade degradada e validar se paths sensíveis ainda aceitam fallback indevido.
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
- Status: Open
- Area: Combat / Chat / Fall Damage
- Issue: chat de fall damage ainda pode resolver causer/instigator de forma ambígua.
- Reproduction: sofrer fall damage em multiplayer e comparar mensagem de chat com evento real.
- Owner: Combat/Feedback
