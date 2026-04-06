# SpellRise - Repositório de Desenvolvimento

SpellRise é um MMORPG PvP hardcore (UE 5.7) com combate skill-based e arquitetura server-authoritative para Dedicated Server.

## Estado atual (2026-04-06)
- Base de gameplay/equipment ativa com integração C++ + Blueprint.
- Fluxo de equip/unequip por inventory e quick slots evoluído.
- Ponto pendente aberto: drop de item no cliente em DS ainda em ajuste final de spawn do pickup (ver `Fontes/BUG_LOG.md`, BUG-2026-04-06-035).

## Stack e regras principais
- Unreal Engine Source Build em `C:\UnrealSource\UnrealEngine`
- Dedicated Server obrigatório
- GAS owner no `PlayerState` (ASC/AttributeSets no `PlayerState`)
- Cliente não decide dano/loot/morte/resultado final

## Estrutura
- `Source/` C++ de runtime e sistemas de rede/gameplay
- `Content/` assets e Blueprints do projeto
- `Plugins/` plugins do projeto (incluindo Narrative)
- `Config/` configurações de engine/projeto
- `Scripts/` automações de build/teste
- `Fontes/` documentação técnica e operacional

## Documentação canônica
- `Fontes/AGENTS.md`
- `Fontes/ARCHITECTURE.md`
- `Fontes/ATTRIBUTE_MATRIX.md`
- `Fontes/MULTIPLAYER_RULES.md`
- `Fontes/COMBAT_DESIGN_PHILOSOPHY.md`
- `Fontes/BACKLOG.md`
- `Fontes/BUG_LOG.md`
- `Fontes/CHANGELOG.md`

## Observação
Toda validação de build/smoke deve usar a Unreal Source local, não a engine da launcher.
