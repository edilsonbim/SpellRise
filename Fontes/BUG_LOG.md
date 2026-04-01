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

## Open Issues
### BUG-2026-03-30-033
- Severity: Critical
- Status: Open
- Area: Networking / Replication / PlayerController
- Issue: `FBitReader::SetOverflowed` em tráfego replicado do `BP_SpellRisePlayerController_C`.
- Reproduction: conectar cliente em DS e observar overflow em payloads como `ClientSetHUD`, `ClientRestart` e `ClientSetCameraMode`.
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
