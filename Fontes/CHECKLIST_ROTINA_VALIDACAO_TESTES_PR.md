# CHECKLIST DE ROTINA - VALIDAÇÃO E TESTES (PR COMBATE/REDE)

Data base: 2026-03-25  
Projeto: SpellRise (UE 5.7)  
Cadência: obrigatório a cada PR com impacto em combate/rede

## Política fixa de engine (obrigatória)
- Toda build e todo teste deste projeto deve usar somente a Unreal Source em `C:\UnrealSource\UnrealEngine`.
- É proibido validar build/testes com engine instalada via launcher para este repositório.
- PR sem evidência de execução na Unreal Source deve ser reprovada.

## 0) Fonte de verdade obrigatória (ordem fixa)
1. PROJECT_STATE.md
2. ARCHITECTURE.md
3. ATTRIBUTE_MATRIX.md
4. MULTIPLAYER_RULES.md
5. SECURITY_MODEL.md
6. BACKLOG.md
7. BUG_LOG.md
8. ROADMAP.md
9. COMBAT_DESIGN_PHILOSOPHY.md

Se houver conflito com código/asset legado, seguir a lista acima e registrar conflito no review.

## 1) Pré-check de contexto (gate de entrada)
Marcar todos os itens antes de executar testes:

- [ ] Mudança tem impacto de combate/rede claramente descrito.
- [ ] Separação Server vs Client explícita no PR.
- [ ] Fluxo de prediction/reconciliation descrito (se houver ability predita).
- [ ] Impacto em authority, RPC e OnRep documentado.
- [ ] Mudança respeita GAS owner no PlayerState (ASC + AttributeSets no PlayerState).
- [ ] Não há decisão autoritativa de dano/custo/cooldown/morte/loot no cliente.
- [ ] Sem uso de multicast para decisão de gameplay autoritativo.

## 2) Build e sanidade de compilação
### 2.1 Build obrigatória
- [ ] `SpellRiseEditor Win64 Development` compila sem erro.
- [ ] Sem warnings críticos novos no recorte da mudança.
- [ ] Warning de metadata de plugin (dependências de módulos/plugins) ausente no output de build.

### 2.2 Comandos manuais (PowerShell)
Ajustar variáveis uma vez por máquina:

```powershell
$ProjectRoot = "C:\Users\biM\Documents\Unreal Projects\SpellRise"
$UProject = "$ProjectRoot\SpellRise.uproject"
$UE = "C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
$LogsDir = "$ProjectRoot\Saved\Logs"
```

Compilar via editor/headless:

```powershell
& $UE $UProject -Unattended -NullRHI -NoSplash -log
```

Observacao: se o time usar pipeline de build externo (UBT/CI), anexar o log dessa pipeline como evidencia equivalente.

## 3) Matriz obrigatória de rede/combate (authority/prediction/RPC/OnRep)
### 3.1 Checklist de validacao funcional
- [ ] Cast/commit/cooldown/custo com recurso suficiente e insuficiente.
- [ ] Sem double commit em rede normal.
- [ ] Fluxo de projetil autoritativo completo:
  - [ ] Client aim trace local (entrada).
  - [ ] Client envia target data.
  - [ ] Server valida target data.
  - [ ] Server commit da ability.
  - [ ] Server spawn de projetil replicado.
  - [ ] Hit/overlap aplica GE no servidor.
- [ ] Sem double fire/double damage.
- [ ] Morte/loot/respawn decididos no servidor.
- [ ] Reconexao basica de cliente sem estado invalido.

### 3.2 Template de review para RPC novo/alterado
Copiar para a descricao do PR:

```md
### RPC Validation Matrix - <RPC_NAME>
- Origem permitida: <Owner | AutonomousProxy | Server-only | Client-only>
- Payload esperado: <campos>
- Limites de payload: <faixa + tamanho max em bytes>
- Validacoes de contexto (server): <estado ator, cooldown, custo, tags, distancia, LOS>
- Anti-spam: <rate limit/cooldown server>
- Falha segura: <reject sem mutacao>
- Log categorizado: <categoria de log, sem LogTemp>
```

### 3.3 Gate de RPC (aprovacao)
- [ ] Origem permitida declarada.
- [ ] Payload com limites objetivos declarado.
- [ ] Validacao de contexto no servidor implementada.
- [ ] Anti-spam implementado no servidor.
- [ ] Reject seguro + log categorizado implementado.

## 4) Smoke multiplayer obrigatorio
Executar em ordem:

1. Standalone
2. Listen Server (host + 1 cliente)
3. Dedicated Server + 2 clientes

