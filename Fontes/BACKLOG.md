# Backlog (Prioritized)

## Foco Atual de Execucao
1. Fechar persistência de produção e observabilidade avançada:
- Economia + reconciliação persistente.
- Integração DB operacional + auth/session end-to-end.
- Audit trail consultável para incident triage.
- Fechar BUG_LOG High em In Progress (bloqueador formal do gate).

## Critical
- [x] Combat log durable history (FastArray owner-centric no PlayerState)
  - Status: implementado e validado em gate DS+2 normal + lag/loss (2026-03-20, commit `a135b64b`).
- [x] Persistence foundation (server DB-first v1)
  - Character data, inventory data, and recovery path after reconnect.
  - Status: revision/check otimista, telemetria de save e fallback de rollback de cache aplicados (2026-03-20, commit `a135b64b`).
- [x] Attribute pipeline unification
  - Legacy MMC wrappers formula-aligned in C++ (2026-03-20) and runtime moved to canonical classes.
  - `GE_RecalculateResources` migrated to canonical classes, including `MMC_CarryWeightFromPrimaries` (2026-03-20).
  - Legacy aliases `VIG/FOC` removed from runtime C++ accessors (2026-03-20); compat classes remain apenas para backward compatibility passiva.
- [x] Define network budget baseline per combat/network feature
  - NetUpdateFrequency, max RPC/s per player, max RPC payload bytes, max target-data send rate.
- [x] Fix Catalyst SetByCaller tag mismatch
  - Standardized code path on Data.CatalystChargeDelta with legacy fallback for compatibility.

## High
- [ ] Corrigir overflow de replicação no PlayerController (`BUG-2026-03-30-033`)
  - Sintoma: `FBitReader::SetOverflowed` + `ReadFieldHeaderAndPayload` em `BP_SpellRisePlayerController_C` durante init/runtime multiplayer.
  - Escopo: hardening completo de fluxo local (HUD/câmera), revisão de payloads RPC client/server do controller, validação de paridade build/cook client/server.
  - Owner: Multiplayer/Core
  - Target date: 2026-04-03
  - Exit criteria: zero ocorrência de overflow em Standalone, Listen e DS+2 (normal + lag/loss), com sessão estável > 5 min por rodada.
- [ ] Security hardening for Server RPC entrypoints
  - [x] Validate and rate-limit gameplay event forwarding (`ServerSendGameplayEventToSelf`).
  - [x] Validate replicated projectile target data on server (range/cone/context).
  - [x] Harden respawn-bed server RPC payload/context validation with authoritative bed resolution.
  - [x] Expand same guardrails to remaining server gameplay RPC entrypoints (audit C++ concluído em 2026-03-20).
- [x] Fix inventory/storage RPC ownership path (`BUG-2026-03-19-002`)
  - Ensure owner-only RPC is called only from actors/controllers with valid owning connection.
- [ ] Balance pass for resist/penetration/crit under target TTK.
- [ ] Resolver BUG_LOG High em In Progress (bloqueador de release)
  - Status: gate local agora bloqueia quando existe entrada `Severity: High` + `Status: In Progress`.
  - Owner: Combat/GAS
  - Target date: 2026-03-27
  - Exit criteria: todos os bugs High em In Progress migrados para Verified/Closed com evidência em Standalone/Listen/DS+2/lag-loss.
- [x] Remove debug-only number pop spawn path in runtime.
- [x] Resolve duplicated map/default mode settings in config files.
- [x] Replace reflective chat append bridge with native chat API contract.

## Medium
- [ ] Stats and derived formula UI panel.
- [ ] Tooltip system with live values.
- [ ] Automated gameplay validation tests (damage, fall damage, projectile authority).
- [x] Add multiplayer smoke coverage for death/loot/respawn and reconnect baseline.
  - [x] DS+2 normal + DS+2 lag/loss para morte/loot/respawn com gatilho determinístico (`SR_ForceDeath` / `-SRForceDeathOnBeginPlay`).
  - [x] Reconnect baseline automatizado no mesmo fluxo (Client1 reconnect) validado em DS+2 normal e DS+2 lag/loss (2026-03-20).
  - [x] Gate local contínuo criado em `Scripts/Run-Smoke-Gate.ps1` para execução sequencial e fail-fast.

