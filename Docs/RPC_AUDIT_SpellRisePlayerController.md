# RPC Audit - ASpellRisePlayerController

Data: 2026-05-06
Arquivos auditados: `Source/SpellRise/Core/SpellRisePlayerController.h` e `.cpp`

## Escopo
Auditoria dos `UFUNCTION(Client, ...)` e `UFUNCTION(Server, ...)` em `ASpellRisePlayerController`, com foco em contrato de rede, validacoes de contexto e risco para Dedicated Server.

## Lista de RPCs

### 1) `ClientReceiveCombatLogEntry(float Damage, const FString& OtherActorName, bool bIsOutgoing)`
- Tipo: `Client, Reliable`
- Origem permitida: servidor para owner do PlayerController.
- Payload: `float + FString + bool`.
- Validacoes observadas:
  - uso apenas para feedback local de chat/combat log;
  - rejeita `Damage <= 0`;
  - saneia e limita `OtherActorName`;
  - sem mutacao autoritativa de gameplay.
- Risco atual:
  - baixo; payload local limitado.
- Recomendacao:
  - manter somente como feedback client; nao usar para decisao de dano.

### 2) `ClientReceiveChatMessage(const FSpellRiseChatMessage& Message)`
- Tipo: `Client, Reliable`
- Origem permitida: servidor para owner.
- Payload: `FSpellRiseChatMessage`.
- Validacoes observadas:
  - entrega local via `BP_OnChatMessageReceived`;
  - saneia nome/texto/time text antes da entrega local;
  - sem efeito autoritativo de combate.
- Risco atual:
  - medio se novas rotas voltarem a enviar texto sem limite antes do servidor.
- Recomendacao:
  - manter limite de 256 chars e evitar fallback por reflection.

### 3) `InventorySplitSlotSERVER(UObject* FromContainer, int32 Slot, int32 Amount)`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Estado atual:
  - referenciado por assets de UI (`WB_Split`, `WB_ItemSlot`);
  - stub sem mutacao de gameplay;
  - rejeita container nulo, slot negativo e amount <= 0.
- Risco atual:
  - baixo enquanto continuar sem mutacao;
  - medio se for implementado no `PlayerController`.
- Risco quando implementar:
  - ownership/range/spam sem validacao.
- Recomendacao:
  - mover contrato real para componente/subsystem de inventario com ownership/rate limit;
  - manter este RPC apenas como compatibilidade temporaria ou remover apos limpar Blueprints.

### 4) `OnInventorySlotDropSERVER(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex)`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Estado atual:
  - referenciado por `WB_ItemSlot`;
  - stub sem mutacao de gameplay;
  - rejeita container nulo e indices negativos.
- Risco atual:
  - baixo enquanto continuar sem mutacao;
  - medio se for implementado no `PlayerController`.
- Risco quando implementar:
  - acesso cruzado de inventario e indices invalidos.
- Recomendacao:
  - mover contrato real para inventario server-authoritative;
  - validar ownership, indices, stack, tipo e origem/destino.

### 5) `Route_InventorySortBy_SERVER(UObject* InventoryRef, int32 SortBy)`
- Tipo: `Server, Reliable`
- Origem permitida: owner client -> servidor.
- Estado atual:
  - referenciado por `WB_SlotsContainer`;
  - stub sem mutacao de gameplay;
  - rejeita inventory nulo e `SortBy` fora de 0..16.
- Risco atual:
  - baixo enquanto continuar sem mutacao;
  - medio se for implementado no `PlayerController`.
- Risco quando implementar:
  - spam de RPC e acesso indevido a inventario de terceiros.
- Recomendacao:
  - mover ordenacao real para contrato C++ de inventario;
  - validar ownership, faixa de `SortBy` e aplicar rate limit.

## RPC/UI em Dedicated Server
- Nao ha criacao de `CombatHUDWidget` em C++.
- Nao ha aim trace local de HUD no `PlayerController`.
- `PrimaryActorTick.bCanEverTick = false`.
- Fluxos locais restantes sao acionados por eventos:
  - setup de input;
  - posse/troca de pawn;
  - chat client;
  - combat log client;
  - cast bar Blueprint events.
- O Dedicated Server nao depende de HUD/widget/camera para fluxo autoritativo.

## Resultado do PlayerController diet
- Removido `USpellRiseCombatHUDWidget`.
- Removido `USpellRiseCombatUIComponent`.
- Removido aim trace de HUD.
- Removido fallback de chat por reflection.
- Removido tick do controller.
- Removida varredura de widgets por morte.
- Mantidos eventos Blueprint legados ainda referenciados por assets:
  - `BP_StartCastBar`
  - `BP_StopCastBar`
  - `BP_OnCombatLogMessage`
  - `BP_OnChatMessageReceived`

## Checklist curto de validacao

### Standalone
- Entrar no mapa e confirmar input, chat, cast bar e death screen.

### Listen Server
- Host + 1 cliente.
- Confirmar entrega de Client RPC somente para owner correto.
- Confirmar que UI de inventario nao espera mutacao dos stubs no `PlayerController`.

### Dedicated Server
- 2+ clientes.
- Confirmar que PlayerController nao cria HUD e nao executa tick.
- Confirmar ausencia de `PreLogin` indevido conforme modo (Steam/no-Steam).

### Lag/Loss
- Rodar com `Net PktLag` e `Net PktLoss`.
- Confirmar ausencia de novos overflows de payload no PlayerController.
