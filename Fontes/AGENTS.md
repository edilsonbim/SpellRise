# AGENTS.md — SpellRise

## Identidade do projeto
Você está trabalhando no projeto SpellRise.

Trate este repositório como um projeto contínuo, não como tarefas isoladas.

## Fonte de verdade obrigatória
Considere estes documentos como verdade absoluta do projeto, nesta ordem:
1. PROJECT_STATE.md
2. ARCHITECTURE.md
3. ATTRIBUTE_MATRIX.md
4. MULTIPLAYER_RULES.md
5. SECURITY_MODEL.md
6. BACKLOG.md
7. BUG_LOG.md
8. ROADMAP.md
9. COMBAT_DESIGN_PHILOSOPHY.md

Se houver conflito entre código antigo e documentação atual, sinalize o conflito e siga os documentos acima.

## Rotina obrigatória pré-resposta (checklist de referências)
Antes de responder qualquer tarefa, executar este checklist:
1. Ler/validar a ordem da Fonte de verdade obrigatória.
2. Identificar quais documentos da fonte de verdade impactam a tarefa.
3. Verificar referências externas oficiais aplicáveis:
   - Narrative: https://docs.narrativetools.io/
   - GAS (Unreal): https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine
   - Lyra: https://dev.epicgames.com/documentation/en-us/unreal-engine/lyra-sample-game-in-unreal-engine
4. Confirmar aderência de arquitetura para a resposta:
   - authority (Server vs Client),
   - prediction/reconciliation (quando houver),
   - RPC/OnRep.
5. Só então responder/propor implementação.

## Regras gerais de resposta
- Responda em português do Brasil.
- Responda em passos curtos.
- Para tarefas não triviais, faça primeiro um plano curto e aguarde confirmação antes de alterar muitos arquivos.
- Priorize C++ para Unreal Engine 5.7.
- Evite solução estrutural genérica em Blueprint.
- Use Blueprint apenas quando fizer mais sentido para assets, data assets, UI, animação, efeitos ou configuração.
- Preserve nomes, arquitetura, convenções e fluxo já existentes.
- Ao corrigir algo, devolva arquivos completos, nunca trechos soltos.
- Tudo deve permanecer compilável.
- Não invente API, classe, tag, asset ou função sem antes verificar se já existe algo equivalente no projeto.
- Sempre considerar impacto em replicação, authority, prediction, RPC e OnRep.

## Engine de build obrigatória
- Sempre buildar o SpellRise com a Source Engine em `C:\UnrealSource\UnrealEngine`.
- Não usar a engine instalada pelo launcher da Epic para build deste projeto.
- Política fixa de execução: toda build, smoke test, teste manual, automação local e validação de PR deve usar exclusivamente a Unreal Source.
- Fluxo obrigatório após qualquer alteração de código/config/assets:
  1. Fechar o editor (`UnrealEditor.exe`).
  2. Rodar build na Unreal Source (`Build.bat` do engine source).
  3. Abrir o editor novamente.
- Comando padrão de build:
  - `C:\UnrealSource\UnrealEngine\Engine\Build\BatchFiles\Build.bat SpellRiseEditor Win64 Development "C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject" -WaitMutex -NoHotReloadFromIDE`

## Verdade técnica do SpellRise
- Dedicated server é obrigatório.
- Full Loot faz parte da direção do projeto.
- Free Target.
- Skill-Based.
- Sem auto-aim.
- Combate server-authoritative.
- O owner autoritativo do GAS é o PlayerState.
- ASC e AttributeSets vivem no PlayerState.
- O Character consome ASC/ActorInfo vindo do PlayerState.
- O ASC usa replication mode Mixed.
- Atributos principais válidos: STR, AGI, INT, WIS.
- Não usar aliases legados como VIG e FOC em novos assets ou novas implementações.
- O cliente nunca é autoridade para dano, recursos, atributos primários, morte, loot ou decisão final de gameplay.

## Contrato de rede obrigatório
### Server
- Resolve dano, custos, cooldowns, recursos, morte, loot e mutação de estado.
- Valida payload de RPC e contexto de chamada.
- Spawna projéteis autoritativos.
- Aplica GameplayEffects autoritativos.
- Decide transição de death state.
- Executa fall damage e catalyst charge.
- Mantém segurança contra bypass de custo/cooldown e spoof de payload.

### Client
- Coleta input.
- Faz prediction apenas para UX.
- Exibe UI, VFX, SFX e feedback local.
- Pode fazer aim trace local como entrada.
- Nunca decide dano final, resultado final de hit, custo, cooldown, morte ou loot.

### Regras adicionais
- Nunca usar multicast para decisão autoritativa de gameplay.
- Multicast é apenas apresentação replicada.
- Ability code não deve depender apenas de HasAuthority() para fluxo de ativação.
- Em ability predita, separe evento cosmético local de evento real de gameplay no servidor.
- Dedicated server deve funcionar sem dependência de câmera, widget, HUD ou qualquer lógica de UI.

## Orçamento de rede obrigatório (AAA baseline)
- Toda feature de combate/rede deve declarar orçamento de rede antes de merge.
- Definir e revisar por feature:
  - NetUpdateFrequency dos atores relevantes.
  - taxa máxima de RPC por segundo por player.
  - tamanho máximo de payload de RPC (bytes).
  - taxa máxima de envio de target data.
- Mudança sem orçamento explícito deve ser tratada como risco de regressão.

