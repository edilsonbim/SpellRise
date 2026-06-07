# SpellRise - Macro Mapa Mundial

## Objetivo
Definir uma planta macro original para o mundo do SpellRise antes de expandir o greybox 3D.

## Referencias de direcao
- Darkfall Unholy Wars: continente central com quatro terras externas e foco PvP aberto.
- Mortal Online 2: mundo sandbox grande, leitura territorial e deslocamento com peso.
- Game of Thrones: geografia politica com regiões muito distintas, costas irregulares e mares relevantes.
- Geografia real: massas de terra assimetricas, baías, peninsulas, estreitos e ilhas com tamanhos diferentes.

## Arquivo visual
- `Fontes/MapDesign/SpellRise_World_Macro_Map.svg`

## Estrutura
- Pangeia central irregular.
- Quatro ilhas grandes em volta.
- Mar largo o bastante para virar sistema de navegacao no futuro.
- Quatro cidades raciais na pangeia.
- Safe zone verde ao redor de cada cidade racial.
- Zonas amarelas, vermelhas e pretas por distancia/risco.
- Centro da pangeia como zona preta principal.
- Teleportes antigos para ilhas e rotas longas enquanto barcos nao existem.

## Regiao inicial recomendada
Comecar no noroeste da pangeia:
- uma cidade racial;
- floresta dark fantasy;
- costa proxima;
- zona verde;
- primeira faixa amarela;
- bordas bloqueadas para areas ainda nao trabalhadas.

## Regras de producao
- O macro mapa pode existir desde ja, mas so uma regiao vira gameplay primeiro.
- Areas nao trabalhadas ficam bloqueadas por relevo, mar, muralha natural, tempestade ou portal inativo.
- Zonas de risco devem ser server-authoritative.
- Teleportes devem validar origem, destino, cooldown, combate recente, carga e estado de loot.
- Dungeons devem ser instanciadas para proteger performance do Dedicated Server.

## Proximo passo
Substituir o `LV_SpellRise_Greybox` por um blockout macro maior:
- massa continental grande;
- mar ao redor;
- primeira regiao jogavel detalhada;
- demais areas representadas mas bloqueadas;
- marcadores de cidade racial, zona verde, zona amarela e portal antigo.
