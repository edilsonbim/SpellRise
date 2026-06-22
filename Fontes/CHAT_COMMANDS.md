# Chat e Comandos Runtime

## Objetivo

Inventariar os comandos existentes, separar permissões de player e admin e registrar impacto de CPU, rede, replicação, persistência e segurança.

Este documento não contém senha, segredo ou valor de autenticação administrativa.

Para o mapa detalhado dos Blueprints, funções, variáveis, ordem de eventos e inspeção sem screenshots, consulte `CHAT_BLUEPRINT_MAP.md`.

## Fluxo do chat

- O fluxo de produção usa `ASpellRisePlayerController::SubmitChatMessageForConversation` como entrada explícita da UI (`Text + Channel + ConversationId`); o próprio `PlayerController` roteia `Global` para `ServerSubmitChatMessage` e `Whisper` para `SendWhisperToConversation`.
- `SubmitChatMessage` permanece como adaptador compatível e usa a conversa ativa local, mas widgets novos devem passar o `ConversationId` explicitamente para evitar divergência entre estado da aba e estado do controller.
- O RPC Blueprint legado `SendChatToSERVER` existe somente como adaptador temporário e é consumido antes de executar o Blueprint.
- Nome e texto são sanitizados e limitados no servidor.
- Nome: máximo de 32 caracteres.
- Texto: máximo de 256 caracteres.
- Horário: máximo de 16 caracteres.
- Mensagens públicas são roteadas pelo `USpellRiseChatComponent` do `GameState` e entregues por RPC client `Unreliable` individual.
- Mensagens de sistema, combat log e respostas administrativas são enviadas somente ao controller alvo.
- Mensagens públicas exibem o nome no formato `Player [Level]`.
- Canal Combat é somente leitura para o cliente.

## Comandos de player

| Comando | Função | Authority | Persistência | Impacto |
|---|---|---|---|---|
| `/name NovoNome` | Altera o nome do próprio personagem | Servidor | Salva personagem imediatamente | Baixo; validação linear do remetente e uma operação de save |
| `/online` | Lista os nomes dos jogadores conectados | Servidor | Não | Baixo; percorre controllers e responde somente ao solicitante |
| `/help` | Lista comandos disponíveis para a sessão | Servidor | Não | Baixo; resposta privada sem busca externa |
| `/clear` | Limpa somente o histórico local da aba ativa | Cliente local | Não | Sem RPC, replicação ou custo no servidor |
| `/invite Player` | Adiciona o player online à Party do remetente e habilita canal/marcador compartilhado | Servidor | Não | Baixo; busca linear por nome e duas alterações pontuais de PlayerState |
| `/remove Player` | Leader remove um membro da Party | Servidor | Não | Baixo; busca linear e atualização pontual |
| `/leader Player` | Transfere liderança para outro membro | Servidor | Não | Baixo; atualiza leader nos PlayerStates da Party |
| `/leave` | Sai da Party; se for leader, transfere para um membro restante | Servidor | Não | Baixo; dissolve Party com somente um membro |
| `/party` | Lista membros online e identifica o leader | Servidor | Não | Baixo; busca linear e resposta privada |
| `/w Player Mensagem` | Envia whisper privado ao player | Servidor | Não | Baixo; duas entregas client-only |
| `/whisper Player Mensagem` | Alias completo de `/w` | Servidor | Não | Baixo; duas entregas client-only |
| `/r Mensagem` | Responde à última conversa de whisper | Servidor | Não | Baixo; resolve o último ID de conversa |

### Regras de `/name`

- Permitido somente no canal Global.
- Nome deve ter entre 3 e 24 caracteres.
- Aceita letras, números, espaço, `_` e `-`.
- Não permite alteração para o mesmo nome.
- A resposta de sucesso ou rejeição é privada.

## Comandos de admin

Todos os comandos abaixo:

- exigem sessão admin autenticada;
- são interceptados no `PlayerController` proprietário antes do broadcast;
- executam exclusivamente no servidor;
- não aparecem no chat público;
- compartilham rate-limit de um comando a cada `0,20s`, máximo prático de 5 comandos por segundo;
- recebem resposta privada por `ClientReceiveChatMessage`.

