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

## Recorte de gameplay 2026-06-14
- Reportados como corrigidos pelo operador: inventario/vendor/loot fechando aceitavelmente, barra de ability ao morrer, regen de atributos, animacao equip/unequip, camera sem arma, AoE sem dano, `Blizzard` nao entrando mais no solo e `shoot arrow`.
- Pendentes ativos: inventario nao equipando item, recriar projetil de flecha, menu de settings para hotkeys, sockets de arma no `VisualOverride`, ragdoll, fluxo `dead`/`revive` e revisao de luzes.
- Validacao formal nao foi executada neste registro documental; nao tratar os itens como `Verified` ate passar pelos gates aplicaveis.

## Regras de validacao
- Build de editor deve usar apenas `C:\UnrealSource\UnrealEngine`.
- Build padrao de agente: `SpellRiseEditor Win64 Development`.
- Smoke multiplayer deve usar Dedicated Server quando validar rede, combate, GAS, RPC, persistencia ou fluxo client/server.
