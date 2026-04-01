# Persistence Runtime Contract

## Objetivo
Definir o contrato operacional entre runtime, rede e persistência no SpellRise.

## Server vs Client
### Server
- decide quando carregar
- decide quando salvar
- valida auth/session
- valida contexto de mutação
- confirma revisão persistente
- aplica estado persistido ao runtime
- resolve rollback e reconciliação

### Client
- solicita entrada em sessão
- recebe estado replicado pronto
- nunca confirma save/load autoritativo
- nunca envia estado final de inventário, economia ou progressão como verdade

## Pontos de integração obrigatórios
### Login / entrada
- jogador não entra no loop normal antes de o servidor decidir o estado persistido aplicável
- fluxo degradado deve ser explícito quando auth/DB não estiverem disponíveis

### Reconnect
- reconnect deve buscar estado persistido/reconciliado no servidor
- snapshot fallback deve respeitar revisão mais recente
- reconnect não pode restaurar item já perdido por full loot ou transação concluída

### Morte / Full Loot / Respawn
- decisão permanece no servidor
- persistência só grava resultado final autoritativo
- não gravar estado intermediário ambíguo da transferência
- rollback deve impedir duplicação se a etapa de inventário falhar

### Construção futura
- toda mutação persistente de construção deve seguir o mesmo contrato: server-only, revisão e rollback

## Modos operacionais
### Modo normal
- DB disponível
- auth válida
- save/load normais
- reconnect com fonte primária no DB

### Modo degradado explícito
- provider primário indisponível ou auth restrita
- fallback permitido apenas se o ambiente tiver política declarada
- logs devem marcar claramente o modo degradado
- features sensíveis podem ser bloqueadas em vez de cair para fallback silencioso

### Modo bloqueado
- sem auth válida para fluxo que exige identidade real
- sem persistência primária e sem fallback autorizado
- servidor rejeita progressão persistente de forma segura

## Regras de revisão
- todo snapshot persistente relevante deve carregar revisão/versionamento
- conflito de revisão deve impedir overwrite cego
- overwrite só pode acontecer por regra explícita do servidor

## Regras de telemetria
Registrar no mínimo:
- player/session id lógico
- tipo de operação
- resultado
- motivo de falha
- revisão anterior e nova quando aplicável
- modo operacional: normal, degradado, bloqueado

## Smoke obrigatório desta área
- Standalone
- Listen Server
- Dedicated Server + 2 clientes
- Dedicated Server + 2 clientes com reconnect
- Dedicated Server + 2 clientes com lag/loss

## Casos obrigatórios
1. login normal com load válido
2. reconnect após save bem-sucedido
3. disconnect durante mutação de inventário
4. morte + full loot + reconnect
5. falha do provider primário com comportamento degradado esperado
6. auth inválida em fluxo que exige identidade real

## Bloqueadores de merge
- cliente influenciando estado persistente final
- save confirmado antes de persistência autoritativa
- overwrite sem revisão/check
- fallback silencioso para arquivo em fluxo de produção
- reconnect com duplicação/perda de inventário
- falta de logs categorizados de falha
