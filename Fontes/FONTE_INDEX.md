# Fonte - Índice Operacional

## Objetivo
Manter um conjunto enxuto de documentos vivos para guiar desenvolvimento, revisão e operação do SpellRise em padrão de produção multiplayer AAA.

## Ordem de leitura
1. `PROJECT_STATE.md`
2. `AGENTS.md`
3. `ARCHITECTURE.md`
4. `ATTRIBUTE_MATRIX.md`
5. `MULTIPLAYER_RULES.md`
6. `SECURITY_MODEL.md`
7. `COMBAT_DESIGN_PHILOSOPHY.md`
8. `REFERENCIAS_JOGOS.md`
9. `BACKLOG.md`
10. `BUG_LOG.md`
11. `CHANGELOG.md`
12. `ENGINEERING_PLAYBOOK.md`

## Verdades de runtime
- UE 5.7.
- Dedicated Server obrigatório.
- Fonte de verdade única de gameplay: `ASC + GameplayEffects + GameplayTags` no servidor.
- GAS com owner autoritativo no `PlayerState`.
- `ASC` e `AttributeSets` no `PlayerState`.
- `ASC` em replication mode `Mixed`.
- Primários canônicos: `STR`, `AGI`, `INT`, `WIS`.
- Combate server-authoritative.
- Cliente prevê UX; servidor decide gameplay.
- Estados fora do ASC são auxiliares (cache/UI) e não definem resultado final.
- Fluxo de projétil: aim local -> target data -> validação server -> commit -> spawn replicado -> hit/GE no servidor.

## Regra de manutenção
- Documento vivo fica em `Fontes/`.
- Auditorias, relatórios de ciclo, planos faseados e pós-mortem devem sair de `Fontes/` e ir para `Docs/Archive/` ou pasta equivalente.
- `Fontes/` deve conter regra atual, não histórico extenso.

## Critério de inclusão em Fontes
Só manter aqui documento que faça ao menos uma destas funções:
- define contrato de runtime;
- define política de engenharia/rede;
- define alvo de design/balance;
- define prioridade ativa de execução;
- registra bugs abertos relevantes.
