# Chat e Comandos Runtime

## Objetivo

Inventariar os comandos existentes, separar permissões de player e admin e registrar impacto de CPU, rede, replicação, persistência e segurança.

Este documento não contém senha, segredo ou valor de autenticação administrativa.

## Fluxo do chat

- O cliente envia texto pelo RPC legado `SendChatToSERVER`.
- Nome e texto são sanitizados e limitados no servidor.
- Nome: máximo de 32 caracteres.
- Texto: máximo de 256 caracteres.
- Horário: máximo de 16 caracteres.
- Mensagens públicas são distribuídas por multicast `Unreliable`.
- Mensagens de sistema, combat log e respostas administrativas são enviadas somente ao controller alvo.
- Mensagens públicas exibem o nome no formato `Player [Level]`.
- Canal Combat é somente leitura para o cliente.

## Comandos de player

| Comando | Função | Authority | Persistência | Impacto |
|---|---|---|---|---|
| `/name NovoNome` | Altera o nome do próprio personagem | Servidor | Salva personagem imediatamente | Baixo; validação linear do remetente e uma operação de save |
| `/online` | Lista os nomes dos jogadores conectados | Servidor | Não | Baixo; percorre controllers e responde somente ao solicitante |
| `/help` | Lista comandos disponíveis para a sessão | Servidor | Não | Baixo; resposta privada sem busca externa |

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

- Uma entrada cliente-servidor por mensagem.
- Uma saída multicast `Unreliable` para cada mensagem pública.
- Payload lógico máximo: aproximadamente 304 caracteres antes do overhead de `FText`, RPC e serialização.
- Custo de distribuição cresce com o número de conexões relevantes: `O(players)` por mensagem.
- Não cria propriedades replicadas nem `OnRep`.
- Não usa Tick.

### Comandos administrativos

- Reutilizam o RPC de chat existente; não adicionam RPC.
- Respostas usam uma chamada client reliable somente para o admin.
- Busca por nome percorre controllers conectados: `O(players)` somente quando um comando é executado.
- `/set level` pode executar até 998 iterações no pior caso absoluto, uma única vez; custo aceitável para operação administrativa rara.
- `/set resetpoints` percorre arrays/componentes do personagem alvo e força updates owner-only pontuais.
- `/set fly` adiciona apenas um pawn temporário por admin em modo fly.
- `/online` é `O(players)` e owner-only; não gera multicast.
- `/help` mostra comandos administrativos somente quando o controller está autenticado como admin.
- `/ban` adiciona uma operação PostgreSQL síncrona e deve permanecer rara.
- Não há target data, multicast administrativo, payload replicado novo ou `OnRep` administrativo.

## Riscos encontrados

### P0 — autenticação temporária

A autenticação somente por senha não é adequada para produção. Qualquer player que obtenha o segredo pode executar comandos autoritativos. Antes de release, exigir simultaneamente:

- SteamID64 autorizado no servidor;
- segredo configurado fora do código;
- auditoria de login e comandos;
- bloqueio progressivo após tentativas inválidas.

### P1 — chat público sem rate-limit próprio

O chat público não possui rate-limit server-side dedicado. Um cliente pode enviar mensagens repetidamente e causar:

- custo de multicast proporcional ao número de players;
- spam de UI;
- aumento de tráfego e processamento de `FText`;
- risco de burst em servidores com muitos jogadores.

Gate recomendado antes de produção:

- máximo inicial de 2 mensagens por segundo por player;
- burst máximo de 4;
- cooldown progressivo para abuso;
- rejeição categorizada sem eco público;
- métrica de mensagens aceitas/rejeitadas por minuto.

### P1 — identidade do remetente no fluxo legado

O componente de chat ainda resolve o remetente público pelo nome declarado recebido do fluxo Blueprint legado. Comandos admin já usam o `PlayerController` real, mas o chat público deve migrar para identidade derivada exclusivamente do owner da conexão.

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
- Chat público é o principal risco de escala por não ter rate-limit dedicado.
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
