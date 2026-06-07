#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SpellRise / Elarion tiled heightmap generator - v02

Corrige o erro da v01: a referência principal agora é obrigatoriamente a imagem
SpellRise_Elarion_Satellite_Concept_v01, e a silhueta/biomas/alturas são derivados
dessa imagem, não de um mapa procedural genérico.

Saída principal:
- 64 tiles .r16 little-endian, 16-bit grayscale real, quando usado --final
- previews PNG leves
- masks globais
- metadata JSON
- relatório MD

Dependências:
    py -m pip install numpy pillow

Uso seguro:
    py generate_spellrise_elarion_heightmaps_v02.py --prototype --yes

Uso final pesado:
    py generate_spellrise_elarion_heightmaps_v02.py --final --yes --export-png-preview
"""

from __future__ import annotations

import argparse
import json
import logging
import math
import shutil
import sys
import time
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

try:
    import numpy as np
except ImportError as exc:  # pragma: no cover
    raise SystemExit("ERRO: numpy não está instalado. Execute: py -m pip install numpy pillow") from exc

try:
    from PIL import Image, ImageFilter
except ImportError as exc:  # pragma: no cover
    raise SystemExit("ERRO: Pillow não está instalado. Execute: py -m pip install numpy pillow") from exc


DEFAULT_INPUT_DIR = Path(r"C:\Users\biM\Documents\Unreal Projects\SpellRise\Fontes\MapDesign")
DEFAULT_OUTPUT_DIR = Path(r"C:\Users\biM\Documents\Unreal Projects\SpellRise\SourceArt\World\Heightmaps")
DEFAULT_REFERENCE_BASENAME = "SpellRise_Elarion_Satellite_Concept_v01"
WORLD_NAME = "SR_Elarion"
SEA_LEVEL_NORM = 0.335
SCRIPT_VERSION = "2.0-reference-driven"

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
    1: (225, 235, 245),    # Snow
    2: (45, 105, 55),      # Forest
    3: (190, 145, 70),     # Desert
    4: (105, 45, 35),      # Volcanic
    5: (55, 85, 65),       # Swamp
    6: (95, 55, 135),      # Corrupted
    7: (115, 135, 80),     # Plains
    8: (200, 185, 125),    # Coast
}


@dataclass(frozen=True)
class GeneratorConfig:
    input_dir: str
    output_dir: str
    reference_image: Optional[str]
    reference_fit: str
    require_reference: bool
    reference_size: int
    grid: int
    tile_size: int
    chunk_rows: int
    seed: int
    sea_level_norm: float
    export_png_preview: bool
    export_png16: bool
    export_global_masks: bool
    preview_size: int
    global_preview_size: int
    smooth_strength: float
    procedural_detail_strength: float
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
    entries: List[str] = []
    for p in sorted(path.iterdir(), key=lambda x: x.name.lower()):
        try:
            size = p.stat().st_size if p.is_file() else 0
        except OSError:
            size = 0
        suffix = "/" if p.is_dir() else ""
        entries.append(f"{p.name}{suffix} ({size} bytes)")
    return entries


def safe_backup_and_clean_output(output_dir: Path, allow_clean: bool, dry_run: bool) -> Optional[Path]:
    logging.info("Pasta de saída: %s", output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

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


def smoothstep_np(edge0: float, edge1: float, x: np.ndarray) -> np.ndarray:
    t = np.clip((x - edge0) / max(edge1 - edge0, 1e-8), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def saturate_np(x: np.ndarray) -> np.ndarray:
    return np.clip(x, 0.0, 1.0)


def hash2d(ix: np.ndarray, iy: np.ndarray, seed: int) -> np.ndarray:
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


class ReferenceMap:
    """Mapa de referência analisado diretamente da imagem SpellRise_Elarion_Satellite_Concept_v01."""

    def __init__(self, image_path: Path, max_size: int = 2048, fit: str = "stretch") -> None:
        if fit not in {"stretch", "contain"}:
            raise ValueError("reference_fit precisa ser 'stretch' ou 'contain'.")
        self.image_path = image_path
        self.fit = fit

        img = Image.open(image_path).convert("RGB")
        self.original_width, self.original_height = img.size
        scale = min(float(max_size) / max(self.original_width, self.original_height), 1.0)
        resize_w = max(64, int(round(self.original_width * scale)))
        resize_h = max(64, int(round(self.original_height * scale)))
        if (resize_w, resize_h) != img.size:
            img = img.resize((resize_w, resize_h), Image.Resampling.LANCZOS)
        self.width, self.height = img.size
        arr = np.asarray(img, dtype=np.float32) / 255.0
        self.rgb = arr.astype(np.float32)
        self.analysis = self._build_analysis(arr)
        logging.info(
            "Referência obrigatória usada: %s | original=%dx%d | análise=%dx%d | fit=%s",
            image_path,
            self.original_width,
            self.original_height,
            self.width,
            self.height,
            self.fit,
        )

    def _build_analysis(self, arr: np.ndarray) -> Dict[str, np.ndarray]:
        h, w, _ = arr.shape
        r = arr[..., 0]
        g = arr[..., 1]
        b = arr[..., 2]
        lum = (0.2126 * r + 0.7152 * g + 0.0722 * b).astype(np.float32)
        sat = (np.max(arr, axis=2) - np.min(arr, axis=2)).astype(np.float32)
        yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
        u = xx / np.float32(max(w - 1, 1))
        v = yy / np.float32(max(h - 1, 1))

        # Água real do conceito: oceano escuro/azul e baías turquesa.
        # Protege o núcleo roxo/corrompido para não virar oceano por ter canal B alto.
        central_radius = np.sqrt(((u - 0.515) / 0.185) ** 2 + ((v - 0.510) / 0.165) ** 2)
        corrupted_protect = (
            (central_radius < 1.32)
            & (b > g * 1.02)
            & (r > g * 0.88)
            & (sat < 0.24)
            & (lum > 0.14)
            & (lum < 0.64)
        )

        water_raw = (
            ((b > 0.18) & (b > r * 1.18) & (b > g * 1.03) & (sat > 0.10))
            | ((b > 0.26) & (g > 0.20) & (r < 0.20) & (b > r * 1.35))
            | ((g > 0.32) & (b > 0.32) & (r < 0.22) & (sat > 0.18))
        )
        water_raw = water_raw & ~corrupted_protect

        water_img = Image.fromarray((water_raw.astype(np.uint8) * 255), mode="L")
        # Fecha espuma/ruído do oceano, mas preserva ilhas grandes e continente.
        water_closed = water_img.filter(ImageFilter.MaxFilter(7)).filter(ImageFilter.MinFilter(7))
        water_soft_img = water_closed.filter(ImageFilter.GaussianBlur(1.8))
        water_soft = np.asarray(water_soft_img, dtype=np.float32) / 255.0
        water_soft[central_radius < 1.20] *= 0.18
        land_binary = (water_soft < 0.40).astype(np.float32)

        land_soft_img = Image.fromarray((land_binary * 255.0).astype(np.uint8), mode="L").filter(ImageFilter.GaussianBlur(4.0))
        land_soft = np.asarray(land_soft_img, dtype=np.float32) / 255.0
        land_soft = np.clip(land_soft, 0.0, 1.0)
        land_soft[central_radius < 1.04] = np.maximum(land_soft[central_radius < 1.04], 0.92)

        # Costa = transição local land/ocean.
        coast_band = np.clip(1.0 - np.abs(land_soft - 0.50) * 2.0, 0.0, 1.0).astype(np.float32)
        coast_band = np.maximum(coast_band, ((land_soft > 0.25) & (land_soft < 0.94)).astype(np.float32) * 0.35)

        lum_blur = np.asarray(
            Image.fromarray((lum * 255.0).astype(np.uint8), mode="L").filter(ImageFilter.GaussianBlur(10.0)),
            dtype=np.float32,
        ) / 255.0
        texture = np.clip(np.abs(lum - lum_blur) * 4.8, 0.0, 1.0).astype(np.float32)
        gy, gx = np.gradient(lum.astype(np.float32))
        edge = np.sqrt(gx * gx + gy * gy)
        edge = edge / max(float(np.percentile(edge, 99.4)), 1e-6)
        edge = np.clip(edge, 0.0, 1.0).astype(np.float32)

        # Biomas por cor + posição do próprio concept.
        # Neve: branca/fria, mas o grande bloco claro do nordeste é tratado como deserto/árido, não neve.
        ne_desert_prior = np.exp(-(((u - 0.835) / 0.220) ** 2 + ((v - 0.205) / 0.165) ** 2)).astype(np.float32)
        snow = (
            ((lum > 0.56) & (sat < 0.18))
            | ((lum > 0.48) & (b >= r * 0.92) & (b >= g * 0.90) & (sat < 0.22))
        ).astype(np.float32) * land_soft
        # Remove praias e salinas/deserto claro do NE da máscara de neve.
        snow *= (1.0 - np.clip(coast_band * 1.15, 0.0, 1.0))
        snow *= (1.0 - np.clip(ne_desert_prior * 1.10, 0.0, 0.92))

        forest = ((g >= r * 0.96) & (g >= b * 0.88) & (lum < 0.42) & (sat > 0.035)).astype(np.float32) * land_soft
        desert_color = ((r > b * 1.12) & (g > b * 1.04) & (lum > 0.30) & (sat > 0.045)).astype(np.float32)
        desert = np.clip(desert_color + ne_desert_prior * 0.90, 0.0, 1.0).astype(np.float32) * land_soft
        volcanic_pos = np.exp(-(((u - 0.820) / 0.190) ** 2 + ((v - 0.805) / 0.155) ** 2)).astype(np.float32)
        volcanic = np.clip(((lum < 0.32) & (sat < 0.16)).astype(np.float32) * volcanic_pos * 1.45, 0.0, 1.0) * land_soft
        purple_color = (
            (b > g * 1.05)
            & (r > g * 0.88)
            & (b > r * 0.82)
            & (lum > 0.16)
            & (lum < 0.58)
            & (sat > 0.025)
        ).astype(np.float32)
        corrupted_core = np.exp(-(((u - 0.515) / 0.105) ** 2 + ((v - 0.510) / 0.090) ** 2)).astype(np.float32)
        corrupted = np.clip(corrupted_core * 1.15 + purple_color * corrupted_core * 0.75, 0.0, 1.0).astype(np.float32)
        corrupted *= land_soft
        swamp = np.clip(
            np.exp(-(((u - 0.455) / 0.120) ** 2 + ((v - 0.790) / 0.100) ** 2))
            + np.exp(-(((u - 0.610) / 0.110) ** 2 + ((v - 0.660) / 0.085) ** 2)),
            0.0,
            1.0,
        ).astype(np.float32) * land_soft * (1.0 - snow)

        # Rios visíveis/baías internas: pixels de água próximos ou dentro do land mass.
        inland_water = np.clip(water_soft * (land_soft > 0.28).astype(np.float32), 0.0, 1.0)

        # Altura base guiada por brilho/textura do concept.
        land_lum = np.clip((lum_blur - 0.12) / 0.58, 0.0, 1.0).astype(np.float32)

        return {
            "r": r.astype(np.float32),
            "g": g.astype(np.float32),
            "b": b.astype(np.float32),
            "lum": lum.astype(np.float32),
            "lum_blur": lum_blur.astype(np.float32),
            "sat": sat.astype(np.float32),
            "water_soft": water_soft.astype(np.float32),
            "land": land_soft.astype(np.float32),
            "coast": coast_band.astype(np.float32),
            "texture": texture.astype(np.float32),
            "edge": edge.astype(np.float32),
            "snow": snow.astype(np.float32),
            "forest": forest.astype(np.float32),
            "desert": desert.astype(np.float32),
            "volcanic": volcanic.astype(np.float32),
            "corrupted": corrupted.astype(np.float32),
            "swamp": swamp.astype(np.float32),
            "inland_water": inland_water.astype(np.float32),
            "land_lum": land_lum.astype(np.float32),
        }

    def _map_uv_to_image_uv(self, u: np.ndarray, v: np.ndarray) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        if self.fit == "stretch":
            inside = np.ones_like(u, dtype=bool)
            return np.clip(u, 0.0, 1.0), np.clip(v, 0.0, 1.0), inside

        # Mantém aspect ratio da imagem dentro de um heightmap quadrado, com oceano de padding.
        img_aspect = self.original_width / max(float(self.original_height), 1.0)
        target_aspect = 1.0
        if img_aspect > target_aspect:
            used_h = target_aspect / img_aspect
            pad = (1.0 - used_h) * 0.5
            iv = (v - pad) / max(used_h, 1e-6)
            iu = u
            inside = (iv >= 0.0) & (iv <= 1.0)
        else:
            used_w = img_aspect / target_aspect
            pad = (1.0 - used_w) * 0.5
            iu = (u - pad) / max(used_w, 1e-6)
            iv = v
            inside = (iu >= 0.0) & (iu <= 1.0)
        return np.clip(iu, 0.0, 1.0), np.clip(iv, 0.0, 1.0), inside

    def sample(self, name: str, u: np.ndarray, v: np.ndarray, outside_value: float = 0.0) -> np.ndarray:
        arr = self.analysis[name]
        iu, iv, inside = self._map_uv_to_image_uv(u, v)
        x = np.clip(iu * (self.width - 1), 0, self.width - 1)
        y = np.clip(iv * (self.height - 1), 0, self.height - 1)
        x0 = np.floor(x).astype(np.int32)
        y0 = np.floor(y).astype(np.int32)
        x1 = np.minimum(x0 + 1, self.width - 1)
        y1 = np.minimum(y0 + 1, self.height - 1)
        tx = (x - x0).astype(np.float32)
        ty = (y - y0).astype(np.float32)
        a = arr[y0, x0]
        b = arr[y0, x1]
        c = arr[y1, x0]
        d = arr[y1, x1]
        out = ((a * (1 - tx) + b * tx) * (1 - ty) + (c * (1 - tx) + d * tx) * ty).astype(np.float32)
        if not np.all(inside):
            out = np.where(inside, out, np.float32(outside_value)).astype(np.float32)
        return out


def find_reference_image(input_dir: Path, explicit_path: Optional[Path]) -> Optional[Path]:
    if explicit_path is not None:
        if explicit_path.exists() and explicit_path.is_file():
            return explicit_path
        raise FileNotFoundError(f"Referência explícita não encontrada: {explicit_path}")

    if not input_dir.exists():
        logging.warning("Pasta de referência não existe: %s", input_dir)
        return None

    exts = {".png", ".jpg", ".jpeg", ".tif", ".tiff", ".webp", ".bmp"}
    exact: List[Path] = []
    all_images: List[Path] = []
    for p in input_dir.rglob("*"):
        if not p.is_file() or p.suffix.lower() not in exts:
            continue
        all_images.append(p)
        if DEFAULT_REFERENCE_BASENAME.lower() in p.stem.lower():
            exact.append(p)

    if exact:
        exact.sort(key=lambda p: p.stat().st_size, reverse=True)
        return exact[0]

    if not all_images:
        logging.warning("Nenhuma imagem de referência encontrada em: %s", input_dir)
        return None

    logging.warning("Imagem %s não encontrada. Nenhuma imagem alternativa será usada sem --reference-image.", DEFAULT_REFERENCE_BASENAME)
    return None


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
    p = (1.0 - smoothstep_np(radius * 0.38, radius, d)) * land
    p = np.clip(p * strength, 0.0, 0.92).astype(np.float32)
    return (height * (1.0 - p) + target_height * p).astype(np.float32)


def compute_terrain(
    u: np.ndarray,
    v: np.ndarray,
    seed: int,
    reference: ReferenceMap,
    detail_strength: float,
    aux: bool = False,
) -> Tuple[np.ndarray, Optional[Dict[str, np.ndarray]]]:
    """Função global contínua baseada na imagem SpellRise_Elarion_Satellite_Concept_v01."""
    land = reference.sample("land", u, v, outside_value=0.0)
    water = reference.sample("water_soft", u, v, outside_value=1.0)
    coast = reference.sample("coast", u, v, outside_value=0.0)
    land_lum = reference.sample("land_lum", u, v, outside_value=0.0)
    texture = reference.sample("texture", u, v, outside_value=0.0)
    edge = reference.sample("edge", u, v, outside_value=0.0)
    snow = reference.sample("snow", u, v, outside_value=0.0)
    forest = reference.sample("forest", u, v, outside_value=0.0)
    desert = reference.sample("desert", u, v, outside_value=0.0)
    volcanic = reference.sample("volcanic", u, v, outside_value=0.0)
    corrupted_ref = reference.sample("corrupted", u, v, outside_value=0.0)
    swamp = reference.sample("swamp", u, v, outside_value=0.0)
    inland_water = reference.sample("inland_water", u, v, outside_value=0.0)

    # Ruídos determinísticos: só adicionam microforma/erosão, não trocam a silhueta do concept.
    low = fbm(u + 2.17, v - 1.41, 7.5, 4, seed + 10)
    mid = fbm(u - 3.70, v + 5.30, 23.0, 3, seed + 20)
    ridge = ridged_fbm(u + 0.90, v - 4.20, 31.0, 4, seed + 30)
    fine = fbm(u + 11.0, v - 8.0, 72.0, 2, seed + 40)

    ocean_floor = SEA_LEVEL_NORM - 0.070 - np.clip(water * 0.035, 0.0, 0.045) + (low - 0.5) * 0.010
    base_land = SEA_LEVEL_NORM + 0.040 + land_lum * 0.088 + (low - 0.5) * 0.040 + (mid - 0.5) * 0.022
    height = ocean_floor * (1.0 - land) + base_land * land

    # Montanhas guiadas por neve, textura clara e edge do concept.
    mountain_from_image = np.clip(snow * 1.35 + edge * 0.52 + texture * 0.30, 0.0, 1.0) * land
    # Regiões de montanha visíveis no próprio concept: NW, norte central, leste, sudeste.
    mountain_priors = np.maximum.reduce([
        gauss(u, v, 0.180, 0.165, 0.175, 0.130),
        gauss(u, v, 0.405, 0.250, 0.185, 0.085),
        gauss(u, v, 0.735, 0.450, 0.180, 0.230),
        gauss(u, v, 0.650, 0.710, 0.240, 0.115),
    ]).astype(np.float32)
    mountains = np.clip(mountain_from_image * 0.72 + mountain_priors * snow * 0.70 + mountain_priors * edge * 0.38, 0.0, 1.0)
    height += land * mountains * (0.130 + 0.220 * ridge) * detail_strength

    # Neve = picos mais altos, não praia branca.
    height += land * snow * (0.090 + 0.115 * ridge) * detail_strength

    # Deserto/árido no NE: relevo médio quebrado, menos extremo que montanhas.
    height += land * desert * (0.030 + 0.048 * ridge + 0.020 * texture) * detail_strength

    # Floresta/plains: relevo jogável, menor ruído.
    height += land * forest * (0.018 + 0.020 * mid) * detail_strength

    # Vulcão SE: usa máscara do concept + caldeira geométrica local.
    dx = (u - 0.815) / 0.145
    dy = (v - 0.805) / 0.125
    volcano_r = np.sqrt(dx * dx + dy * dy).astype(np.float32)
    volcano_ring = np.exp(-((volcano_r - 0.68) / 0.190) ** 2).astype(np.float32)
    volcano_core = np.exp(-((volcano_r) / 0.310) ** 2).astype(np.float32)
    volcano_rough = ridged_fbm(u + 9.4, v - 2.1, 35.0, 3, seed + 140)
    volcano_mask = np.clip(volcanic + volcano_ring * 0.90, 0.0, 1.0) * land
    height += volcano_mask * (volcano_ring * 0.235 - volcano_core * 0.095 + volcano_rough * 0.068) * detail_strength

    # Centro corrompido: crater/depressão e ridges radiais, mas acima do mar para gameplay.
    cx, cy = 0.515, 0.510
    r = np.sqrt((u - cx) ** 2 + (v - cy) ** 2).astype(np.float32)
    theta = np.arctan2(v - cy, u - cx).astype(np.float32)
    corrupt_core = np.exp(-((r / 0.065) ** 2)).astype(np.float32)
    corrupt_ring = np.exp(-(((r - 0.128) / 0.050) ** 2)).astype(np.float32)
    corrupt_outer = np.exp(-(((r - 0.210) / 0.095) ** 2)).astype(np.float32)
    radial = ((np.sin(theta * 13.0 + fbm(u, v, 9.0, 2, seed + 150) * 5.0) + 1.0) * 0.5).astype(np.float32)
    corrupted_height = np.clip(corrupted_ref + corrupt_outer * 0.35, 0.0, 1.0) * land
    corrupted_biome = np.clip(corrupted_ref, 0.0, 1.0) * land
    height += corrupted_height * (-corrupt_core * 0.080 + corrupt_ring * 0.085 + radial * corrupt_outer * 0.046) * detail_strength
    # Segurança: núcleo fica rebaixado, mas não vira oceano.
    height = np.where(corrupt_core > 0.38, np.maximum(height, SEA_LEVEL_NORM + 0.030), height)

    # Rios visíveis pela imagem + alguns canais guiados para garantir drenagem macro plausível.
    river_guides = np.zeros_like(u, dtype=np.float32)
    river_defs = [
        ([(0.330, 0.255), (0.405, 0.365), (0.395, 0.510), (0.300, 0.625), (0.185, 0.610)], 0.0065),
        ([(0.555, 0.410), (0.535, 0.540), (0.505, 0.655), (0.470, 0.765)], 0.0062),
        ([(0.720, 0.375), (0.810, 0.305), (0.890, 0.255)], 0.0058),
        ([(0.675, 0.635), (0.725, 0.695), (0.850, 0.765)], 0.0058),
        ([(0.255, 0.755), (0.325, 0.725), (0.365, 0.820)], 0.0055),
    ]
    for pts, width in river_defs:
        river_guides = np.maximum(river_guides, polyline_mask(u, v, pts, width))
    rivers = np.clip(np.maximum(inland_water * 0.80, river_guides * 0.75) * land, 0.0, 1.0)
    height -= rivers * 0.045

    # Pântano: mais baixo e jogável.
    height -= swamp * 0.028

    # Costa: transição suave para praias/baías; evita penhascos constantes no litoral.
    beach_target = SEA_LEVEL_NORM + 0.010 + (mid - 0.5) * 0.006
    height = height * (1.0 - coast * 0.24) + beach_target * coast * 0.24

    # Plataformas jogáveis: vilas, castelos, portos, arenas naturais.
    plateaus = [
        (0.350, 0.595, 0.030, SEA_LEVEL_NORM + 0.078, 0.66),
        (0.485, 0.345, 0.034, SEA_LEVEL_NORM + 0.105, 0.62),
        (0.645, 0.480, 0.032, SEA_LEVEL_NORM + 0.100, 0.60),
        (0.522, 0.505, 0.042, SEA_LEVEL_NORM + 0.058, 0.40),
        (0.232, 0.690, 0.030, SEA_LEVEL_NORM + 0.065, 0.64),
        (0.755, 0.285, 0.034, SEA_LEVEL_NORM + 0.074, 0.58),
        (0.820, 0.690, 0.028, SEA_LEVEL_NORM + 0.095, 0.56),
        (0.200, 0.540, 0.026, SEA_LEVEL_NORM + 0.045, 0.62),
        (0.500, 0.780, 0.032, SEA_LEVEL_NORM + 0.052, 0.66),
        (0.595, 0.255, 0.026, SEA_LEVEL_NORM + 0.085, 0.54),
    ]
    for px, py, pr, target, strength in plateaus:
        height = apply_plateau(height, u, v, px, py, pr, target, strength, land)

    # Micro detalhe final, mascarado para não destruir jogabilidade.
    playable_noise = ((fine - 0.5) * 0.010 + (ridge - 0.5) * 0.012) * land
    height += playable_noise * detail_strength * (1.0 - coast * 0.75) * (1.0 - rivers * 0.85)

    height = np.clip(height, 0.020, 0.985).astype(np.float32)

    if not aux:
        return height, None

    biome = np.full(u.shape, 7, dtype=np.uint8)
    biome[land < 0.35] = 0
    biome[snow > 0.20] = 1
    biome[forest > 0.26] = 2
    biome[desert > 0.22] = 3
    biome[volcano_mask > 0.22] = 4
    biome[swamp > 0.24] = 5
    biome[corrupted_biome > 0.24] = 6
    biome[(land >= 0.35) & (coast > 0.34)] = 8
    biome[land < 0.35] = 0

    aux_data = {
        "land": land.astype(np.float32),
        "water": water.astype(np.float32),
        "coast": coast.astype(np.float32),
        "river": rivers.astype(np.float32),
        "snow": snow.astype(np.float32),
        "forest": forest.astype(np.float32),
        "desert": desert.astype(np.float32),
        "volcanic": volcano_mask.astype(np.float32),
        "swamp": swamp.astype(np.float32),
        "corrupted": corrupted_biome.astype(np.float32),
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
    se = padded[y0 + 1:y0 + 1 + rows, x0 + 1:x0 + cols + 1]
    smoothed = c * 0.52 + (n + s + w + e) * 0.095 + (nw + ne + sw + se) * 0.025
    return (c * (1.0 - strength) + smoothed * strength).astype(np.float32)


def norm_to_u16(height: np.ndarray) -> np.ndarray:
    return np.round(np.clip(height, 0.0, 1.0) * 65535.0).astype("<u2", copy=False)


def tile_name(tx: int, ty: int, ext: str = ".r16") -> str:
    return f"{WORLD_NAME}_Height_X{tx:02d}_Y{ty:02d}{ext}"


def preview_name(tx: int, ty: int) -> str:
    return f"{WORLD_NAME}_Preview_X{tx:02d}_Y{ty:02d}.png"


def generate_tile(cfg: GeneratorConfig, output_dir: Path, tx: int, ty: int, reference: ReferenceMap) -> Path:
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
                detail_strength=cfg.procedural_detail_strength,
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
    Image.fromarray(arr, mode="I;16").save(output_dir / tile_name(tx, ty, ".png"))


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

    for ty in range(cfg.grid):
        for tx in range(cfg.grid - 1):
            a = open_tile(tx, ty, "r")
            b = open_tile(tx + 1, ty, "r+") if fix else open_tile(tx + 1, ty, "r")
            right = np.asarray(a[:, -1], dtype=np.int32)
            left = np.asarray(b[:, 0], dtype=np.int32)
            dmax = int(np.abs(right - left).max())
            max_diff = max(max_diff, dmax)
            if fix and dmax != 0:
                logging.warning("Corrigindo seam vertical X%02d/Y%02d -> X%02d/Y%02d | max diff=%d", tx, ty, tx + 1, ty, dmax)
                b[:, 0] = a[:, -1]
                b.flush()
            comparisons.append({"type": "vertical", "a": [tx, ty], "b": [tx + 1, ty], "max_diff": dmax})
            del a, b

    for ty in range(cfg.grid - 1):
        for tx in range(cfg.grid):
            a = open_tile(tx, ty, "r")
            b = open_tile(tx, ty + 1, "r+") if fix else open_tile(tx, ty + 1, "r")
            bottom = np.asarray(a[-1, :], dtype=np.int32)
            top = np.asarray(b[0, :], dtype=np.int32)
            dmax = int(np.abs(bottom - top).max())
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


def save_u8(path: Path, arr: np.ndarray) -> None:
    Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8), mode="L").save(path, optimize=True)


def export_global_previews_and_masks(output_dir: Path, cfg: GeneratorConfig, reference: ReferenceMap) -> Dict[str, str]:
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
        detail_strength=cfg.procedural_detail_strength,
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
    coast = aux["coast"]
    biome = aux["biome"].astype(np.uint8)

    wear = np.clip(slope_norm * land * 1.35, 0.0, 1.0)
    deposit = np.clip((river * 1.8 + coast * 0.55) * (1.0 - slope_norm), 0.0, 1.0) * land

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
    files["biome_mask"] = str(biome_path)

    for biome_id, biome_name in BIOME_IDS.items():
        if biome_id == 0:
            continue
        path = output_dir / f"{WORLD_NAME}_Mask_Biome_{biome_name}.png"
        save_u8(path, (biome == biome_id).astype(np.float32) * 255.0)
        files[f"biome_{biome_name}"] = str(path)

    return files


def write_metadata_and_report(
    output_dir: Path,
    cfg: GeneratorConfig,
    backup_dir: Optional[Path],
    reference_path: Optional[Path],
    generated_tiles: List[str],
    seam_result: Dict[str, object],
    global_files: Dict[str, str],
    elapsed_seconds: float,
) -> None:
    metadata = {
        "world_name": WORLD_NAME,
        "script_version": SCRIPT_VERSION,
        "generated_at": datetime.now().isoformat(timespec="seconds"),
        "config": asdict(cfg),
        "reference_image": str(reference_path) if reference_path else None,
        "backup_dir": str(backup_dir) if backup_dir else None,
        "tile_count": cfg.grid * cfg.grid,
        "tile_resolution": [cfg.tile_size, cfg.tile_size],
        "global_unique_resolution": [cfg.global_unique_size, cfg.global_unique_size],
        "format": "R16 little-endian unsigned 16-bit grayscale",
        "sea_level_norm": SEA_LEVEL_NORM,
        "sea_level_u16": int(round(SEA_LEVEL_NORM * 65535.0)),
        "expected_r16_bytes_per_tile": cfg.expected_r16_bytes_per_tile,
        "expected_r16_total_bytes": cfg.expected_r16_total_bytes,
        "generated_tiles": [Path(p).name for p in generated_tiles],
        "seam_validation": seam_result,
        "global_files": {k: Path(v).name for k, v in global_files.items()},
        "elapsed_seconds": elapsed_seconds,
    }

    meta_path = output_dir / f"{WORLD_NAME}_Heightmap_Metadata.json"
    with open(meta_path, "w", encoding="utf-8") as f:
        json.dump(metadata, f, indent=2, ensure_ascii=False)

    report_path = output_dir / f"{WORLD_NAME}_Generation_Report.md"
    with open(report_path, "w", encoding="utf-8") as f:
        f.write(f"# {WORLD_NAME} Heightmap Generation Report\n\n")
        f.write(f"Generated: {metadata['generated_at']}\n\n")
        f.write("## Correction applied in v02\n")
        f.write(f"- The generator is now reference-driven from `{DEFAULT_REFERENCE_BASENAME}`.\n")
        f.write("- The previous generic procedural landmass is no longer the source of truth.\n")
        f.write("- Coastline, islands, central corrupted area, snow/desert/forest/volcanic regions are sampled from the concept image.\n\n")
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
        f.write("4. Start with prototype/custom lower resolution before importing final 8x8 8129.\n")
        f.write("5. Keep `.png` previews out of the import folder if UE tile detection becomes confused.\n")
        f.write("\n## Technical risks\n")
        f.write("- 8x8 tiles of 8129 are extremely heavy for disk, RAM, UE import and editor interaction.\n")
        f.write("- This script creates a usable terrain base, not a replacement for final Gaea erosion/art pass.\n")
        f.write("- Satellite concept colors are interpreted heuristically; manual review of masks is required before final import.\n")
        f.write("\n## Gaea next steps\n")
        f.write("1. Rebuild the reference silhouette as Landmass/Base Shape nodes.\n")
        f.write("2. Import the generated masks as guides.\n")
        f.write("3. Add hydraulic erosion, thermal erosion, wear/deposit/flow outputs.\n")
        f.write("4. Use tiled build and validate seams before Unreal import.\n")

    logging.info("Metadata: %s", meta_path)
    logging.info("Relatório: %s", report_path)


def build_arg_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description="SpellRise Elarion tiled heightmap generator v02 - reference-driven")
    p.add_argument("--input-dir", type=Path, default=DEFAULT_INPUT_DIR)
    p.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR)
    p.add_argument("--reference-image", type=Path, default=None, help="Caminho direto para SpellRise_Elarion_Satellite_Concept_v01.png")
    p.add_argument("--reference-fit", choices=["stretch", "contain"], default="stretch")
    p.add_argument("--no-require-reference", action="store_true", help="Não recomendado: permite rodar sem referência. Atualmente o script falha sem referência por segurança.")
    p.add_argument("--reference-size", type=int, default=2048, help="Tamanho máximo da análise da imagem de referência.")
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
    p.add_argument("--smooth-strength", type=float, default=0.32)
    p.add_argument("--procedural-detail-strength", type=float, default=1.0)

    p.add_argument("--export-png-preview", action="store_true", default=True)
    p.add_argument("--no-png-preview", action="store_true")
    p.add_argument("--export-png16", action="store_true", help="Exporta também PNG 16-bit por tile. Muito pesado.")
    p.add_argument("--no-global-masks", action="store_true")
    p.add_argument("--dry-run", action="store_true")
    p.add_argument("--yes", action="store_true", help="Permite backup + limpeza da pasta de saída.")
    return p


def make_config(args: argparse.Namespace) -> GeneratorConfig:
    if args.prototype:
        grid = 2
        tile_size = 257 if args.tile_size is None else args.tile_size
        chunk_rows = args.chunk_rows or 64
        preview_size = args.preview_size or min(257, tile_size)
        global_preview_size = args.global_preview_size or 1024
    elif args.final:
        grid = 8
        tile_size = 8129
        chunk_rows = args.chunk_rows or 96
        preview_size = args.preview_size or 1024
        global_preview_size = args.global_preview_size or 4096
    else:
        if args.grid is None or args.tile_size is None:
            raise SystemExit("--custom exige --grid e --tile-size")
        grid = args.grid
        tile_size = args.tile_size
        chunk_rows = args.chunk_rows or min(128, tile_size)
        preview_size = args.preview_size or min(1024, tile_size)
        global_preview_size = args.global_preview_size or 2048

    if tile_size < 17:
        raise SystemExit("tile-size muito baixo.")
    if grid < 1:
        raise SystemExit("grid inválido.")
    if chunk_rows < 8:
        raise SystemExit("chunk-rows muito baixo.")
    if args.reference_size < 256:
        raise SystemExit("reference-size muito baixo. Use >= 256.")

    return GeneratorConfig(
        input_dir=str(args.input_dir),
        output_dir=str(args.output_dir),
        reference_image=str(args.reference_image) if args.reference_image else None,
        reference_fit=args.reference_fit,
        require_reference=not args.no_require_reference,
        reference_size=int(args.reference_size),
        grid=grid,
        tile_size=tile_size,
        chunk_rows=chunk_rows,
        seed=args.seed,
        sea_level_norm=SEA_LEVEL_NORM,
        export_png_preview=not args.no_png_preview,
        export_png16=args.export_png16,
        export_global_masks=not args.no_global_masks,
        preview_size=preview_size,
        global_preview_size=global_preview_size,
        smooth_strength=max(0.0, min(1.0, args.smooth_strength)),
        procedural_detail_strength=max(0.0, min(2.0, args.procedural_detail_strength)),
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

    logging.info("SpellRise Elarion Heightmap Generator v02 - reference-driven")
    logging.info("Config: %s", json.dumps(asdict(cfg), indent=2, ensure_ascii=False))
    logging.info("Resolução global única: %dx%d", cfg.global_unique_size, cfg.global_unique_size)
    logging.info("Estimativa .r16 total: %.3f GiB", bytes_to_gib(cfg.expected_r16_total_bytes))

    if cfg.final_mode:
        logging.warning("MODO FINAL: 64 tiles 8129x8129. Isso é pesado para disco, importação e edição no UE.")
        if not cfg.allow_clean and not cfg.dry_run:
            raise RuntimeError("Use --yes para confirmar backup + limpeza antes da geração final.")

    explicit_ref = Path(cfg.reference_image) if cfg.reference_image else None
    reference_path = find_reference_image(input_dir, explicit_ref)
    if reference_path is None:
        message = (
            f"Referência obrigatória não encontrada. Coloque `{DEFAULT_REFERENCE_BASENAME}.png` em {input_dir} "
            "ou informe --reference-image \"C:\\...\\SpellRise_Elarion_Satellite_Concept_v01.png\"."
        )
        if cfg.require_reference:
            raise FileNotFoundError(message)
        raise FileNotFoundError(message)

    backup_dir = safe_backup_and_clean_output(output_dir, allow_clean=cfg.allow_clean, dry_run=cfg.dry_run)
    if not cfg.dry_run:
        attach_file_logging(output_dir)

    reference = ReferenceMap(reference_path, max_size=cfg.reference_size, fit=cfg.reference_fit)

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