| Comando | Função | Estado afetado | Persistência | Impacto |
|---|---|---|---|---|
| `/admin <senha>` | Abre sessão administrativa transitória | `PlayerController` server-side | Não | Baixo; comparação de string |
| `/set level <1-999> Player` | Define level e XP cumulativa; concede somente rewards nunca concedidos | Progressão no `PlayerState` | Marca personagem dirty | Baixo/médio; itera apenas pelos levels ainda não recompensados |
| `/set resetpoints Player` | Devolve pontos de atributos e talentos, reseta primários e talentos | ASC, progressão e Talent Tree | Marca personagem dirty | Médio e pontual; reflexão no componente de talentos e atualização owner-only |
| `/goto Player` | Teleporta o pawn controlado pelo admin até o player | Pawn do admin | Não | Baixo; uma busca de controller e um teleporte |
| `/bringto Player` | Teleporta o player para 150 cm à frente do personagem do admin | Pawn alvo | Não | Baixo; uma busca de controller e um teleporte |
| `/give ip Player` | Mostra o endereço de rede do player somente ao admin | Nenhum gameplay state | Não | Baixo; leitura local da conexão e resposta owner-only |
| `/set invisible` | Oculta o personagem e ignora colisões de gameplay, mantendo `WorldStatic` e `WorldDynamic` | Componentes primitivos do pawn | Não | Baixo e pontual; percorre componentes do pawn uma vez |
| `/set visible` | Restaura visual e respostas de colisão anteriores | Componentes primitivos do pawn | Não | Baixo e pontual |
| `/set fly` | Possui um `SpectatorPawn` sem colisão para câmera livre | Pawn/controller do admin | Não | Baixo; cria um ator temporário replicado/relevante conforme regras padrão |
| `/set walk` | Devolve posse ao personagem original e destrói o pawn de câmera | Pawn/controller do admin | Não | Baixo |
| `/kick Player Motivo` | Expulsa o player da sessão atual | Conexão do player | Não | Baixo e pontual |
| `/ban Player Tempo Motivo` | Persiste ban por SteamID64 e expulsa o player | PostgreSQL e conexão | Sim | Médio e pontual por executar uma gravação SQL |
| `/revive Player` | Revive um player em `State.Downed` usando o fluxo existente | Character/ASC | Não | Baixo |
| `/heal Player` | Restaura Health, Mana e Stamina ao máximo | ASC do `PlayerState` | Não | Baixo; atualizações normais de atributos |
| `/unstuck Player` | Move o player para o respawn seguro calculado pelo servidor | Pawn alvo | Não | Baixo; validação de transform e teleporte |

## Authority e segurança

- O cliente envia somente a linha de chat, limitada a 256 caracteres.
- O servidor localiza players pelo `PlayerState::PlayerName`; nomes devem ser únicos para operação administrativa determinística.
- Level, XP, pontos e atributos são mutados no `PlayerState`/ASC.
- Teleportes são executados pelo servidor.
- IP não é enviado por multicast e não deve entrar em logs de sucesso.
- A sessão admin é transitória e não é uma propriedade replicada.
- A autenticação atual por senha apenas é temporária para testes. Produção deve voltar a exigir identidade autorizada server-side além do segredo.

## Budget de rede

### Chat público

- Uma entrada reliable cliente-servidor por mensagem aceita.
- Uma saída client `Unreliable` para cada conexão destinatária da mensagem pública.
- Payload lógico máximo: aproximadamente 304 caracteres antes do overhead de `FText`, RPC e serialização.
- Custo de distribuição cresce com o número de conexões relevantes: `O(players)` por mensagem.
- Não cria propriedades replicadas nem `OnRep`.
- Não usa Tick.

### Comandos administrativos

- Reutilizam `ServerSubmitChatMessage`; não criam um RPC por comando.
- Respostas usam uma chamada client reliable somente para o admin.
- Busca por nome percorre controllers conectados: `O(players)` somente quando um comando é executado.
- `/set level` pode executar até 998 iterações no pior caso absoluto, uma única vez; custo aceitável para operação administrativa rara.
- `/set resetpoints` percorre arrays/componentes do personagem alvo e força updates owner-only pontuais.
- `/set fly` adiciona apenas um pawn temporário por admin em modo fly.
- `/online` é `O(players)` e owner-only; não gera multicast.
- `/help` mostra comandos administrativos somente quando o controller está autenticado como admin.
- Whisper usa canal `4`, nunca multicast, e entrega somente para remetente e destinatário via client reliable.
- Whisper aceita no máximo 4 mensagens a cada 2 segundos por remetente.
- Cada mensagem carrega `ConversationId`, `ConversationName` e `bOutgoing` para a UI criar abas locais.
- Block list de whisper é validada no servidor e não replica payload para terceiros.
- Unread e histórico ficam apenas no cliente; não criam propriedade replicada nem `OnRep`.
- Horários de todas as abas usam somente `HH:mm`; segundos não são transportados nem exibidos.
- Duplo clique no nome resolve localmente o PlayerState e abre uma aba whisper; não envia RPC até existir mensagem.
- `/clear` é interceptado antes de qualquer RPC e remove apenas mensagens cujo `TabId` corresponde à aba ativa.
- Party usa `PartyId` e `PartyLeaderId` no `PlayerState`, replicados para todos, e roteia mensagens somente aos membros com o mesmo ID.
- `/invite` cria convite pendente por 30 segundos. O alvo responde `Y` ou `N`; somente `Y` muta a Party.
- Player que já está em Party não pode receber novo convite. Somente o leader pode convidar, remover ou transferir liderança.
- Logout remove o player da Party antes de destruir o PlayerState; se era leader, transfere liderança para um membro restante.
- Mudança de Party reconcilia todos os NavigationMarkers locais para tolerar ordem variável entre replicação de PlayerState e Pawn.
- O marker do Character permanece owner-only por padrão, mas também é registrado localmente para PlayerStates com o mesmo `PartyId`.
- Unread é indexado por `TabId`: `GLOBAL`, `PARTY`, `GUILD`, `COMBAT` ou `ConversationId` no whisper. Toda mensagem recebida em uma aba não ativa incrementa o contador, inclusive mensagem própria devolvida pelo servidor.
- Whisper não pode ser enviado sem `ConversationId`; o `PlayerController` rejeita localmente esse contexto incompleto antes de criar RPC.
- `/ban` adiciona uma operação PostgreSQL síncrona e deve permanecer rara.
- Não há target data, multicast administrativo, payload replicado novo ou `OnRep` administrativo.

## Riscos encontrados

### P0 — autenticação temporária

A autenticação somente por senha não é adequada para produção. Qualquer player que obtenha o segredo pode executar comandos autoritativos. Antes de release, exigir simultaneamente:

- SteamID64 autorizado no servidor;
- segredo configurado fora do código;
- auditoria de login e comandos;
- bloqueio progressivo após tentativas inválidas.

### P1 — adaptador Blueprint legado

O transporte autoritativo já deriva identidade do owner do `PlayerController`. O `W_Chat` deve usar `SubmitChatMessageForConversation`; o RPC Blueprint legado `SendChatToSERVER` e o adaptador de `ProcessEvent` devem ser removidos após confirmar que nenhum asset ainda os referencia.

### P1 — nomes duplicados

Comandos que recebem `Player` selecionam o primeiro nome equivalente. Produção deve exigir nome único ou aceitar SteamID64/ID interno como identificador administrativo.

### P2 — privacidade do IP

`/give ip` deve permanecer:

- restrito a admin;
- com resposta privada;
- ausente de multicast;
- ausente de logs comuns;
- sujeito à política operacional e de privacidade.

### P2 — fly administrativo

O pawn original permanece no mundo enquanto o admin usa a câmera livre. Definir regra operacional para dano, morte, relevância e timeout da sessão fly. O comando não deve ser usado como mecanismo normal de gameplay.

## Impacto esperado

### Standalone

- Impacto desprezível.
- Comandos administrativos continuam resolvidos server-side localmente.

### Listen Server

- Baixo em uso normal.
- Host e clientes devem receber chat público uma vez.
- Verificar posse e restauração do pawn no fly/walk.

### Dedicated Server

- Baixo para comandos administrativos esporádicos.
- Chat público possui rate-limit inicial de 4 mensagens por 2 segundos por player.
- Com 100 players, uma mensagem pública gera distribuição para as conexões relevantes; spam contínuo não é aceitável.

### Lag/loss

- Chat público `Unreliable` pode perder mensagens, comportamento aceitável para apresentação.
- Respostas administrativas são reliable e podem chegar atrasadas, mas não devem duplicar mutações.
- Teleporte e posse podem reconciliar visualmente com atraso; a decisão permanece no servidor.

## Checklist de validação

1. Player usa `/name` válido e inválido.
2. Chat mostra `Nome [Level]: mensagem`.
3. Canal Combat rejeita envio.
4. Comando admin não aparece para outros players.
5. Admin inválido não executa mutação.
6. `/set level` concede rewards uma única vez, inclusive após reduzir e subir novamente.
7. `/set resetpoints` persiste após reconexão.
8. Invisible mantém colisão com terreno e restaura respostas ao voltar visible.
9. Fly cria uma câmera livre; walk restaura o pawn e remove o pawn temporário.
10. `/goto` e `/bringto` reconciliam em DS+2.
11. `/give ip` aparece apenas ao admin.
12. Rodar DS+2 normal e perfis `120ms/1%` e `180ms/3%`.
13. Confirmar ausência de `FBitReader::SetOverflowed`.
14. Executar teste de spam após implementar rate-limit dedicado do chat público.
15. Confirmar kick sem save perdido.
16. Confirmar ban temporário e permanente bloqueando novo `PreLogin`.
17. Confirmar revive somente em `State.Downed`.
18. Confirmar heal replicando Health/Mana/Stamina corretamente.
19. Confirmar unstuck em spawn seguro.
20. Confirmar `/online` privado e correto em DS+2.
21. Confirmar `/help` sem comandos admin para player comum.
22. Confirmar `/help` completo após autenticação admin.
