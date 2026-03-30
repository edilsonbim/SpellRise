# PR Template - Combate/Rede (SpellRise)

## 1) Resumo da mudança
- Escopo:
- Motivação:
- Arquivos/sistemas afetados:

## 2) Fonte de verdade validada (ordem obrigatória)
- [ ] PROJECT_STATE.md
- [ ] ARCHITECTURE.md
- [ ] ATTRIBUTE_MATRIX.md
- [ ] MULTIPLAYER_RULES.md
- [ ] SECURITY_MODEL.md
- [ ] BACKLOG.md
- [ ] BUG_LOG.md
- [ ] ROADMAP.md
- [ ] COMBAT_DESIGN_PHILOSOPHY.md

Conflito com legado identificado?
- [ ] Não
- [ ] Sim (descrever):

## 3) Server vs Client
### Server
- O que foi alterado no fluxo autoritativo:
- Onde dano/custo/cooldown/morte/loot são decididos:
- Validações de payload/contexto adicionadas:

### Client
- O que é apenas input/predição UX:
- O que é apenas apresentação (UI/VFX/SFX):
- Estratégia de reconciliação:

## 4) Riscos de authority / prediction / RPC / OnRep
- Authority:
- Prediction:
- RPC:
- OnRep:
- Mitigações:

## 5) Matriz de validação de RPC (preencher para cada RPC novo/alterado)
### RPC Validation Matrix - <RPC_NAME>
- Origem permitida: <Owner | AutonomousProxy | Server-only | Client-only>
- Payload esperado: <campos>
- Limites de payload: <faixa + tamanho max em bytes>
- Validações de contexto (server): <estado ator, cooldown, custo, tags, distancia, LOS>
- Anti-spam: <rate limit/cooldown server>
- Falha segura: <reject sem mutação>
- Log categorizado: <categoria, sem LogTemp>

## 6) Network Budget por feature (obrigatório)
### Network Budget - <FEATURE_NAME>
- NetUpdateFrequency (atores relevantes): <atual -> proposto>
- Max RPC/s por player: <valor>
- Payload max RPC (bytes): <valor>
- Target data rate max (envios/s): <valor>
- Target data payload max (bytes): <valor>
- Resultado sob lag/loss (120ms/5%): <pass | fail>

## 7) Checklist operacional (gate)
### Build
- [ ] SpellRiseEditor Win64 Development compila
- [ ] Sem warning crítico novo

### Funcional de combate/rede
- [ ] Cast/commit/cooldown/custo com recurso suficiente/insuficiente
- [ ] Fluxo projétil autoritativo completo (aim local -> target data -> commit server -> spawn replicado -> hit/GE server)
- [ ] Sem double commit/double fire/double damage
- [ ] Morte/loot/respawn com decisão no servidor
- [ ] Reconexão básica de cliente

### Modos de rede obrigatórios
- [ ] Standalone
- [ ] Listen Server (host + 1 cliente)
- [ ] Dedicated Server + 2 clientes

### Rede degradada obrigatória
- [ ] `Net PktLag=120`
- [ ] `Net PktLoss=5`
- [ ] Repetição dos casos críticos (cast/commit/cooldown/custo + projétil)
- [ ] Sem regressão visível de OnRep no escopo alterado

## 8) Evidências de teste
| Cenário | Resultado | Evidência de log | Status |
|---|---|---|---|
| Standalone |  |  |  |
| Listen Server |  |  |  |
| Dedicated +2 |  |  |  |
| Dedicated +2 lag/loss |  |  |  |

Metadados:
- Data/hora:
- Owner:
- Commit/PR:
- Build:
- Pendências:

## 9) Bloqueadores de merge (reprovação imediata)
- [ ] Não existe cliente decidindo dano/custo/cooldown/morte/loot
- [ ] Não existe multicast para corrigir estado autoritativo
- [ ] Não existe RPC sem validação de payload/contexto/anti-spam
- [ ] Dedicated server não quebrou
- [ ] PR declara orçamento de rede
- [ ] Evidências de log anexadas para cenários obrigatórios

## 10) Dívida técnica (se alterada/criada)
- Severidade: <S0 | S1 | S2 | S3>
- Owner:
- Prazo alvo:
- Critério de saída:
