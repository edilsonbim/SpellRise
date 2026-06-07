# SpellRise - Escala do Mundo

## Objetivo
Definir uma escala de referência para Elarion antes de transformar o conceito em blockout 3D com World Partition.

## Documentos operacionais relacionados
- `Fontes/MapDesign/SpellRise_Map_Production_Playbook.md`
- `Fontes/MapDesign/SpellRise_Map_Conversation_Log.md`
- `Fontes/MapDesign/SpellRise_Elarion_Production_Stages.md`

## Decisão recomendada
Elarion deve ser planejado como um mundo grande desde o início, mas produzido em fases.

### Escala macro final aprovada
A escala de referencia aprovada para SpellRise e **64 km x 64 km** incluindo oceano, pangeia, ilhas externas e areas temporariamente bloqueadas.

- Mundo total com oceano: **64 km x 64 km**.
- Canvas total: **4096 km²**.
- Pangeia jogavel principal: aproximadamente **40 km x 36 km**.
- Ilhas externas: entre **6 km x 6 km** e **14 km x 12 km** cada.
- Mar navegavel entre pangeia e ilhas: minimo visual de **4 km a 14 km** de agua util.
- Area de terra final alvo: **700 km² a 1.400 km²** antes de descontar bloqueios, montanhas, mares internos e regioes temporariamente fechadas.

O mundo pode ser lancado com regioes bloqueadas e liberadas por temporada, expansao ou milestone de producao.

## Modelo de estimativa por tempo de travessia
Numeros publicos de jogos antigos variam muito, entao a referencia de design sera tempo de travessia.

Assumir faixas:
- caminhada humana real: **5 km/h**;
- corrida/autorun de MMO sem montaria: **10 km/h a 15 km/h**;
- montaria ou deslocamento rapido: **18 km/h a 28 km/h**.

### Darkfall / Agon
Referencias historicas citam:
- continente principal atravessado em cerca de **4 horas**;
- diagonal do mapa em cerca de **6 horas**;
- mundo com massa central e quatro subcontinentes.

Simulacao:
- se 4h forem caminhada lenta de 5 km/h: largura percebida de **20 km**;
- se 4h forem corrida MMO de 10 km/h: largura percebida de **40 km**;
- se 4h forem autorun rapido de 15 km/h: largura percebida de **60 km**.

Faixa util de design para Agon: **400 km² a 1.800 km²** de terra/mundo percebido, dependendo da velocidade real assumida.

### Mortal Online 2 / Myrland
Fontes de comunidade frequentemente citam cerca de **384 km²**, mas isso nao deve ser tratado como numero oficial rigido.

Simulacao:
- 384 km² equivale a um quadrado de aproximadamente **19.6 km x 19.6 km**;
- com deslocamento de 10 km/h, cruzar ponta a ponta levaria perto de **2h** se o caminho fosse direto;
- com terreno, montanhas, rotas indiretas e perigo PvP, a travessia percebida pode ficar muito maior.

Faixa util de design para Myrland: **300 km² a 600 km²** como estimativa conservadora de comunidade.

### Meta SpellRise
Para ficar na mesma familia de escala percebida:
- pangeia final deve mirar **700 km² a 1.400 km²** de terra relevante;
- mundo total com oceano e ilhas usa **4096 km²** de canvas;
- travessia da pangeia a pe deve ficar na faixa de **3h a 5h** sem teleporte/montaria;
- diagonal extrema do mundo com ilhas deve parecer **5h a 8h+**, antes de sistemas navais.

Essa meta preserva a fantasia de mundo enorme, mas permite cortar o escopo por regioes.

### Area produzida inicialmente
- Região inicial noroeste: **8 km x 8 km** de terreno representado.
- Área realmente jogável no primeiro corte: **4 km x 4 km**.
- Área detalhada de cidade/safe zone: **1.2 km x 1.2 km**.
- Faixa amarela inicial: até **2.5 km** a partir da cidade.
- Resto da região: bloqueado por relevo, costa, floresta densa, ponte quebrada, tempestade liminar ou portal inativo.

