# Backlog

## Foco atual
1. Fechar persistência de produção.
2. Corrigir overflow de replicação no `PlayerController`.
3. Fechar gating de auth/session para fluxos sensíveis de DS.
4. Consolidar building mode com budget e validação de RPC.
5. Avançar automação multiplayer em gate contínuo/CI.

## Blockers de alta prioridade
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
