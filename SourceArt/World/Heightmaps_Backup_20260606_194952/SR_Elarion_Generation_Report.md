# SR_Elarion Heightmap Generation Report

Generated: 2026-06-06T19:46:19

## Correction applied in v02
- The generator is now reference-driven from `SpellRise_Elarion_Satellite_Concept_v01`.
- The previous generic procedural landmass is no longer the source of truth.
- Coastline, islands, central corrupted area, snow/desert/forest/volcanic regions are sampled from the concept image.

## Output
- Grid: 2x2
- Tile resolution: 257x257
- Unique global vertex resolution with shared borders: 513x513
- R16 per tile: 0.000 GiB
- R16 total: 0.000 GiB
- Sea level normalized/U16: 0.335 / 21954
- Reference image: C:\Users\biM\Documents\Unreal Projects\SpellRise\Fontes\MapDesign\SpellRise_Elarion_Satellite_Concept_v01.png
- Backup: C:\Users\biM\Documents\Unreal Projects\SpellRise\SourceArt\World\Heightmaps_Backup_20260606_194614

## Seam validation
- First pass max diff: 0
- Final max diff: 0
- Expected final diff: 0

## Generated tile files
- `SR_Elarion_Height_X00_Y00.r16`
- `SR_Elarion_Height_X01_Y00.r16`
- `SR_Elarion_Height_X00_Y01.r16`
- `SR_Elarion_Height_X01_Y01.r16`

## Global files
- global_preview: `SR_Elarion_GlobalPreview.png`
- mask_LandOcean: `SR_Elarion_Mask_LandOcean.png`
- mask_Slope: `SR_Elarion_Mask_Slope.png`
- mask_Flow: `SR_Elarion_Mask_Flow.png`
- mask_ErosionWear: `SR_Elarion_Mask_ErosionWear.png`
- mask_Deposit: `SR_Elarion_Mask_Deposit.png`
- biome_mask: `SR_Elarion_BiomeMask.png`
- biome_Snow: `SR_Elarion_Mask_Biome_Snow.png`
- biome_Forest: `SR_Elarion_Mask_Biome_Forest.png`
- biome_Desert: `SR_Elarion_Mask_Biome_Desert.png`
- biome_Volcanic: `SR_Elarion_Mask_Biome_Volcanic.png`
- biome_Swamp: `SR_Elarion_Mask_Biome_Swamp.png`
- biome_Corrupted: `SR_Elarion_Mask_Biome_Corrupted.png`
- biome_Plains: `SR_Elarion_Mask_Biome_Plains.png`
- biome_Coast: `SR_Elarion_Mask_Biome_Coast.png`

## Unreal Engine 5 import notes
1. Use Landscape Mode > Import from File.
2. Select one `.r16` tile. UE should detect the tiled path.
3. If tiles appear inverted on Y, use Flip Y Axis.
4. Start with prototype/custom lower resolution before importing final 8x8 8129.
5. Keep `.png` previews out of the import folder if UE tile detection becomes confused.

## Technical risks
- 8x8 tiles of 8129 are extremely heavy for disk, RAM, UE import and editor interaction.
- This script creates a usable terrain base, not a replacement for final Gaea erosion/art pass.
- Satellite concept colors are interpreted heuristically; manual review of masks is required before final import.

## Gaea next steps
1. Rebuild the reference silhouette as Landmass/Base Shape nodes.
2. Import the generated masks as guides.
3. Add hydraulic erosion, thermal erosion, wear/deposit/flow outputs.
4. Use tiled build and validate seams before Unreal import.
