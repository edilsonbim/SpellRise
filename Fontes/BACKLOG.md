# Backlog

## Foco atual
1. Fechar persistência de produção.
2. Corrigir overflow de replicação no `PlayerController`.
3. Fechar gating de auth/session para fluxos sensíveis de DS.
4. Consolidar building mode com budget e validação de RPC.
5. Avançar automação multiplayer em gate contínuo/CI.

## Blockers de alta prioridade
### 0. DS 100 load gate / login bootstrap
- Severidade: Critical
- Owner: Multiplayer/Core
- Alvo: substituir carga baseada em 100 processos Editor por cliente/bot headless leve e fechar timeout de bootstrap/login observado em 32 clientes locais.
- Evidencia: `Saved\Logs\LoadNoSteam\2026-06-10_16-59-51` falhou com 15/32 conectados, timeout de conexoes aceitas e zero `FBitReader::SetOverflowed`.
- Automacao local deve limitar rodadas baseadas em processos Editor a 12 clientes salvo override explicito; 16 clientes conectou 16/16, mas gerou OOM/pagefile em `Saved\Logs\LoadNoSteam\2026-06-10_19-17-57`.
- Binarios `Binaries\Win64\SpellRiseClient.exe`/`SpellRiseServer.exe` sem cook nao servem como carga leve: falham carregando asset registry/pacotes uncooked. Usar build staged/cooked para `-UseProjectBinaries`.
- Build staged/cooked em `Saved\LoadTestBuild` permite subir `DS+2` com `SpellRiseServer.exe`/`SpellRiseClient.exe`.
- Evidencia limpa: `Saved\Logs\LoadNoSteam\2026-06-10_20-59-26` conectou 2/2, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `BitReaderOverflowSignals=0`, sem erro `FocalLength/FieldOfView` e sem spam `DLWE_Interaction`.
- Evidencia de escala: `Saved\Logs\LoadNoSteam\2026-06-10_21-02-52` conectou 12/12 com zero timeout, zero prelogin failure, zero network failure e zero `FBitReader::SetOverflowed`, mas ficou instavel por 8 ensures client-side de Gameplay Cameras (`FocalLength/FieldOfView`).
- Evidencia apos mitigacao C++: `Saved\Logs\LoadNoSteam\2026-06-10_21-38-14` conectou 12/12 com zero timeout/overflow, mas ainda ficou instavel por 4 ensures de `CameraRigAsset_2147482084`.
- Evidencia limpa apos ajuste de CameraAsset/CameraDirector: `Saved\Logs\LoadNoSteam\2026-06-11_08-49-18` conectou 12/12, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia de escala local: `Saved\Logs\LoadNoSteam\2026-06-11_09-42-59` conectou 16/16, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia com lag/loss Perfil A: `Saved\Logs\LoadNoSteam\2026-06-11_09-55-56` conectou 16/16 com `PktLag=120/PktLoss=1`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia com lag/loss Perfil B: `Saved\Logs\LoadNoSteam\2026-06-11_10-00-58` conectou 16/16 com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia `DS+24`: primeira tentativa `Saved\Logs\LoadNoSteam\2026-06-11_10-05-01` falhou por `Server full.`; runner agora injeta `?MaxPlayers=100` no servidor de carga. Nova tentativa `Saved\Logs\LoadNoSteam\2026-06-11_10-17-12` conectou 24/24, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`.
- Evidencia `DS+24` com lag/loss Perfil A: `Saved\Logs\LoadNoSteam\2026-06-11_10-30-45` conectou 24/24 com `PktLag=120/PktLoss=1`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`, sem `Server full.`.
- Evidencia `DS+24` com lag/loss Perfil B: `Saved\Logs\LoadNoSteam\2026-06-11_11-05-33` conectou 24/24 com `PktLag=180/PktLoss=3`, `LagLossApplied=True`, `OverallUnstable=False`, `EnsureSignals=0`, `TimeoutSignals=0`, `PreLoginFailures=0`, `NetworkFailures=0`, `BitReaderOverflowSignals=0`, sem `Server full.`.
- Divida visual separada: ainda ha 1 warning `Both FocalLength and FieldOfView` por cliente no perfil headless; nao bloqueia gate de rede, mas deve ser removido antes de validar cliente visual real.
- Proximo criterio de saida: executar `DS+32` curto e monitorado com cleanup/kill por arvore de processo; 24 processos cooked ja exigiram intervencao manual no pos-run em Perfil A/B antes desse hardening.

### 1. PlayerController replication overflow
- Severidade: Critical
- Owner: Multiplayer/Core
- Alvo: eliminar `FBitReader::SetOverflowed` em `Standalone`, `Listen`, `DS+2` e `DS+2 lag/loss`.

### 2. Steam DS bootstrap / auth gating
- Severidade: Critical / High
- Owner: Backend/Online
- Alvo: DS com Steam válido, sem fallback indevido para `NULL`, e gating seguro de identidade.

### 3. Persistência de produção
- Severidade: S1
- Owner: Backend/Persistence
- Alvo: economia + DB operacional + safeguards + reconciliação determinística.
- Progresso: tabela `spellrise_death_events`, gravação server-side de morte e snapshot de personagem schema 7 com `CharacterCreated`/`VisualConfigurationJson` adicionados; falta validar criação/reconexão em DS+2 normal e lag/loss.

### 4. Building mode network contract
- Severidade: S2
- Owner: Gameplay/Network
- Alvo: budget de rede, matriz RPC e validação server-side de contexto/alcance/LOS.

## Alta prioridade
- balance pass de resist / penetration / crit sob TTK alvo;
- automação multiplayer em CI;
- revisão da base GAS para fronteiras explícitas de activation / commit / prediction / replication.

## Média prioridade
- painel de stats e fórmulas derivadas;
- tooltip com valores live;
- testes automatizados de dano, fall damage e autoridade de projétil.

## Regra de governança
Toda dívida nova ou alterada deve registrar:
- severidade;
- owner;
- data alvo;
- critério objetivo de saída.

## Hotfix em andamento (2026-04-06)
- Fechar BUG-2026-04-06-035: garantir spawn replicado de pickup no drop de cliente em DS com `ItemClass` valido no `BP_BasicItemPickup`.
- Revalidar fluxo completo de drop em Standalone, Listen e DS+Client apos ajuste final.
