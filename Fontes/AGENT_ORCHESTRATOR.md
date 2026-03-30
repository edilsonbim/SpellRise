# Agent Orchestrator (SpellRise)

Orquestrador de agentes focado em fluxo de desenvolvimento Unreal no projeto SpellRise.

## Objetivo

Padronizar a execucao de stages tecnicos com guardrails de GAS, rede e dedicated server:

1. build
2. gas
3. net
4. input
5. smoke

## Script

- Caminho: `Scripts/Run-Agent-Orchestrator.ps1`
- Relatorios: `Saved/Logs/AgentOrchestrator/<RunId>/`

## Modos

### 1) Checklist (padrao)
Carrega e imprime prompts dos templates em `Fontes/Agentes` sem executar build/smoke.

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Run-Agent-Orchestrator.ps1 -Mode checklist
```

### 2) Run
Executa stages automatizados (`build`, `smoke`) e gera tarefa manual para stages de analise (`gas`, `net`, `input`).

```powershell
powershell -ExecutionPolicy Bypass -File .\Scripts\Run-Agent-Orchestrator.ps1 -Mode run
```

## Parametros uteis

- `-Stages build,gas,net,input,smoke`: escolhe stages.
- `-FailFast`: interrompe no primeiro erro (default ativo).
- `-ManualBlocksRun`: trata stage manual como bloqueio (falha imediata no run).
- `-NoSteam`: repassa para smoke.
- `-SkipLagLoss`: repassa para smoke.
- `-CopyPrompt`: copia prompt para clipboard (checklist).

Exemplos:

```powershell
# Pipeline completo com smoke sem lag/loss
powershell -ExecutionPolicy Bypass -File .\Scripts\Run-Agent-Orchestrator.ps1 -Mode run -SkipLagLoss

# So build + smoke
powershell -ExecutionPolicy Bypass -File .\Scripts\Run-Agent-Orchestrator.ps1 -Mode run -Stages build,smoke

# Rodar com bloqueio quando houver etapa manual
powershell -ExecutionPolicy Bypass -File .\Scripts\Run-Agent-Orchestrator.ps1 -Mode run -ManualBlocksRun
```

## Status de saida

- `0`: sucesso completo.
- `2`: existe etapa manual pendente (`manual-required`).
- `1`: falha.

## Artefatos por run

1. `<RunId>-summary.json`
2. `<RunId>-summary.txt`
3. `<RunId>-<stage>-task.md` (para stages manuais)

## Guardrails obrigatorios refletidos no relatorio

- Checklist de budget de rede por feature:
  - NetUpdateFrequency por ator relevante
  - max RPC/s por player
  - max payload RPC (bytes)
  - max taxa de target data
- Matriz de validacao:
  - Standalone
  - Listen Server
  - Dedicated Server (2+ clientes)
  - Dedicated Server (2+ clientes) com lag/loss
