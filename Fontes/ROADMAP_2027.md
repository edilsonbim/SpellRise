# Roadmap 2027

Documento de planejamento anual do SpellRise para 2027.

Este roadmap parte do ritmo observado entre marco e junho de 2026: alta velocidade de commits em gameplay core, GAS, persistencia, equipamentos, full loot, observabilidade e rede, mas ainda com blockers criticos em Dedicated Server, Steam auth, overflow/validacao de replicacao, persistencia de producao, building mode e gate continuo.

## Premissas

- 2026 deve encerrar com gate multiplayer continuo, staging inicial, persistencia server-side validada e bugs Critical controlados.
- 2027 deve ser tratado como ano de produto jogavel, escala controlada, conteudo minimo e validacao externa.
- Pre-alpha, Alpha e Beta so devem abrir com Dedicated Server obrigatorio, Steam auth valido e persistencia auditavel.
- Nenhum marco externo deve depender de comportamento client-authoritative.
- Datas abaixo sao janelas-alvo, nao compromissos de release.

## Leitura de velocidade atual

### Sinais positivos

- O projeto entregou muitos marcos tecnicos em pouco tempo:
  - GAS activation hardening;
  - fluxo de input multiplayer;
  - full loot;
  - death events;
  - persistencia de talentos;
  - criacao de personagem;
  - weapon runtime;
  - equipment flow;
  - combat log;
  - reducao de burst de replicacao;
  - observabilidade runtime.
- O ritmo atual permite fechar sistemas verticais quando o escopo e bem isolado.

### Limitadores

- O projeto ainda esta mais forte em infraestrutura do que em loop de conteudo continuo.
- MMO PvP full loot exige validacao mais pesada que jogo single-player/co-op.
- Steam DS, persistencia de producao, building mode e CI multiplayer sao gates reais antes de qualquer teste publico.
- Conteudo, mapa, economia, balance e live ops ainda precisam de ciclos dedicados.

### Conclusao de planejamento

- Pre-alpha viavel em 2027 se 2026 fechar staging e gate continuo.
- Alpha viavel em 2027 com escopo controlado e numero limitado de jogadores.
- Beta viavel no fim de 2027 ou inicio de 2028.
- Lancamento completo mais seguro em 2028.

## Marcos de produto

| Marco | Janela alvo | Escopo | Condicao minima |
|---|---:|---|---|
| Pre-alpha interna | 2027-03 | equipe/dev testers | DS, Steam auth, persistencia, loop PvP/PvE minimo |
| Pre-alpha fechada | 2027-04 | grupo pequeno externo | onboarding, combate, loot, reconnect e suporte basico |
| Alpha fechada | 2027-06 | testers convidados | mapa jogavel, economia basica, progressao e balance inicial |
| Alpha ampliada | 2027-08 | grupo maior controlado | estabilidade, logs, moderacao, wipe policy e conteudo expandido |
| Beta fechada | 2027-10 | teste de escala controlado | staging robusto, CI multiplayer, antiexploit basico |
| Beta aberta limitada | 2027-12 | janela publica curta | estabilidade de sessao, rollback, suporte e live ops |
| Lancamento recomendado | 2028-06 | release inicial | beta aberta sem blocker estrutural e hardening pos-beta concluido |

## 2027-01 - Vertical slice jogavel

### Planejado

- Fechar loop minimo:
  - login Steam;
  - criacao de personagem;
  - spawn em mapa jogavel;
  - combate PvP/PvE;
  - morte;
  - full loot;
  - respawn;
  - reconnect;
  - persistencia de personagem, inventario e talentos.
- Fechar mapa jogavel inicial:
  - zona inicial;
  - pontos de interesse;
  - areas de risco;
  - spawns seguros;
  - locais de teste PvP;
  - rotas claras para traversal.

### Criterio de saida

- Loop minimo completo em DS sem perda de personagem e sem trust indevido no cliente.

## 2027-02 - Readability de combate e estabilizacao interna

### Planejado

