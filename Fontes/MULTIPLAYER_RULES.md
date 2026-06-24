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
| Persistencia de hotbar/equipamento | save/load server-side | servidor valida schema, GUID, definition, slot e ownership | Servidor | FastArrays privados já existentes | nao | restore fora de ordem ou migração legacy ambígua |
| Progressao personagem/XP | servidor/persistencia/reward de inimigo | servidor valida fonte de XP e tabela de level | Servidor | `PlayerState` owner-only, RepNotify apenas quando muda | nao | cliente tentar forcar XP/level ou UI stale |
| Gasto de Attribute Points | UI local -> `PlayerState::ServerSpendAttributePoints` com enum + quantidade | owner, perfil aplicado, atributo canonico, quantidade `1..10`, saldo, cap `100`, max `5 RPC/s` | Servidor via ASC + saldo no componente de progressao | atributos GAS e saldo existentes, ambos `OwnerOnly` | nao | spam reliable ou UI observar saldo/atributo em frames distintos |
| Compra/ativacao de booster | UI -> `ServerPurchaseCombatBooster(enum)` ou `ServerSetCombatBoosterActive(enum,level,bool)` | owner, perfil, enum, level `1..4`, ordem sequencial, saldo/custo, posse e limite de 4 ativos; max `5 RPC/s` | Servidor debita compra ou altera loadout ativo | 8 `uint8` `OwnerOnly` | nao | spam reliable ou UI observar saldo/loadout em frames distintos |
| Progressao arma/escola | servidor/persistencia | servidor valida tag e nivel `1..100` | Servidor | `PlayerState` owner-only | nao | cliente tentar forcar nivel ou UI stale |
| Target Data (spell/projétil) | trace local como intenção | payload/alcance/LOS/ownership | Servidor | resultado autoritativo | Sim (pré-visual) | cliente tentar forçar hit inválido |
| Projétil | após validação do cast | spawn + trajetória inicial | Servidor | actor replicado + hit no servidor | opcional visual local | hit fantasma por ordem de eventos |
| Atributos/Recursos | n/a | ExecCalc/MMC/GE no servidor | Servidor | `Health/MaxHealth` público; `Mana/Stamina/Regen/CarryWeight` `OwnerOnly` | não | trust indevido no cliente |
| Cura GAS | input local/evento autorizado | servidor valida alvo, tags, custo, cooldown, contexto e ownership | Servidor | `Health/MaxHealth` público; cues/feedback conforme GAS | Sim, apenas UX | cliente tentar forçar valor/alvo de cura ou desync visual com RTT alto |
| Lifesteal | GE duration/buff no atacante | servidor calcula após dano real aplicado | Servidor | `LifestealPercent` owner-only; cura via `Health` público | só UX/tag de estado | dupla escala, lifesteal recursivo ou cura baseada em dano mitigado incorreto |
| Morte/Downed/Loot/Respawn | `USpellRiseLifeStateComponent` + widget contextual após trace local | servidor valida owner, estado replicado, tags, interactor, distância e LOS | Servidor | `LifeState` público no componente, tags GAS, recursos e atores de loot | trace/UI local | alvo stale entre abertura do widget e RPC |
| Inventário próprio | UI/ViewModel -> `USpellRiseInventoryComponent` no `PlayerState` | GUID, owner, slot, quantidade `1..1000`, peso/capacidade, request id e rate-limit | Servidor | FastArray `OwnerOnly` | só UX | reliable spam, delta stale ou perda/duplicação transacional |
| Equipamento próprio | UI/ViewModel -> `USpellRiseEquipmentComponent` no `PlayerState` | GUID, slot, ownership, conflito 2H/OffHand, grants e rate-limit | Servidor | privado `OwnerOnly`; visual público pendente no Character | só apresentação | inventário/equipamento chegarem fora de ordem |
| Loot/containers Narrative | UI local -> Narrative no `PlayerState`/ator de loot | servidor valida owner, source, quantidade e rate-limit | Servidor | `LootSource` `OwnerOnly`; contrato legado Narrative | só UX | source stale durante respawn; migração ainda incompleta |
| Building Mode | input local | contexto/material/range/LOS/RPC rate | Servidor | estado mínimo necessário | opcional ghost local | abuso de RPC/payload |
| PlayerController runtime | input/UI local | apenas RPCs permitidos | Servidor | somente dados essenciais | não | overflow de replicação |
| Chat/Whisper | UI local -> `ASpellRisePlayerController::SubmitChatMessageForConversation(text,channel,conversationId)` | owner, tamanho `<=256`, canal, identidade/conversation id, rate-limit, block list | Servidor roteia | Global por client unreliable; whisper/system por client reliable | nao | spam reliable, fan-out global e adaptador BP legado |

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
- Persistência de hotbar/equipamento não adiciona RPC novo; restore usa chamadas server-side e payload de snapshot validado.
- Progressão/XP não salva no storage a cada kill; rewards marcam o personagem como dirty e o snapshot periódico salva apenas players dirty. Level-up pode forçar net update owner-only, XP comum usa o update normal do `PlayerState`.
- Gasto de Attribute Points usa no maximo `5 RPC/s` por jogador, payload aproximado de `5 bytes` antes de overhead (`uint8` + `int32`), nao cria propriedade replicada e marca o snapshot como dirty para o autosave server-side.
- Compra usa payload aproximado de `1 byte`; ativacao usa aproximadamente `2 bytes` (`enum + bool`). Ambos compartilham maximo `5 RPC/s`. O componente replica oito contadores `uint8` owner-only.
- Inventário próprio: FastArray `OwnerOnly`, sem Tick; entrada replica apenas GUID, `PrimaryAssetId`, slot, quantidade, durabilidade, flags e revisão.
- Equipamento próprio: estado privado `OwnerOnly`; replicação visual para observadores deve ser implementada no Character relevante antes de habilitar a feature.
- Move/split/merge compartilham limite de `6 RPCs/0,25s`; drop usa `2 RPCs/0,25s`; quantidade máxima por request é `1000`. Equip/unequip compartilham `6 RPCs/0,25s`.
- RPCs recebem apenas intenção mínima (`ItemInstanceId`, destino/slot, quantidade e request id). Origem, definition, peso e resultado são derivados no servidor.
- Narrative permanece em paralelo para loot/containers/full loot e assets antigos até a migração desses fluxos.
- RPC crítico de gameplay: máximo recomendado `<= 20/s` por jogador por fluxo.
- Inventário/loot/use/store: máximo inicial de 6 RPCs por 0,25s por componente e quantity `1..1000`; rejeições devem usar log categorizado.
- Chat público entra por um RPC reliable owner-bound e sai por client RPC unreliable individual; limite inicial de 4 mensagens por 2 segundos por player.
- Whisper entra pela API explícita do `PlayerController`, com `ConversationId` da aba ativa, e cai em `ServerSendWhisperToConversation`; sai reliable somente para dois participantes e não replica histórico.
- Death/corpse/equipment visual multicast é apresentação e não deve usar reliable; autoridade vem de GE/tag/estado replicado pelo servidor.
- Revive/Gank usa `ServerResolveDownedAction(TargetCharacter,bRevive)`: origem owner do Character, payload aproximado actor NetGUID + 1 bit, máximo funcional de uma escolha por abertura do widget; servidor rejeita alvo stale, distância ou LOS inválidos.
- `State.Downed` não altera movimento/input; CharacterMovement replica a locomoção normal e qualquer bloqueio/slow deve vir de outro sistema explícito, não do estado downed.
- `LifeState` replica `COND_None` no componente do Character; `OnRep_LifeState` emite somente apresentação. Timers de downed, cooldown e recovery existem apenas no servidor.
- `State.Downed.Cooldown` e `State.ReviveRecovery` são tags temporizadas pelo servidor no ASC. `bIsDowned` replica publicamente apenas como espelho de interação/apresentação.
- Eventos de gameplay server-side: manter rate-limit por tag.
- Cura usa o mesmo contrato de atributos/recursos: valor final server-side, sem RPC de valor curado vindo do cliente.
- Lifesteal não usa RPC novo; o buff altera `LifestealPercent` por GE e o pós-dano aplica cura server-side.
- Mudança de feature só entra se declarar impacto de tráfego esperado.

## Observabilidade mínima obrigatória
- Métricas/logs por sessão:
  - ativações de ability: tentativas, sucesso, falha e `FailureTags`;
  - RPC rejeitado por motivo;
  - cura rejeitada por alvo inválido, custo/cooldown, tag bloqueante ou contexto inválido;
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
- inventário/equipamento: sem duplicação/perda, GUID preservado após reconexão, bloqueio 2H correto e sem grants duplicados após respawn;
- validar DS+2 normal, `120/1` e `180/3`, incluindo spam reliable, late join e ordem diferente entre FastArrays.
