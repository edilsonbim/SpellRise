# Plano de Correção Faseado - SpellRise (C++/Config Crítico)

Data: 2026-03-19  
Baseado em: `Fontes/AUDITORIA_CONFORMIDADE_CPP_CONFIG_2026-03-19.md`

## Objetivo
Executar correções sem quebrar dedicated server, mantendo server-authority e separação clara entre UX predita no cliente e decisão final no servidor.

## Lote 1 (S0/S1) - Segurança, authority e orçamento de rede

### 1.1 Formalizar orçamento de rede por feature de combate
- Severidade: S1
- Owner: Gameplay Network C++
- Prazo alvo: 2026-03-24
- Entrega:
  - Documento versionado de budget com:
    - `NetUpdateFrequency` / `MinNetUpdateFrequency` por ator relevante (PlayerState, Character, Projectile).
    - teto de RPC/s por player por endpoint server.
    - payload máximo (bytes) por RPC.
    - taxa máxima de envio de target data.
- Critério de saída:
  - Budget aprovado e referenciado no PR de toda feature de combate/rede.

### 1.2 Migrar logs críticos de LogTemp para categorias explícitas
- Severidade: S1
- Owner: Core Combat C++
- Prazo alvo: 2026-03-25
- Entrega:
  - Categorias dedicadas para: death/loot, fall damage, projectile ability, ASC cue lifecycle, input replication.
  - Logs de rejeição/security com categoria estável.
- Critério de saída:
  - `rg -n "LogTemp" Source/SpellRise` sem ocorrências em caminho crítico de combate/rede.

## Lote 2 (S1/S2) - RPC hardening e reconciliação

### 2.1 Endurecer `ServerSetArchetype`
- Severidade: S2
- Owner: Character GAS C++
- Prazo alvo: 2026-03-26
- Entrega:
  - validação de contexto (pawn/controlador/estado de morte/lock de gameplay quando aplicável);
  - anti-spam simples por janela no servidor;
  - rejeição segura com log categorizado.
- Critério de saída:
  - RPC rejeita spam/payload inválido sem alterar estado autoritativo.

### 2.2 Endurecer `ServerSetSelectedAbilityInputTag`
- Severidade: S2
- Owner: Character GAS C++
- Prazo alvo: 2026-03-26
- Entrega:
  - validação de contexto e origem;
  - limite de taxa por player;
  - trilha de rejeição categorizada.
- Critério de saída:
  - sem churn de replicação sob spam de input wheel.

### 2.3 Matriz operacional de RPC no repositório
- Severidade: S3
- Owner: Gameplay Network C++
- Prazo alvo: 2026-03-26
- Entrega:
  - tabela única por RPC: origem permitida, payload, limites, contexto, anti-spam, comportamento em falha.
- Critério de saída:
  - toda alteração de RPC exige atualização da matriz no mesmo PR.

## Lote 3 (S2) - Unificação de atributos/tags e redução de legado

### 3.1 Resolver mismatch de primários (20..100 vs 20..120)
- Severidade: S2
- Owner: Systems Balance + GAS C++
- Prazo alvo: 2026-03-28
- Entrega:
  - decisão única de faixa efetiva e alinhamento entre `BasicAttributeSet` e `MMC_DerivedStats`.
- Critério de saída:
  - fórmula e clamp coerentes em runtime + documentação atualizada.

### 3.2 Isolar/desativar MMCs legados em runtime crítico
- Severidade: S2
- Owner: GAS Migration
- Prazo alvo: 2026-03-29
- Entrega:
  - plano de migração de assets para STR/AGI/INT/WIS;
  - reduzir uso de `GetVigorAttribute/GetFocusAttribute` a compatibilidade temporária controlada.
- Critério de saída:
  - pipeline principal de combate sem dependência de aliases legados.

## Lote 4 (S2/S3) - Observabilidade e regressão contínua

### 4.1 Instrumentação mínima de combate (obrigatória)
- Severidade: S2
- Owner: Combat Runtime
- Prazo alvo: 2026-03-30
- Entrega:
  - contadores/logs de: ativação de ability, commit, aplicação de GE, dano aplicado/negado, motivo de rejeição.
- Critério de saída:
  - eventos críticos rastreáveis por categoria sem ruído excessivo.

### 4.2 Suite de smoke multiplayer para gate de regressão
- Severidade: S3
- Owner: QA Automation + Gameplay
- Prazo alvo: 2026-04-02
- Entrega:
  - smoke validando cast/commit/cooldown/custo, projétil autoritativo, morte/loot/respawn, reconexão.
- Critério de saída:
  - execução obrigatória em Standalone, Listen, Dedicated (2+ clientes).

## Sequência de execução recomendada
1. Lote 1 completo (orçamento + logs).
2. Lote 2 completo (RPC hardening + matriz).
3. Lote 3 (atributos/legado).
4. Lote 4 (observabilidade + automação).

## Checklist de teste por lote
- Standalone.
- Listen Server.
- Dedicated Server com 2+ clientes.
- Rede degradada:
  - `Net PktLag=120`
  - `Net PktLoss=5`
- Critérios extras:
  - sem double fire/double commit/dano duplicado após reconciliação.
  - sem regressão de authority/prediction/RPC/OnRep.
  - compilação limpa sem warning crítico novo.
