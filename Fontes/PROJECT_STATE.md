# Spell Rise - Project State (2026-03-30)

## Engine / Build
- Engine association: UE 5.7
- Build/Test Policy (fixed): all builds and all tests must run only with Unreal Source at `C:\UnrealSource\UnrealEngine`.
- Targets present: Game, Client, Server, Editor
- GAS-based combat architecture
- Latest validation (2026-03-30): clean package Client/Server via Unreal Source succeeded to `Desktop\SpellRise_TestBuild`.
- Latest networking status (2026-03-30): multiplayer instability still reproduces with replication/serialization errors (`FBitReader::SetOverflowed`) in `BP_SpellRisePlayerController_C` payload processing path.

## Narrative Plugins Reference (Official)
- Official docs for Narrative plugins (source of truth for plugin-level changes): https://docs.narrativetools.io/

## External Technical References (Official)
- GAS (Gameplay Ability System, Unreal Engine): https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine
- Lyra Sample Game (Unreal Engine): https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine

## Confirmed Runtime Architecture
- ASC and AttributeSets are owned by PlayerState.
- Character initializes/consumes ASC from PlayerState actor info.
- Attribute sets in use:
  - BasicAttributeSet
  - CombatAttributeSet
  - ResourceAttributeSet
  - CatalystAttributeSet
  - DerivedStatsAttributeSet

## Implemented Core Systems
- Damage execution calculation with channels, resistances, crit, penetration, and fall damage handling.
- Derived stat MMC pipeline for STR/AGI/INT/WIS.
- Projectile base gameplay ability with replicated target data flow.
- Replicated projectile actor base applying GE damage on authority.
- Catalyst charge/proc component integrated with damage pipeline.
- Fall damage server-only component integrated with movement events.
- Damage number pipeline (multicast + gameplay cue + local controller rendering).
- Combat log event feed to chat channel `Combat` for damage dealt/received (source + target player only).
- Server-side guard for chat RPC `SendChatToSERVER` blocking manual posts to `Combat`.
- Native chat transport in C++ (`USpellRiseChatComponent` on `ASpellRiseGameState`) with targeted per-player combat feed.
- Combat log enriched with damage type labels and death notifications.
- Server RPC gameplay-event hardening on `ServerSendGameplayEventToSelf`:
  - explicit event-tag allowlist,
  - payload validation (magnitude/context),
  - per-tag window rate limit,
  - security telemetry for rejected payloads.
- Server-side validation for replicated projectile target data (range + cone + trace start context) before commit/spawn path.
- Server-side validation for respawn-bed RPC payload and authoritative bed resolution from world context.
- Full loot death handling now processes bag spawn immediately on server to avoid disconnect/death overlap window.
- Server combat-lock guard added to respawn scheduling (delay uses max of base respawn delay and active combat-lock remaining time).
- Hook determinístico de QA para morte foi adicionado no `PlayerController` (`SR_ForceDeath` -> `ServerSR_ForceDeath`, non-shipping) para validar pipeline death/full-loot/respawn em DS+2 automatizado.
- Script de smoke DS+2 agora suporta reconnect automático de cliente (`-ReconnectClient1`) e foi validado em cenário normal e com lag/loss (2026-03-20).
- Gate local contínuo de smoke multiplayer adicionado em `Scripts/Run-Smoke-Gate.ps1` (execução sequencial/fail-fast dos cenários DS+2 reconnect normal e lag/loss).
- Parser do smoke DS endurecido contra falso negativo intermitente de join no log do servidor: conectividade agora usa contador efetivo (join server ou inferência por welcomes de cliente), preservando critérios de reconnect (2026-03-20).
- FullLoot recebeu política explícita de elegibilidade de inventários no servidor (allowlist/denylist por owner/component) e rollback anti-duplicação quando remoção da origem falha após add na bag.
- FullLoot agora aplica filtro server-side de slots não dropáveis (flags/tags/textos como `NoDrop/Bound/Quest/Protected`) antes da transferência para bag.
- Validação E2E de death/full-loot/respawn confirmada em Standalone e Listen (2026-03-20), além dos cenários Dedicated (normal + lag/loss) já validados.
- Combat log durável v1 implementado em FastArray owner-centric no `PlayerState`, com buffer circular, truncamento monitorado e snapshot fallback rate-limited (2026-03-20, `a135b64b`).
- Persistência foundation v1 atualizada para fluxo server DB-first com snapshots separados de personagem/inventário, revision/check otimista e telemetria operacional de save/load (2026-03-20, `a135b64b`).
- Persistência operacional atual utiliza provider de arquivo JSON (`Saved/SpellRise/Persistence`) para personagens/inventário/mundo; banco relacional ainda não está ativo no runtime.
- Modo de construção isolado em componente dedicado no PlayerController (`USpellRiseConstructionModeComponent`) com gate de input no fluxo de gameplay.
- `NarrativeInventory` com dependência explícita de `CommonUI` no `.uplugin` (warning de build removido em 2026-03-25).
- Atualização de compatibilidade UE 5.7 em `FString::LeftInline` no `GameMode` (remoção de warning C4996 em 2026-03-25).
- Hardening parcial do `ASpellRisePlayerController` em andamento (2026-03-30):
  - gate local adicional `CanRunLocalPawnRuntime` aplicado no fluxo BeginPlay/Tick/PawnChanged/HUD bind;
  - bloqueios extras para caminho local de HUD/câmera sem `Pawn + PlayerState + ASC`;
  - auditoria de RPC documentada em `Docs/RPC_AUDIT_SpellRisePlayerController.md`.
