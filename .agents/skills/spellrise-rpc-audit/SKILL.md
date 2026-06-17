---
name: spellrise-rpc-audit
description: Use para auditar, criar ou alterar RPCs em SpellRise, especialmente PlayerController, building mode, inventario, loot, persistencia, GAS target data e fluxos net-critical com risco de FBitReader::SetOverflowed.
---

# SpellRise RPC Audit

Use esta skill antes de aceitar qualquer RPC novo ou alterado.

## Fonte de verdade
- Leia `PROJECT_STATE.md`, `AGENTS.md`, `Fontes/MULTIPLAYER_RULES.md` e `Fontes/SECURITY_MODEL.md`.
- Para PlayerController, trate overflow de replicacao como risco ativo do projeto.

## Contrato obrigatorio de RPC
Todo RPC deve declarar:
- origem permitida;
- owner esperado;
- payload maximo;
- limites por campo;
- validacao de contexto;
- validacao de ownership;
- rate-limit/anti-spam;
- comportamento em rejeicao;
- categoria de log;
- por que precisa existir se afetar gameplay.

## Checklist de implementacao
- RPC client->server carrega apenas intencao minima.
- Servidor resolve resultado final.
- Servidor valida actor, owner, pawn, PlayerState, distancia, LOS, tags, cooldown/custo quando aplicavel.
- Servidor rejeita payload invalido sem mutar estado.
- Rejeicao registra log categorizado com motivo.
- Nenhum valor final confiavel vem do cliente.
- RPC reliable so e usado quando perda de pacote quebraria contrato funcional.
- Fluxo com spam potencial tem rate-limit por jogador/instancia.

## PlayerController
PlayerController e somente input, UX local e ponte de RPC validada.

Proibido:
- estado autoritativo de gameplay no PlayerController;
- payload cosmetico replicado pelo PlayerController;
- decidir dano, custo, cooldown, morte, loot, persistencia ou resultado final;
- depender de HUD/widget/camera em fluxo usado por Dedicated Server.

Qualquer replicacao nova no PlayerController exige:
- justificativa de gameplay;
- alternativa considerada;
- condicao de replica;
- budget de rede;
- risco de `FBitReader::SetOverflowed`.

## Saida esperada
Para cada RPC, responda:
1. RPC
2. Origem permitida
3. Owner esperado
4. Payload e limites
5. Validacoes server-side
6. Rate-limit
7. Rejeicao/log
8. Risco de overflow/OnRep
9. Checklist DS+2 normal e lag/loss

