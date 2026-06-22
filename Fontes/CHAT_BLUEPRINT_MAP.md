# Mapa Blueprint do Chat

## Objetivo

Este documento é o ponto de entrada para manutenção do chat em Blueprint.

Antes de pedir screenshots, inspecione os assets e funções listados aqui. O contrato de rede, comandos, limites e authority permanece em `CHAT_COMMANDS.md`.

## Assets principais

| Asset | Papel |
|---|---|
| `/Game/UI/Widgets/Chat/W_Chat` | Widget proprietário: input, histórico visível, criação/registro das abas, seleção, unread e rebuild |
| `/Game/UI/Widgets/Chat/WBP_ChatTab` | Uma aba individual: clique, ativação, visual ativo, contador unread e fechamento |
| `/Game/Core/GameModes/BP_SpellRisePlayerController` | Ponte dos eventos C++ para `W_Chat`; não deve decidir destinatário ou authority |
| `/Game/Core/GameModes/BP_SpellRisePlayerState` | Não participa do roteamento atual do chat |
| `/Game/UI/Widgets/Chat/Enum/E_ChatChannel` | Enum visual dos canais; deve manter os mesmos valores numéricos de `SpellRiseChatChannel` |
| `/Game/UI/Widgets/Chat/Structure/S_ChatMessage` | Struct Blueprint legado; o fluxo atual usa preferencialmente `FSpellRiseChatMessage` nativo |

## C++ relacionado

| Arquivo/símbolo | Responsabilidade |
|---|---|
| `Source/SpellRise/Core/SpellRiseChatTypes.h` | `FSpellRiseChatMessage` e canais nativos |
| `ASpellRisePlayerController::SubmitChatMessageForConversation` | Entrada explícita da UI: texto, canal e conversation ID |
| `ASpellRisePlayerController::ReceiveChatMessageLocal` | Histórico local, resolução de `TabId` e unread |
| `ASpellRisePlayerController::SetActiveChatTab` | Define a aba ativa e zera seu unread |
| `ASpellRisePlayerController::GetChatUnreadCount` | Retorna unread local por `TabId` |
| `BP_OnChatMessageReceived` | Evento de apresentação para adicionar/reconstruir mensagem |
| `BP_OnChatUnreadChanged` | Evento genérico de unread para todas as abas |
| `BP_OnChatHistoryCleared` | Solicita ao widget reconstruir a aba após `/clear` local |
| `OpenWhisperWithPlayerName` | Resolve localmente nome exibido para PlayerState/ConversationId |
| `BP_OnWhisperOpenRequested` | Solicita que o widget ative a aba whisper criada/garantida |
| `USpellRiseChatComponent` | Serviço server-side para chat público |
| `DeliverWhisper_Server` | Resolução e entrega autoritativa do whisper |

## IDs canônicos das abas

Não traduzir nem alterar estes IDs:

| Canal | `TabId` |
|---|---|
| Global | `GLOBAL` |
| Party | `PARTY` |
| Guild | `GUILD` |
| Combat | `COMBAT` |
| Whisper | `FSpellRiseChatMessage.ConversationId` |

O label exibido pode ser traduzido. O `TabId` é identidade lógica e chave de unread.

## Fluxo de envio

`W_Chat.EventGraph` trata `ChatInputBox.OnTextCommitted`.

Ordem esperada:

1. Aceitar somente commit `On Enter`.
2. Rejeitar texto vazio.
3. Obter e converter o owning player para `BP_SpellRisePlayerController`.
4. Chamar `SubmitChatMessageForConversation`:
   - `Text`: texto digitado;
   - `Channel`: `W_Chat.ActiveChannel`;
   - `ConversationId`: `W_Chat.ActiveWhisperId`.
5. Atualizar histórico local de textos digitados, se mantido.
6. Limpar o input e restaurar o modo/foco de UI.

Para canais não-whisper, `ConversationId` pode ser vazio. Para whisper, deve ser o ID da aba ativa.