- Consolidar feedback de hit.
- Consolidar feedback de dano.
- Adicionar telegraph basico.
- Confirmar combat log owner-only.
- Melhorar clareza de morte, loot e respawn.
- Resolver bloqueios restantes de traversal, interact, respawn visual e reconnect.

### Criterio de saida

- Sessao interna de 60 minutos sem crash de DS, sem overflow e sem perda indevida de inventario.

## 2027-03 - Pre-alpha interna

### Marco alvo

- Pre-alpha interna.

### Planejado

- Rodar sessoes internas de 60 a 90 minutos.
- Validar build `SpellRiseEditor Win64 Development`.
- Validar DS+2 normal.
- Validar DS+2 reconnect.
- Validar persistencia de personagem, inventario e talentos.
- Validar morte, full loot e respawn.
- Validar Steam auth em DS sem fallback indevido para `NULL`.

### Criterio de saida

- Nenhum Critical aberto em auth, persistencia, morte, loot ou inventory.
- Sem `FBitReader::SetOverflowed` no recorte validado.

## 2027-04 - Pre-alpha fechada

### Marco alvo

- Pre-alpha fechada.

### Planejado

- Abrir para 10 a 30 testers.
- Usar janelas curtas.
- Assumir wipe esperado.
- Coletar logs detalhados.
- Focar em crash, exploit, auth, reconnect e fluxo de morte/loot.
- Ajustar onboarding minimo.

### Criterio de saida

- Testers externos pequenos conseguem completar loop de jogo sem suporte manual constante.
- Bugs Critical zerados no fim da janela.

## 2027-05 - Preparacao da Alpha fechada

### Planejado

- Melhorar estabilidade de persistencia.
- Fechar economia basica.
- Adicionar vendors/crafting simples se o escopo permitir.
- Ajustar balance inicial de TTK.
- Consolidar progressao de talentos.
- Garantir inventario/equipamento sem perda indevida.
- Preparar moderacao minima:
  - ban por SteamId64;
  - denylist IP quando aplicavel;
  - auditoria de morte;
  - auditoria de persistencia;
  - logs por sessao.

### Criterio de saida

- Alpha fechada pronta para testers convidados.
- Bugs High aceitos apenas com mitigacao e ticket documentado.

## 2027-06 - Alpha fechada

### Marco alvo

- Alpha fechada.

### Planejado

- Abrir para 50 a 150 testers.
- Validar mapa jogavel.
- Validar economia basica.
- Validar progressao.
- Validar balance inicial.
- Validar DS+2 com lag/loss.
- Confirmar logs de RPC rejeitado, target data rejeitado e persistencia recusada.

### Criterio de saida

- Sessao externa controlada sem blocker estrutural.
- Economy basica sem duplicacao conhecida.
- Bugs Critical zerados.

## 2027-07 - Conteudo e sistemas sociais basicos

### Planejado

- Expandir pontos de interesse.
- Adicionar mais inimigos.
- Adicionar mais loot.
- Adicionar mais armas.
- Adicionar mais abilities.
- Criar objetivos de mundo simples.
- Implementar party basica se o escopo permitir.
- Melhorar chat operacional.
- Preparar identificacao de player e base para guildas/clans.

### Criterio de saida

- Conteudo suficiente para sessoes repetidas sem depender apenas de teste tecnico.

## 2027-08 - Alpha ampliada

### Marco alvo

- Alpha ampliada.

### Planejado

- Abrir para grupo maior controlado.
- Testar 150 a 300 jogadores convidados em janelas curtas.
- Medir budget de replicacao por ator.
- Medir RPC/s por jogador.
- Analisar burst em combate e building mode.
- Validar economy com sinks e sources controlados.
- Documentar wipe policy.

### Criterio de saida

- Sessao ampliada sem falha estrutural de DS, persistencia ou replicacao.
- Loop de PvP/full loot compreensivel para testers sem intervencao direta.

## 2027-09 - Hardening para Beta fechada

### Planejado

