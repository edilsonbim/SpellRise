# Combat Lab Release Checklist

## Purpose
Operational release gate for combat and network changes in SpellRise.

## 1. Entry conditions
- [ ] Change scope identifies affected combat systems.
- [ ] Source of truth reviewed before validation.
- [ ] Server vs Client boundaries are documented.
- [ ] Prediction/reconciliation path is documented when applicable.
- [ ] RPC / OnRep / replication scope is documented when applicable.

## 2. Build and package
- [ ] Build executed with Unreal Source only.
- [ ] Required package/cook pipeline completed.
- [ ] Client/Server parity validated for the test build.
- [ ] No critical new warning or packaging blocker in the affected scope.

## 3. Authority gate
- [ ] Client does not decide final damage.
- [ ] Client does not decide final hit result.
- [ ] Client does not decide final cost/cooldown.
- [ ] Client does not decide death or loot.
- [ ] Projectile spawn remains authoritative on server.
- [ ] Multicast is presentation only.

## 4. Scenario selection
- [ ] Relevant combat lab scenarios selected from COMBAT_LAB_TEST_MATRIX.md.
- [ ] Formula scenarios included if attributes/MMC/ExecCalc/GE changed.
- [ ] Projectile scenarios included if projectile or target-data flow changed.
- [ ] Prediction scenarios included if predicted abilities changed.
- [ ] Death/loot/respawn scenarios included if death pipeline changed.
- [ ] Presentation parity scenario included if local UX/cue/HUD/camera-adjacent combat behavior changed.

## 5. Required modes
- [ ] Standalone run complete when applicable.
- [ ] Listen Server run complete when applicable.
- [ ] Dedicated Server + 2 clients complete.
- [ ] Dedicated Server + 2 clients with lag/loss complete for net-critical changes.

## 6. Balance gate
- [ ] Relevant TTK scenario executed if balance was affected.
- [ ] Light armor remains within 5-8s when in scope.
- [ ] Medium armor remains within 8-12s when in scope.
- [ ] Heavy armor remains within 12-18s when in scope.
- [ ] Current clamps remain respected unless the change intentionally updates them.

## 7. Regression gate
- [ ] No double fire.
- [ ] No double commit.
- [ ] No duplicate damage.
- [ ] No invalid owner vs remote divergence in outcome.
- [ ] No DS crash/disconnect in combat path.
- [ ] No reconnect corruption for relevant combat state.

## 8. Evidence
- [ ] Scenario table filled with PASS/FAIL.
- [ ] Logs archived.
- [ ] Build/package identifier recorded.
- [ ] Map and preset pair recorded.
- [ ] Lag/loss settings recorded when used.
- [ ] Root cause and next action recorded for every failed scenario.

## 9. Blockers
Automatic FAIL if any of the following occurs:
- [ ] client-trusted damage or death decision
- [ ] authoritative state repaired via multicast
- [ ] server projectile authority broken
- [ ] duplicated damage/commit after reconciliation
- [ ] DS instability in required scenarios
- [ ] TTK severely outside target envelope without approved design change
- [ ] missing evidence for required scenarios

## 10. Release decision
- [ ] PASS
- [ ] PASS WITH RISK
- [ ] FAIL

Decision notes:
- Owner:
- Build/Package:
- Date:
- Change scope:
- Accepted risks:
- Follow-up actions:
