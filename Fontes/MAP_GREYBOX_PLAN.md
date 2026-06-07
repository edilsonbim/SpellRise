# SpellRise Map Greybox Plan

## Objetivo
Criar o primeiro mapa proprio jogavel do SpellRise como vertical slice de mundo PvP full loot, antes de fidelidade visual.

## Escopo inicial
- Level alvo: `/Game/Maps/SpellRiseWorld/LV_SpellRise_Greybox`.
- Area jogavel: `2 km x 2 km`.
- Foco: rotas, leitura de combate, POIs e risco de emboscada.
- Fora do escopo: arte final, PCG final, economia definitiva, persistencia de recursos e eventos live.

## Layout
- Centro: ruinas de catalyst como hotspot PvP.
- Norte: floresta de emboscada com cobertura alta.
- Leste: ridge de mineracao com vantagem de altura.
- Sul: costa/extracao com rotas abertas.
- Oeste: pantano de ervas raras com mobilidade e visibilidade piores.
- Rotas principais: eixo norte-sul e leste-oeste.
- Atalhos de risco: diagonais passando perto do centro.

## Regras de gameplay
- O mapa nao decide loot, dano, recurso, morte ou persistencia.
- Qualquer recurso interativo futuro deve ser validado no servidor.
- Obstaculos do greybox existem para testar linha de visao, colisao e leitura de free target.
- Vegetacao futura nao pode destruir legibilidade de PvP.

## Server
- Dedicated Server deve carregar o mapa sem depender de HUD, widget, camera ou logica local.
- Spawns iniciais ficam afastados para smoke DS+2.
- Nenhum estado autoritativo novo e introduzido neste passo.

## Client
- Cliente apenas renderiza ambiente e coleta input normal.
- Sem prediction nova neste passo.
- Sem UI obrigatoria para carregar o mapa.

## Riscos
- Streaming/World Partition ainda nao validado neste primeiro greybox.
- Tamanho final e densidade visual podem afetar performance se forem definidos antes de testes.
- Cobertura excessiva pode prejudicar free target.
- POIs precisam ser testados com dois jogadores antes de virar arte final.

## Checklist de teste
1. Abrir `/Game/Maps/SpellRiseWorld/LV_SpellRise_Greybox` no editor.
2. Verificar escala de deslocamento entre spawns e centro.
3. Testar Standalone com personagem real.
4. Testar Listen Server com dois players.
5. Testar Dedicated Server + 2 clientes.
6. Validar sem erro de load, colisao bloqueante indevida ou queda infinita.
7. Medir leitura de combate no centro, floresta e ridge.
8. Antes de arte final, decidir World Partition, HLOD e Data Layers.
