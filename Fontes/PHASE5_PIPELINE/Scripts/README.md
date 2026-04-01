# SpellRise Pipeline Scripts

## Objetivo
Starter kit operacional para transformar a documentação das fases 1-4 em gate executável.

## Scripts
- `Run-SpellRise-ReleaseGate.ps1`
  - build + package + package parity + smoke gate
- `Test-SpellRise-PackageParity.ps1`
  - compara metadata/config entre package de client e server
- `Run-SpellRise-PersistenceGate.ps1`
  - encadeia o release gate e gera checklist de persistência
- `Run-SpellRise-CombatLab.ps1`
  - cria matriz de run do combat lab e pode disparar smoke gate
- `SpellRise.Pipeline.Common.ps1`
  - helpers compartilhados

## Dependências
- Unreal Source em `C:\UnrealSource\UnrealEngine`
- `SpellRise.uproject`
- script de projeto `Scripts\Run-Smoke-Gate.ps1` para a parte automatizada de smoke

## Uso rápido

### Release gate
```powershell
powershell -ExecutionPolicy Bypass -File .\Run-SpellRise-ReleaseGate.ps1 -NoSteam
```

### Persistence gate
```powershell
powershell -ExecutionPolicy Bypass -File .\Run-SpellRise-PersistenceGate.ps1 -NoSteam
```

### Combat lab
```powershell
powershell -ExecutionPolicy Bypass -File .\Run-SpellRise-CombatLab.ps1 -RunSmokeGate -ScenarioIds CL-007,CL-008,CL-012
```

## Observação
Os scripts são deliberadamente conservadores:
- falham quando o smoke gate do projeto não existe
- exigem paridade explícita client/server
- gravam summary/log em `Saved\Logs\Automation`
