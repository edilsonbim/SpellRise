# Roadmap (Execution-Oriented)

## Phase 0 - Documentation Alignment (Done)
- [x] Sync architecture, combat, network, and project-state docs to runtime code.
- [x] Register current technical debt and known mismatches.
- [x] Refresh canonical docs to runtime snapshot 2026-03-25 (build health + plugin metadata warning fix + UE 5.7 deprecation cleanup).

## Phase 1 - Combat Runtime Hardening (Done)
- [x] Resolve catalyst SetByCaller tag mismatch across code/assets/tags.
- [x] Remove debug-only FX paths from runtime damage number flow.
- [x] Unify attribute pipeline (migrate off legacy MMC usage and remove legacy aliases from new assets).
  - [x] Align legacy MaxHealth/MaxMana/MaxStamina MMC formulas to canonical STR/AGI/INT/WIS runtime formula (C++ layer).
  - [x] Migrate BP/GE assets to canonical classes for resource recalculation (`GE_RecalculateResources` now uses `MMC_CarryWeightFromPrimaries`).
  - [x] Remove legacy primary aliases (`VIG/FOC`) from runtime C++ accessors (`CombatAttributeSet`).
- [x] Security hardening for authoritative gameplay entrypoints (Sprint Vertical Curto - concluído no recorte C++/Config)
  - [x] Allowlist + payload validation for `ServerSendGameplayEventToSelf`.
  - [x] Rate limit per tag/window for high-frequency gameplay event RPC.
  - [x] Server-side validation for replicated projectile target data (range/cone/context).
  - [x] Dedicated security telemetry category for rejected gameplay event/target payload.
  - [x] Harden respawn-bed server RPC payload/context validation with authoritative bed resolution.
  - [x] Expand same policy to all remaining high-frequency server gameplay RPC entrypoints (C++ recorte auditado em 2026-03-20; sem novos Server RPCs fora da matriz operacional).
- [x] Resolve inventory/storage RPC ownership issue (`BUG-2026-03-19-002`) in authoritative owner-only flow (verified on DS+2 and DS+2 with lag/loss on 2026-03-20).

### Phase 1 Exit Criteria
- [x] No open High-severity authority/RPC exploit in combat gameplay path.
  - Status: varredura final do recorte C++/Config + smoke multiplayer atual (2026-03-20) sem achado aberto de severidade alta.
- [x] RPC validation matrix updated for all altered/new server RPCs (origin, payload bounds, context checks, anti-spam, failure behavior).
  - Status: cobertura conferida para todos os `UFUNCTION(Server, ...)` atuais no código (5 RPCs) na matriz operacional.
- [x] Network budget declared per combat/network feature:
  - [x] Recalculo de recursos/atributos canônicos documentado no baseline (`NETWORK_BUDGET_BASELINE_V1_2026-03-19.md`).
  - [x] Cobertura por feature de Phase 1 consolidada (RPC críticos, target data de projétil, recálculo canônico).
  - NetUpdateFrequency for relevant actors.
  - Max RPC/s per player.
  - Max RPC payload size (bytes).
  - Max target-data send rate.
- [x] Multiplayer smoke validated in Standalone, Listen Server, and Dedicated Server (2+ clients).

## Phase 2 - Hardcore Loop (Done)
- [x] Server-authoritative death -> loot drop -> respawn pipeline.
- [x] Inventory transfer/drop rules for full-loot scenario.
  - [x] Anti-duplicação em falha de remoção da origem: rollback server-side na bag quando `RemoveSlotsFromInventory` falha após add (2026-03-20).
  - [x] Política explícita de elegibilidade de inventários no death-loot (allowlist/denylist por componente e owner Character/PlayerState) com logs de auditoria (2026-03-20).
- [x] Edge-case handling (disconnect/death overlap/combat lock).
  - [x] Mitigação aplicada para `disconnect/death overlap`: full loot processado imediatamente no servidor (sem janela de delay vulnerável).
  - [x] Guard de `combat lock` no servidor integrado ao agendamento de respawn (respawn respeita lock ativo após dano recente).
  - [x] Hook determinístico non-shipping para QA (`SR_ForceDeath` + `-SRForceDeathOnBeginPlay`) validado em DS+2 normal e DS+2 lag/loss.
- [x] Reconnect baseline after death/loot events.
  - [x] Smoke DS+2 com reconnect automático do Client1 implementado e validado (normal + lag/loss, 2026-03-20).

### Phase 2 Exit Criteria
- [x] End-to-end death->loot->respawn validated on Standalone, Listen Server, and Dedicated Server (2+ clients).
- [x] No client-authority path for death, loot ownership, or respawn decision.
- [x] Dedicated server flow runs without UI/HUD dependencies.
- [x] Latency/loss simulation validated with acceptable reconciliation behavior.

## Phase 3 - Persistence (In Progress)
- [x] 3.1 Character snapshot/load lifecycle (base server-authoritative).
- [x] 3.2 Inventory snapshot/load + rollback-safe writes (revision/check otimista v1).
- [ ] 3.3 Economy persistence and reconciliation.
- [ ] 3.4 Steam session/auth integration for character load path.
- [ ] 3.5 Database integration and operational safeguards.

