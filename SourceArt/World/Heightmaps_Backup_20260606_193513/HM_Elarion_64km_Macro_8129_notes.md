# HM_Elarion_64km_Macro_8129

Heightmap macro unico para ter nocao do mundo completo de 64 km x 64 km.

## Importante
Este arquivo e para referencia de escala/blockout macro, nao para terreno final detalhado.
Aureth detalhada continua sendo tile/regiao separada.

Para terrain final, usar tiled landscape:
- grid: `8 x 8`;
- cada tile: `8129 x 8129`;
- cada tile: cerca de `8.128 km x 8.128 km`;
- escala XY por tile: `100, 100`;
- total aproximado: `65.024 km x 65.024 km`.

Nao usar este macro heightmap como terrain final porque a escala XY `787.5` deixaria a densidade do terreno baixa demais para gameplay de MMO em terceira pessoa.

## Configuracao Unreal
- Import from File.
- Heightmap: `SourceArt/World/Heightmaps/HM_Elarion_64km_Macro_8129.png`.
- Se perguntar `Tiled Image?`, responder `No`.
- Section Size: `127x127 Quads`.
- Sections Per Component: `2x2 Sections`.
- Number of Components: `32 x 32`.
- Overall Resolution: `8129 x 8129`.
- Scale XY para 64 km: `787.5, 787.5`.
- Scale Z inicial: `700`.
- Se relevo ficar baixo: testar Z `1000`.

## Resultado
- Canvas aproximado: 64 km x 64 km.
- Pangeia central Elarion.
- Ilhas: Frostvein, Glassmere, Rootwold, Ashkar.
- Oceano e shelf costeiro.
- Coracao Negro no centro.