- Executar soak test de DS.
- Revisar custo de servidor.
- Fechar antiexploit basico.
- Proteger economia contra duplicacao conhecida.
- Revisar rollback.
- Revisar hotfix process.
- Revisar moderacao minima.
- Preparar staging robusto.

### Criterio de saida

- Beta fechada pronta para escala controlada.
- CI multiplayer operacional e confiavel.

## 2027-10 - Beta fechada

### Marco alvo

- Beta fechada.

### Planejado

- Abrir para 300 a 800 testers por janela.
- Validar estabilidade.
- Validar combate em grupos.
- Validar loot sob carga.
- Validar reconnect em massa.
- Validar persistencia sob carga.
- Monitorar overflow de replicacao.

### Criterio de saida

- DS aguenta sessoes longas com telemetria aceitavel.
- Persistencia nao perde dados fora de rollback planejado.
- Exploits Critical de economia, loot e auth zerados.

## 2027-11 - Preparacao da Beta aberta limitada

### Planejado

- Ajustar onboarding.
- Ajustar balance pos-beta fechada.
- Corrigir exploits encontrados.
- Melhorar suporte basico.
- Validar funil de instalacao e login.
- Revisar processo de wipe.
- Preparar comunicacao de acesso controlado.

### Criterio de saida

- Beta aberta limitada pronta para janela publica curta.

## 2027-12 - Beta aberta limitada

### Marco alvo

- Beta aberta limitada.

### Planejado

- Abrir janela publica curta.
- Manter acesso controlado.
- Anunciar wipe se necessario.
- Coletar crash/log.
- Validar suporte basico.
- Medir retencao inicial.
- Medir estabilidade.
- Medir custo de servidor.
- Avaliar bugs S1/Critical.
- Avaliar exploits conhecidos.

### Criterio de saida

- Beta aberta limitada completa sem blocker estrutural.
- Decisao de lancamento 2028 documentada com risco real.

## 2028-03 - Hardening pos-beta

### Planejado

- Corrigir exploits restantes.
- Ajustar balance.
- Revisar custo de infraestrutura.
- Melhorar onboarding.
- Consolidar suporte.
- Decidir wipe final.
- Validar rollback e hotfix.

### Criterio de saida

- Projeto pronto para decisao final de release inicial.

## 2028-06 - Lancamento recomendado

### Marco alvo

- Lancamento inicial recomendado.

### Condicao minima

- Todos os gates de Beta aprovados.
- Nenhum Critical aberto.
- High bugs aceitos somente com mitigacao documentada.
- Persistencia auditavel.
- Economy server-side.
- Auth Steam validado.
- DS sem dependencia de HUD, widget, camera ou input local.
- Plano de wipe/rollback publicado internamente.

## Cenarios de lancamento

### 2027-12 - Cenario agressivo

- Early Access limitado junto da beta aberta limitada.
- Condicao: sem blocker Critical, sem exploit economico conhecido e com DS estavel.
- Risco: alto.

### 2028-06 - Cenario recomendado

- Early Access ou lancamento inicial.
- Condicao: beta aberta em 2027 concluida, ciclo de hardening em 2028-03 e staging estavel.
- Risco: medio.

### 2028-10 - Cenario conservador

- Lancamento se economy, mapa, building/siege, conteudo ou escala exigirem mais ciclos.
- Risco: baixo/medio.

## Resumo executivo

- 2027-01: vertical slice jogavel.
- 2027-02: readability de combate e estabilizacao interna.
- 2027-03: Pre-alpha interna.
- 2027-04: Pre-alpha fechada.
- 2027-05: preparacao da Alpha fechada.
- 2027-06: Alpha fechada.
- 2027-07: conteudo e sistemas sociais basicos.
- 2027-08: Alpha ampliada.
- 2027-09: hardening para Beta fechada.
- 2027-10: Beta fechada.
- 2027-11: preparacao da Beta aberta limitada.
- 2027-12: Beta aberta limitada.
- 2028-03: hardening pos-beta.
- 2028-06: lancamento inicial recomendado.
