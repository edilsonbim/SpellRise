from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[2]
OUT_DIR = ROOT / "SourceArt" / "World" / "Heightmaps"
HEIGHTMAP_PATH = OUT_DIR / "HM_Aureth_Tile_8129_v04_docks.png"
RAW_R16_PATH = OUT_DIR / "HM_Aureth_Tile_8129_v04_docks.r16"
PREVIEW_PATH = OUT_DIR / "HM_Aureth_Tile_8129_v04_docks_preview.png"
NOTES_PATH = OUT_DIR / "HM_Aureth_Tile_8129_v04_docks_notes.md"

SIZE = 8129


def smooth_noise(size, rng, grid):
    small = rng.random((grid, grid), dtype=np.float32)
    image = Image.fromarray((small * 255).astype(np.uint8), mode="L")
    image = image.resize((size, size), Image.Resampling.BICUBIC)
    return np.asarray(image, dtype=np.float32) / 255.0


def ridge(distance, width):
    return np.clip(1.0 - np.abs(distance) / width, 0.0, 1.0)


def smoothstep(edge0, edge1, value):
    t = np.clip((value - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(5713)

    y, x = np.mgrid[0:SIZE, 0:SIZE].astype(np.float32)
    u = x / (SIZE - 1)
    v = y / (SIZE - 1)

    # Base continental tilt: higher inland north/east, lower toward west/south coast.
    h = 0.30 + 0.22 * u + 0.14 * (1.0 - v)

    # Multi-scale erosion-like variation.
    h += 0.105 * smooth_noise(SIZE, rng, 33)
    h += 0.055 * smooth_noise(SIZE, rng, 81)
    h += 0.025 * smooth_noise(SIZE, rng, 193)

    # Sea/coast on west and south with organic embayments.
    harbor_bay = 0.145 * np.exp(-(((v - 0.43) / 0.13) ** 2))
    coast_west = 0.115 + harbor_bay + 0.045 * np.sin(v * 10.0) + 0.030 * smooth_noise(SIZE, rng, 47)
    coast_south = 0.145 + 0.050 * np.sin(u * 9.0 + 1.4) + 0.025 * smooth_noise(SIZE, rng, 55)
    sea_west = np.clip((coast_west - u) / 0.120, 0.0, 1.0)
    sea_south = np.clip((v - (1.0 - coast_south)) / 0.120, 0.0, 1.0)
    sea = np.maximum(sea_west, sea_south)
    h = h * (1.0 - sea) + (0.145 + 0.025 * smooth_noise(SIZE, rng, 29)) * sea

    # Coastal shelf/lowlands.
    coast_influence = np.clip((0.31 - np.minimum(u, 1.0 - v)) / 0.31, 0.0, 1.0)
    h -= 0.075 * coast_influence * (1.0 - sea)

    # Aureth city plateau: coastal harbor town sized to host Medieval_Docks as the initial city.
    cx, cy = 0.315, 0.425
    d_city = np.sqrt(((u - cx) / 0.155) ** 2 + ((v - cy) / 0.120) ** 2)
    plateau_mask = np.clip(1.0 - d_city, 0.0, 1.0) ** 2
    plateau_height = 0.355 + 0.006 * smooth_noise(SIZE, rng, 101)
    h = h * (1.0 - plateau_mask * 0.86) + plateau_height * (plateau_mask * 0.86)

    # Harbor inlet directly west of Aureth, kept low for docks/water integration.
    harbor = np.exp(-(((u - 0.185) / 0.075) ** 2 + ((v - 0.445) / 0.105) ** 2))
    h = h * (1.0 - harbor * 0.72) + (0.170 + 0.010 * smooth_noise(SIZE, rng, 131)) * (harbor * 0.72)

    # Breakwater/headland north and south of the bay.
    north_headland = np.exp(-(((u - 0.205) / 0.050) ** 2 + ((v - 0.315) / 0.055) ** 2))
    south_headland = np.exp(-(((u - 0.210) / 0.055) ** 2 + ((v - 0.565) / 0.060) ** 2))
    h += 0.075 * (north_headland + south_headland)

    # Northern/eastern blocked highlands for future tile seams and diegetic barriers.
    north_highlands = np.clip((0.18 - v) / 0.18, 0.0, 1.0)
    east_highlands = np.clip((u - 0.78) / 0.22, 0.0, 1.0)
    h += 0.260 * north_highlands ** 1.7
    h += 0.320 * east_highlands ** 1.5

    # Mountain chains and ridges.
    h += 0.190 * ridge((v - (0.17 + 0.11 * np.sin(u * 7.5))), 0.045) * smoothstep(0.28, 0.42, u)
    h += 0.230 * ridge((u - (0.77 + 0.045 * np.sin(v * 9.0))), 0.050) * (1.0 - smoothstep(0.76, 0.90, v))
    h += 0.145 * ridge((v - (0.68 - 0.15 * u + 0.04 * np.sin(u * 12.0))), 0.038) * smoothstep(0.30, 0.46, u)

    # Main river valley from northern hills toward southwest coast.
    river_center = 0.34 + 0.07 * np.sin(v * 7.0 + 0.3) + 0.15 * v
    river_valley = ridge(u - river_center, 0.030) * smoothstep(0.16, 0.24, v) * (1.0 - smoothstep(0.82, 0.90, v))
    h -= 0.170 * river_valley

    # Secondary creek crossing the initial yellow-zone route.
    creek_center = 0.58 - 0.20 * u + 0.035 * np.sin(u * 14.0)
    creek = ridge(v - creek_center, 0.022) * smoothstep(0.18, 0.28, u) * (1.0 - smoothstep(0.66, 0.76, u))
    h -= 0.055 * creek

    # Dungeon/ruin depression east of Aureth.
    dx, dy = (u - 0.53) / 0.085, (v - 0.43) / 0.065
    ruin = np.clip(1.0 - np.sqrt(dx * dx + dy * dy), 0.0, 1.0) ** 2
    h -= 0.070 * ruin
    h += 0.030 * ridge(np.sqrt(dx * dx + dy * dy) - 0.9, 0.18)

    # First village/field low plateau south-east of city.
    vx, vy = 0.50, 0.60
    village = np.clip(1.0 - np.sqrt(((u - vx) / 0.075) ** 2 + ((v - vy) / 0.055) ** 2), 0.0, 1.0) ** 2
    h = h * (1.0 - village * 0.50) + (0.355 + 0.006 * smooth_noise(SIZE, rng, 117)) * (village * 0.50)

    # Keep future seam borders broad, not final-detail noisy.
    border = np.maximum.reduce([
        np.clip((0.055 - u) / 0.055, 0.0, 1.0),
        np.clip((u - 0.945) / 0.055, 0.0, 1.0),
        np.clip((0.055 - v) / 0.055, 0.0, 1.0),
        np.clip((v - 0.945) / 0.055, 0.0, 1.0),
    ])
    macro = 0.34 + 0.13 * u + 0.08 * (1.0 - v)
    h = h * (1.0 - border * 0.55) + macro * (border * 0.55)

    h = np.clip(h, 0.0, 1.0)
    height_u16 = (h * 65535.0).astype(np.uint16)

    Image.fromarray(height_u16, mode="I;16").save(HEIGHTMAP_PATH)
    height_u16.tofile(RAW_R16_PATH)
    preview = ((h - h.min()) / max(h.max() - h.min(), 0.0001) * 255.0).astype(np.uint8)
    Image.fromarray(preview, mode="L").save(PREVIEW_PATH)

    NOTES_PATH.write_text(
        "\n".join([
            "# HM_Aureth_Tile_8129_v04_docks",
            "",
            "Heightmap 16-bit para o primeiro tile de Aureth.",
            "",
            "## Configuracao Unreal",
            "- Import from File.",
            "- Heightmap preferencial: `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v04_docks.r16`.",
            "- Alternativa PNG: `SourceArt/World/Heightmaps/HM_Aureth_Tile_8129_v04_docks.png`.",
            "- Section Size: `127x127 Quads`.",
            "- Sections Per Component: `2x2 Sections`.",
            "- Number of Components: `32 x 32`.",
            "- Overall Resolution: `8129 x 8129`.",
            "- Scale recomendado: `100, 100, 300` para relevo visivel.",
            "- Se ainda ficar baixo, testar Z Scale `500`.",
            "- Tamanho aproximado: `8.128 km x 8.128 km`.",
            "",
            "## Intencao do relevo",
            "- Costa orgânica no oeste/sul.",
            "- Baia/porto no oeste para encaixar `PL_Medieval_Docks` como Aureth.",
            "- Plateau costeiro maior para cidade inicial.",
            "- Primeira vila ao sudeste da cidade.",
            "- Depressao/ruina de dungeon inicial a leste.",
            "- Rio principal descendo para a costa sudoeste.",
            "- Highways naturais por vales.",
            "- Norte/leste com highlands e bloqueios diegeticos.",
            "- Bordas suavizadas para costura futura com tiles vizinhos.",
            "",
            "## Regra de producao",
            "Detalhar o centro de Aureth primeiro. Nao finalizar as bordas antes de criar os tiles vizinhos.",
            "",
        ]),
        encoding="utf-8",
    )

    print(HEIGHTMAP_PATH)
    print(PREVIEW_PATH)
    print(NOTES_PATH)


if __name__ == "__main__":
    main()