### Phase 3 Exit Criteria
- [ ] Deterministic recovery after reconnect/restart for character + inventory core state.
- [ ] Write-path rollback/consistency checks in place for inventory/economy mutations.
- [ ] Session/auth context consistently gates persistence reads/writes.

## Phase 4 - Observability and Quality (Planned, promoted MVP)
### Security Telemetry Track
- [x] Security log stream for rejected gameplay event and invalid projectile target data.
- [x] Replicated suspicious sequence timeline store (FastArray owner-centric no PlayerState para combat log durável v1).
- [ ] Audit trail query workflow for incident triage.

### UX + Debug Telemetry Track
- [x] Replicated combat log for player UX and debugging (delta + snapshot fallback em reconnect/profile apply).
- [x] Durable history/backlog para eventos de combate (buffer circular com truncamento).
- [ ] Balance telemetry hooks for TTK and build diversity tracking.

### Quality Gates
- [ ] Automated tests for damage, projectile authority, and fall damage.
- [ ] Automated smoke for cast/commit/cooldown/cost, projectile hit/overlap on server, death/loot/respawn, and reconnect baseline.
  - [x] Cobertura DS+2 para morte/loot/respawn + reconnect baseline adicionada no gate local contínuo (`Scripts/Run-Smoke-Gate.ps1`), incluindo cenário lag/loss.

## Phase 5 - Expansion (Future)
- [ ] Guild systems.
- [ ] Territory control.
- [ ] Long-term economy warfare loops.
- [ ] Bow and arrow combat package.
- [ ] Block system.
- [ ] Main world map production (World Partition + PCG + HLOD).
  - [ ] 5.1 Baseline de mundo principal com streaming e navegação (sem dependência de UI no DS).
  - [ ] 5.2 Pipeline PCG inicial (biomas/folhagem/props) com geração determinística por seed.
  - [ ] 5.3 Pipeline HLOD por camada de distância com budget de draw calls/memória.
  - [ ] 5.4 Validação multiplayer (Standalone, Listen, DS+2, DS+2 lag/loss) com carregamento/streaming estável.

## Phase 5.5 - Building Mode Module (In Progress)
- [x] Separar modo de construção da gameplay em módulo dedicado no PlayerController (componente próprio).
- [x] Gate de input: inputs de gameplay bloqueados em modo construção.
- [x] Gate de input: inputs de construção só executam quando modo construção estiver ativo.
- [ ] Integrar ações de construção (confirm/cancel/rotate) ao fluxo final de placement/preview.
- [ ] Definir budget de rede de construção antes de merge final:
  - max RPC/s por jogador para toggle/place/rotate/cancel.
  - payload máximo por ação (bytes).
  - validação server-side de contexto/alcance/LOS e anti-spam.

### Phase 5.5 Exit Criteria
- [ ] Sem ativação acidental de abilities/combate durante modo construção.
- [ ] Toggle de modo construção com reconciliação estável cliente-servidor (sem estado preso).
- [ ] Dedicated server funcional sem dependência de HUD/widget para fluxo de construção.
- [ ] Matriz de validação RPC registrada para entrypoints de construção.

## Active Technical Debt Governance (Mandatory)
- Every debt item changed/created in this roadmap must include:
  - Severity (S0, S1, S2, S3)
  - Owner
  - Target date
  - Exit criteria
- Debt without owner and exit criteria is not considered trackable.

## Requested Additions (User Notes)
- [ ] Save system (mapped to Phase 3 milestones).
- [ ] Full loot drop bag (mapped to Phase 2).
- [ ] Steam session for auth and character load (Phase 3.4).
- [ ] Database integration (Phase 3.5).
- [ ] Main world map with PCG + HLOD (Phase 5).
- [ ] Building module isolated from gameplay with mode-gated input (Phase 5.5).

## Current Execution Focus (Sequencial)
1. Avançar de foundation v1 para persistência de produção:
- 3.3 Economia persistence + reconciliation.
- 3.4 Session/auth gating completo no caminho de load/save.
- 3.5 Integração DB operacional (safeguards, retries e observabilidade).
2. Consolidar módulo de construção:
- Definir budget de rede de construção.
- Finalizar matriz RPC de construção quando entrypoints server forem introduzidos.

## Gates Congelados dos Críticos (Objetivo)
1. CombatLog FastArray
- [x] DS+2 (normal e lag/loss): backlog consistente após hits/mortes (2026-03-20).
- [x] Reconnect: snapshot de histórico entregue sem flood (rate limit aplicado) (2026-03-20).
- [x] Orçamento de rede declarado e respeitado (tamanho entrada/backlog/frequência) no recorte atual.
2. Persistence Foundation (server DB-first path)
- [x] Save/load personagem + inventário com revision/check otimista.
- [x] Reconnect com restauração determinística sem duplicação/perda no smoke gate atual.
- [x] Falha parcial de escrita não confirma estado híbrido em cache do servidor (v1).

## Referencias de fechamento
1. `Fontes/RELATORIO_FECHAMENTO_PHASE2_2026-03-20.md`
2. `Fontes/AUDITORIA_PHASE2_AUTHORITY_UI_2026-03-20.md`
3. `Fontes/REFERENCIAS_JOGOS.md`
4. Commit de fechamento dos críticos: `a135b64b` (2026-03-20).