## Fluxo de recebimento

1. Servidor entrega `FSpellRiseChatMessage` ao owning client.
2. `ReceiveChatMessageLocal` sanitiza e adiciona ao histórico nativo.
3. C++ resolve o `TabId`:
   - canal fixo usa ID canônico;
   - whisper usa `ConversationId`.
4. Se `TabId != ActiveChatTabId`, incrementa unread local até `999`.
5. C++ dispara:
   - `BP_OnChatMessageReceived(Message)`;
   - `BP_OnChatUnreadChanged(TabId, UnreadCount)`, quando necessário;
   - `BP_OnWhisperConversationReceived`, para garantir a existência da aba whisper.
6. O Blueprint cria/atualiza a aba e apresenta a mensagem conforme `ShouldDisplay`.

Mensagens próprias devolvidas pelo servidor também geram unread quando o canal correspondente não está ativo.

## `W_Chat`

### Variáveis relevantes

| Variável | Contrato |
|---|---|
| `ActiveChannel` | Canal da aba selecionada |
| `ActiveWhisperId` | Conversation ID apenas quando a aba ativa é whisper |
| `ActiveWhisperName` | Label da conversa ativa; somente apresentação |
| `ChatTabs` | Map `TabId -> WBP_ChatTab` |
| `OrderedTabIds` | Ordem visual/navegação das abas |
| `ActiveTabIndex` | Índice da aba ativa em `OrderedTabIds` |
| `NewChatTab` | Referência temporária durante criação |

### `EventGraph`

- `Construct`:
  - registra `ChatWidgetRef` no controller;
  - limpa maps/arrays visuais;
  - cria as abas fixas;
  - seleciona `GLOBAL`.
- `OnTextCommitted`:
  - chama `SubmitChatMessageForConversation`.

### `AppendMessage`

Responsabilidades:

1. Quebrar `FSpellRiseChatMessage`.
2. Para whisper com `ConversationId` válido, chamar `EnsureChatTab(ConversationId, ConversationName)`.
3. Avaliar `ShouldDisplay`.
4. Criar `W_ChatText`, adicionar ao `ChatScrollBox` e fazer `ScrollToEnd`.

Não deve ativar a aba criada.

### `EnsureChatTab`

Responsabilidades:

1. Se `ChatTabs` já contém `TabId`, retornar sem criar duplicata.
2. Criar `WBP_ChatTab` com:
   - `TabId`;
   - `TabLabel`;
   - `OwnerChat = self`;
   - `Channel = Whisper`;
   - `IsWhisper = true`;
   - `Closable = true`.
3. Bind de `OnCloseRequested` para `CloseWhisperTab`.
4. Adicionar ao container, `ChatTabs` e `OrderedTabIds`.
5. Consultar `GetChatUnreadCount(TabId)`.
6. Chamar `NewChatTab.SetUnreadCount`.

Proibido em `EnsureChatTab`:

- chamar `SetActiveChatTab`;
- chamar `SetActiveWhisperConversation`;
- alterar `ActiveChannel`;
- selecionar automaticamente a nova conversa.

Isso garante que a primeira mensagem recebida crie a aba já com unread.

### `CreateFixedChatTab`

Cria as abas `GLOBAL`, `PARTY`, `GUILD` e `COMBAT`.

Após criar cada aba:

1. registrar em `ChatTabs` e `OrderedTabIds`;
2. chamar `GetChatUnreadCount(TabId)`;
3. aplicar `SetUnreadCount`.

### `UpdateChatUnread`

Função Blueprint genérica recomendada:

Entrada:

- `TabId: String`;
- `UnreadCount: Integer`.

Fluxo:

1. `ChatTabs.Find(TabId)`;
2. validar a referência;
3. chamar `SetUnreadCount(UnreadCount)`.

O evento `BP_OnChatUnreadChanged` do controller deve encaminhar diretamente para essa função.

