# SpellRise - Playbook de Producao do Mapa

## Objetivo
Manter a direcao viva da criacao de Elarion e registrar as decisoes tomadas durante a producao do mapa.

Este documento deve ser atualizado a cada nova rodada relevante de conversa sobre mapa, mundo, regioes, ferramentas, escala, biome, assentamentos, zonas de risco ou pipeline.

## Direcao aprovada
- Mundo final: **64 km x 64 km**.
- Estrutura: pangeia central **Elarion** + 4 ilhas externas.
- Producao por partes.
- Lancamento pode acontecer com areas bloqueadas e liberacao progressiva.
- Primeiro foco de producao: **Aureth**, no noroeste da pangeia.
- Criar o canvas macro do mundo, mas editar de verdade apenas Aureth no primeiro ciclo.

## Referencias principais
- Darkfall Unholy Wars: massa central + quatro terras externas, teleportes, PvP aberto, holdings e mundo grande.
- Mortal Online 2: escala sandbox, deslocamento com peso, mundo persistente e territorial.
- Game of Thrones: costas irregulares, regioes politicas distintas e geografia com identidade.
- Albion Online: zonas de risco verde, amarela, vermelha e preta como referencia de progressao de perigo.

## Ferramentas e tecnologias recomendadas
### Unreal Engine 5.7
- Base do projeto.
- Dedicated Server obrigatorio.
- World Partition para mundo grande.
- Data Layers para regioes bloqueadas, eventos, fases e expansoes.
- HLOD para representacao distante.
- Nanite para rochas, ruinas, cliffs, arquitetura e props de alto detalhe.
- Lumen para iluminacao dinamica e ciclo dia/noite, com budget de performance.

### World Partition
Uso obrigatorio para o mundo final.

Direcao:
- Criar canvas 64 km x 64 km.
- Nao detalhar tudo de uma vez.
- Carregar/editar regioes por celula.
- Usar Data Layers para bloquear conteudo nao pronto.

### Landscape - regra tecnica Epic
Referencia oficial: `Landscape Technical Guide` da Epic.

Pontos importantes:
- Landscape e dividido em Components.
- Components sao unidade base de render, visibilidade e colisao.
- Cada Component tem custo de CPU/render thread.
- Cada section e um draw call.
- Para Landscapes grandes, a Epic recomenda no maximo **1024 Landscape Components** por Landscape.
- Tamanho recomendado maximo por Landscape: **8129 x 8129 vertices**, usando `127 quads/section`, `2x2 sections/component`, `32x32 components`.

Implicacao para SpellRise:
- Nao criar um unico Landscape de **64771 x 64771** com **127 x 127 components**.
- Esse setup gera **16.129 components**, muito acima da recomendacao.
- O mundo 64 km x 64 km deve ser composto por tiles/regioes, nao por um unico Landscape gigante editavel.

Configuracao recomendada para tiles:
- Cada tile principal: **8129 x 8129 vertices**.
- Escala XY: **100**.
- Tamanho aproximado por tile: **8.128 km x 8.128 km**.
- Components por tile: **32 x 32 = 1024**.
- Quads/section: **127**.
- Sections/component: **2x2**.

Canvas 64 km x 64 km:
- Usar grid logico de **8 x 8 tiles**.
- Cada tile representa cerca de **8 km x 8 km**.
- Total aproximado: **64 km x 64 km**.
- Primeiro tile de producao: Aureth/noroeste.

Regra para terrain final:
- Nao usar um unico heightmap `8129 x 8129` escalado para 64 km como terrain final.
- Esse macro unico serve apenas para escala/preview.
- Terrain final deve ser importado como **tiled landscape 8 x 8**, com 64 heightmaps.
- Cada tile deve manter `Scale XY = 100` para preservar densidade proxima de 1 metro por vertex.

### PCG
Uso recomendado para povoamento de ambiente:
- florestas;
- pedras;
- ruinas;
- grama;
- props naturais;
- distribuicao por bioma.

Regra:
- Preferir PCG em editor/bake para ambiente.
- Nao usar geracao procedural runtime para gameplay critico sem necessidade.

### Gaea / World Machine / ferramenta de terrain externo
Uso recomendado para heightmaps macro:
- erosao;
- montanhas;
- rios;
- costas;
- mascaras de bioma;
- slope/altitude/flow maps.

Para SpellRise, Gaea e a opcao preferencial pelo custo-beneficio e fluxo com Unreal.

### Fab / Megascans
Uso recomendado para acelerar prototipo visual:
- rochas;
- cliffs;
- materiais;
- ruinas;
- vegetacao base;
- props de ambiente.

Regra:
- Usar como base produtiva, nao como identidade final unica.
- Curadoria forte para evitar mapa generico.

### SpeedTree / vegetacao
Uso recomendado para vegetacao AAA quando a direcao visual estiver mais madura.

Regra:
- Vegetacao nunca pode destruir legibilidade de PvP free target.
- Floresta deve ter funcao de rota, cobertura, emboscada e leitura visual.

