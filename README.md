# SpellRise - Repositório de Desenvolvimento

SpellRise é um MMORPG PvP hardcore em Unreal Engine Source com combate skill-based e arquitetura server-authoritative para Dedicated Server.

## Estado atual (2026-06-25)
- Branch de migração concluído e pronto para virar `develop`.
- `main` deve permanecer como linha estável; `develop` passa a receber a migração concluída.
- Base multiplayer live-service em Unreal Engine Source com Dedicated Server obrigatório.
- Persistência server-side ativa com snapshots revisionados de personagem, inventário, mundo e eventos de morte.
- Contratos de inventário/equipamento próprios existem em paralelo ao Narrative; ativação completa ainda depende de validação DS+2 e migração final de assets/fluxos.
- Problema de áudio pós-migração foi isolado no Ultra Dynamic Sky e resolvido pelo operador ao remover/adicionar o plugin/asset novamente.
- Bugs remanescentes são herdados da versão anterior, não blockers novos da migração.

## Stack e regras principais
- Unreal Engine Source Build em `C:\UnrealSource\UnrealEngine`
- Dedicated Server obrigatório
- GAS owner no `PlayerState` (ASC/AttributeSets no `PlayerState`)
- Cliente não decide dano/loot/morte/resultado final
- Persistência de produção é server-side e indexada por identidade Steam real
- `main` = linha estável; `develop` = linha ativa de integração.

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
