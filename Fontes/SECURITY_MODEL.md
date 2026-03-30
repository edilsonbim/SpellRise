# Security Model

## Threat Model (Current Scope)
- Malicious client input (cooldown/resource bypass attempts).
- Forged or spammed RPC calls.
- Client-side manipulation of aim/target payloads.
- Illegitimate state mutation attempts (damage/stats/death flow).

## Current Controls
- Server authoritative damage and resource mutation.
- Server-side ability commit checks (cost/cooldown via GAS).
- Server-side death state and combat effect application.
- Server-only fall damage and catalyst charge mutation paths.
- No client trust for direct stat writes.
- Generic server gameplay-event forwarding now has explicit allowlist + payload validation.
- Generic server gameplay-event forwarding now enforces per-tag rate limiting.
- Replicated projectile target data is validated on server by range/cone/context before use.
- Security-relevant rejections are emitted through dedicated telemetry categories.
- Respawn-bed and projectile target-data paths are validated on server with context and sanity checks.
- Chat `Combat` channel is server-controlled; manual client posting is blocked in authoritative path.

## Required Hardening (Next)
- Expand validation/rate-limit policy to future high-frequency server gameplay entrypoints (including building-mode RPCs when introduced).
- Introduce replicated combat log/audit trail for suspicious sequences.
- Split debug-only instrumentation out of shipping runtime paths.

## Operational Notes
- Keep gameplay tags canonical and consistent with code.
- Enforce one source of truth for maps and runtime boot config.
- Prefer explicit categories over LogTemp for security-relevant logs.