## Decisoes de mundo
### Elarion
- Continente ferido pela Ruptura do Limiar.
- Antes era dividido por Sete Reinos.
- Agora e dividido por risco.
- Centro da pangeia e o **Coracao Negro**, ligado a Primeira Ancora/Zona de Colapso.

### Ilhas externas
- **Frostvein**: ilha boreal, aurora morta, gelo liminar, tempo quebrado.
- **Glassmere**: deserto cristalizado, sal, miragens, Dragões de Vidro.
- **Rootwold**: selva/pantano, serpes de raiz, madeira rara, venenos.
- **Ashkar**: vulcanica/cinzas, metal negro, Dragões de Cinza.

### Capitais raciais
Nomes aprovados em estilo anglo-fantasy:
- **Aureth**: noroeste, primeira regiao jogavel.
- **Kharven**: montanhas e mineracao.
- **Mournfall**: brejos, alquimia e rotas costeiras.
- **Drakholm**: cinzas, metal raro e fortalezas quebradas.

## Zonas de risco
- **Verde**: safe zone maior para jogadores casuais, aprendizado, craft basico, coleta comum, social e mercado local.
- **Amarela**: risco moderado, drop parcial da bag, recursos intermediarios e PvP leve.
- **Vermelha**: full loot com consequencia/reputacao para agressor, recursos raros e entradas de dungeons valiosas.
- **Preta**: full loot sem punicao, recursos maximos, bosses, ruinas liminares, cidadelas e dungeons mais valiosas.

Regra:
- Recursos raros nao nascem em zona verde.
- Dungeons de alto valor ficam em vermelha/preta.
- Dungeons iniciais podem existir em verde/amarela, mas com recompensa limitada.

## Pipeline recomendado
### Fase 1 - Preparar canvas macro
1. Criar novo level para mundo final: `/Game/Maps/SpellRiseWorld/LV_Elarion_World`.
2. Ativar World Partition.
3. Definir escala macro 64 km x 64 km como grid logico de **8 x 8 tiles**.
4. Criar primeiro tile real: Aureth, com **8129 x 8129** vertices.
5. Deixar demais tiles como planejados/bloqueados, nao como Landscape detalhado.
6. Criar ou importar base macro bruta da pangeia e ilhas apenas como referencia/mesh/placeholder leve.
7. Marcar areas bloqueadas em Data Layers.
8. Nao detalhar regioes fora de Aureth.

### Fase 2 - Aureth blockout
1. Criar recorte inicial de Aureth.
2. Representar **8 km x 8 km**.
3. Tornar jogavel inicialmente **4 km x 4 km a 6 km x 6 km**.
4. Criar costa recortada, floresta sombria, cidade, vila, rota de coleta, dungeon inicial e bordas bloqueadas.
5. Testar escala andando.
6. Ajustar distancias antes de arte final.

Heightmap inicial:
- `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v03_docks_r16.png`
- Preview: `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v03_docks_preview.png`
- Notas: `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v03_docks_notes.md`

Base visual de Aureth:
- Candidato original: `/Game/Hivemind/Medieval_Docks/Levels/PL_Medieval_Docks`.
- Copia de trabalho: `/Game/Maps/SpellRiseWorld/Aureth/Medieval_Docks/Levels/PL_Medieval_Docks`.
- Usar como cidade costeira/porto inicial.
- Antes de integrar ao mundo final, auditar sky/weather, water, foliage, colisao e dependencias para Dedicated Server.

### Fase 3 - Gameplay de regiao
1. Cidade racial Aureth.
2. Zona verde maior.
3. Primeira vila/township.
4. Zona amarela.
5. Dungeon inicial instanciada.
6. Recursos comuns e intermediarios.
7. Primeiro ponto de PvP leve.
8. Primeiro portal antigo inativo.

### Fase 4 - Arte e PCG
1. Landscape material inicial.
2. Relevo organico.
3. PCG de floresta, pedras e ruinas.
4. Cliffs/costa.
5. Lighting dark fantasy.
6. Passada de legibilidade de PvP.

### Fase 5 - Validacao multiplayer
1. Standalone.
2. Listen Server.
3. Dedicated Server + 2 clientes.
4. Lag/loss quando tocar teleporte, loot, dungeon, RPC, combate, recursos ou sistema net-critical.

## Regras tecnicas
- Dedicated Server nao depende de HUD, widget, camera ou UI.
- Mapa nao decide loot, dano, persistencia, morte ou resultado de gameplay.
- Zonas de risco sao server-authoritative.
- Recursos, dungeons, teleportes e loot precisam de validacao server-side.
- Teleportes validam origem, destino, cooldown, combate recente, carga e estado de loot.
- Dungeons devem ser instanciadas para proteger performance.
- Multicast apenas para apresentacao, nunca decisao de gameplay.

## Proximo passo recomendado
Criar a planta detalhada de **Aureth**:
- cidade racial;
- safe zone;
- primeira vila;
- zona amarela;
- dungeon inicial;
- floresta;
- costa;
- rotas;
- bloqueios naturais.

Depois transformar essa planta em blockout 3D no Unreal.
