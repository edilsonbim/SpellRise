# PlayerController Decomposition Plan

## Goal
Stabilize multiplayer runtime by turning `ASpellRisePlayerController` into a thin coordinator and moving stateful concerns to the correct runtime owners.

## Current target
Primary target of this phase:
- eliminate or isolate the runtime paths most likely involved in `FBitReader::SetOverflowed` / `ReadFieldHeaderAndPayload` around `BP_SpellRisePlayerController_C`.

## Phase plan

### Phase A - Audit and freeze
- Inventory every controller RPC and client function.
- Inventory every replicated property and Blueprint-exposed runtime path.
- Classify each item as:
  - local UX only;
  - owner-only delivery;
  - wrong owner;
  - should move to `PlayerState`;
  - should move to `Character`;
  - should move to component/subsystem.
- Freeze new controller feature growth until this audit is closed.

### Phase B - Move state to correct owners
#### Move to `PlayerState`
- reconnect-relevant player-local durable state;
- owner-centric combat/backlog style feeds;
- persistence-facing player state.

#### Move to `Character`
- avatar-dependent local hooks;
- pawn-driven input interpretation;
- interaction state that depends on possessed pawn context.

#### Move to components
- camera mode policy;
- construction mode;
- HUD/view binding helpers;
- local interaction helper.

### Phase C - Kill fragile controller paths
- remove controller-side reflection routing from production runtime when equivalent native path exists;
- remove oversized or ambiguous Blueprint payload handoff paths;
- forbid controller from becoming a generic transport for unrelated systems.

### Phase D - Harden remaining controller surface
Keep only the minimum controller API required for:
- input bootstrap;
- owner-only local UX delivery;
- explicit non-shipping debug hooks.

For every remaining controller RPC/client function:
- document caller;
- document payload size;
- validate context;
- define spam policy;
- register failure logs.

## Review checklist per moved responsibility
For each item moved out of controller, document:
1. previous owner = `PlayerController`;
2. new owner;
3. reason for ownership change;
4. replication impact;
5. reconnect impact;
6. DS safety impact.

## Non-negotiables
- no client-authoritative combat logic in controller;
- no persistence write authority in controller;
- no loot/death/respawn authority in controller;
- no controller-only replicated state required for dedicated gameplay correctness;
- no new production dependency on controller Blueprint reflection.

## Exit criteria
This phase is complete only when:
- controller RPC/client surface has a documented minimal set;
- the overflow bug path is either removed or no longer reproduces;
- local HUD/camera logic is isolated from authoritative state flow;
- DS+2 normal and lag/loss stay connected without controller payload failure;
- reconnect remains stable after controller slimming.