`UpdateWhisperUnread` pode permanecer apenas como wrapper legado.

### `/clear`

`BP_SpellRisePlayerController.BP_OnChatHistoryCleared` deve chamar
`ChatWidgetRef.RebuildChatFromHistory`. O histórico nativo da aba já foi removido
antes do evento; o Blueprint apenas limpa e reconstrói a apresentação.

### Duplo clique no nome

`W_ChatText` deve tratar duplo clique somente no texto do nome e chamar
`OwningPlayerController.OpenWhisperWithPlayerName(Message.Name.ToString())`.
O C++ remove sufixos de level e resolve identidade pelo `PlayerArray`.

`BP_OnWhisperOpenRequested` deve garantir a aba com `EnsureChatTab`, ativá-la e
reconstruir o histórico. Não há RPC até o usuário enviar a primeira mensagem.

### `SelectTabById`

Fluxo observado:

1. localizar `TabId` em `OrderedTabIds`;
2. se índice válido, definir `ActiveTabIndex`;
3. chamar `RefreshTabVisuals`.

A ativação completa continua em `WBP_ChatTab.ActivateTab`, acionada pelo clique/ciclo correspondente.

### `CloseWhisperTab`

Responsabilidades:

1. localizar e remover o widget;
2. remover de `ChatTabs` e `OrderedTabIds`;
3. se era a aba ativa:
   - limpar `ActiveWhisperId` e `ActiveWhisperName`;
   - selecionar uma aba fixa segura, normalmente `GLOBAL`;
   - chamar `SetActiveChatTab("GLOBAL")`;
   - reconstruir o histórico visível.

Fechar uma aba não apaga o histórico nativo. Nova mensagem pode recriá-la.

### `RebuildChatFromHistory`

Deve:

1. limpar somente os widgets de mensagem;
2. ler `GetNativeChatHistory`;
3. filtrar pela aba ativa com `ShouldDisplay`;
4. recriar mensagens sem alterar unread ou seleção.

## `WBP_ChatTab`

### Variáveis relevantes

| Variável | Contrato |
|---|---|
| `TabId` | ID canônico ou Conversation ID |
| `TabLabel` | Texto exibido |
| `OwnerChat` | Referência ao `W_Chat` proprietário |
| `Channel` | Canal visual da aba |
| `IsWhisper` | Distingue aba dinâmica |
| `Closable` | Permite fechar whisper |
| `CurrentUnreadCount` | Espelho visual local |

### `ActivateTab`

Fluxo obrigatório:

1. Validar `OwnerChat`.
2. Se whisper:
   - `OwnerChat.ActiveChannel = Whisper`;
   - `OwnerChat.ActiveWhisperId = TabId`;
   - `OwnerChat.ActiveWhisperName = TabLabel`;
   - controller `SetActiveWhisperConversation(TabId)`;
   - controller `SetActiveChatTab(TabId)`.
3. Se canal fixo:
   - `OwnerChat.ActiveChannel = Channel`;
   - limpar `ActiveWhisperId` e `ActiveWhisperName`;
   - controller `SetActiveWhisperConversation("")`;
   - controller `SetActiveChatTab(TabId)`.
4. `OwnerChat.RefreshTabVisuals(TabId)`.
5. `OwnerChat.SelectTabById(TabId)`.
6. `OwnerChat.RebuildChatFromHistory`.
7. Disparar `OnClick`, caso consumidores ainda dependam dele.

`SetActiveChatTab` zera o unread da aba selecionada e dispara `BP_OnChatUnreadChanged(TabId, 0)`.

### `SetUnreadCount`

Deve:

1. salvar `CurrentUnreadCount`;
2. mostrar badge/texto quando `UnreadCount > 0`;
3. ocultar quando `UnreadCount == 0`;
4. limitar apresentação visual, se necessário, sem alterar o valor C++ já limitado a `999`.

## `BP_SpellRisePlayerController`

Eventos esperados:

