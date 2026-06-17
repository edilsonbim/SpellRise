---
name: spellrise-replication-budget
description: Use para revisar budget de rede, propriedades replicadas, RepNotify, PlayerController, PlayerState, Character, projeteis, building mode, inventario, loot e qualquer mudanca com risco de overflow ou OnRep fora de ordem em SpellRise.
---

# SpellRise Replication Budget

## Objetivo
Garantir que mudancas net-critical sejam compativeis com Dedicated Server, DS+2, lag/loss e com o foco ativo de corrigir overflow de replicacao no PlayerController.

## Antes de agir
- Leia `PROJECT_STATE.md`, `AGENTS.md`, `Fontes/ARCHITECTURE.md` e `Fontes/MULTIPLAYER_RULES.md`.
- Identifique se o estado pertence a `PlayerState`, `Character`, `PlayerController`, ator de mundo, componente GAS ou UI local.

## Matriz rapida
- Estado GAS e atributos: `PlayerState`/ASC/AttributeSet.
- Inventario de player: `PlayerState`.
- Hotbar: `PlayerState`, owner-only.
- Input e UX local: `PlayerController`.
- Projeteis: ator replicado, hit/GE no servidor.
- UI/VFX/SFX: cliente, cue ou apresentacao; nunca authority.

## Checklist de budget
Informe sempre:
- atores replicados afetados;
- propriedades replicadas novas/alteradas;
- condicao (`OwnerOnly`, `SkipOwner`, `None` etc.);
- `NetUpdateFrequency` dos atores relevantes;
- RPC/s maximo esperado por jogador;
- tamanho aproximado de payload;
- taxa/tamanho de target data quando aplicavel;
- risco de burst;
- risco de `OnRep` fora de ordem;
- impacto esperado em DS+2 normal e DS+2 com lag/loss.

## Regras de OnRep
- `OnRep` e apenas reconciliacao/apresentacao.
- `OnRep` nao decide dano, custo, cooldown, hit, morte, loot ou persistencia.
- `OnRep` precisa tolerar ordem diferente entre propriedades correlatas.
- Evite `OnRep` com payload grande, alta frequencia ou side effects pesados.

## Bloqueios
Marque como defeito se encontrar:
- payload cosmetico replicado pelo PlayerController;
- estado autoritativo duplicado fora do ASC/PlayerState sem justificativa;
- `DOREPLIFETIME` sem motivo/condicao avaliados;
- multicast decidindo gameplay;
- cliente enviando valor final em vez de intencao.

