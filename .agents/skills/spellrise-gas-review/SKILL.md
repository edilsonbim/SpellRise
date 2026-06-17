---
name: spellrise-gas-review
description: Use para revisar ou alterar Gameplay Ability System em SpellRise, incluindo abilities, AttributeSets, GameplayEffects, ExecCalc, MMC, target data, projeteis, healing, damage, prediction e replicacao.
---

# SpellRise GAS Review

## Fonte obrigatoria
- `PROJECT_STATE.md`
- `AGENTS.md`
- `Fontes/ARCHITECTURE.md`
- `Fontes/ATTRIBUTE_MATRIX.md`
- `Fontes/MULTIPLAYER_RULES.md`
- `Fontes/SECURITY_MODEL.md`
- `Fontes/COMBAT_DESIGN_PHILOSOPHY.md`

## Arquitetura fixa
- Owner autoritativo do GAS: `ASpellRisePlayerState`.
- ASC e AttributeSets vivem no PlayerState.
- Character consome ASC/ActorInfo do PlayerState.
- ASC replication mode: `Mixed`.
- Primarios canonicos: `STR`, `AGI`, `INT`, `WIS`.
- `VIG` e `FOC` so podem existir como compatibilidade passiva.

## Fluxo obrigatorio de ability
Separe claramente:
- ativacao;
- target data;
- commit;
- aplicacao de GE;
- cue/apresentacao replicada;
- cosmetico local;
- end/cancel/cleanup.

## Projeteis
Fluxo obrigatorio:
1. aim local;
2. target data;
3. validacao server;
4. commit;
5. spawn replicado;
6. hit/GE no servidor.

## Atributos
Ao alterar atributos, informe impacto em:
- AttributeSet;
- MMC;
- ExecCalc;
- GameplayEffect;
- UI;
- replicacao.

## Checklist de review
- Cliente nao decide hit/dano/cura/custo/cooldown/morte/loot.
- Servidor valida target data, ownership, contexto, range/LOS quando aplicavel.
- Commit autoritativo ocorre no servidor.
- GE autoritativo e aplicado pelo ASC correto.
- Prediction e apenas UX.
- Cues e multicast sao apresentacao, nao decisao.
- FailureTags/commit negado/target data rejeitado sao observaveis.

## Saida esperada
1. Onde inicia
2. Onde commita
3. Onde o servidor valida
4. Onde o cliente apenas preve
5. Onde replica
6. Riscos de authority / prediction / RPC / OnRep
7. Checklist Standalone, Listen, DS+2, DS+2 lag/loss

