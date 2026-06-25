# Project State

## Estado atual
- Branch de migração concluído com sucesso pelo operador; a promoção esperada é `develop -> main` para manter a linha estável e, em seguida, `codex/ue5.8-migration -> develop` para a linha ativa.
- Projeto Unreal Engine Source com Dedicated Server obrigatorio.
- Arquitetura de gameplay orientada a GAS, com owner autoritativo no PlayerState.
- ASC e AttributeSets vivem no PlayerState; Character consome ASC/ActorInfo do PlayerState.
- Combate PvP hardcore, full loot, free target, skill-based e server-authoritative.
- Persistencia server-side ativa permanece nos schemas `14/2`; contratos `15/3` foram adicionados para rollout futuro, desativado por padrão.
- Inventário/equipamento próprios foram adicionados em paralelo ao Narrative no `PlayerState`, com FastArray, RPCs validados, persistência por GUID e ViewModel event-driven. Migração runtime e de assets ainda não está concluída.
- Problema de áudio observado após a migração foi atribuído ao Ultra Dynamic Sky e resolvido pelo operador ao remover/adicionar novamente o plugin/asset; não manter workaround global de AudioMixer para esse caso.
- Bugs remanescentes são herdados da versão anterior: câmera, validação de abilities após downed/revive e demais itens abertos do backlog.
- Bootstrap/auth Steam em DS, overflow de replicacao no PlayerController, building mode, bugs herdados de câmera/abilities e gate multiplayer continuo seguem como foco ativo.

## Prioridades ativas
1. Corrigir overflow de replicacao no PlayerController.
2. Fechar bootstrap/auth Steam em Dedicated Server.
3. Fechar validacao final da persistencia de producao em DS+2/reconexao.
4. Consolidar building mode com budget de rede e matriz de RPC.
5. Avancar automacao multiplayer em gate continuo/CI.

## Recorte de migração 2026-06-25
- Migração concluída e funcional no editor segundo validação operacional do operador.
- Ultra Dynamic Sky foi removido/adicionado novamente e o ruído de áudio desapareceu.
- Não há blocker novo da migração registrado neste recorte; pendências seguem como bugs herdados.
- Bug de travar o personagem no downed foi resolvido; o fluxo de abilities após renascer/sair de downed ainda precisa de validação funcional e correção se reproduzir.
- Manter `main` como linha estável e mover a migração para `develop` antes de continuar trabalho diário.
- Validacao formal de build/smoke não foi executada neste registro documental.

## Recorte de gameplay 2026-06-23
- Reportados como corrigidos pelo operador: inventario/vendor/loot fechando aceitavelmente, barra de ability ao morrer, ability bar ativa, regen de atributos, animacao equip/unequip, camera sem arma, AoE sem dano, `Blizzard` nao entrando mais no solo e `shoot arrow`.
- Reportados como feitos pelo operador: indicador visual da ability selecionada, remake do chat com funcoes de player/admin e whisper, e Party v1.
- Implementado em C++ e com build `SpellRiseEditor Win64 Development` aprovada em 2026-06-22; smoke multiplayer pendente: rework de `dead`/`revive` com cooldown de Downed, recovery e recursos escalados por WIS. Clamp autoritativo dos atributos continua em progresso.
- Implementado em C++ e compilado no recorte, atrás de flag default-off: `ItemDefinition`, inventário FastArray `OwnerOnly`, equipamento com nove slots/2H, grants GAS por GUID, contratos de persistência `15/3` e ViewModel de UI.
- Pendências do inventário: migrar `WeaponComponent`, peso/movimento, full loot, loot bags, vendor/containers e assets Narrative; montar/ligar a UI lateral UMG; validar migração `14/2 -> 15/3`, DS+2, lag/loss, spam e reconexão.
- Outros pendentes ativos: cue do tornado, mapa incompleto, recriar projétil de flecha, settings de hotkeys, sockets no `VisualOverride`, ragdoll, luzes, talent tree, durabilidade funcional e spell de retorno de dano.
- Validacao formal nao foi executada neste registro documental; nao tratar os itens como `Verified` ate passar pelos gates aplicaveis.

## Regras de validacao
- Build de editor deve usar apenas `C:\UnrealSource\UnrealEngine`.
- Build padrao de agente: `SpellRiseEditor Win64 Development`.
- Smoke multiplayer deve usar Dedicated Server quando validar rede, combate, GAS, RPC, persistencia ou fluxo client/server.