### `BP_OnChatMessageReceived`

- encaminhar `Message` para `ChatWidgetRef.AppendMessage`;
- não resolver destinatário;
- não incrementar unread manualmente.

### `BP_OnWhisperConversationReceived`

- chamar `ChatWidgetRef.EnsureChatTab(ConversationId, ConversationName)`;
- não ativar a aba.

### `BP_OnChatUnreadChanged`

- chamar `ChatWidgetRef.UpdateChatUnread(TabId, UnreadCount)`.

### Eventos legados

`BP_OnWhisperUnreadChanged`, `GetWhisperUnreadCount`, `SetActiveWhisperConversation` e `SubmitChatMessage` permanecem por compatibilidade. Novo trabalho deve priorizar as APIs genéricas.

## Ordem crítica da primeira mensagem whisper

O caso correto é:

1. C++ recebe a mensagem.
2. C++ incrementa `ChatUnreadByTabId[ConversationId]`.
3. Blueprint recebe o evento de conversa e cria a aba sem ativá-la.
4. `EnsureChatTab` chama `GetChatUnreadCount(ConversationId)`.
5. A aba nasce exibindo unread `1`.

Não dependa da ordem entre `BP_OnChatUnreadChanged` e criação visual da aba. A consulta no final de `EnsureChatTab` é a reconciliação obrigatória.

## Authority e rede

- Abas, histórico visual e unread são UX local.
- Não possuem `UPROPERTY(Replicated)`.
- Não possuem `OnRep`.
- Não criam RPC.
- O servidor continua validando texto, canal, conversation ID, destinatário, block list e rate-limit.
- Dedicated Server não acessa widgets.
- Chat público sai por client RPC `Unreliable`.
- Whisper/system sai por client RPC `Reliable` somente aos participantes necessários.

## Como inspecionar sem screenshots

Use o commandlet Editor-only `CodexBlueprintBridge`:

```powershell
C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor-Cmd.exe `
  "C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" `
  -run=CodexBlueprintBridge `
  -Input="<request.json>" `
  -Output="<result.json>" `
  -unattended -nop4 -nosplash -nullrhi
```

Exemplo de request:

```json
{
  "version": 1,
  "operation": "DescribeGraphNodes",
  "asset": "/Game/UI/Widgets/Chat/W_Chat",
  "graph": "EnsureChatTab"
}
```

Graphs prioritários:

- `W_Chat.EventGraph`;
- `W_Chat.AppendMessage`;
- `W_Chat.EnsureChatTab`;
- `W_Chat.CreateFixedChatTab`;
- `W_Chat.UpdateChatUnread`;
- `W_Chat.SelectTabById`;
- `W_Chat.CloseWhisperTab`;
- `W_Chat.RebuildChatFromHistory`;
- `WBP_ChatTab.ActivateTab`;
- `WBP_ChatTab.SetUnreadCount`;
- `BP_SpellRisePlayerController.EventGraph`.

Não execute o commandlet simultaneamente com uma sessão do editor salvando os mesmos assets.

## Checklist de regressão

1. Global ativo recebe mensagem: sem unread em Global.
2. Whisper ativo recebe Global: Global incrementa.
3. Whisper ativo envia Global por comando/fluxo permitido: retorno próprio incrementa Global.
4. Global ativo recebe primeiro whisper: aba nasce com unread `1`.
5. Whisper A ativo recebe Whisper B: B incrementa.
6. Selecionar qualquer aba zera apenas o unread dela.
7. Fechar whisper ativo seleciona fallback seguro.
8. Reabrir conversa pelo recebimento preserva histórico nativo.
9. Combat incrementa quando não está ativo.
10. Party/Guild seguem a mesma regra quando o roteamento desses canais for habilitado.
11. Confirmar ausência de replicação nova e `FBitReader::SetOverflowed`.
12. Validar DS+2 normal e perfis de lag/loss antes de fechar mudança de rede relacionada.
