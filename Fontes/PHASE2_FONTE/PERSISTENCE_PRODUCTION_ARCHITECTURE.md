# Persistence Production Architecture

## Objetivo
Fechar a persistência de produção do SpellRise com contrato server-authoritative, consistente com Dedicated Server, Full Loot, reconnect e economia persistente.

## Escopo
Este documento define o alvo de produção para persistência. Ele complementa `PROJECT_STATE.md`, `ARCHITECTURE.md`, `SECURITY_MODEL.md`, `BACKLOG.md` e `ROADMAP.md`.

## Princípios obrigatórios
- Toda leitura e escrita persistente relevante para gameplay ocorre no servidor.
- Cliente nunca grava estado autoritativo diretamente.
- Persistência deve ser determinística sob reconnect, restart de servidor e falha parcial.
- O caminho de produção é DB-first.
- Fallback em arquivo é apenas modo degradado explícito, nunca comportamento silencioso.
- Toda mutação sensível deve ser rastreável por telemetria e motivo de falha.
- Dedicated Server não depende de UI, HUD ou fluxo local para save/load.

## Estado-alvo de produção
### Identidade
- Toda operação de persistência deve estar vinculada a identidade de sessão válida.
- Fluxos que exigem autenticação real não podem aceitar identidade placeholder/offline sem modo degradado explícito.
- O gate de auth deve acontecer antes de carregar ou confirmar estado persistente sensível.

### Modelo de ownership
- `PlayerState` continua como owner autoritativo do runtime de jogador.
- Persistência de personagem e inventário é aplicada no servidor e então refletida ao runtime.
- Cliente apenas recebe estado replicado após aplicação autoritativa.

### Modelo de escrita
- Escrita principal: banco de dados.
- Fallback opcional: arquivo local versionado apenas quando o ambiente estiver em modo degradado declarado.
- Não permitir estado híbrido implícito entre DB e arquivo.
- Toda escrita deve registrar sucesso, rejeição, timeout ou rollback.

### Modelo de leitura
- Load sempre parte do servidor.
- Load deve validar identidade, versão, revisão e compatibilidade mínima de schema.
- Estado inválido, incompleto ou incompatível deve falhar de forma segura, com log categorizado.

## Domínios persistentes obrigatórios
### Personagem
- identidade do personagem
- atributos persistentes permitidos
- posição/estado seguro quando aplicável
- flags de progressão válidas

### Inventário
- conteúdo do inventário
- ownership correto
- revisão para detectar escrita concorrente
- prevenção de duplicação/perda em reconnect e falha parcial

### Economia
- saldos e moedas
- transações relevantes
- reconciliação após reconnect/restart
- trilha mínima para incidentes

## Contrato de consistência
### Regras obrigatórias
- Nenhuma confirmação de save antes de persistência autoritativa concluir.
- Falha parcial não pode deixar runtime e storage em estados conflitantes sem marcação explícita.
- Operações críticas devem usar revisão/check otimista ou mecanismo equivalente.
- Toda mutação com risco de duplicação deve ter rollback seguro.
- Reconnect não pode gerar reaplicação cega de delta antigo.

### Casos críticos
- morte + full loot + save
- disconnect durante transferência de inventário
- reconnect com snapshot anterior em cache
- restart de servidor durante janela de escrita
- auth degradada em ambiente que exige identidade real

## Pipeline recomendado
1. Validar sessão/auth no servidor.
2. Resolver identidade persistente do jogador.
3. Carregar snapshot de personagem.
4. Carregar snapshot de inventário.
5. Validar revisão/schema.
6. Aplicar estado ao runtime autoritativo.
7. Liberar replicação normal para cliente.

### Escrita recomendada
1. Coletar delta/snapshot no servidor.
2. Validar contexto da mutação.
3. Escrever no provider primário.
4. Confirmar revisão nova.
5. Atualizar cache de runtime apenas após sucesso.
6. Emitir telemetria de sucesso ou falha.

## Telemetria mínima obrigatória
- `LoadRequested`
- `LoadSucceeded`
- `LoadRejected`
- `SaveRequested`
- `SaveSucceeded`
- `SaveRejected`
- `SaveRetried`
- `RollbackTriggered`
- `RevisionConflict`
- `AuthGateRejected`

Todos os eventos devem usar categoria explícita de log.

## Segurança
- Nunca confiar em payload de cliente para estado persistente final.
- Nunca permitir escrita autoritativa via Blueprint convenience path no cliente.
- Toda entrada server RPC relacionada a persistência deve validar ownership, contexto, frequência e sanidade.
- Dados sensíveis devem ter redaction em logs operacionais quando necessário.

## Não objetivos desta fase
- economia completa final
- ferramentas web externas
- analytics de produto fora do necessário para operação

## Critério de saída desta fase
A persistência de produção só é considerada pronta quando:
- o caminho DB-first está ativo e documentado
- fallback degradado está explícito e controlado
- reconnect restaura personagem + inventário sem duplicação/perda
- escrita parcial não confirma estado híbrido inválido
- auth/session gate protege flows sensíveis
- smoke Dedicated Server cobre reconnect e falha controlada