### 4.1 Comandos de sessao (manual)
Observacao: os parametros de mapa/sessao podem variar por ambiente. Ajustar `<MAP>` e porta conforme padrao atual do time.

Server dedicado:

```powershell
& $UE $UProject <MAP> -server -log -Port=7777 -NetDriverListenPort=7777
```

Cliente 1:

```powershell
& $UE $UProject 127.0.0.1:7777 -game -log -ResX=1280 -ResY=720 -WinX=0 -WinY=0
```

Cliente 2:

```powershell
& $UE $UProject 127.0.0.1:7777 -game -log -ResX=1280 -ResY=720 -WinX=1280 -WinY=0
```

### 4.2 Casos de smoke obrigatorios
- [ ] Cast/commit/cooldown/custo.
- [ ] Projetil autoritativo (target data -> commit -> spawn -> hit/GE server).
- [ ] Morte/loot/respawn.
- [ ] Reconexao basica.

## 5) Smoke com rede degradada (obrigatorio)
Aplicar em cada cliente de teste pelo console:

```text
Net PktLag=120
Net PktLoss=5
```

Reexecutar, no minimo:
- [ ] Cast/commit/cooldown/custo.
- [ ] Fluxo de projetil autoritativo.

Aceite obrigatorio:
- [ ] Sem double commit/double damage apos reconciliacao.
- [ ] Sem RPC autoritativo aceito fora de contexto de owner.
- [ ] Sem regressao visivel de OnRep no escopo alterado.

## 6) Gate de orcamento de rede por feature (obrigatorio)
Preencher no PR para cada feature de combate/rede:

```md
### Network Budget - <FEATURE_NAME>
- NetUpdateFrequency (atores relevantes): <valor atual -> valor proposto>
- Max RPC/s por player: <valor>
- Payload max RPC (bytes): <valor>
- Target data rate max (envios/s): <valor>
- Target data payload max (bytes): <valor>
- Resultado sob lag/loss (120ms/5%): <pass | fail>
```

Baseline v1 atual (referencia): `NETWORK_BUDGET_BASELINE_V1_2026-03-19.md`.

## 7) Registro de evidencias (obrigatorio)
### 7.1 Coleta de logs
Ao terminar cada cenario, arquivar logs com nome padronizado:

```powershell
$Stamp = Get-Date -Format "yyyy-MM-dd_HH-mm"
New-Item -ItemType Directory -Path "$ProjectRoot\Saved\Logs\QA\$Stamp" -Force | Out-Null
Copy-Item "$LogsDir\*.log" "$ProjectRoot\Saved\Logs\QA\$Stamp\" -Force
```

### 7.2 Template de resultado por cenario
```md
| Cenario | Resultado | Evidencia de log | Status |
|---|---|---|---|
| Standalone | <resumo> | <arquivo.log> | <Aprovado/Reprovado> |
| Listen Server | <resumo> | <arquivo.log> | <Aprovado/Reprovado> |
| Dedicated +2 | <resumo> | <arquivo.log> | <Aprovado/Reprovado> |
| Dedicated +2 lag/loss | <resumo> | <arquivo.log> | <Aprovado/Reprovado> |
```

Campos obrigatorios no fechamento:
- Data/hora da execucao
- Owner responsavel
- Commit/PR avaliado
- Build usada
- Pendencias abertas (se houver)

## 8) Bloqueadores de merge (reprovacao imediata)
- [ ] Double commit/double damage em qualquer modo de rede.
- [ ] Cliente decidindo dano final/custo/cooldown/morte/loot.
- [ ] Quebra de dedicated server (dependencia de HUD/UI/camera em fluxo autoritativo).
- [ ] RPC sem validacao de payload/contexto/anti-spam.
- [ ] Uso de multicast para corrigir estado autoritativo.
- [ ] Regressao de replicacao/OnRep no escopo alterado.
- [ ] PR sem declaracao de orcamento de rede.
- [ ] Ausencia de evidencia de log para os cenarios obrigatorios.

## 9) Definicao de pronto (DoD operacional desta rotina)
Uma PR de combate/rede so pode ser aprovada quando:

- [ ] Compila.
- [ ] Sem warning critico novo.
- [ ] Respeita GAS e arquitetura de ownership (PlayerState autoritativo).
- [ ] Separa claramente Server vs Client.
- [ ] Dedicated server validado.
- [ ] Sem trust indevido no cliente.
- [ ] Riscos de authority/prediction/RPC/OnRep registrados.
- [ ] Smoke multiplayer completo executado (Standalone, Listen, DS+2).
- [ ] Smoke lag/loss executado (Net PktLag=120, Net PktLoss=5).
- [ ] Evidencias anexadas.
