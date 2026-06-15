# Roadmap 2026

Documento de planejamento anual do SpellRise para 2026.

Este roadmap separa marcos ja criados, com base no historico Git local e nos documentos vivos do projeto, dos objetivos ainda planejados. O foco e tratar SpellRise como multiplayer live service com Dedicated Server obrigatorio, combate server-authoritative, persistencia segura e gate continuo.

## Premissas

- Unreal Engine 5.7.
- Dedicated Server obrigatorio.
- GAS autoritativo com ASC e AttributeSets no PlayerState.
- Combate hardcore PvP, full loot, free target e skill-based.
- Cliente nunca decide dano final, custo, cooldown, morte, loot, persistencia ou economia.
- Validacao final de rede deve cobrir DS+2, reconnect e lag/loss quando tocar sistemas net-critical.

## 2026-03 - Fundacao tecnica e migracao

### Marcos ja criados

- Migracao para mirror source-only.
- Scripts e perfis para Steam auth e no-steam.
- Estabilizacao inicial de login `nosteam` e inicializacao de ASC.
- Hook inicial de cast bar HUD em abilities.
- Hardening inicial do fluxo de PlayerController.
- Atualizacao dos documentos base do projeto.

### Objetivos

- Fechar base documental de arquitetura, rede, GAS e validacao.
- Formalizar regras de Dedicated Server e build via Unreal Source.
- Definir PlayerState como owner autoritativo do GAS.
- Separar fluxo Steam real de fallback de teste.

### Criterio de saida

- Regras de authority, GAS e build registradas nos documentos fonte.
- Fluxo base de auth/teste documentado.

## 2026-04 - GAS, multiplayer gate, loot e equipamentos

### Marcos ja criados

- Git LFS configurado para assets Unreal.
- Primeira formalizacao do gate multiplayer.
- Hardening de GAS activation.
- Delay server-side de 3s no full loot e ground check da bag.
- Fluxo de equipment-driven abilities e persistencia atualizado.
- Weapon quick slots, preview e limpeza de inventario integrados.
- Ajustes de input multiplayer para abilities server-only.
- Validacao server-side e diagnosticos de ativacao GAS.
- Correcao de cast input release.
- Commit de spell movido para etapa de execucao.
- Correcao de double-commit em spell instantanea.
- Atualizacoes de dash, shield e Echo assets.

### Objetivos

- Fechar fluxo death -> full loot -> respawn no recorte atual.
- Separar activation, target data, commit, GE, presentation e cleanup.
- Validar DS+2 baseline sem overflow de replicacao.
- Consolidar fluxo de equip/unequip com inventory e quick slots.

### Criterio de saida

- Gate multiplayer baseline documentado.
- Full loot autoritativo no servidor.
- Equipment flow funcional sem trust no cliente.

## 2026-05 - Assets, runtime de armas, talentos e persistencia

### Marcos ja criados

- Weapon equipment workflow atualizado.
- Efeitos de armadura via SetByCaller adicionados.
- Gameplay pipeline e controller refactor consolidados.
- Equipment e ability HUD integrados.
- Animation pipeline e gameplay updates integrados.
- Conteudo medieval docks adicionado.
- Weapon data assets e gameplay assets recuperados integrados.
- CastTimeReduction removido dos derived stats.
- Talentos persistidos e inventory weight flow endurecido.
- Eventos de morte persistidos server-side em `spellrise_death_events`.
- Repository cleanup e ajustes de tracking de assets pesados.

### Objetivos

- Fechar persistencia de talentos por `SteamId64`.
- Remover dependencia de atributos derivados indevidos.
- Endurecer inventario/equipamento contra perda indevida.
- Validar death event, loot e combat log em fluxo multiplayer.

### Criterio de saida

- Talentos e death events persistidos server-side.
- Weapon runtime e equipment flow consolidados.
- Nenhum novo sistema dependente de atributos fora da matriz canonica STR/AGI/INT/WIS.

## 2026-06 - Observabilidade, persistencia de criacao e correcao de burst

### Marcos ja criados

- Live progression persistence e talent UI refresh corrigidos.
- Weapon runtime component unificado.
- Burst de replicacao do combat log reduzido.
- `MoveSpeed` e freeze corrigidos via `CharacterMovement->MaxWalkSpeed`.
- Observabilidade runtime melhorada.
- Ruido de logs dev reduzido.
- Estado de criacao de personagem persistido.
- Colisao de unity build da persistencia corrigida.

### Objetivos

- Fechar `PlayerController replication overflow` como `Verified`, nao apenas `Fixed`.
- Validar overflow em DS+2 normal, reconnect e lag/loss.
- Fechar bootstrap/auth Steam em Dedicated Server com Steam real.
- Validar persistencia de criacao/reconexao em DS+2 normal e lag/loss.
- Corrigir traversal sem replicacao em multiplayer.
- Fechar auth gating para fluxos sensiveis em DS.

