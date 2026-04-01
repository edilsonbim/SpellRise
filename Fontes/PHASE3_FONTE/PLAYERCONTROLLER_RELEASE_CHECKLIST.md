# PlayerController Release Checklist

## Scope
Use this checklist for any change touching:
- `ASpellRisePlayerController`;
- `BP_SpellRisePlayerController_C`;
- controller components;
- owner-only client RPC delivery;
- HUD/camera/input bootstrap routed by controller.

## 1. Ownership review
- [ ] This change keeps `PlayerController` as a thin coordinator.
- [ ] No authoritative gameplay state was added to controller runtime.
- [ ] Any reconnect-relevant player state lives in `PlayerState`.
- [ ] Any pawn-specific runtime state lives in `Character` or pawn-owned component.
- [ ] Any local-only feature was isolated into a component/subsystem when appropriate.

## 2. RPC and replication review
- [ ] Every controller RPC/client function has explicit caller and purpose.
- [ ] Payloads are bounded and minimal.
- [ ] No generic transport payload was introduced in Blueprint controller paths.
- [ ] Replicated properties on controller are minimal and justified.
- [ ] Failure/reject paths use explicit logs.

## 3. Dedicated-server safety
- [ ] No controller change introduces dependency on HUD, widget, or camera for authoritative runtime.
- [ ] Dedicated Server remains functional without local presentation objects.
- [ ] Local-only controller code is gated out of DS paths.

## 4. Regression targets
Mandatory checks against current active risks:
- [ ] No `FBitReader::SetOverflowed`.
- [ ] No `ReadFieldHeaderAndPayload` failure around controller runtime.
- [ ] No disconnect caused by controller payload processing.
- [ ] No regression in building mode component ownership/gating.

## 5. Validation matrix
- [ ] Standalone
- [ ] Listen Server
- [ ] Dedicated Server + 2 clients
- [ ] Dedicated Server + 2 clients with reconnect
- [ ] Dedicated Server + 2 clients with `Net PktLag=120`
- [ ] Dedicated Server + 2 clients with `Net PktLoss=5`

## 6. Functional cases
- [ ] Possess / restart flow stable.
- [ ] HUD bind flow stable.
- [ ] Camera mode flow stable.
- [ ] Input bootstrap stable.
- [ ] Owner-only combat/chat/local notifications still arrive correctly.
- [ ] No authority leakage from controller to gameplay state.

## 7. Release blocker
Any one of these blocks release:
- controller overflow/serialization error reproduced;
- controller Blueprint path carries unstable replicated payload;
- DS client disconnect caused by controller init/runtime path;
- authoritative state moved into controller for convenience.
