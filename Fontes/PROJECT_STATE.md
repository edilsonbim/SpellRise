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

## Recorte de gameplay 2026-06-21
- Reportados como corrigidos pelo operador: calculo de dano/TTK, bonus concedidos pelos atributos base e vendor.
- Reportado como feito pelo operador: boosters dos atributos base.
- Em progresso: fluxo `dead`/`revive` e clamp autoritativo dos atributos.
- Prioridade P0: corrigir inventario que nao equipa item, barra ativa e bloqueio com arma 2H.
- Prioridade P1: drag and drop no inventario; indicador visual da ability selecionada; corrigir mapa que nao abre por completo; recriar projetil de flecha; menu de settings para hotkeys; corrigir socket de arma no `VisualOverride`.
- Prioridade P2: melhorar ragdoll; revisar luzes; melhorar talent tree com nivel setavel e curva de gastos de Talent Points; decidir entre rework do `TalentTreeComponent` ou extensao C++ da implementacao atual.
- Prioridade P3: refazer visual da talent tree com animacoes interativas por escolas; incluir durabilidade de itens; criar spell de retorno de dano.
- Validacao formal nao foi executada neste registro documental; nao tratar os itens como `Verified` ate passar pelos gates aplicaveis.

## Regras de validacao
- Build de editor deve usar apenas `C:\UnrealSource\UnrealEngine`.
- Build padrao de agente: `SpellRiseEditor Win64 Development`.
- Smoke multiplayer deve usar Dedicated Server quando validar rede, combate, GAS, RPC, persistencia ou fluxo client/server.
