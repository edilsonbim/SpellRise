# Package and Smoke Gate - SpellRise

## Objetivo
Definir o gate mínimo obrigatório para mudanças com impacto em runtime multiplayer, combate, replicação, persistência, online e inicialização de sessão.

Este gate existe para bloquear regressão antes de merge/release.

## Escopo
Aplicar obrigatoriamente quando a mudança tocar qualquer um destes pontos:
- combate
- GAS
- replication / RPC / OnRep
- PlayerController runtime
- persistência
- online / sessão / auth
- mapa de entrada, config de boot ou caminho de package
- Dedicated Server bootstrap

## Fonte de verdade
1. PROJECT_STATE.md
2. ARCHITECTURE.md
3. ATTRIBUTE_MATRIX.md
4. MULTIPLAYER_RULES.md
5. SECURITY_MODEL.md
6. BACKLOG.md
7. BUG_LOG.md
8. ROADMAP.md
9. COMBAT_DESIGN_PHILOSOPHY.md
10. ENGINEERING_PLAYBOOK.md

Se houver conflito entre legado e documentação atual, registrar no review e seguir os documentos.

Definição operacional fixa:
- Fonte de verdade de gameplay em runtime = `ASC + GameplayEffects + GameplayTags` no servidor.
- Qualquer estado fora do ASC é somente suporte de UX/telemetria e não pode decidir resultado final.

## Política fixa
- Toda build, cook, package e smoke deve usar somente a Unreal Source.
- Client e Server devem ser gerados no mesmo ciclo de build/package.
- Não aprovar mudança sem evidência de paridade entre artefatos de Client e Server.
- Não aprovar mudança sem smoke Dedicated Server.

## Fases obrigatórias do gate
### 1. Build
- Build `SpellRiseEditor Win64 Development`
- Build `SpellRiseClient Win64 Development`
- Build `SpellRiseServer Win64 Development`
- Sem erro
- Sem warning crítico novo no recorte alterado

### 2. Cook / Package
- Package de Client
- Package de Server
- Mesmo commit
- Mesmo conjunto de config
- Mesmo mapa de entrada esperado
- Mesmo perfil de OnlineSubsystem do cenário validado

### 3. Package parity
Validar no mínimo:
- mesmo commit/branch
- mesmo perfil de build
- mesmas configs críticas de runtime
- presença dos mapas obrigatórios
- presença dos assets críticos do fluxo testado
- ausência de mismatch óbvio entre conteúdo do Client e do Server

### 4. Smoke funcional
Executar no mínimo:
- Standalone
- Listen Server
- Dedicated Server + 2 clientes
- Dedicated Server + 2 clientes com reconnect
- Dedicated Server + 2 clientes com lag/loss

### 5. Casos críticos obrigatórios
- boot do mapa correto
- login/entrada em sessão
- cast/commit/cooldown/custo
- projétil autoritativo completo
- morte/loot/respawn
- reconnect básico
- persistência básica do recorte alterado, quando aplicável

## Critérios de reprovação imediata
- `FBitReader::SetOverflowed`
- `ReadFieldHeaderAndPayload` com degradação de sessão
- mismatch client/server que afete login, HUD, controller ou spawn
- cliente decidindo dano/custo/cooldown/morte/loot
- Dedicated Server dependendo de HUD/UI/câmera
- RPC sem validação de payload/contexto
- double commit / double fire / double damage
- regressão de reconnect
- regressão de auth/session no modo validado

## Casos focais atuais do projeto
O gate deve observar com prioridade:
- overflow de replicação/serialização no `BP_SpellRisePlayerController_C`
- bootstrap/auth Steam no Dedicated Server
- persistência de produção ainda não fechada

## Resultado mínimo para aprovação
A mudança só passa quando:
- Build = PASS
- Package Client = PASS
- Package Server = PASS
- Package parity = PASS
- Standalone = PASS
- Listen = PASS
- DS+2 = PASS
- DS+2 reconnect = PASS
- DS+2 lag/loss = PASS
- sem erro crítico novo nos logs

## Evidências mínimas obrigatórias
- commit avaliado
- data/hora
- perfil de build
- perfil de online/auth usado
- caminho dos logs
- resumo PASS/FAIL por cenário
- causa raiz quando houver falha

## Saída padrão do gate
1. Build
2. Package
3. Package parity
4. Smoke por cenário
5. Evidências
6. Falhas encontradas
7. Causa raiz
8. Ação corretiva
9. Status final