Esses valores sao referencia inicial, nao contrato fixo. A escala das zonas pode mudar por teste de fluxo, retencao e densidade de jogadores.

## Por que não começar pequeno
Um mapa de 2 km x 2 km serve para mecânica, mas não sustenta a fantasia de MMORPG full loot.

O macro precisa existir cedo para:
- posicionar cidades raciais;
- planejar rotas comerciais;
- reservar espaço de navegação;
- evitar mundo colado artificialmente depois;
- distribuir cidades de clã, hamlets e vilas;
- controlar progressão de risco verde, amarelo, vermelho e preto.

## Por que não produzir tudo agora
Um mundo 64 km x 64 km completo exigiria arte, streaming, HLOD, PCG, colisão, QA, navegação e validação multiplayer demais para o estágio atual.

O caminho correto é:
1. Criar o mundo macro com escala final.
2. Bloquear áreas não produzidas.
3. Detalhar uma região inicial.
4. Validar Standalone, Listen Server e Dedicated Server.
5. Expandir por bioma/região.

## Distancias de gameplay alvo
- Cidade racial ate primeira vila: **300 m a 900 m**.
- Cidade racial ate borda da zona verde: **1.5 km a 3.5 km**.
- Cidade racial ate primeira zona amarela real: **2 km a 4 km**.
- Cidade racial ate primeira cidade de cla proxima: **4 km a 7 km**.
- Cidade racial ate zona vermelha: **7 km a 11 km**.
- Cidade racial ate centro preto da pangeia: **15 km a 24 km**.
- Travessia costeira curta futura: **4 km a 8 km**.
- Travessia maritima longa futura: **10 km a 18 km**.

## Politica de risco por zona
- **Verde**: maior que o primeiro plano original para acolher jogador casual, aprendizado, craft basico, social, mercado local e gathering comum.
- **Amarela**: risco moderado, perda parcial da bag, recursos intermediarios e primeiras rotas de disputa.
- **Vermelha**: full loot com consequencia/reputacao para agressor, recursos raros, entradas de dungeon e cidades de cla relevantes.
- **Preta**: full loot sem punicao, recursos maximos, bosses, ruinas liminares, zonas de colapso, cidadelas e dungeons mais valiosas.

## Dungeons e recursos
- Dungeons de alto valor devem ficar em zonas vermelhas e pretas.
- Dungeons iniciais podem existir em verde/amarela, mas com recompensa limitada e funcao de aprendizado.
- Recursos raros nao devem nascer em zona verde.
- Zona verde pode ter recursos comuns e economia basica para sustentar jogadores casuais sem competir com risco alto.

## Produção inicial recomendada
Primeiro blockout real:
- Level: `/Game/Maps/SpellRiseWorld/LV_Elarion_World`.
- World Partition obrigatório.
- Terreno macro: **64 km x 64 km**.
- Região noroeste detalhada em **8 km x 8 km**.
- Área jogável inicial em **4 km x 4 km**.
- Pangeia e ilhas podem existir como silhueta/heightfield bruto, mas bloqueadas.

## Regras técnicas
- Dedicated Server não depende de HUD, widget, câmera ou lógica local.
- Zonas de risco são server-authoritative.
- Teleportes validam origem, destino, cooldown, combate recente e estado de loot.
- Dungeons ficam instanciadas.
- O mar é reservado para navegação futura, não usado como simples fim de mapa.

## Riscos
- Mundo grande sem World Partition e HLOD vai quebrar produção.
- Clutter visual cedo demais atrapalha leitura de PvP.
- Distâncias grandes sem montaria, teleporte ou objetivos intermediários geram tédio.
- Zonas bloqueadas precisam parecer naturais, não parede invisível.

## Decisão prática
Usar **64 km x 64 km** como canvas final, mas produzir primeiro apenas o noroeste.

Isso preserva a ambição de Darkfall/Mortal Online sem obrigar produção AAA completa no primeiro corte.
