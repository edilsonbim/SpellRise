# AAA Live Service Gate

## Objetivo
Transformar as instrucoes de engenharia do SpellRise em um gate operacional para multiplayer live service.

Este gate nao substitui os scripts existentes. Ele orquestra:
- `Run-SpellRise-ReleaseGate.ps1`
- `Run-SpellRise-PersistenceGate.ps1`
- `Run-SpellRise-CombatLab.ps1`
- `Run-Smoke-Gate.ps1`

## Script principal
```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Pipeline\Run-SpellRise-AAAGate.ps1
```

## Modos
| Modo | Uso | O que valida |
|---|---|---|
| `Auto` | padrao local/CI | classifica arquivos alterados e escolhe o gate |
| `DocsOnly` | docs/instrucoes | preflight de documentos fonte |
| `Build` | C++ simples | `SpellRiseEditor Win64 Development` |
| `NetCritical` | RPC, PlayerController, GAS, combate, projetil, replicacao | build + smoke DS+2 |
| `Persistence` | persistencia, loot, economia, auth/session | persistence gate |
| `Combat` | matriz de combate/rede | combat lab |
| `Release` | fechamento amplo | release gate completo |

## Comandos recomendados
### Mudanca comum
```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Pipeline\Run-SpellRise-AAAGate.ps1
```

### Mudanca net-critical
```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Pipeline\Run-SpellRise-AAAGate.ps1 -GateMode NetCritical
```

### Persistencia/economia
```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Pipeline\Run-SpellRise-AAAGate.ps1 -GateMode Persistence
```

### Release completo
```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Pipeline\Run-SpellRise-AAAGate.ps1 -GateMode Release
```

## Evidencia
Cada execucao grava:
- `Saved\Logs\Automation\AAAOperationalGate\<RunId>\summary.txt`
- `Saved\Logs\Automation\AAAOperationalGate\<RunId>\summary.json`
- logs dos steps executados

## Condicoes bloqueantes
O gate deve falhar quando ocorrer:
- build com erro;
- smoke DS+2 com erro;
- `FBitReader::SetOverflowed`;
- documento fonte obrigatorio ausente;
- `Build.bat` fora de `C:\UnrealSource\UnrealEngine`;
- falha propagada por release, persistence ou combat gate.

## Politica de PR
Mudanca net-critical so deve entrar com:
- authority/server/client explicitos;
- RPC novo/alterado documentado;
- budget de rede declarado;
- risco de `OnRep` registrado;
- evidencia de DS+2 normal e lag/loss quando aplicavel;
- plano de rollback quando tocar gameplay, economia, loot, persistencia ou auth/session.
