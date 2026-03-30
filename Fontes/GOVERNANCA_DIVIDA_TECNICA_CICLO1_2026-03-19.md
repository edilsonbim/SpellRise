# Governança de Dívida Técnica - Ciclo C++/Config Crítico (2026-03-19)

## Escopo
- Ciclo 1: `Source/` + `Config/`.
- Fonte de verdade: `PROJECT_STATE.md`, `ARCHITECTURE.md`, `ATTRIBUTE_MATRIX.md`, `MULTIPLAYER_RULES.md`, `SECURITY_MODEL.md`.

## Registro rastreável (obrigatório)
| ID | Dívida | Severidade | Owner | Prazo alvo | Critério de saída |
|---|---|---|---|---|---|
| TD-001 | RPC de seleção/arquetipo ainda depende de cobertura de testes de abuso (spam/flood) em DS 2+ clientes | S1 | Network/Gameplay (C++) | 2026-03-26 | Teste com limite de RPC/s por player validado e rejeições auditadas em log categorizado |
| TD-002 | Reconciliação visual de abilities preditas ainda sem suíte automatizada de regressão (latência/perda) | S1 | Gameplay/GAS | 2026-03-29 | Cenários com lag/loss executados sem double fire/double commit/double damage |
| TD-003 | Coexistência de MMC legado com pipeline novo (compatibilidade temporária) | S2 | Gameplay Systems | 2026-04-05 | Remover dependência de mapeamento legado em runtime crítico e manter equivalência numérica validada |
| TD-004 | `Data.CatalystCharge` vs `Data.CatalystChargeDelta` coexistem por fallback de assets antigos | S2 | Gameplay Attributes | 2026-04-02 | Todos assets/magnitudes migrados para `Data.CatalystChargeDelta`; fallback legado removido |
| TD-005 | Combat log replicado completo ainda não implementado como feature dedicada | S2 | Multiplayer/UX | 2026-04-09 | Pipeline de combat log replicado implementado com orçamento de rede declarado |
| TD-006 | Pipeline Full Loot + respawn ainda com pontos de integração pendentes | S2 | Gameplay/Core Loop | 2026-04-12 | Fluxo fim-a-fim validado em Standalone, Listen e DS (2+ clientes) |
| TD-007 | Persistência ainda não implementada para estados críticos de progressão | S2 | Backend/Online | 2026-04-19 | Persistência mínima funcional com critérios de consistência e recuperação |
| TD-008 | Configuração de mapas com histórico de conflito (entrada em `DefaultEngine.ini`) | S3 | Core/Build | 2026-03-31 | Config única definida, validada em boot de editor e cook |

## Itens resolvidos neste ciclo (parcial)
| ID | Item | Resultado |
|---|---|---|
| R-001 | Padronização de logs críticos em combate/rede | `LogTemp` removido de `Source/SpellRise`; categorias explícitas adicionadas |
| R-002 | Instrumentação mínima de combate | Eventos de activation/commit/spawn/damage com counters de runtime |
| R-003 | Endurecimento RPC de arquetipo/skill wheel | validação de owner/contexto + rate limit + auditoria de rejeição |
| R-004 | Endurecimento RPC de respawn bed (`PlayerState`) | rate-limit dedicado (`1.0s`, max `2`) + rejeição segura auditada |

## Governança
- Mudança nova em combate/rede sem owner, severidade, prazo e critério de saída: bloqueada para merge.
- Mudança de contrato de rede sem orçamento explícito (NetUpdateFrequency, RPC/s, payload bytes, target data rate): classificada como risco de regressão.
