# Multiplayer Rules (Runtime Contract)

- All health-affecting damage is resolved on server through GAS execution/effects.
- Client never writes authoritative damage, resources, or primary attributes.
- Use server authority for state mutation; use local prediction only for UX.
- Projectile fire uses client aim trace as input, but spawn/damage stays server-authoritative.
- Ability code must not rely on HasAuthority() alone for activation flow.
- For predicted abilities, separate local cosmetic events from server gameplay events.
- Dedicated server must run without UI/camera dependencies.
- Multicast is for replicated presentation, never for authoritative gameplay decisions.
- Every server RPC entrypoint must validate payload and calling context.
- Every combat/network feature must declare network budget before merge:
  - NetUpdateFrequency of relevant actors
  - max RPC/s per player
  - max RPC payload bytes
  - max target-data send rate

## Mandatory Validation Matrix
- Standalone
- Listen Server
- Dedicated Server (2+ clients)
- Dedicated Server (2+ clients) with lag/loss simulation
