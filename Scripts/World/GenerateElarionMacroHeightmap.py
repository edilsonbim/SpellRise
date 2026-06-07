from pathlib import Path

import numpy as np
from PIL import Image


ROOT = Path(__file__).resolve().parents[2]
OUT_DIR = ROOT / "SourceArt" / "World" / "Heightmaps"
HEIGHTMAP_PATH = OUT_DIR / "HM_Elarion_64km_Macro_8129.png"
PREVIEW_PATH = OUT_DIR / "HM_Elarion_64km_Macro_8129_preview.png"
NOTES_PATH = OUT_DIR / "HM_Elarion_64km_Macro_8129_notes.md"

SIZE = 8129


def smooth_noise(size, rng, grid):
    small = rng.random((grid, grid), dtype=np.float32)
    image = Image.fromarray((small * 255).astype(np.uint8), mode="L")
    image = image.resize((size, size), Image.Resampling.BICUBIC)
    return np.asarray(image, dtype=np.float32) / 255.0


def smoothstep(edge0, edge1, value):
    t = np.clip((value - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def ellipse_field(u, v, cx, cy, rx, ry, angle=0.0):
    ca, sa = np.cos(angle), np.sin(angle)
    x = u - cx
    y = v - cy
    xr = x * ca + y * sa
    yr = -x * sa + y * ca
    return np.sqrt((xr / rx) ** 2 + (yr / ry) ** 2)


def land_from_ellipses(u, v, ellipses):
    value = np.zeros_like(u, dtype=np.float32)
    for cx, cy, rx, ry, angle, weight in ellipses:
        d = ellipse_field(u, v, cx, cy, rx, ry, angle)
        value = np.maximum(value, weight * (1.0 - d))
    return value


def ridge(distance, width):
    return np.clip(1.0 - np.abs(distance) / width, 0.0, 1.0)


def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(64064)

    y, x = np.mgrid[0:SIZE, 0:SIZE].astype(np.float32)
    u = x / (SIZE - 1)
    v = y / (SIZE - 1)

    n1 = smooth_noise(SIZE, rng, 45)
    n2 = smooth_noise(SIZE, rng, 113)
    n3 = smooth_noise(SIZE, rng, 257)
    coast_noise = (n1 - 0.5) * 0.18 + (n2 - 0.5) * 0.08

    pangea = [
        (0.47, 0.48, 0.30, 0.38, -0.18, 1.20),
        (0.39, 0.31, 0.18, 0.20, 0.35, 0.95),
        (0.60, 0.33, 0.19, 0.22, -0.55, 0.90),
        (0.35, 0.66, 0.20, 0.24, -0.28, 0.92),
        (0.59, 0.67, 0.22, 0.23, 0.42, 0.94),
        (0.50, 0.78, 0.18, 0.16, 0.10, 0.70),
        (0.24, 0.48, 0.12, 0.18, 0.08, 0.56),
        (0.74, 0.51, 0.13, 0.18, -0.20, 0.58),
    ]
    islands = [
        (0.31, 0.12, 0.115, 0.065, 0.08, 0.80),  # Frostvein
        (0.79, 0.22, 0.120, 0.085, 0.35, 0.78),  # Glassmere
        (0.18, 0.77, 0.120, 0.105, -0.18, 0.82), # Rootwold
        (0.78, 0.79, 0.130, 0.085, 0.22, 0.84),  # Ashkar
    ]

    land = np.maximum(
        land_from_ellipses(u, v, pangea),
        land_from_ellipses(u, v, islands),
    )
    land += coast_noise
    land_mask = smoothstep(-0.03, 0.10, land)

    # Ocean floor and continental shelf.
    h = 0.185 + 0.035 * n2
    shelf = smoothstep(-0.18, 0.12, land)
    h += 0.075 * shelf

    # Land elevation.
    land_height = 0.36 + 0.15 * n1 + 0.07 * n2 + 0.03 * n3
    h = h * (1.0 - land_mask) + land_height * land_mask

    # Pangea mountain systems and blocked highlands.
    h += 0.18 * ridge(v - (0.23 + 0.07 * np.sin(u * 10.0)), 0.026) * smoothstep(0.28, 0.70, u) * land_mask
    h += 0.22 * ridge(u - (0.66 + 0.04 * np.sin(v * 12.0)), 0.030) * smoothstep(0.20, 0.72, v) * land_mask
    h += 0.13 * ridge(v - (0.70 - 0.18 * u + 0.04 * np.sin(u * 14.0)), 0.028) * smoothstep(0.24, 0.70, u) * land_mask

    # Rivers/valleys on pangea.
    river1 = ridge(u - (0.47 + 0.08 * np.sin(v * 7.5)), 0.013) * smoothstep(0.24, 0.74, v)
    river2 = ridge(v - (0.54 - 0.17 * u + 0.04 * np.sin(u * 13.0)), 0.012) * smoothstep(0.24, 0.78, u)
    h -= 0.060 * (river1 + river2) * land_mask

    # Coração Negro / Ruptura do Limiar in central pangea.
    collapse = np.exp(-(((u - 0.51) / 0.095) ** 2 + ((v - 0.52) / 0.080) ** 2))
    h -= 0.100 * collapse
    h += 0.040 * ridge(np.sqrt(((u - 0.51) / 0.095) ** 2 + ((v - 0.52) / 0.080) ** 2) - 0.90, 0.12)

    # Island-specific relief.
    frost = np.exp(-(((u - 0.31) / 0.12) ** 2 + ((v - 0.12) / 0.07) ** 2))
    glass = np.exp(-(((u - 0.79) / 0.13) ** 2 + ((v - 0.22) / 0.09) ** 2))
    root = np.exp(-(((u - 0.18) / 0.13) ** 2 + ((v - 0.77) / 0.11) ** 2))
    ash = np.exp(-(((u - 0.78) / 0.14) ** 2 + ((v - 0.79) / 0.09) ** 2))
    h += 0.12 * frost * ridge(v - (0.12 + 0.02 * np.sin(u * 20.0)), 0.020)
    h += 0.07 * glass * n3
    h -= 0.04 * root * smooth_noise(SIZE, rng, 71)
    h += 0.16 * ash * np.exp(-(((u - 0.79) / 0.035) ** 2 + ((v - 0.79) / 0.030) ** 2))

    # Keep border mostly ocean/deep water.
    border = np.maximum.reduce([
        np.clip((0.035 - u) / 0.035, 0.0, 1.0),
        np.clip((u - 0.965) / 0.035, 0.0, 1.0),
        np.clip((0.035 - v) / 0.035, 0.0, 1.0),
        np.clip((v - 0.965) / 0.035, 0.0, 1.0),
    ])
    h = h * (1.0 - border) + (0.12 + 0.02 * n2) * border

    h = np.clip(h, 0.0, 1.0)
    height_u16 = (h * 65535.0).astype(np.uint16)
    Image.fromarray(height_u16, mode="I;16").save(HEIGHTMAP_PATH)

    preview = ((h - h.min()) / max(h.max() - h.min(), 0.0001) * 255.0).astype(np.uint8)
    Image.fromarray(preview, mode="L").save(PREVIEW_PATH)

    NOTES_PATH.write_text(
        "\n".join([
            "# HM_Elarion_64km_Macro_8129",
            "",
            "Heightmap macro unico para ter nocao do mundo completo de 64 km x 64 km.",
            "",
            "## Importante",
            "Este arquivo e para referencia de escala/blockout macro, nao para terreno final detalhado.",
            "Aureth detalhada continua sendo tile/regiao separada.",
            "",
            "## Configuracao Unreal",
            "- Import from File.",
            "- Heightmap: `SourceArt/World/Heightmaps/HM_Elarion_64km_Macro_8129.png`.",
            "- Se perguntar `Tiled Image?`, responder `No`.",
            "- Section Size: `127x127 Quads`.",
            "- Sections Per Component: `2x2 Sections`.",
            "- Number of Components: `32 x 32`.",
            "- Overall Resolution: `8129 x 8129`.",
            "- Scale XY para 64 km: `787.5, 787.5`.",
            "- Scale Z inicial: `700`.",
            "- Se relevo ficar baixo: testar Z `1000`.",
            "",
            "## Resultado",
            "- Canvas aproximado: 64 km x 64 km.",
            "- Pangeia central Elarion.",
            "- Ilhas: Frostvein, Glassmere, Rootwold, Ashkar.",
            "- Oceano e shelf costeiro.",
            "- Coracao Negro no centro.",
            "",
        ]),
        encoding="utf-8",
    )

    print(HEIGHTMAP_PATH)
    print(PREVIEW_PATH)
    print(NOTES_PATH)


if __name__ == "__main__":
    main()