- Scripts operacionais adicionados para testes:
  - `Scripts/Set-SteamAuthProfile.ps1` e `.bat` (troca rápida `prod/test`);
  - `Scripts/Run-Load-NoSteam.ps1` e `Scripts/Run-Load-NoSteam-Scale.ps1` (carga local sem Steam com relatório).

## Current Combat Formula Snapshot
- Melee multiplier: 1.00 + 0.50 * T(STR)
- Bow multiplier: 1.00 + 0.50 * T(AGI)
- Spell multiplier: 1.00 + 0.50 * T(INT)
- Crit chance: 0.05 + 0.20 * T(AGI) (clamped to 25%)
- Crit damage: 1.50 + 0.50 * T(WIS) (clamped to 2.0x)
- Armor penetration: 0.00 + 0.30 * T(STR) (clamped to 30%)
- Mana cost reduction: 0.00 + 0.20 * T(WIS)
- Resistances clamped to 75%
- T(X) = clamp((X - 20) / 100, 0, 1)

## Known Mismatches / Debt
- Legacy MMC class names (MMC_MaxHealth, MMC_MaxMana, MMC_MaxStamina, MMC_CarryWeight) still exist for asset compatibility.
- MaxHealth/MaxMana/MaxStamina legacy MMC implementations were aligned to the canonical STR/AGI/INT/WIS formulas in C++ (2026-03-20), reducing runtime formula drift.
- `GE_RecalculateResources` is now fully mapped to canonical calculation classes (`MMC_MaxHealthFromPrimaries`, `MMC_MaxManaFromPrimaries`, `MMC_MaxStaminaFromPrimaries`, `MMC_CarryWeightFromPrimaries`) and validated on DS+2 and DS+2 lag/loss on 2026-03-20.
- Legacy aliases `VIG/FOC` were removed from runtime C++ accessors (`CombatAttributeSet`) on 2026-03-20; canonical primaries remain STR/AGI/INT/WIS.
- UI rendering remains Blueprint widget-driven; legacy reflection routing in PlayerController is optional fallback and disabled by default.

## Not Implemented Yet (High Impact)
- Production-grade persistence hardening completa (DB operacional final, economia completa, trilha de auditoria avançada).
- Automação completa de smoke multiplayer em CI (hoje existe gate local contínuo).

## Current Delivery Focus
1. Phase 1 hardening: fechado no recorte atual (2026-03-20), com matriz RPC consolidada para os Server RPCs atuais e smoke multiplayer validado.
2. Phase 2 current: criterios de saida tecnicos (authority/UI-independencia/E2E/lag-loss) atendidos no recorte atual.
3. Phase 3 em andamento: avançar de foundation v1 para persistência de produção (economia, DB operacional e trilha de auditoria).
