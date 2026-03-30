# Network Model

## Authority Boundaries
- ASC and AttributeSets live on PlayerState (authoritative owner).
- Damage calculation and health mutation: server only.
- Death state transition and ragdoll trigger: server decides, multicast presents.
- Fall damage and catalyst charge updates: server only.

## Prediction Model
- Input and cast feel are client-responsive.
- Projectile abilities can use local aim trace and send target data to server.
- Server waits for replicated target data when required, then commits and spawns.

## Target Data Flow (Projectile)
1. Client traces aim from local camera/controller.
2. Client sends target data via ServerSetReplicatedTargetData.
3. Server consumes replicated target data.
4. Server commits ability and spawns replicated projectile actor.
5. Projectile overlap/hit applies server-side gameplay effect.

## Replication Notes
- ASC replication mode is Mixed.
- Health/mana/stamina and derived attributes replicate via AttributeSets.
- Damage numbers use replicated events/cues for presentation.

## No Client Trust
- No client-authoritative damage values.
- No client stat mutation.
- No client-side acceptance of cooldown/resource bypass.
