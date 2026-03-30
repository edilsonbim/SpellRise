# RPC Audit - ASpellRisePlayerController

Data: 2026-03-30  
Arquivos auditados: `Source/SpellRise/Core/SpellRisePlayerController.h` e `.cpp`

## Escopo
Auditoria dos `UFUNCTION(Client, ...)` e `UFUNCTION(Server, ...)` em `ASpellRisePlayerController`, com foco em contrato de rede, validacoes de contexto e risco para Dedicated Server.

## Lista de RPCs

### 1) `ClientReceiveCombatLogEntry(float Damage, const FString& OtherActorName, bool bIsOutgoing)`
- Tipo: `Client, Reliable`
- Origem permitida: servidor para owner do PlayerController.
- Payload: `float + FString + bool`.
- Validacoes observadas:
  - uso apenas para feedback local (chat/HUD);
  - sem mutacao autoritativa de gameplay.
- Risco atual:
  - `FString` sem limite explicito.
- Recomendacao:
  - truncar `OtherActorName` no servidor (ex.: 64 chars).

### 2) `ClientReceiveChatMessage(const FSpellRiseChatMessage& Message)`
- Tipo: `Client, Reliable`
- Origem permitida: servidor para owner.
- Payload: `FSpellRiseChatMessage`.
- Validacoes observadas:
  - append local de chat;
  - sem efeito autoritativo de combate.
- Risco atual:
  - struct pode crescer e ampliar custo de serializacao.
- Recomendacao:
  - limite de tamanho de mensagem no servidor (ex.: 256 chars).

### 3) `ServerSR_ForceDeath()`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Payload: sem payload.
- Validacoes observadas:
  - bloqueio em shipping;
  - valida authority em `ExecuteForceDeath_Server`;
  - valida pawn e ASC.
- Risco atual:
  - RPC de debug indevida fora do contexto de QA.
- Recomendacao:
  - manter guardas atuais e opcionalmente condicionar por cvar de debug.

### 4) `InventorySplitSlotSERVER(UObject* FromContainer, int32 Slot, int32 Amount)`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Estado atual: stub com warning de "sem implementacao ativa".
- Risco quando implementar:
  - ownership/range/spam sem validacao.
- Recomendacao:
  - validar ownership do container, faixa de slot e quantidade > 0.

### 5) `OnInventorySlotDropSERVER(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex)`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Estado atual: stub com warning.
- Risco quando implementar:
  - acesso cruzado de inventario e indices invalidos.
- Recomendacao:
  - validar ownership, indices e regras de stack/tipo.

### 6) `Route_InventorySortBy_SERVER(UObject* InventoryRef, int32 SortBy)`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Estado atual: stub com warning.
- Risco quando implementar:
  - spam de RPC e acesso indevido a inventario de terceiros.
- Recomendacao:
  - validar ownership e faixa de `SortBy`; aplicar rate limit.

## RPC/UI em Dedicated Server
- Nao ha RPC de UI que deva executar no DS.
- O fluxo local do controller esta guardado por:
  - `CanRunLocalHUDFlow`
  - `CanRunLocalPawnRuntime`
- Esses gates bloqueiam execucao quando:
  - `NM_DedicatedServer`
  - controller nao local
  - sem viewport/local player
  - sem `Pawn + PlayerState + ASC`

## Checklist curto de validacao

### Standalone
- Entrar no mapa e confirmar sem overflow/ensure no controller.

### Listen Server
- Host + 1 cliente.
- Confirmar entrega de Client RPC somente para owner correto.

### Dedicated Server
- 2+ clientes.
- Confirmar que HUD/camera nao executa no DS.
- Confirmar ausencia de `PreLogin` indevido conforme modo (Steam/no-Steam).

### Lag/Loss
- Rodar com `Net PktLag` e `Net PktLoss`.
- Confirmar ausencia de novos overflows de payload no PlayerController.
