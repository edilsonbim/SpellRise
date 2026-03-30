# Runbook de Smoke Multiplayer - Ciclo 1 (C++/Config)

Data: 2026-03-19  
Escopo: validação mínima obrigatória de authority/prediction/RPC/OnRep

## 1) Ambientes obrigatórios

1. Standalone
2. Listen Server (host + 1 cliente)
3. Dedicated Server + 2 clientes

## 2) Pré-condições

1. Build `SpellRiseEditor Win64 Development` concluído.
2. `DefaultEngine.ini` consistente com mapa de entrada do projeto.
3. Logs de categorias críticos ativos (sem `LogTemp` no runtime crítico).

## 3) Casos de smoke (ordem de execução)

1. Cast/commit/cooldown/custo
- Passos:
  - Acionar habilidade primária e secundária com recurso suficiente e insuficiente.
  - Repetir com input press/release rápido para testar reconciliação.
- Esperado:
  - Commit só no servidor.
  - Sem `double commit` em latência.
  - Cooldown/custo não bypassáveis pelo cliente.

2. Projétil autoritativo
- Passos:
  - Cliente faz aim local e dispara ability de projétil.
  - Validar sequência: target data -> commit server -> spawn replicado -> hit/GE no server.
- Esperado:
  - Spawn real do projétil apenas no servidor.
  - Sem `double fire`/`double damage`.
  - Logs de `[COMBAT][ABILITY_COMMIT_*]`, `[COMBAT][PROJECTILE_SPAWN]`, `[COMBAT][DAMAGE_APPLIED]`.

3. Morte/loot/respawn
- Passos:
  - Forçar kill entre dois jogadores.
  - Validar evento de morte, feedback multicast cosmético e fluxo de loot/respawn.
- Esperado:
  - Transição de morte decidida pelo servidor.
  - Cliente só recebe apresentação e mensagens.

4. Reconexão básica
- Passos:
  - Desconectar cliente e reconectar na mesma sessão.
  - Revalidar HUD/estado mínimo replicado.
- Esperado:
  - Sem estado autoritativo inválido no cliente após reconexão.

## 4) Rede degradada (obrigatório)

Aplicar no cliente de teste via console:
- `Net PktLag=120`
- `Net PktLoss=5`

Repetir os casos 1 e 2.

Critérios adicionais de aprovação:
1. Sem duplicidade de dano/commit após reconciliação.
2. Sem RPC autoritativo aceito fora de contexto de owner.
3. Sem regressão de OnRep visível para arquetipo/skill wheel.

## 5) Resultado por cenário (template)

| Cenário | Resultado | Evidência de log | Status |
|---|---|---|---|
| Standalone | Não executado neste lote | N/A | Pendente |
| Listen Server | Não executado neste lote | N/A | Pendente |
| Dedicated +2 | Server e 2 clientes conectaram no mesmo mapa sem crash durante janela de observação | `Saved/Logs/Smoke_DS_Server.log`, `Saved/Logs/Smoke_DS_Client1.log`, `Saved/Logs/Smoke_DS_Client2.log` | Parcialmente aprovado |
| Dedicated +2 com lag/loss | `Net PktLag=120` e `Net PktLoss=5` aplicados nos 2 clientes; conexão concluída e sessão estável na janela de observação | `Saved/Logs/Smoke_DS_LagLoss_Server.log`, `Saved/Logs/Smoke_DS_LagLoss_Client1.log`, `Saved/Logs/Smoke_DS_LagLoss_Client2.log` | Parcialmente aprovado |

## 5.1) Evidência objetiva coletada (2026-03-19)

1. Dedicated +2 (normal):
- Server registrou dois joins bem-sucedidos (`LogNet: Join succeeded`) no arquivo `Smoke_DS_Server.log`.
- Cliente 1 e Cliente 2 registraram `LogNet: Welcomed by server` nos arquivos `Smoke_DS_Client1.log` e `Smoke_DS_Client2.log`.

2. Dedicated +2 (lag/loss):
- Cliente 1 e Cliente 2 registraram aplicação de rede degradada:
  - `LogNet: PktLoss set to 5`
  - `LogNet: PktLag set to 120`
- Ambos também registraram `LogNet: Welcomed by server`.
- Server registrou joins bem-sucedidos no `Smoke_DS_LagLoss_Server.log`.

## 5.2) Alertas observados no smoke

1. `UNetDriver::ProcessRemoteFunction: No owning connection ... SetAllowedItems_SERVER will not be processed.` (atores de storage)

Observação: o alerta de `DefaultMappingContext` foi corrigido no recheck DS+2 (`Smoke_DS_Recheck_Client1.log` e `Smoke_DS_Recheck_Client2.log` sem ocorrência).  
Permanece pendente apenas o alerta de ownership do storage RPC (ciclo de correção em BP/assets).

## 7) Atualização de validação - 2026-03-20 (pós migração de recálculo canônico)

1. Build
- `SpellRiseEditor Win64 Development` compilou com sucesso após inclusão de `MMC_CarryWeightFromPrimaries`.

