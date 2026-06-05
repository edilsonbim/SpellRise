# SpellRise - Repositório de Desenvolvimento

SpellRise é um MMORPG PvP hardcore (UE 5.7) com combate skill-based e arquitetura server-authoritative para Dedicated Server.

## Estado atual (2026-06-05)
- Base multiplayer live-service em Unreal Engine 5.7 com Dedicated Server obrigatório.
- Persistência server-side ativa com snapshots revisionados de personagem, inventário, mundo e eventos de morte.
- Snapshot de personagem schema 7 persiste criação de personagem e configuração visual por `SteamId64`.
- Bootstrap/auth Steam em Dedicated Server falha cedo quando o OnlineSubsystem degrada para `NULL`.

## Stack e regras principais
- Unreal Engine Source Build em `C:\UnrealSource\UnrealEngine`
- Dedicated Server obrigatório
- GAS owner no `PlayerState` (ASC/AttributeSets no `PlayerState`)
- Cliente não decide dano/loot/morte/resultado final
- Persistência de produção é server-side e indexada por identidade Steam real

## Estrutura
- `Source/` C++ de runtime e sistemas de rede/gameplay
- `Content/` assets e Blueprints do projeto
- `Plugins/` plugins do projeto (incluindo Narrative)
- `Config/` configurações de engine/projeto
- `Scripts/` automações de build/teste
- `Fontes/` documentação técnica e operacional

## Documentação canônica
- `Fontes/FONTE_INDEX.md`
- `Fontes/PROJECT_STATE.md`
- `Fontes/AGENTS.md`
- `Fontes/ARCHITECTURE.md`
- `Fontes/ATTRIBUTE_MATRIX.md`
- `Fontes/MULTIPLAYER_RULES.md`
- `Fontes/SECURITY_MODEL.md`
- `Fontes/COMBAT_DESIGN_PHILOSOPHY.md`
- `Fontes/REFERENCIAS_JOGOS.md`
- `Fontes/BACKLOG.md`
- `Fontes/BUG_LOG.md`
- `Fontes/CHANGELOG.md`
- `Fontes/ENGINEERING_PLAYBOOK.md`

## Observação
Toda validação de build/smoke deve usar a Unreal Source local, não a engine da launcher.