## Low
- [ ] Technical glossary for gameplay tags and naming conventions.
- [ ] Documentation automation for changelog and project state snapshots.

## Active Debt Register (Trackable)
- [ ] Debt: Balance pass resist/penetration/crit
  - Severity: S2
  - Owner: Combat Design
  - Target date: 2026-04-12
  - Exit criteria: TTK scenarios (light/medium/heavy) within `COMBAT_DESIGN_PHILOSOPHY.md` targets in Standalone, Listen and DS+2.
- [ ] Debt: Economy persistence + reconciliation (Phase 3.3)
  - Severity: S1
  - Owner: Backend/Persistence
  - Target date: 2026-04-30
  - Exit criteria: deterministic recovery after reconnect without item duplication/loss in DS+2 reconnect smoke.
- [ ] Debt: Database integration and operational safeguards (Phase 3.5)
  - Severity: S1
  - Owner: Backend/Persistence
  - Target date: 2026-05-15
  - Exit criteria: DB-backed save/load enabled in runtime with retry policy, telemetry and rollback-safe writes validated in smoke.
- [ ] Debt: Automated multiplayer smoke in CI
  - Severity: S2
  - Owner: QA/Automation
  - Target date: 2026-04-20
  - Exit criteria: CI gate executes DS+2 normal + lag/loss + reconnect scenarios and blocks merge on failure.
- [ ] Debt: GAS base review for activation/commit/prediction boundaries
  - Severity: S2
  - Owner: Combat/Gameplay
  - Target date: 2026-04-10
  - Exit criteria: Base ability flow explicitly documents activation, commit, server validation, local prediction, replication, and reconciliação; no ambiguous shared path remains between cosmetic-only and authoritative gameplay work.
- [ ] Debt: Steam auth gating for session-sensitive DS flows
  - Severity: S1
  - Owner: Backend/Online
  - Target date: 2026-04-15
  - Exit criteria: Dedicated-server session and persistence entrypoints reject placeholder/offline identity when Steam auth is required; DS smoke validates both authenticated and degraded identity paths with safe rejection logs.
- [ ] Debt: Steam DS bootstrap AppId/OSS mismatch
  - Severity: S0
  - Owner: Backend/Online
  - Target date: 2026-04-05
  - Exit criteria: Dedicated Server em AWS inicializa `OnlineSubsystemSteam` sem fallback para `NULL`, loga GameServer API inicializada com AppID válido (nunca `AppId: 0`), e permite pré-login Steam (`STEAM:*`) sem `incompatible_unique_net_id` nos testes DS+2.
- [ ] Debt: DB/file write-path fallback contract for persistence
  - Severity: S1
  - Owner: Backend/Persistence
  - Target date: 2026-05-01
  - Exit criteria: Primary save/write path is documented and enforced with explicit DB-first behavior and file fallback only in declared degraded mode; rollback/retry behavior is validated in DS+2 reconnect smoke.

## Debt Governance (Mandatory)
- Every new/changed debt item must include: Severity (S0-S3), Owner, Target date, Exit criteria.
- Debt without owner and exit criteria is not considered trackable.

## Tracked Technical Debt (Current)
- [ ] PostgreSQL adapter atual via `psql` CLI (sem driver nativo UE)
  - Severity: S2
  - Owner: Backend/Persistence
  - Target date: 2026-04-10
  - Exit criteria: provider PostgreSQL nativo (sem dependência de processo externo) com mesma suíte de smoke/gate aprovada.
  - Update: hardening aplicado em 2026-03-20 (`6d7674ff`) com execução SQL por arquivo temporário e melhor telemetria de falha; dívida permanece aberta até migração para driver nativo UE.
- [x] Higiene de segurança para setup local de DB/GitHub
  - Status: concluído em 2026-03-20 (`da191ecf`): scripts sem credenciais hardcoded, logs de conexão com redaction, policy de versionamento de DB documentada.


