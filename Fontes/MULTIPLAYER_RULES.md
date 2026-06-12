# Multiplayer Rules — SpellRise

## Objetivo
Definir contrato único de authority, prediction, RPC, replicação e critérios de aceitação para multiplayer de produção.

## Princípios obrigatórios
- Fonte de verdade de gameplay: `ASC + GameplayEffects + GameplayTags` no servidor.
- Cliente só prevê para UX e nunca decide resultado final de gameplay.
- Multicast é só apresentação.
- Toda mutação autoritativa exige validação de ownership, contexto e anti-spam.
- ASC no `PlayerState` em `Mixed`.
- Qualquer estado duplicado fora do ASC é apenas espelho de apresentação, nunca autoridade.

## Matriz de replicação por sistema

| Sistema | Início | Validação Server | Commit | Replicação | Client Prediction | Risco principal |
|---|---|---|---|---|---|---|
| GA Instant/Cast/Channel | Input local -> `SR_ProcessAbilityInput` | `CanActivate/Commit` + tags/cost/cooldown | Servidor | estado de ability + GE/cues | Sim (UX) | desync de confirmação com RTT alto |
| Ability Hotbar 8+8 | input local -> slot lógico -> `InputTag` | servidor valida alteração de slot, grupo Weapon/Common e rate-limit | GAS ativa ability já concedida | slots `OwnerOnly` no `PlayerState` | só UX/seleção | slot stale após troca de arma ou OnRep fora de ordem |
| Target Data (spell/projétil) | trace local como intenção | payload/alcance/LOS/ownership | Servidor | resultado autoritativo | Sim (pré-visual) | cliente tentar forçar hit inválido |
| Projétil | após validação do cast | spawn + trajetória inicial | Servidor | actor replicado + hit no servidor | opcional visual local | hit fantasma por ordem de eventos |
| Atributos/Recursos | n/a | ExecCalc/MMC/GE no servidor | Servidor | `Health/MaxHealth` público; `Mana/Stamina/Regen/CarryWeight` `OwnerOnly` | não | trust indevido no cliente |
| Morte/Loot/Respawn | dano autoritativo | estado de morte + regras de loot | Servidor | tags/estado/atores de loot | só UI | corrida de OnRep e apresentação |
| Building Mode | input local | contexto/material/range/LOS/RPC rate | Servidor | estado mínimo necessário | opcional ghost local | abuso de RPC/payload |
| PlayerController runtime | input/UI local | apenas RPCs permitidos | Servidor | somente dados essenciais | não | overflow de replicação |

## Contrato de RPC
- Todo RPC deve declarar:
  - origem permitida;
  - payload com limites explícitos;
  - validação de contexto/ownership;
  - janela de rate-limit;
  - comportamento de rejeição com log categorizado.
- Proibido RPC autoritativo sem validação de contexto.

## Contrato de OnRep
- `OnRep` só para reconciliação e apresentação.
- `OnRep` não decide dano, custo, cooldown, morte, loot ou hit final.
- Cada variável replicada nova deve registrar:
  - condição (`OwnerOnly`, `SkipOwner`, `None`);
  - motivo da replicação;
  - impacto de ordem/race com outros `OnRep`.

## Budget de rede (baseline inicial)
- `PlayerController`: zero payload cosmético replicado.
- Hotbar de abilities: `PlayerState` owner-only, 16 slots; RPC de edição envia `SlotIndex + InputTag + AbilityClass`, rate-limitado no servidor.
- RPC crítico de gameplay: máximo recomendado `<= 20/s` por jogador por fluxo.
- Inventário/loot/use/store: máximo inicial de 6 RPCs por 0,25s por componente e quantity `1..1000`; rejeições devem usar log categorizado.
- Chat público é apresentação e não deve usar reliable.
- Death/corpse/equipment visual multicast é apresentação e não deve usar reliable; autoridade vem de GE/tag/estado replicado pelo servidor.
- Eventos de gameplay server-side: manter rate-limit por tag.
- Mudança de feature só entra se declarar impacto de tráfego esperado.

## Observabilidade mínima obrigatória
- Métricas/logs por sessão:
  - ativações de ability: tentativas, sucesso, falha e `FailureTags`;
  - RPC rejeitado por motivo;
  - contagem de `OnRep` crítico por minuto;
  - erros de overflow (`FBitReader::SetOverflowed`);
  - latência de confirmação cliente->servidor para cast.

## Gate de aceitação multiplayer
Executar obrigatoriamente:
1. Standalone
2. Listen Server
3. Dedicated Server + 2 clientes
4. Dedicated Server + 2 clientes com lag/loss

Perfis mínimos de rede:
- Perfil A: `PktLag=120`, `PktLoss=1`
- Perfil B: `PktLag=180`, `PktLoss=3`

Critérios de aprovação:
- sem overflow de replicação;
- sem trust indevido no cliente;
- sem regressão de authority do servidor;
- taxa de falha de ativação de GA dentro do esperado funcional do cenário;
- sem regressão funcional em morte/loot/respawn e projétil autoritativo.