## GAS
- Respeite arquitetura baseada em GAS já existente.
- Não misture cast, channel e charge na mesma base se isso enfraquecer a herança.
- Sempre separar:
  - ativação
  - commit
  - target data
  - aplicação de GE
  - cue
  - cosmético local
  - encerramento
- Quando mexer em abilities, informar:
  - onde a ability inicia
  - onde commit acontece
  - onde o servidor valida
  - onde o cliente só prevê
  - onde a réplica acontece
- Quando mexer em atributos, informar impacto em:
  - AttributeSet
  - MMC
  - ExecCalc
  - GameplayEffect
  - UI
  - replicação
- Quando mexer em Gameplay Tags, manter nomenclatura canônica e consistente com código e assets.

## Prediction e Reconciliation
- Toda ability com prediction deve documentar:
  - o que é previsto localmente (somente UX).
  - qual evento é confirmado pelo servidor.
  - como ocorre a reconciliação em caso de divergência.
  - tolerância visual aceitável (ex.: rollback de VFX/SFX).
- Não permitir fluxo que gere double fire, double commit ou dano duplicado após reconciliação.

## Fluxo obrigatório para projéteis
Preservar este padrão:
1. Client faz aim trace local.
2. Client envia target data.
3. Server consome target data replicado.
4. Server commita a ability.
5. Server spawna o projétil replicado.
6. Hit/overlap aplica GE no servidor.

Não mover dano final para o cliente.

## Matriz obrigatória de validação de RPC
Para cada RPC novo/alterado, registrar no review:
- origem permitida (Owner, AutonomousProxy, Server-only, etc.).
- payload esperado e limites (faixa, tamanho, sanidade).
- validações de contexto (estado do ator, cooldown, custo, tags, distância, LOS quando aplicável).
- proteção anti-spam (rate limit/cooldown no servidor).
- comportamento em falha (reject seguro + log categorizado).

## Forma de entrega
Sempre responder neste formato, quando aplicável:

1. Problema
2. Causa provável
3. Correção exata
4. Server
5. Client
6. Riscos de authority / prediction / RPC / OnRep
7. Arquivos completos
8. Checklist de teste

## Regras para debugging
Quando o pedido for debug:
- Liste primeiro a causa mais provável.
- Depois a correção exata.
- Depois o risco de replicação envolvido.
- Depois o que testar em:
  - Standalone
  - Listen Server
  - Dedicated Server
- Se houver risco de authority, prediction, RPC ou OnRep, diga explicitamente.
- Incluir também teste com latência e perda de pacote simuladas.

## Regras para código
- Entregar código completo e compilável.
- Manter includes corretos.
- Não quebrar arquitetura existente.
- Não renomear classe, função, variável ou asset sem necessidade real.
- Não remover suporte a replicação já existente.
- Se adicionar variável replicada, incluir:
  - UPROPERTY correta
  - GetLifetimeReplicatedProps
  - condição de réplica, quando aplicável
  - OnRep, quando necessário
- Se adicionar RPC, incluir validação de payload e contexto.
- Preferir clareza, previsibilidade e segurança de rede.
- Evitar LogTemp para fluxo importante; preferir categoria explícita.
- Para fluxo crítico de combate, instrumentar logs/counters mínimos:
  - ativação da ability
  - commit
  - aplicação de GE
  - dano aplicado/negado
  - motivo de rejeição de validação

## Regras de design de combate
- Skill > Gear
- Reaction > RNG
- Positioning > Burst
- Sem aim assist
- Sem tab target
- Crescimento de poder controlado
- Respeitar clamps e limites atuais antes de propor rework

## Dívidas técnicas atuais que precisam ser respeitadas
Ao tocar nesses sistemas, sinalize antes de alterar:
- mismatch entre Data.CatalystCharge e Data.CatalystChargeDelta
- coexistência de MMCs legados com pipeline novo
- debug FX ainda no runtime path
- conflito de config de mapas
- combat log replicado ainda não implementado
- pipeline full loot + respawn ainda não fechado
- persistência ainda não implementada

## Governança de dívida técnica
- Toda dívida alterada/criada deve registrar:
  - severidade (S0, S1, S2, S3)
  - owner responsável
  - prazo alvo
  - critério de saída (como validar que foi resolvida)
- Dívida sem owner e sem critério de saída não deve ser considerada rastreável.

## Testes multiplayer obrigatórios
- Toda alteração de combate/rede deve validar no mínimo:
  - Standalone
  - Listen Server
  - Dedicated Server com 2+ clientes
- Cobrir smoke tests de:
  - cast/commit/cooldown/custo
  - spawn de projétil + hit/overlap no servidor
  - morte, loot e respawn
  - reconexão básica de cliente
- Sempre que possível, manter automação desses smoke tests para regressão.

## O que não fazer
- Não sugerir mover lógica autoritativa para Blueprint por conveniência.
- Não colocar cálculo final de dano no cliente.
- Não confiar em dados do cliente sem validação.
- Não usar multicast para corrigir estado autoritativo.
- Não introduzir auto-aim.
- Não misturar UI com lógica obrigatória de servidor dedicado.
- Não responder com pseudo-código quando o pedido for implementação.
- Não devolver só patch parcial quando o pedido exigir arquivo final.

## Definição de pronto
Uma tarefa só está pronta quando:
- compila
- sem warnings críticos novos
- respeita GAS
- respeita replicação
- separa Server vs Client
- não quebra dedicated server
- não cria trust indevido no cliente
- informa riscos de authority / prediction / RPC / OnRep
- inclui checklist objetivo de teste
- valida cenário com latência/perda de pacote simulada quando a mudança for de combate/rede
