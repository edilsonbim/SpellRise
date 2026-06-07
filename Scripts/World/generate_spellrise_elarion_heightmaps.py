#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SpellRise / Elarion tiled heightmap generator

Gera heightmaps .r16 little-endian em tiles 8x8 para Unreal Engine 5 / World Partition.
Foco: produção local, geração tile-by-tile, validação de seams, backup seguro e relatório.

Dependências:
    pip install numpy pillow

Exemplos:
    # Protótipo seguro 2x2, resolução reduzida
    python generate_spellrise_elarion_heightmaps.py --prototype --yes

    # Final solicitado: 8x8 tiles de 8129x8129. Gera muitos GB.
    python generate_spellrise_elarion_heightmaps.py --final --yes --export-png-preview

    # Final com PNG 16-bit de inspeção por tile, não recomendado em primeira execução
    python generate_spellrise_elarion_heightmaps.py --final --yes --export-png16
"""

from __future__ import annotations

import argparse
import json
import logging
import math
import os
import shutil
import sys
import time
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Tuple

try:
    import numpy as np
except ImportError as exc:  # pragma: no cover
    raise SystemExit("ERRO: numpy não está instalado. Execute: py -m pip install numpy pillow") from exc

try:
    from PIL import Image
except ImportError as exc:  # pragma: no cover
    raise SystemExit("ERRO: Pillow não está instalado. Execute: py -m pip install numpy pillow") from exc


# -----------------------------------------------------------------------------
# Configuração
# -----------------------------------------------------------------------------

DEFAULT_INPUT_DIR = Path(r"C:\Users\biM\Documents\Unreal Projects\SpellRise\Fontes\MapDesign")
DEFAULT_OUTPUT_DIR = Path(r"C:\Users\biM\Documents\Unreal Projects\SpellRise\SourceArt\World\Heightmaps")
WORLD_NAME = "SR_Elarion"
SEA_LEVEL_NORM = 0.335

BIOME_IDS = {
    0: "Ocean",
    1: "Snow",
    2: "Forest",
    3: "Desert",
    4: "Volcanic",
    5: "Swamp",
    6: "Corrupted",
    7: "Plains",
    8: "Coast",
}

BIOME_RGB = {
    0: (20, 40, 70),       # Ocean
    1: (220, 235, 245),    # Snow
    2: (45, 105, 55),      # Forest
    3: (190, 145, 70),     # Desert
    4: (105, 45, 35),      # Volcanic
    5: (55, 85, 65),       # Swamp
    6: (95, 55, 130),      # Corrupted
    7: (115, 135, 80),     # Plains
    8: (200, 185, 125),    # Coast
}


@dataclass(frozen=True)
class GeneratorConfig:
    input_dir: str
    output_dir: str
    grid: int
    tile_size: int
    chunk_rows: int
    seed: int
    sea_level_norm: float
    export_png_preview: bool
    export_png16: bool
    export_global_masks: bool
    export_full_mask_tiles: bool
    preview_size: int
    global_preview_size: int
    smooth_strength: float
    reference_weight: float
    final_mode: bool
    prototype_mode: bool
    dry_run: bool
    allow_clean: bool

    @property
    def global_unique_size(self) -> int:
        return self.grid * (self.tile_size - 1) + 1

    @property
    def expected_r16_bytes_per_tile(self) -> int:
        return self.tile_size * self.tile_size * 2

    @property
    def expected_r16_total_bytes(self) -> int:
        return self.expected_r16_bytes_per_tile * self.grid * self.grid


# -----------------------------------------------------------------------------
# Logging / filesystem
# -----------------------------------------------------------------------------


def setup_console_logging() -> None:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s | %(levelname)s | %(message)s",
        handlers=[logging.StreamHandler(sys.stdout)],
    )


def attach_file_logging(output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    log_path = output_dir / f"{WORLD_NAME}_Generation.log"
    file_handler = logging.FileHandler(log_path, encoding="utf-8")
    file_handler.setLevel(logging.INFO)
    file_handler.setFormatter(logging.Formatter("%(asctime)s | %(levelname)s | %(message)s"))
    logging.getLogger().addHandler(file_handler)
    logging.info("Log: %s", log_path)


def bytes_to_gib(num_bytes: int) -> float:
    return num_bytes / (1024 ** 3)


def list_directory(path: Path) -> List[str]:
    if not path.exists():
        return []
    entries = []
    for p in sorted(path.iterdir(), key=lambda x: x.name.lower()):
        try:
            size = p.stat().st_size if p.is_file() else 0
        except OSError:
            size = 0
        suffix = "/" if p.is_dir() else ""
        entries.append(f"{p.name}{suffix} ({size} bytes)")
    return entries


def safe_backup_and_clean_output(output_dir: Path, allow_clean: bool, dry_run: bool) -> Optional[Path]:
    """Lista, faz backup e limpa a pasta de saída, com guard rails simples."""
    logging.info("Pasta de saída: %s", output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Guard rail para evitar apagar pasta errada.
    if "heightmaps" not in output_dir.name.lower():
        raise RuntimeError(f"Recusado: a pasta final precisa conter 'Heightmaps' no nome. Recebido: {output_dir}")

    entries = list_directory(output_dir)
    logging.info("Conteúdo atual da pasta de saída (%d item/ns):", len(entries))
    for item in entries:
        logging.info("  - %s", item)

    if dry_run:
        logging.info("DRY-RUN: nenhum arquivo será apagado.")
        return None

    if not allow_clean:
        raise RuntimeError("Limpeza bloqueada. Use --yes para permitir backup + limpeza da pasta de saída.")

    backup_dir = None
    if entries:
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        backup_dir = output_dir.parent / f"Heightmaps_Backup_{timestamp}"
        logging.info("Criando backup automático: %s", backup_dir)
        backup_dir.mkdir(parents=True, exist_ok=False)
        for src in output_dir.iterdir():
            dst = backup_dir / src.name
            if src.is_dir():
                shutil.copytree(src, dst)
            else:
                shutil.copy2(src, dst)
        logging.info("Backup concluído.")
    else:
        logging.info("Pasta vazia: backup não necessário.")

    logging.info("Limpando pasta original...")
    for src in output_dir.iterdir():
        if src.is_dir():
            shutil.rmtree(src)
        else:
            src.unlink()
    logging.info("Pasta limpa.")
    return backup_dir


# -----------------------------------------------------------------------------
# Referência visual opcional
# -----------------------------------------------------------------------------


class ReferenceSampler:
    """
    Usa a maior imagem encontrada em MapDesign como influência leve de land/ocean.

    O script ainda usa uma função procedural global contínua como fonte principal.
    A imagem local apenas empurra a silhueta costeira, com peso baixo, para não quebrar seams.
    """

    def __init__(self, image_path: Path, max_size: int = 2048) -> None:
        self.image_path = image_path
        img = Image.open(image_path).convert("RGBA")
        img.thumbnail((max_size, max_size), Image.Resampling.LANCZOS)
        img = img.resize((max_size, max_size), Image.Resampling.LANCZOS)
        arr = np.asarray(img, dtype=np.float32) / 255.0
        r, g, b, a = arr[..., 0], arr[..., 1], arr[..., 2], arr[..., 3]
        blue_bias = np.clip(b - np.maximum(r, g), 0.0, 1.0)
        luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b
        saturation = np.max(arr[..., :3], axis=2) - np.min(arr[..., :3], axis=2)

        # Heurística conservadora: muito azul/alpha baixo tende a oceano; o restante pode ser terra.
        ocean_hint = smoothstep_np(0.08, 0.28, blue_bias) * smoothstep_np(0.10, 0.35, saturation)
        alpha_land = smoothstep_np(0.12, 0.35, a)
        land_hint = np.clip((1.0 - ocean_hint) * alpha_land, 0.0, 1.0)

        # Evita que uma imagem sem oceano azul destrua a geração: centraliza o contraste.
        mean = float(np.mean(land_hint))
        if mean < 0.15 or mean > 0.95:
            logging.warning(
                "Referência visual detectada, mas land/ocean ficou extremo (mean=%.3f). Peso será reduzido.", mean
            )
            land_hint = 0.5 + (land_hint - mean) * 0.25

        self.land_hint = land_hint.astype(np.float32)
        self.height, self.width = self.land_hint.shape
        logging.info("Referência visual usada: %s (%dx%d)", image_path, self.width, self.height)

    def sample_land(self, u: np.ndarray, v: np.ndarray) -> np.ndarray:
        x = np.clip(u * (self.width - 1), 0, self.width - 1)
        y = np.clip(v * (self.height - 1), 0, self.height - 1)
        x0 = np.floor(x).astype(np.int32)
        y0 = np.floor(y).astype(np.int32)
        x1 = np.minimum(x0 + 1, self.width - 1)
        y1 = np.minimum(y0 + 1, self.height - 1)
        tx = (x - x0).astype(np.float32)
        ty = (y - y0).astype(np.float32)

        a = self.land_hint[y0, x0]
        b = self.land_hint[y0, x1]
        c = self.land_hint[y1, x0]
        d = self.land_hint[y1, x1]
        return ((a * (1 - tx) + b * tx) * (1 - ty) + (c * (1 - tx) + d * tx) * ty).astype(np.float32)


def find_reference_image(input_dir: Path) -> Optional[Path]:
    if not input_dir.exists():
        logging.warning("Pasta de referência não existe: %s", input_dir)
        return None
    exts = {".png", ".jpg", ".jpeg", ".tif", ".tiff", ".webp", ".bmp"}
    candidates = [p for p in input_dir.rglob("*") if p.is_file() and p.suffix.lower() in exts]
    if not candidates:
        logging.warning("Nenhuma imagem de referência encontrada em: %s", input_dir)
        return None
    candidates.sort(key=lambda p: p.stat().st_size, reverse=True)
    return candidates[0]


# -----------------------------------------------------------------------------
# Matemática procedural determinística
# -----------------------------------------------------------------------------


def smoothstep_np(edge0: float, edge1: float, x: np.ndarray) -> np.ndarray:
    t = np.clip((x - edge0) / max(edge1 - edge0, 1e-8), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def saturate_np(x: np.ndarray) -> np.ndarray:
    return np.clip(x, 0.0, 1.0)


def hash2d(ix: np.ndarray, iy: np.ndarray, seed: int) -> np.ndarray:
    """Hash 2D vetorizado estável, retorna float32 em [0,1]."""
    x = ix.astype(np.uint64, copy=False)
    y = iy.astype(np.uint64, copy=False)
    h = x * np.uint64(0x8DA6B343) ^ y * np.uint64(0xD8163841) ^ np.uint64(seed * 0x9E3779B1)
    h ^= h >> np.uint64(13)
    h *= np.uint64(0x85EBCA6B)
    h ^= h >> np.uint64(16)
    return ((h & np.uint64(0x00FFFFFF)).astype(np.float32) / np.float32(0x00FFFFFF)).astype(np.float32)


def value_noise(u: np.ndarray, v: np.ndarray, freq: float, seed: int) -> np.ndarray:
    x = u * np.float32(freq)
    y = v * np.float32(freq)
    x0 = np.floor(x).astype(np.int64)
    y0 = np.floor(y).astype(np.int64)
    xf = (x - x0).astype(np.float32)
    yf = (y - y0).astype(np.float32)
    wx = xf * xf * (3.0 - 2.0 * xf)
    wy = yf * yf * (3.0 - 2.0 * yf)

    n00 = hash2d(x0, y0, seed)
    n10 = hash2d(x0 + 1, y0, seed)
    n01 = hash2d(x0, y0 + 1, seed)
    n11 = hash2d(x0 + 1, y0 + 1, seed)

    nx0 = n00 * (1.0 - wx) + n10 * wx
    nx1 = n01 * (1.0 - wx) + n11 * wx
    return (nx0 * (1.0 - wy) + nx1 * wy).astype(np.float32)


def fbm(u: np.ndarray, v: np.ndarray, base_freq: float, octaves: int, seed: int) -> np.ndarray:
    total = np.zeros_like(u, dtype=np.float32)
    amp = np.float32(0.5)
    norm = np.float32(0.0)
    freq = np.float32(base_freq)
    for i in range(octaves):
        total += value_noise(u, v, float(freq), seed + i * 1013) * amp
        norm += amp
        freq *= np.float32(2.03)
        amp *= np.float32(0.5)
    return (total / max(float(norm), 1e-6)).astype(np.float32)


def ridged_fbm(u: np.ndarray, v: np.ndarray, base_freq: float, octaves: int, seed: int) -> np.ndarray:
    n = fbm(u, v, base_freq, octaves, seed)
    return (1.0 - np.abs(n * 2.0 - 1.0)).astype(np.float32)


def ellipse_score(u: np.ndarray, v: np.ndarray, cx: float, cy: float, rx: float, ry: float) -> np.ndarray:
    d = np.sqrt(((u - cx) / rx) ** 2 + ((v - cy) / ry) ** 2).astype(np.float32)
    return (1.0 - d).astype(np.float32)


def gauss(u: np.ndarray, v: np.ndarray, cx: float, cy: float, sx: float, sy: Optional[float] = None) -> np.ndarray:
    if sy is None:
        sy = sx
    return np.exp(-(((u - cx) / sx) ** 2 + ((v - cy) / sy) ** 2)).astype(np.float32)


def dist_to_segment(u: np.ndarray, v: np.ndarray, ax: float, ay: float, bx: float, by: float) -> np.ndarray:
    abx = bx - ax
    aby = by - ay
    denom = abx * abx + aby * aby + 1e-8
    t = np.clip(((u - ax) * abx + (v - ay) * aby) / denom, 0.0, 1.0).astype(np.float32)
    px = ax + t * abx
    py = ay + t * aby
    return np.sqrt((u - px) ** 2 + (v - py) ** 2).astype(np.float32)


def polyline_mask(u: np.ndarray, v: np.ndarray, points: List[Tuple[float, float]], width: float) -> np.ndarray:
    dmin = np.full_like(u, 10.0, dtype=np.float32)
    for a, b in zip(points[:-1], points[1:]):
        dmin = np.minimum(dmin, dist_to_segment(u, v, a[0], a[1], b[0], b[1]))
    return np.exp(-((dmin / width) ** 2)).astype(np.float32)


def apply_plateau(
    height: np.ndarray,
    u: np.ndarray,
    v: np.ndarray,
    cx: float,
    cy: float,
    radius: float,
    target_height: float,
    strength: float,
    land: np.ndarray,
) -> np.ndarray:
    d = np.sqrt((u - cx) ** 2 + (v - cy) ** 2).astype(np.float32)
    p = (1.0 - smoothstep_np(radius * 0.35, radius, d)) * land
    p = np.clip(p * strength, 0.0, 0.95).astype(np.float32)
    return (height * (1.0 - p) + target_height * p).astype(np.float32)


def compute_terrain(
    u: np.ndarray,
    v: np.ndarray,
    seed: int,
    reference: Optional[ReferenceSampler] = None,
    reference_weight: float = 0.0,
    aux: bool = False,
) -> Tuple[np.ndarray, Optional[Dict[str, np.ndarray]]]:
    """Função global contínua. Não usa coordenada local de tile."""
    coast_noise = fbm(u + 11.37, v - 4.21, 7.0, 4, seed + 10) - 0.5

    central = ellipse_score(u, v, 0.505, 0.515, 0.430, 0.340) + coast_noise * 0.135
    nw = ellipse_score(u, v, 0.205, 0.165, 0.180, 0.130) + coast_noise * 0.120
    sw = ellipse_score(u, v, 0.200, 0.805, 0.190, 0.145) + coast_noise * 0.115
    ne = ellipse_score(u, v, 0.805, 0.205, 0.185, 0.140) + coast_noise * 0.110
    se = ellipse_score(u, v, 0.800, 0.805, 0.190, 0.155) + coast_noise * 0.120

    arch = np.maximum.reduce([
        ellipse_score(u, v, 0.090, 0.515, 0.060, 0.038),
        ellipse_score(u, v, 0.310, 0.075, 0.058, 0.034),
        ellipse_score(u, v, 0.590, 0.090, 0.055, 0.032),
        ellipse_score(u, v, 0.925, 0.545, 0.060, 0.040),
        ellipse_score(u, v, 0.615, 0.910, 0.070, 0.040),
        ellipse_score(u, v, 0.405, 0.905, 0.050, 0.032),
    ]).astype(np.float32)
    arch += (fbm(u, v, 24.0, 2, seed + 911) - 0.5) * 0.08

    land_score = np.maximum.reduce([central, nw, sw, ne, se, arch]).astype(np.float32)

    if reference is not None and reference_weight > 0.0:
        ref_land = reference.sample_land(u, v)
        land_score = land_score + (ref_land - 0.5) * np.float32(0.22 * reference_weight)

    land = smoothstep_np(-0.040, 0.058, land_score).astype(np.float32)
    coast_band = np.clip(1.0 - np.abs(land_score) / 0.080, 0.0, 1.0).astype(np.float32)

    ocean_floor = SEA_LEVEL_NORM - 0.052 + (fbm(u, v, 16.0, 2, seed + 20) - 0.5) * 0.010
    rolling = (fbm(u + 2.7, v + 5.1, 12.0, 4, seed + 30) - 0.5) * 0.085
    micro = (fbm(u - 4.1, v + 8.3, 42.0, 2, seed + 40) - 0.5) * 0.018

    height = ocean_floor * (1.0 - land) + (SEA_LEVEL_NORM + 0.040 + rolling + micro) * land

    # Montanhas e cadeias rochosas.
    nw_mountains = gauss(u, v, 0.285, 0.205, 0.180, 0.120) * ridged_fbm(u, v, 19.0, 4, seed + 100)
    north_chain = gauss(u, v, 0.435, 0.255, 0.230, 0.090) * ridged_fbm(u + 3.2, v, 24.0, 4, seed + 110)
    east_chain = gauss(u, v, 0.735, 0.465, 0.165, 0.250) * ridged_fbm(u, v + 1.8, 21.0, 4, seed + 120)
    southeast_chain = gauss(u, v, 0.665, 0.700, 0.220, 0.130) * ridged_fbm(u - 2.4, v, 18.0, 4, seed + 130)

    height += land * (nw_mountains * 0.280 + north_chain * 0.170 + east_chain * 0.195 + southeast_chain * 0.145)

    # Ilha vulcânica sudeste: borda alta, cratera rebaixada.
    dx = (u - 0.800) / 0.145
    dy = (v - 0.805) / 0.125
    volcano_r = np.sqrt(dx * dx + dy * dy).astype(np.float32)
    volcano_ring = np.exp(-((volcano_r - 0.68) / 0.190) ** 2).astype(np.float32)
    volcano_core = np.exp(-((volcano_r) / 0.310) ** 2).astype(np.float32)
    volcano_rough = ridged_fbm(u + 9.4, v - 2.1, 32.0, 3, seed + 140)
    height += land * (volcano_ring * 0.300 - volcano_core * 0.105 + volcano_rough * volcano_ring * 0.070)

    # Centro corrompido: depressão jogável, ring ridge e ridges radiais.
    cx, cy = 0.515, 0.505
    r = np.sqrt((u - cx) ** 2 + (v - cy) ** 2).astype(np.float32)
    theta = np.arctan2(v - cy, u - cx).astype(np.float32)
    corrupt_core = np.exp(-((r / 0.075) ** 2)).astype(np.float32)
    corrupt_ring = np.exp(-(((r - 0.130) / 0.052) ** 2)).astype(np.float32)
    corrupt_outer = np.exp(-(((r - 0.215) / 0.100) ** 2)).astype(np.float32)
    radial = ((np.sin(theta * 13.0 + fbm(u, v, 9.0, 2, seed + 150) * 5.0) + 1.0) * 0.5).astype(np.float32)
    height += land * (-corrupt_core * 0.082 + corrupt_ring * 0.095 + radial * corrupt_outer * 0.052)

    # Rios principais, desenhados em curvas descendentes plausíveis.
    rivers = np.zeros_like(u, dtype=np.float32)
    river_defs = [
        ([(0.335, 0.245), (0.405, 0.365), (0.395, 0.510), (0.300, 0.625), (0.185, 0.610)], 0.0080),
        ([(0.565, 0.405), (0.535, 0.540), (0.505, 0.655), (0.470, 0.765)], 0.0070),
        ([(0.720, 0.370), (0.810, 0.305), (0.890, 0.255)], 0.0065),
        ([(0.675, 0.635), (0.725, 0.695), (0.850, 0.765)], 0.0065),
        ([(0.255, 0.755), (0.325, 0.725), (0.365, 0.820)], 0.0060),
    ]
    for pts, width in river_defs:
        rivers = np.maximum(rivers, polyline_mask(u, v, pts, width))
    rivers *= land * smoothstep_np(0.15, 0.85, land)
    height -= rivers * 0.046

    # Baías, pântanos e áreas planas de produção.
    swamp = gauss(u, v, 0.625, 0.630, 0.140, 0.090) * land
    height -= swamp * 0.025

    plateaus = [
        (0.350, 0.595, 0.030, SEA_LEVEL_NORM + 0.080, 0.72),  # vila oeste
        (0.485, 0.345, 0.034, SEA_LEVEL_NORM + 0.110, 0.68),  # fortaleza norte-centro
        (0.645, 0.480, 0.032, SEA_LEVEL_NORM + 0.105, 0.66),  # castelo leste
        (0.522, 0.505, 0.042, SEA_LEVEL_NORM + 0.060, 0.42),  # arena central corrompida
        (0.232, 0.690, 0.030, SEA_LEVEL_NORM + 0.065, 0.70),  # acampamento floresta
        (0.755, 0.285, 0.034, SEA_LEVEL_NORM + 0.070, 0.64),  # cidade árida
        (0.820, 0.690, 0.028, SEA_LEVEL_NORM + 0.095, 0.62),  # fortaleza vulcânica
        (0.200, 0.540, 0.026, SEA_LEVEL_NORM + 0.045, 0.68),  # porto oeste
        (0.500, 0.780, 0.032, SEA_LEVEL_NORM + 0.052, 0.72),  # porto sul
        (0.595, 0.255, 0.026, SEA_LEVEL_NORM + 0.085, 0.60),  # passagem norte
    ]
    for px, py, pr, target, strength in plateaus:
        height = apply_plateau(height, u, v, px, py, pr, target, strength, land)

    # Falloff costeiro suave: evita penhascos constantes no litoral.
    height = height * (1.0 - coast_band * 0.18) + (SEA_LEVEL_NORM + 0.010) * coast_band * 0.18

    height = np.clip(height, 0.020, 0.985).astype(np.float32)

    if not aux:
        return height, None

    biome = np.full(u.shape, 7, dtype=np.uint8)  # Plains
    biome[land < 0.35] = 0

    coast_bool = (land >= 0.35) & ((coast_band > 0.36) | (height < SEA_LEVEL_NORM + 0.030))
    forest_bool = (land >= 0.35) & (((u < 0.40) & (v > 0.44)) | (gauss(u, v, 0.205, 0.805, 0.24, 0.18) > 0.22))
    snow_bool = (land >= 0.35) & (((v < 0.285) & (u < 0.445)) | (height > 0.635))
    desert_bool = (land >= 0.35) & (u > 0.645) & (v < 0.365)
    volcanic_bool = (land >= 0.35) & (gauss(u, v, 0.800, 0.805, 0.230, 0.200) > 0.20)
    swamp_bool = (land >= 0.35) & (swamp > 0.28)
    corrupt_bool = (land >= 0.35) & (r < 0.230)

    biome[forest_bool] = 2
    biome[desert_bool] = 3
    biome[volcanic_bool] = 4
    biome[swamp_bool] = 5
    biome[corrupt_bool] = 6
    biome[snow_bool] = 1
    biome[coast_bool] = 8
    biome[land < 0.35] = 0

    aux_data = {
        "land": land.astype(np.float32),
        "river": rivers.astype(np.float32),
        "coast_band": coast_band.astype(np.float32),
        "biome": biome,
    }
    return height, aux_data


def smooth_crop_from_halo(
    halo_height: np.ndarray,
    y_offset: int,
    x_offset: int,
    rows: int,
    cols: int,
    strength: float,
) -> np.ndarray:
    if strength <= 0.0:
        return halo_height[y_offset:y_offset + rows, x_offset:x_offset + cols].astype(np.float32)

    padded = np.pad(halo_height, ((1, 1), (1, 1)), mode="edge")
    y0 = y_offset + 1
    x0 = x_offset + 1
    c = padded[y0:y0 + rows, x0:x0 + cols]
    n = padded[y0 - 1:y0 - 1 + rows, x0:x0 + cols]
    s = padded[y0 + 1:y0 + 1 + rows, x0:x0 + cols]
    w = padded[y0:y0 + rows, x0 - 1:x0 - 1 + cols]
    e = padded[y0:y0 + rows, x0 + 1:x0 + 1 + cols]
    nw = padded[y0 - 1:y0 - 1 + rows, x0 - 1:x0 - 1 + cols]
    ne = padded[y0 - 1:y0 - 1 + rows, x0 + 1:x0 + 1 + cols]
    sw = padded[y0 + 1:y0 + 1 + rows, x0 - 1:x0 - 1 + cols]
    se = padded[y0 + 1:y0 + 1 + rows, x0 + 1:x0 + 1 + cols]

    smoothed = c * 0.52 + (n + s + w + e) * 0.095 + (nw + ne + sw + se) * 0.025
    return (c * (1.0 - strength) + smoothed * strength).astype(np.float32)


def norm_to_u16(height: np.ndarray) -> np.ndarray:
    return np.round(np.clip(height, 0.0, 1.0) * 65535.0).astype("<u2", copy=False)


# -----------------------------------------------------------------------------
# Exportação de tiles
# -----------------------------------------------------------------------------


def tile_name(tx: int, ty: int, ext: str = ".r16") -> str:
    return f"{WORLD_NAME}_Height_X{tx:02d}_Y{ty:02d}{ext}"


def preview_name(tx: int, ty: int) -> str:
    return f"{WORLD_NAME}_Preview_X{tx:02d}_Y{ty:02d}.png"


def generate_tile(
    cfg: GeneratorConfig,
    output_dir: Path,
    tx: int,
    ty: int,
    reference: Optional[ReferenceSampler],
) -> Path:
    tile_path = output_dir / tile_name(tx, ty, ".r16")
    grid_unique = cfg.global_unique_size
    tile_size = cfg.tile_size
    gx0 = tx * (tile_size - 1)
    gy_tile0 = ty * (tile_size - 1)

    logging.info("Gerando tile X%02d Y%02d -> %s", tx, ty, tile_path.name)
    if cfg.dry_run:
        return tile_path

    with open(tile_path, "wb") as f:
        row = 0
        while row < tile_size:
            rows = min(cfg.chunk_rows, tile_size - row)
            gy0 = gy_tile0 + row
            gy1 = gy0 + rows - 1

            x_start = max(0, gx0 - 1)
            x_end = min(grid_unique - 1, gx0 + tile_size - 1 + 1)
            y_start = max(0, gy0 - 1)
            y_end = min(grid_unique - 1, gy1 + 1)

            x_offset = gx0 - x_start
            y_offset = gy0 - y_start

            xg = np.arange(x_start, x_end + 1, dtype=np.float32)
            yg = np.arange(y_start, y_end + 1, dtype=np.float32)
            u = (xg[None, :] / np.float32(grid_unique - 1)).astype(np.float32)
            v = (yg[:, None] / np.float32(grid_unique - 1)).astype(np.float32)
            uu = np.broadcast_to(u, (yg.size, xg.size))
            vv = np.broadcast_to(v, (yg.size, xg.size))

            halo_height, _ = compute_terrain(
                uu,
                vv,
                seed=cfg.seed,
                reference=reference,
                reference_weight=cfg.reference_weight,
                aux=False,
            )
            cropped = smooth_crop_from_halo(
                halo_height,
                y_offset=y_offset,
                x_offset=x_offset,
                rows=rows,
                cols=tile_size,
                strength=cfg.smooth_strength,
            )
            f.write(norm_to_u16(cropped).tobytes(order="C"))
            row += rows

    actual_size = tile_path.stat().st_size
    if actual_size != cfg.expected_r16_bytes_per_tile:
        raise RuntimeError(
            f"Tile com tamanho incorreto: {tile_path} | esperado {cfg.expected_r16_bytes_per_tile}, obtido {actual_size}"
        )
    return tile_path


def save_tile_preview(r16_path: Path, cfg: GeneratorConfig, output_dir: Path, tx: int, ty: int) -> None:
    if cfg.dry_run:
        return
    mm = np.memmap(r16_path, dtype="<u2", mode="r", shape=(cfg.tile_size, cfg.tile_size))
    out_size = min(cfg.preview_size, cfg.tile_size)
    rows = np.linspace(0, cfg.tile_size - 1, out_size).round().astype(np.int64)
    cols = np.linspace(0, cfg.tile_size - 1, out_size).round().astype(np.int64)
    preview = mm[np.ix_(rows, cols)]
    img8 = (preview.astype(np.uint16) >> 8).astype(np.uint8)
    Image.fromarray(img8, mode="L").save(output_dir / preview_name(tx, ty), optimize=True)


def save_tile_png16(r16_path: Path, cfg: GeneratorConfig, output_dir: Path, tx: int, ty: int) -> None:
    if cfg.dry_run:
        return
    mm = np.memmap(r16_path, dtype="<u2", mode="r", shape=(cfg.tile_size, cfg.tile_size))
    arr = np.asarray(mm, dtype=np.uint16)
    # PNG 16-bit para inspeção; pode ser grande e lento.
    Image.fromarray(arr, mode="I;16").save(output_dir / tile_name(tx, ty, ".png"))


# -----------------------------------------------------------------------------
# Validação e correção de seams
# -----------------------------------------------------------------------------


def validate_and_fix_seams(output_dir: Path, cfg: GeneratorConfig, fix: bool = True) -> Dict[str, object]:
    if cfg.dry_run:
        return {"dry_run": True}

    tile_size = cfg.tile_size
    max_diff = 0
    comparisons: List[Dict[str, object]] = []

    def open_tile(x: int, y: int, mode: str) -> np.memmap:
        path = output_dir / tile_name(x, y, ".r16")
        if not path.exists():
            raise FileNotFoundError(path)
        return np.memmap(path, dtype="<u2", mode=mode, shape=(tile_size, tile_size))

    # Passo 1: vertical X -> X+1.
    for ty in range(cfg.grid):
        for tx in range(cfg.grid - 1):
            a = open_tile(tx, ty, "r")
            b = open_tile(tx + 1, ty, "r+") if fix else open_tile(tx + 1, ty, "r")
            right = np.asarray(a[:, -1], dtype=np.int32)
            left = np.asarray(b[:, 0], dtype=np.int32)
            diff = np.abs(right - left)
            dmax = int(diff.max())
            max_diff = max(max_diff, dmax)
            if fix and dmax != 0:
                logging.warning("Corrigindo seam vertical X%02d/Y%02d -> X%02d/Y%02d | max diff=%d", tx, ty, tx + 1, ty, dmax)
                b[:, 0] = a[:, -1]
                b.flush()
            comparisons.append({"type": "vertical", "a": [tx, ty], "b": [tx + 1, ty], "max_diff": dmax})
            del a, b

    # Passo 2: horizontal Y -> Y+1.
    for ty in range(cfg.grid - 1):
        for tx in range(cfg.grid):
            a = open_tile(tx, ty, "r")
            b = open_tile(tx, ty + 1, "r+") if fix else open_tile(tx, ty + 1, "r")
            bottom = np.asarray(a[-1, :], dtype=np.int32)
            top = np.asarray(b[0, :], dtype=np.int32)
            diff = np.abs(bottom - top)
            dmax = int(diff.max())
            max_diff = max(max_diff, dmax)
            if fix and dmax != 0:
                logging.warning("Corrigindo seam horizontal X%02d/Y%02d -> X%02d/Y%02d | max diff=%d", tx, ty, tx, ty + 1, dmax)
                b[0, :] = a[-1, :]
                b.flush()
            comparisons.append({"type": "horizontal", "a": [tx, ty], "b": [tx, ty + 1], "max_diff": dmax})
            del a, b

    if fix:
        second = validate_and_fix_seams(output_dir, cfg, fix=False)
        return {
            "first_pass_max_diff": max_diff,
            "final_max_diff": second.get("final_max_diff", second.get("max_diff", 0)),
            "comparisons": comparisons,
            "second_pass": second,
        }

    return {"max_diff": max_diff, "final_max_diff": max_diff, "comparisons": comparisons}


# -----------------------------------------------------------------------------
# Previews globais e máscaras
# -----------------------------------------------------------------------------


def save_u8(path: Path, arr: np.ndarray) -> None:
    Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8), mode="L").save(path, optimize=True)


def export_global_previews_and_masks(output_dir: Path, cfg: GeneratorConfig, reference: Optional[ReferenceSampler]) -> Dict[str, str]:
    if cfg.dry_run or not cfg.export_global_masks:
        return {}

    res = cfg.global_preview_size
    logging.info("Gerando preview/masks globais em baixa resolução: %dx%d", res, res)
    u = np.linspace(0.0, 1.0, res, dtype=np.float32)
    v = np.linspace(0.0, 1.0, res, dtype=np.float32)
    uu, vv = np.meshgrid(u, v, indexing="xy")
    height, aux = compute_terrain(
        uu,
        vv,
        seed=cfg.seed,
        reference=reference,
        reference_weight=cfg.reference_weight,
        aux=True,
    )
    if aux is None:
        raise RuntimeError("Aux data não gerado.")

    gy, gx = np.gradient(height.astype(np.float32))
    slope = np.sqrt(gx * gx + gy * gy).astype(np.float32)
    slope_norm = slope / max(float(np.percentile(slope, 99.5)), 1e-6)
    slope_norm = np.clip(slope_norm, 0.0, 1.0)

    land = aux["land"]
    river = aux["river"]
    coast_band = aux["coast_band"]
    biome = aux["biome"].astype(np.uint8)

    wear = np.clip(slope_norm * land * 1.35, 0.0, 1.0)
    deposit = np.clip((river * 1.8 + coast_band * 0.55) * (1.0 - slope_norm), 0.0, 1.0) * land

    biome_rgb = np.zeros((res, res, 3), dtype=np.uint8)
    for biome_id, rgb in BIOME_RGB.items():
        biome_rgb[biome == biome_id] = rgb

    files: Dict[str, str] = {}
    height_path = output_dir / f"{WORLD_NAME}_GlobalPreview.png"
    save_u8(height_path, height * 255.0)
    files["global_preview"] = str(height_path)

    masks = {
        "LandOcean": land,
        "Slope": slope_norm,
        "Flow": river,
        "ErosionWear": wear,
        "Deposit": deposit,
    }
    for name, arr in masks.items():
        path = output_dir / f"{WORLD_NAME}_Mask_{name}.png"
        save_u8(path, arr * 255.0)
        files[f"mask_{name}"] = str(path)

    biome_path = output_dir / f"{WORLD_NAME}_BiomeMask.png"
    Image.fromarray(biome_rgb, mode="RGB").save(biome_path, optimize=True)
    files["biome_mask_rgb"] = str(biome_path)

    for biome_id, biome_name in BIOME_IDS.items():
        if biome_id == 0:
            continue
        path = output_dir / f"{WORLD_NAME}_Mask_Biome_{biome_name}.png"
        save_u8(path, (biome == biome_id).astype(np.float32) * 255.0)
        files[f"biome_{biome_name}"] = str(path)

    return files


# -----------------------------------------------------------------------------
# Metadata e relatório
# -----------------------------------------------------------------------------


def write_metadata_and_report(
    output_dir: Path,
    cfg: GeneratorConfig,
    backup_dir: Optional[Path],
    reference_path: Optional[Path],
    generated_tiles: List[str],
    seam_result: Dict[str, object],
    global_files: Dict[str, str],
    elapsed_sec: float,
) -> None:
    metadata = {
        "world": WORLD_NAME,
        "generated_at": datetime.now().isoformat(timespec="seconds"),
        "config": asdict(cfg),
        "backup_dir": str(backup_dir) if backup_dir else None,
        "reference_image": str(reference_path) if reference_path else None,
        "generated_tiles": generated_tiles,
        "global_files": global_files,
        "sea_level_norm": SEA_LEVEL_NORM,
        "sea_level_u16": int(round(SEA_LEVEL_NORM * 65535)),
        "format": {
            "height": ".r16 little-endian uint16 grayscale",
            "png_preview": "8-bit grayscale preview only",
            "png16_optional": "16-bit grayscale PNG, no sRGB metadata guarantee",
        },
        "seam_validation": seam_result,
        "disk_estimate": {
            "r16_bytes_per_tile": cfg.expected_r16_bytes_per_tile,
            "r16_gib_per_tile": bytes_to_gib(cfg.expected_r16_bytes_per_tile),
            "r16_total_bytes": cfg.expected_r16_total_bytes,
            "r16_total_gib": bytes_to_gib(cfg.expected_r16_total_bytes),
        },
        "elapsed_seconds": elapsed_sec,
    }

    meta_path = output_dir / f"{WORLD_NAME}_Heightmap_Metadata.json"
    with open(meta_path, "w", encoding="utf-8") as f:
        json.dump(metadata, f, indent=2, ensure_ascii=False)

    report_path = output_dir / f"{WORLD_NAME}_Generation_Report.md"
    with open(report_path, "w", encoding="utf-8") as f:
        f.write(f"# {WORLD_NAME} Heightmap Generation Report\n\n")
        f.write(f"Generated: {metadata['generated_at']}\n\n")
        f.write("## Output\n")
        f.write(f"- Grid: {cfg.grid}x{cfg.grid}\n")
        f.write(f"- Tile resolution: {cfg.tile_size}x{cfg.tile_size}\n")
        f.write(f"- Unique global vertex resolution with shared borders: {cfg.global_unique_size}x{cfg.global_unique_size}\n")
        f.write(f"- R16 per tile: {bytes_to_gib(cfg.expected_r16_bytes_per_tile):.3f} GiB\n")
        f.write(f"- R16 total: {bytes_to_gib(cfg.expected_r16_total_bytes):.3f} GiB\n")
        f.write(f"- Sea level normalized/U16: {SEA_LEVEL_NORM:.3f} / {metadata['sea_level_u16']}\n")
        f.write(f"- Reference image: {reference_path if reference_path else 'not found'}\n")
        f.write(f"- Backup: {backup_dir if backup_dir else 'not needed'}\n\n")

        f.write("## Seam validation\n")
        f.write(f"- First pass max diff: {seam_result.get('first_pass_max_diff', seam_result.get('max_diff'))}\n")
        f.write(f"- Final max diff: {seam_result.get('final_max_diff')}\n")
        f.write("- Expected final diff: 0\n\n")

        f.write("## Generated tile files\n")
        for p in generated_tiles:
            f.write(f"- `{Path(p).name}`\n")
        f.write("\n## Global files\n")
        for key, p in global_files.items():
            f.write(f"- {key}: `{Path(p).name}`\n")

        f.write("\n## Unreal Engine 5 import notes\n")
        f.write("1. Use Landscape Mode > Import from File.\n")
        f.write("2. Select one `.r16` tile. UE should detect the tiled path.\n")
        f.write("3. If tiles appear inverted on Y, use Flip Y Axis.\n")
        f.write("4. Use World Partition and import as tiled landscape/subregion when applying to an existing landscape.\n")
        f.write("5. Start with a small prototype before importing the 8x8 8129 production set.\n")
        f.write("6. Keep `.png` previews out of the import folder if UE tile detection becomes confused.\n")

        f.write("\n## Technical risks\n")
        f.write("- 8x8 tiles of 8129 are extremely heavy for editing/import.\n")
        f.write("- A single 8129 tile already maps to Epic's largest recommended landscape component count.\n")
        f.write("- 64 such tiles can become impractical unless split into production streaming strategy, lower terrain density, or external terrain authoring pass.\n")
        f.write("- Full-resolution mask tiles are intentionally disabled by default to avoid exploding disk usage.\n")

        f.write("\n## Gaea next steps\n")
        f.write("1. Rebuild this silhouette in Gaea using Landmass/Base Shape nodes.\n")
        f.write("2. Add Erosion nodes and export Wear/Deposit/Flow outputs.\n")
        f.write("3. Use Tiled Build for final resolution.\n")
        f.write("4. Compare Gaea seams against this script's seam validator.\n")

    logging.info("Metadata: %s", meta_path)
    logging.info("Relatório: %s", report_path)


# -----------------------------------------------------------------------------
# CLI
# -----------------------------------------------------------------------------


def build_arg_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description="SpellRise Elarion tiled heightmap generator")
    p.add_argument("--input-dir", type=Path, default=DEFAULT_INPUT_DIR)
    p.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR)
    p.add_argument("--seed", type=int, default=13371337)

    mode = p.add_mutually_exclusive_group(required=True)
    mode.add_argument("--prototype", action="store_true", help="Gera 2x2 tiles pequenos para validar seams rapidamente.")
    mode.add_argument("--final", action="store_true", help="Gera o set final 8x8 8129x8129 solicitado.")
    mode.add_argument("--custom", action="store_true", help="Usa --grid e --tile-size informados.")

    p.add_argument("--grid", type=int, default=None, help="Usado com --custom.")
    p.add_argument("--tile-size", type=int, default=None, help="Usado com --custom.")
    p.add_argument("--chunk-rows", type=int, default=None)
    p.add_argument("--preview-size", type=int, default=None)
    p.add_argument("--global-preview-size", type=int, default=None)
    p.add_argument("--smooth-strength", type=float, default=0.35)
    p.add_argument("--reference-weight", type=float, default=0.15, help="Peso leve da imagem MapDesign na silhueta costeira. Use 0 para ignorar.")

    p.add_argument("--export-png-preview", action="store_true", default=True)
    p.add_argument("--no-png-preview", action="store_true")
    p.add_argument("--export-png16", action="store_true", help="Exporta também PNG 16-bit por tile. Muito pesado.")
    p.add_argument("--no-global-masks", action="store_true")
    p.add_argument("--export-full-mask-tiles", action="store_true", help="Reservado. Não recomendado para final 8129.")

    p.add_argument("--dry-run", action="store_true")
    p.add_argument("--yes", action="store_true", help="Permite backup + limpeza da pasta de saída.")
    return p


def make_config(args: argparse.Namespace) -> GeneratorConfig:
    if args.prototype:
        grid = 2
        tile_size = 257 if args.tile_size is None else args.tile_size
        chunk_rows = args.chunk_rows or 64
        preview_size = args.preview_size or 257
        global_preview_size = args.global_preview_size or 1024
    elif args.final:
        grid = 8
        tile_size = 8129
        chunk_rows = args.chunk_rows or 256
        preview_size = args.preview_size or 1024
        global_preview_size = args.global_preview_size or 4096
    else:
        if args.grid is None or args.tile_size is None:
            raise SystemExit("--custom exige --grid e --tile-size")
        grid = args.grid
        tile_size = args.tile_size
        chunk_rows = args.chunk_rows or min(256, tile_size)
        preview_size = args.preview_size or min(1024, tile_size)
        global_preview_size = args.global_preview_size or 2048

    if tile_size < 17:
        raise SystemExit("tile-size muito baixo.")
    if grid < 1:
        raise SystemExit("grid inválido.")
    if chunk_rows < 8:
        raise SystemExit("chunk-rows muito baixo.")

    # Para Unreal Landscape, valores recomendados são do tipo 2^n + 1 ou fórmulas por componentes.
    # 8129 é mantido por solicitação do projeto; custom aceita outros tamanhos para teste.
    return GeneratorConfig(
        input_dir=str(args.input_dir),
        output_dir=str(args.output_dir),
        grid=grid,
        tile_size=tile_size,
        chunk_rows=chunk_rows,
        seed=args.seed,
        sea_level_norm=SEA_LEVEL_NORM,
        export_png_preview=not args.no_png_preview,
        export_png16=args.export_png16,
        export_global_masks=not args.no_global_masks,
        export_full_mask_tiles=args.export_full_mask_tiles,
        preview_size=preview_size,
        global_preview_size=global_preview_size,
        smooth_strength=max(0.0, min(1.0, args.smooth_strength)),
        reference_weight=max(0.0, min(1.0, args.reference_weight)),
        final_mode=bool(args.final),
        prototype_mode=bool(args.prototype),
        dry_run=bool(args.dry_run),
        allow_clean=bool(args.yes),
    )


def main() -> int:
    parser = build_arg_parser()
    args = parser.parse_args()
    cfg = make_config(args)
    output_dir = Path(cfg.output_dir)
    input_dir = Path(cfg.input_dir)

    setup_console_logging()
    start = time.time()

    logging.info("SpellRise Elarion Heightmap Generator")
    logging.info("Config: %s", json.dumps(asdict(cfg), indent=2, ensure_ascii=False))
    logging.info("Resolução global única: %dx%d", cfg.global_unique_size, cfg.global_unique_size)
    logging.info("Estimativa .r16 total: %.3f GiB", bytes_to_gib(cfg.expected_r16_total_bytes))

    if cfg.final_mode:
        logging.warning("MODO FINAL: 64 tiles 8129x8129. Isso é pesado para disco, importação e edição no UE.")
        if not cfg.allow_clean and not cfg.dry_run:
            raise RuntimeError("Use --yes para confirmar backup + limpeza antes da geração final.")

    backup_dir = safe_backup_and_clean_output(output_dir, allow_clean=cfg.allow_clean, dry_run=cfg.dry_run)
    if not cfg.dry_run:
        attach_file_logging(output_dir)

    reference_path = find_reference_image(input_dir)
    reference = None
    if reference_path and cfg.reference_weight > 0.0:
        try:
            reference = ReferenceSampler(reference_path)
        except Exception as exc:
            logging.exception("Falha ao carregar referência visual. Continuando com procedural puro. Erro: %s", exc)
            reference = None

    generated_tiles: List[str] = []
    if not cfg.dry_run:
        output_dir.mkdir(parents=True, exist_ok=True)

    for ty in range(cfg.grid):
        for tx in range(cfg.grid):
            r16_path = generate_tile(cfg, output_dir, tx, ty, reference)
            generated_tiles.append(str(r16_path))
            if cfg.export_png_preview:
                save_tile_preview(r16_path, cfg, output_dir, tx, ty)
            if cfg.export_png16:
                save_tile_png16(r16_path, cfg, output_dir, tx, ty)

    logging.info("Validando seams...")
    seam_result = validate_and_fix_seams(output_dir, cfg, fix=True)
    logging.info("Seam final max diff: %s", seam_result.get("final_max_diff"))

    global_files = export_global_previews_and_masks(output_dir, cfg, reference)

    elapsed = time.time() - start
    write_metadata_and_report(output_dir, cfg, backup_dir, reference_path, generated_tiles, seam_result, global_files, elapsed)
    logging.info("Concluído em %.1f segundos.", elapsed)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        logging.error("Interrompido pelo usuário.")
        raise SystemExit(130)
    except Exception as exc:
        logging.exception("Falha: %s", exc)
        raise SystemExit(1)
