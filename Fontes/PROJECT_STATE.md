# Project State

## Estado atual
- Projeto Unreal Engine 5.7 com Dedicated Server obrigatorio.
- Arquitetura de gameplay orientada a GAS, com owner autoritativo no PlayerState.
- ASC e AttributeSets vivem no PlayerState; Character consome ASC/ActorInfo do PlayerState.
- Combate PvP hardcore, full loot, free target, skill-based e server-authoritative.
- Persistencia server-side usa snapshots revisionados em PostgreSQL/arquivo; schema 7 de personagem inclui estado de criação e configuração visual por `SteamId64`.
- Bootstrap/auth Steam em DS, overflow de replicacao no PlayerController, building mode e gate multiplayer continuo seguem como foco ativo.

## Prioridades ativas
1. Corrigir overflow de replicacao no PlayerController.
2. Fechar bootstrap/auth Steam em Dedicated Server.
3. Fechar validacao final da persistencia de producao em DS+2/reconexao.
4. Consolidar building mode com budget de rede e matriz de RPC.
5. Avancar automacao multiplayer em gate continuo/CI.

## Recorte de gameplay 2026-06-18
- Reportados como corrigidos/feitos pelo operador: persistencia de ability bar, armas e itens equipados; XP, level, Craft Points e Talent Points no `USpellRiseProgressionComponent`; cooldown por nivel de ability.
- Prioridade P0: fechar fluxo `dead`/`revive` e corrigir inventario que nao equipa item.
- Prioridade P1: criar mecanica de gasto de Attribute Points em STR/AGI/INT/WIS; recriar projetil de flecha; concluir rework do calculo de dano/TTK; corrigir socket de arma no `VisualOverride`; melhorar talent tree com nivel setavel e curva de custo.
- Prioridade P2: implementar durabilidade de itens; refazer bonus dos atributos base; criar boosters de atributos base; criar menu de settings para hotkeys; melhorar ragdoll.
- Prioridade P3: criar spell de retorno de dano; refazer visual da talent tree por escolas; revisar luzes.
- Validacao formal nao foi executada neste registro documental; nao tratar os itens como `Verified` ate passar pelos gates aplicaveis.

## Regras de validacao
- Build de editor deve usar apenas `C:\UnrealSource\UnrealEngine`.
- Build padrao de agente: `SpellRiseEditor Win64 Development`.
- Smoke multiplayer deve usar Dedicated Server quando validar rede, combate, GAS, RPC, persistencia ou fluxo client/server.
