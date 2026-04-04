# PlayerController Runtime Boundaries

## Objective
Reduce `ASpellRisePlayerController` to a thin orchestration layer.

The controller should own only:
- local input routing;
- local UX/session glue;
- owner-only client messaging;
- non-authoritative local view coordination.

The controller must not become the source of authoritative gameplay state.

## Why this matters now
Current project state still reports multiplayer instability with replication/serialization overflow in `BP_SpellRisePlayerController_C` during init/runtime. The controller is also explicitly listed as an active hardening area, while building mode has already been moved into a dedicated controller component. This makes controller-slimming the correct next step.

## Runtime ownership policy

### PlayerController owns
- Enhanced Input bootstrap and local mapping changes.
- Local camera mode requests.
- HUD/widget binding triggers.
- Owner-only client notifications.
- Non-shipping debug commands explicitly scoped for QA.

### PlayerController must not own
- final combat result;
- inventory authority;
- persistence writes;
- death / respawn decision;
- loot decision;
- attribute mutation;
- replicated gameplay state that belongs naturally to `PlayerState`, `Pawn/Avatar`, `GameState`, or dedicated components.

## Recommended boundaries

### Move out of PlayerController when possible
#### To PlayerState
- owner-authoritative player runtime state;
- durable replicated player feeds;
- persistence-facing player identity state.

#### To Pawn/Avatar
- avatar-specific local gameplay hooks;
- pawn-scoped interaction with movement/combat presentation;
- aim/input interpretation that is avatar-dependent.

#### To GameState / dedicated subsystem
- shared match/runtime services;
- chat transport;
- combat feed aggregation;
- server-side coordination services.

#### To controller components / local subsystems
- building mode;
- camera policy;
- HUD binding/view model glue;
- local interaction helpers;
- debug-only local tooling.

## Replication policy
- Keep replicated payloads in `PlayerController` to the minimum possible.
- Prefer owner-only client RPC only for presentation or local UX delivery.
- Prefer replicated durable state on `PlayerState` when the state is player-owned and reconnect-relevant.
- Do not replicate large or structurally unstable payloads through controller-local Blueprint paths.
- Avoid controller-driven reflection bridges for production runtime.

## RPC policy
Every controller RPC must declare:
- allowed caller;
- payload bounds;
- context validation;
- failure behavior;
- whether it is gameplay-authoritative or owner-local UX only.

If the RPC affects gameplay authority, the default answer should be: move it out of `PlayerController`.

## Blueprint policy
`BP_SpellRisePlayerController_C` should remain a thin presentation/config layer.
Do not accumulate replicated gameplay payload or fragile init-time data routing in Blueprint controller graphs.

## Definition of done
Controller work is considered correct only when:
- controller responsibilities are thinner after the change, not broader;
- no new authoritative gameplay state is introduced in controller runtime;
- local UX remains owner-only and dedicated-safe;
- payload size/churn in controller replication paths is reduced or unchanged;
- Standalone, Listen, and DS+2 remain stable.
