# SpellRise - Registro de Conversas sobre Mapa

## 2026-06-06 - Direcao inicial de mundo
Decisao:
- O mapa deve seguir uma direcao de MMO hardcore, dark fantasy, full loot e server-authoritative.
- Referencias principais: Darkfall Unholy Wars, Mortal Online 2, Albion Online e Game of Thrones.
- Foi descartada a ideia de pensar apenas em um mapa pequeno isolado.

Resultado:
- Criada a direcao de pangeia central com quatro ilhas externas.
- Definido que o mundo pode nascer grande, mas ser produzido por partes.

Arquivos relacionados:
- `Fontes/MapDesign/SpellRise_World_Macro_Map.md`
- `Fontes/MapDesign/SpellRise_World_Macro_Map.svg`

## 2026-06-06 - Conceito satelite
Problema:
- A primeira planta SVG parecia cartografica, nao imagem de satelite.

Decisao:
- Criar um bitmap com leitura de satelite/orbital.

Resultado:
- Criado `SpellRise_Elarion_Satellite_Concept_v01.png`.
- O PNG passou a ser a referencia visual primaria.

Arquivos relacionados:
- `Fontes/MapDesign/SpellRise_Elarion_Satellite_Concept_v01.png`
- `Fontes/MapDesign/SpellRise_Elarion_Regions.md`

## 2026-06-06 - Assentamentos e nomes
Decisao:
- Usar estrutura inspirada em Darkfall: cidades de cla, hamlets, vilas e capitais raciais.
- Usar nomes em estilo anglo-fantasy/RPG, nao portugues literal.

Capitais aprovadas:
- Aureth
- Kharven
- Mournfall
- Drakholm

Resultado:
- Criado overlay de assentamentos.
- Criada lista inicial com 116 pontos nomeados.

Arquivos relacionados:
- `Fontes/MapDesign/SpellRise_Elarion_Settlements_v01.png`
- `Fontes/MapDesign/SpellRise_Elarion_Settlements_v01.md`

## 2026-06-06 - Escala final
Discussao:
- Darkfall e Mortal Online 2 tem numeros publicos inconsistentes.
- A referencia de design mais util e tempo de travessia percebido.

Decisao:
- SpellRise tera mundo final de **64 km x 64 km**.
- Canvas total: **4096 km²**.
- Pangeia e ilhas serao criadas por fases.
- O jogo pode lancar com regioes bloqueadas e liberar conteudo com o tempo.

Resultado:
- Documento de escala atualizado.

Arquivos relacionados:
- `Fontes/MapDesign/SpellRise_World_Scale.md`

## 2026-06-06 - Estagios de producao
Decisao:
- Primeiro foco: regiao de Aureth, no noroeste de Elarion.
- A regiao inicial deve conter zona verde maior para jogadores casuais.
- Dungeons e recursos raros devem ficar em areas de maior risco.

Ilhas nomeadas:
- Frostvein
- Glassmere
- Rootwold
- Ashkar

Resultado:
- Criado roadmap por estagios.

Arquivos relacionados:
- `Fontes/MapDesign/SpellRise_Elarion_Production_Stages.md`

## Politica de atualizacao
Sempre que houver nova decisao sobre mapa, regioes, nomes, escala, pipeline, zonas de risco ou ferramentas, atualizar:
1. este arquivo;
2. o documento especifico afetado;
3. `SpellRise_Map_Production_Playbook.md` quando a decisao mudar direcao de producao.

## 2026-06-06 - Landscape Technical Guide
Fonte consultada:
- `https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine`

Pontos tecnicos registrados:
- Components sao unidade base de render, visibilidade e colisao.
- Cada Component tem custo de CPU/render thread.
- Cada section e um draw call.
- Para Landscapes grandes, a Epic recomenda no maximo **1024 Landscape Components**.
- Tamanho recomendado maximo por Landscape: **8129 x 8129 vertices** com **32 x 32 components**.

Decisao:
- Nao criar um unico Landscape 64 km x 64 km com 16.129 components.
- Criar Elarion como grid logico de **8 x 8 tiles**, cada tile com cerca de **8 km x 8 km**.
- Primeiro tile real de producao: Aureth.

## 2026-06-06 - Heightmap inicial de Aureth
Decisao:
- Criar um heightmap 16-bit inicial para o tile Aureth.
- Tamanho: **8129 x 8129**.
- Uso: blockout/import inicial, nao arte final.

Arquivos criados:
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v02_r16.png`
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v02_preview.png`
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v02_notes.md`
- `Scripts/World/GenerateAurethHeightmap.py`

Intencao do relevo:
- costa no oeste/sul;
- plateau de Aureth no noroeste interno;
- primeira vila ao sudeste;
- depressao/ruina de dungeon inicial a leste;
- rio principal rumo a costa sudoeste;
- highlands no norte/leste para bloqueios diegeticos e costura futura.

## 2026-06-06 - Correcao sobre terrain final 64 km
Decisao:
- O heightmap macro unico de 64 km nao deve ser usado como terrain final.
- Para terrain final, usar tiled landscape em grid **8 x 8**.
- Cada tile deve ter **8129 x 8129** vertices e `Scale XY = 100`.
- O mundo final fica com aproximadamente **65 km x 65 km**, alinhado com a meta de 64 km x 64 km.

Motivo:
- Um unico heightmap `8129 x 8129` escalado para 64 km teria densidade horizontal baixa demais para gameplay final.
- Tiled landscape preserva densidade, edicao por regiao e compatibilidade com World Partition.

## 2026-06-06 - Medieval Docks como Aureth
Decisao:
- Usar `/Game/Hivemind/Medieval_Docks/Levels/PL_Medieval_Docks` como candidato/base visual para a cidade inicial **Aureth**.
- Aureth passa a ser concebida como cidade costeira/porto em baia no noroeste de Elarion.

Inspecao:
- O level contem sublevels de buildings, docks, foliage, landscape, water e props.
- Existem atores grandes de sky/weather que precisam ser revisados antes de uso em Dedicated Server.
- O pacote ja aparece em `MapsToCook` e tambem como `ServerDefaultMap` em `DefaultEngine.ini`.

Arquivos gerados:
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v03_docks_r16.png`
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v03_docks_preview.png`
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v03_docks_notes.md`

Risco:
- Reaproveitar asset pronto acelera producao, mas exige auditoria de dependencies, water, sky/weather, foliage, collisions e runtime no Dedicated Server.

## 2026-06-06 - Duplicacao de Medieval Docks para Aureth
Decisao:
- Seguir a opcao B: duplicar/adaptar o conteudo para namespace de Aureth.
- O asset original em `/Game/Hivemind/Medieval_Docks` nao deve ser alterado diretamente.

Destino criado:
- `/Game/Maps/SpellRiseWorld/Aureth/Medieval_Docks/Levels`

Script:
- `Scripts/World/DuplicateMedievalDocksForAureth.py`

Proximo passo:
- Abrir o level duplicado e validar sublevels/referencias.
- Depois integrar visualmente ao `LV_Aureth` como cidade costeira.