### Criterio de saida

- `FBitReader::SetOverflowed` zerado no gate local DS+2 e lag/loss.
- Steam DS sem fallback indevido para `NULL` em ambiente obrigatorio.
- Persistencia de personagem, inventario, talentos e morte validada em DS+2/reconnect.

## 2026-07 - Persistencia de producao

### Planejado

- Fechar snapshots revisionados.
- Fechar economia server-side basica.
- Implementar reconciliacao deterministica.
- Definir rollback seguro.
- Confirmar compatibilidade de schema.
- Bloquear `DEV_`/`NULL` em DB de producao.

### Criterio de saida

- Persistencia e economia sem trust no cliente.
- Reconnect validado com personagem, inventario, talentos e estado de criacao.

## 2026-08 - Building mode e matriz de RPC

### Planejado

- Consolidar matriz de RPC do building mode.
- Validar contexto, ownership, alcance e LOS no servidor.
- Definir budget de rede por jogador.
- Adicionar rate-limit/anti-spam.
- Logar rejeicoes de RPC.
- Testar DS+2 com lag/loss.

### Criterio de saida

- Building mode com contrato de rede documentado e validado.
- Risco de burst e overflow medido.

## 2026-09 - Testes automatizados e balance inicial

### Planejado

- Criar testes automatizados para dano.
- Criar testes automatizados para fall damage.
- Criar testes automatizados para morte e loot.
- Criar testes automatizados para autoridade de projetil.
- Criar testes automatizados para persistencia e reconnect.
- Fazer balance pass inicial de resist, penetration, crit e TTK.

### Criterio de saida

- Testes cobrindo caminhos criticos de gameplay autoritativo.
- Balance inicial registrado com metricas de TTK.

## 2026-10 - Gate continuo e staging inicial

### Planejado

- Colocar build `SpellRiseEditor Win64 Development` em gate continuo.
- Adicionar DS+2 normal.
- Adicionar DS+2 reconnect.
- Adicionar DS+2 lag/loss Perfil A.
- Adicionar DS+2 lag/loss Perfil B.
- Criar staging dedicado com Steam real e DB separado.

### Criterio de saida

- CI executando gate multiplayer de forma repetivel.
- Staging funcional com Steam real e banco separado.

## 2026-11 - Soak test e live ops minimo

### Planejado

- Executar sessao longa em DS.
- Testar morte e loot repetidos.
- Testar reconnect repetido.
- Testar troca de equipamento e talentos.
- Testar combate com burst.
- Testar building mode sob carga.
- Definir configs de rollback e denylist operacional.

### Criterio de saida

- Soak test documentado com resultado, logs e riscos.
- Live ops minimo preparado para testes externos pequenos.

## 2026-12 - Preparacao de Alpha tecnica

### Planejado

- Fechar criterios de Alpha tecnica.
- Garantir ausencia de trust indevido no cliente.
- Garantir ausencia de dependencia de HUD/UI em Dedicated Server.
- Confirmar persistencia auditavel.
- Confirmar economia server-side.
- Confirmar auth Steam validado.
- Documentar rollback.
- Zerar bugs Critical ou registrar bloqueio real.
- Preparar planejamento operacional de 2027.

### Criterio de saida

- Alpha tecnica com criterio objetivo de entrada e saida.
- 2027 preparado para vertical slice jogavel e Pre-alpha.

## Riscos ativos

- `PlayerController replication overflow` ainda precisa confirmacao fora do recorte local antes de ser considerado completamente verificado.
- Bootstrap/auth Steam em Dedicated Server ainda depende de build/link e validacao Steam real.
- Persistencia de producao ainda precisa validacao final de DS+2/reconnect e reconciliacao.
- Building mode ainda precisa matriz RPC, budget de rede e validacao server-side completa.
- Traversal multiplayer esta em investigacao e nao deve receber refator estrutural antes de isolar causa raiz.

## Resumo executivo

- 2026-03: fundacao tecnica, migracao e regras de autoridade.
- 2026-04: GAS, full loot, multiplayer gate e equipamentos.
- 2026-05: assets, weapon runtime, talentos e persistencia.
- 2026-06: observabilidade, criacao de personagem e reducao de burst.
- 2026-07: persistencia de producao.
- 2026-08: building mode e matriz de RPC.
- 2026-09: testes automatizados e balance inicial.
- 2026-10: gate continuo e staging.
- 2026-11: soak test e live ops minimo.
- 2026-12: preparacao de Alpha tecnica.