2. Standalone
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_12-52-13_Standalone`
- Resultado: sem `Fatal error`, `Assertion failed` ou `ensure(`.

3. Listen Server (+1 cliente)
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_12-53-30_Listen`
- Evidência: cliente recebeu `LogNet: Welcomed by server`.
- Resultado: sem `Fatal error`, `Assertion failed` ou `ensure(`.

4. Dedicated Server + 2 clientes
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_12-39-22`
- Summary: `OverallPass=True`.

5. Dedicated Server + 2 clientes com lag/loss
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_12-40-30`
- Summary: `OverallPass=True`, `Client1LagLossApplied=True`, `Client2LagLossApplied=True`.

6. Alertas críticos de ownership RPC
- Não reproduzido `No owning connection ... SetAllowedItems_SERVER` nos runs acima.

## 8) Atualização de validação - 2026-03-20 (fluxo morte/loot/respawn determinístico DS+2)

1. Gancho de teste aplicado
- `ASpellRisePlayerController` recebeu hook non-shipping `SR_ForceDeath` (`Exec` + `ServerSR_ForceDeath`) para forçar `Health=0` no servidor com validação de contexto.
- Automação headless usa flag de linha de comando `-SRForceDeathOnBeginPlay` (disparo com delay no `BeginPlay/Tick` local).

2. Execução Dedicated +2 (normal)
- Comando: `.\Scripts\Run-Smoke-DS.ps1 -NoSteam -ClientExtraArgs "-SRForceDeathOnBeginPlay"`.
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_13-37-37`.
- Summary: `OverallPass=True`.
- Evidência servidor:
  - `[Debug][ForceDeath] Forcando Health=0 no servidor`
  - `[FullLoot][Death] OnHealthChanged matou personagem`
  - `[DeathLoot] Disparando FullLoot via OnHealthChanged`
  - `[FullLoot][Respawn] CombatLock ativo`
  - `[FullLoot][Respawn] Agendado respawn`

3. Execução Dedicated +2 (lag/loss)
- Comando: `.\Scripts\Run-Smoke-DS.ps1 -NoSteam -WithLagLoss -ClientExtraArgs "-SRForceDeathOnBeginPlay"`.
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_13-38-50`.
- Summary: `OverallPass=True`, `Client1LagLossApplied=True`, `Client2LagLossApplied=True`.
- Evidência servidor: mesmas assinaturas de morte/loot/respawn do item anterior.

4. Execução Dedicated +2 com reconexão (normal)
- Comando: `.\Scripts\Run-Smoke-DS.ps1 -NoSteam -ClientExtraArgs "-SRForceDeathOnBeginPlay" -ReconnectClient1 -ReconnectClient1AtSeconds 20 -ReconnectClient1DowntimeSeconds 3 -TestDurationSeconds 55`.
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_13-50-58`.
- Summary: `OverallPass=True`, `ServerJoinSucceededCount=3`, `Client1ReconnectWelcomedCount=1`, `PassReconnectClient1=True`.
- Evidência: servidor registrou terceiro `Join succeeded` após reconnect; cliente reconectado registrou `Welcomed by server`.

5. Execução Dedicated +2 com reconexão (lag/loss)
- Comando: `.\Scripts\Run-Smoke-DS.ps1 -NoSteam -WithLagLoss -ClientExtraArgs "-SRForceDeathOnBeginPlay" -ReconnectClient1 -ReconnectClient1AtSeconds 20 -ReconnectClient1DowntimeSeconds 3 -TestDurationSeconds 55`.
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_13-52-14`.
- Summary: `OverallPass=True`, `PassReconnectClient1=True`, `Client1LagLossApplied=True`, `Client2LagLossApplied=True`.
- Evidência: cliente reconectado registrou `PktLag set to 120`, `PktLoss set to 5` e `Welcomed by server`.

6. Gate contínuo local (sequencial, fail-fast)
- Script: `Scripts/Run-Smoke-Gate.ps1`.
- Cenários executados pelo gate:
  - DS+2 reconnect (normal)
  - DS+2 reconnect (lag/loss), quando `-SkipLagLoss` não for usado
- Exemplo validado: `.\Scripts\Run-Smoke-Gate.ps1 -NoBuild -NoSteam -SkipLagLoss -TestDurationSeconds 45`.
- RunDir evidência do gate validado: `Saved/Logs/SmokeAuto/2026-03-20_14-00-58`.

## 9) Atualização de validação - 2026-03-20 (E2E Phase 2: Standalone + Listen)

1. Standalone (death/full-loot/respawn)
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_14-42-16_Phase2_Standalone`
- Evidência:
  - `StandaloneDeathEvidenceCount=1`
  - `StandaloneLootEvidenceCount=2`
  - `StandaloneRespawnEvidenceCount=1`
  - `StandaloneFatalLikeCount=0`

2. Listen Server (+1 cliente)
- RunDir: `Saved/Logs/SmokeAuto/2026-03-20_14-42-16_Phase2_Listen`
- Evidência:
  - `ListenJoinSucceededCount=1`
  - `ListenWelcomedCount=1`
  - `ListenDeathEvidenceCount=2`
  - `ListenLootEvidenceCount=4`
  - `ListenRespawnEvidenceCount=2`
  - `ListenFatalLikeCount=0`

3. Summary consolidado
- `Saved/Logs/SmokeAuto/2026-03-20_14-42-16_Phase2_Standalone_Listen_Summary.txt`

## 6) Definição de pronto (item de rede/combate)

1. Compila.
2. Sem warning crítico novo.
3. Sem regressão de replicação/authority/prediction/RPC/OnRep.
4. Orçamento de rede declarado para a feature (`NetUpdateFrequency`, RPC/s, payload bytes, target data rate).
