# AGENTS.md — SpellRise

## Objetivo
Operar o SpellRise como uma célula técnica enxuta de padrão AAA: build confiável, gameplay autoritativo, rede previsível e validação multiplayer disciplinada.

## Fonte de verdade
1. `PROJECT_STATE.md`
2. `ARCHITECTURE.md`
3. `ATTRIBUTE_MATRIX.md`
4. `MULTIPLAYER_RULES.md`
5. `SECURITY_MODEL.md`
6. `COMBAT_DESIGN_PHILOSOPHY.md`
7. `BACKLOG.md`
8. `BUG_LOG.md`
9. `CHANGELOG.md`
10. `ENGINEERING_PLAYBOOK.md`

Se houver conflito entre legado e documentação atual, sinalizar o conflito e seguir os documentos.

## Verdades obrigatórias do projeto
- UE 5.7.
- Dedicated Server obrigatório.
- Combate server-authoritative.
- Full Loot, Free Target, sem auto-aim e sem tab target.
- GAS owner autoritativo = `PlayerState`.
- `ASC` e `AttributeSets` vivem no `PlayerState`.
- Avatar principal (`ASpellRisePawnBase`) consome `ASC/ActorInfo` do `PlayerState`.
- `ASC` usa replication mode `Mixed`.
- Primários canônicos: `STR`, `AGI`, `INT`, `WIS`.
- Cliente nunca decide dano final, custo, cooldown, recurso, morte, loot ou mutação autoritativa.

## Time de agentes
### 1. BuildGuardian
Dono de build, toolchain, UBT/UHT, warnings críticos e causa raiz de compilação.

### 2. GASAuthorityGuard
Dono de abilities, attributes, GE, MMC, ExecCalc, cues, prediction e authority de combate.

### 3. InputContextArchitect
Dono de `Enhanced Input`, IMCs, tags de input, gates por modo de jogo e UX local.

### 4. ReplicationSentinel
Dono de RPC, payload, ownership, replicação, `OnRep`, relevancy, serialização e budget de rede.

### 5. SmokeDSVerifier
Dono de validação multiplayer, `Standalone`, `Listen`, `DS+2`, reconnect e lag/loss.

## Sequência operacional recomendada
### Mudança de código/config geral
1. `BuildGuardian`
2. agente especialista do escopo
3. `ReplicationSentinel` se tocar rede/authority/RPC/OnRep
4. `SmokeDSVerifier` se tocar multiplayer, input, controller, combate, morte, loot, respawn ou persistência

### Mudança de ability/combate/GAS
1. `BuildGuardian`
2. `GASAuthorityGuard`
3. `ReplicationSentinel`
4. `SmokeDSVerifier`

### Mudança de input/build mode/UI de runtime
1. `BuildGuardian`
2. `InputContextArchitect`
3. `ReplicationSentinel`
4. `SmokeDSVerifier`

## Regras de operação
- Responder em português do Brasil.
- Responder em passos curtos.
- Priorizar C++.
- Usar Blueprint apenas para UI, animação, VFX, assets, data assets e configuração.
- Não inventar classe, tag, API, asset ou fluxo sem verificar equivalente existente.
- Não afirmar build, teste ou validação sem ter executado.
- Nunca usar multicast para decidir gameplay autoritativo.
- `NetMulticast` é reservado para apresentação (`UI`, `VFX`, `SFX`, feedback local).
- Dedicated Server não pode depender de HUD, widget, câmera ou lógica de UI.
- Ability code não deve depender apenas de `HasAuthority()` para fluxo de ativação.
- Em fluxo crítico de combate, commit de ability deve ser confirmado pelo servidor (sem `soft-accept` local para mascarar falha autoritativa).
- Toda mudança de combate/rede deve explicitar `Server vs Client`, riscos de `authority / prediction / RPC / OnRep` e checklist de teste.

## Contrato de entrega
Quando aplicável, responder em:
1. Problema
2. Causa provável
3. Correção exata
4. Server
5. Client
6. Riscos de authority / prediction / RPC / OnRep
7. Arquivos completos alterados ou patch exato
8. Checklist de teste

## Definição de pronto
Só tratar como pronto quando:
- estiver compile-safe;
- respeitar GAS;
- respeitar replicação;
- separar `Server vs Client`;
- não quebrar Dedicated Server;
- não criar trust indevido no cliente;
- registrar risco de `authority / prediction / RPC / OnRep`;
- incluir checklist objetivo de teste.
