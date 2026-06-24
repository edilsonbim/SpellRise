# Project State

## Estado atual
- Projeto Unreal Engine 5.7 com Dedicated Server obrigatorio.
- Arquitetura de gameplay orientada a GAS, com owner autoritativo no PlayerState.
- ASC e AttributeSets vivem no PlayerState; Character consome ASC/ActorInfo do PlayerState.
- Combate PvP hardcore, full loot, free target, skill-based e server-authoritative.
- Persistencia server-side ativa permanece nos schemas `14/2`; contratos `15/3` foram adicionados para rollout futuro, desativado por padrão.
- Inventário/equipamento próprios foram adicionados em paralelo ao Narrative no `PlayerState`, com FastArray, RPCs validados, persistência por GUID e ViewModel event-driven. Migração runtime e de assets ainda não está concluída.
- Bootstrap/auth Steam em DS, overflow de replicacao no PlayerController, building mode e gate multiplayer continuo seguem como foco ativo.

## Prioridades ativas
1. Corrigir overflow de replicacao no PlayerController.
2. Fechar bootstrap/auth Steam em Dedicated Server.
3. Fechar validacao final da persistencia de producao em DS+2/reconexao.
4. Consolidar building mode com budget de rede e matriz de RPC.
5. Avancar automacao multiplayer em gate continuo/CI.

## Recorte de gameplay 2026-06-23
- Reportados como corrigidos pelo operador: calculo de dano/TTK, bonus concedidos pelos atributos base, vendor e ability bar ativa.
- Reportados como feitos pelo operador: boosters dos atributos base, indicador visual da ability selecionada, remake do chat com funcoes de player/admin e whisper, e Party v1.
- Implementado em C++ e com build `SpellRiseEditor Win64 Development` aprovada em 2026-06-22; smoke multiplayer pendente: rework de `dead`/`revive` com cooldown de Downed, recovery e recursos escalados por WIS. Clamp autoritativo dos atributos continua em progresso.
- Implementado em C++ e compilado no recorte, atrás de flag default-off: `ItemDefinition`, inventário FastArray `OwnerOnly`, equipamento com nove slots/2H, grants GAS por GUID, contratos de persistência `15/3` e ViewModel de UI.
- Prioridade P0: migrar `WeaponComponent`, peso/movimento, full loot, loot bags, vendor/containers e assets Narrative para o contrato próprio; validar migração/reconexão e ausência de duplicação/perda.
- Prioridade P1: criar e ligar UI lateral UMG com drag-and-drop/rollback visual; executar DS+2 normal, lag/loss e spam; refazer cue do tornado, mapa, projétil de flecha, hotkeys e socket no `VisualOverride`.
- Prioridade P2: melhorar ragdoll; revisar luzes; melhorar talent tree com nivel setavel e curva de gastos de Talent Points; decidir entre rework do `TalentTreeComponent` ou extensao C++ da implementacao atual.
- Prioridade P3: refazer visual da talent tree com animacoes interativas por escolas; incluir durabilidade de itens; criar spell de retorno de dano.
- Validacao formal nao foi executada neste registro documental; nao tratar os itens como `Verified` ate passar pelos gates aplicaveis.

## Regras de validacao
- Build de editor deve usar apenas `C:\UnrealSource\UnrealEngine`.
- Build padrao de agente: `SpellRiseEditor Win64 Development`.
- Smoke multiplayer deve usar Dedicated Server quando validar rede, combate, GAS, RPC, persistencia ou fluxo client/server.
