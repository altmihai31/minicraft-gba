# Minicraft Expanded Edition - art pack

Original sprites for the RAM-free feature list: 14 blocks, 40 item icons, 10 mobs (5 frames each),
furniture, projectiles and HUD icons. Verified with the project's own build: `make res && make` passes,
RAM unchanged (EWRAM 97.8 %; the IWRAM figure first quoted here, 93.7 %, came from an older compiler –
see CLAUDE.md for current numbers), ROM 129 KB -> 142 KB, no VRAM overlaps.

## Install
1. Copy `res/` over your fork's `res/` (images, palettes and resources.json).
2. Apply `code-changes.patch` (`git apply code-changes.patch`). It only moves VRAM offsets and loads the new sheet:
   - loads `mobs_tileset` at OBJ tile 744
   - player light 320 -> 352, lantern light 336 -> 368 (items now need 256-350)
   - level light 192 -> 224 in BG charblock 1 (item icons now need 128-222)
3. Wire features in code (Claude Code): tables below give every tile number and palette row.

## Golden rule
The converter stores each tile relative to the FIRST palette row containing all its colours.
Always draw with the palette row listed here, or colours come out wrong.

## Blocks (res/images/level.png) - BG palette

| Cell | 8x8 tiles | Palette | Block |
|---:|---|---:|---|
| 22 | 88-91 | 2 | coal ore |
| 23 | 92-95 | 3 | obsidian |
| 24 | 96-99 | 0 | planks |
| 25 | 100-103 | 2 | stone bricks |
| 26 | 104-107 | 10 | clay bricks |
| 27 | 108-111 | 0 | door closed |
| 28 | 112-115 | 0 | door open |
| 29 | 116-119 | 5 | pumpkin |
| 30 | 120-123 | 1 | melon |
| 31 | 124-127 | 0 | sugar cane |
| 32 | 128-131 | 10 | mushrooms |
| 33 | 132-135 | 10 | torch |
| 34 | 136-139 | 7 | spawner |
| 35 | 140-143 | 6 | ice |

## Item icons (res/images/items.png) - items palette

Icons 30-54 are the tool tiers, so new item types can't use `icon = type` any more.
Give new item types (33+) `icon = type + 22`, i.e. item type 33 -> icon 55. HUD/inventory tile = 128 + icon,
world/held sprite tile = 256 + icon. `.palette` must be the row below.

| Icon | Palette | Item |
|---:|---:|---|
| 55 | 0 | planks |
| 56 | 1 | stone bricks |
| 57 | 0 | brick |
| 58 | 1 | clay |
| 59 | 2 | obsidian |
| 60 | 0 | door |
| 61 | 0 | torch |
| 62 | 0 | pumpkin |
| 63 | 1 | melon slice |
| 64 | 1 | sugar cane |
| 65 | 1 | paper |
| 66 | 0 | mushroom |
| 67 | 3 | ice |
| 68 | 0 | raw meat |
| 69 | 0 | cooked meat |
| 70 | 0 | egg |
| 71 | 0 | mushroom stew |
| 72 | 0 | cake |
| 73 | 0 | leather |
| 74 | 1 | wool |
| 75 | 1 | feather |
| 76 | 1 | bone |
| 77 | 1 | string |
| 78 | 1 | spore powder |
| 79 | 3 | bucket |
| 80 | 3 | water bucket |
| 81 | 3 | lava bucket |
| 82 | 1 | shears |
| 83 | 0 | bow |
| 84 | 3 | arrow |
| 85 | 3 | fishing rod |
| 86 | 2 | raw fish |
| 87 | 0 | cooked fish |
| 88 | 0 | bed |
| 89 | 2 | map |
| 90 | 0 | leather armor |
| 91 | 1 | iron armor |
| 92 | 2 | gold armor |
| 93 | 3 | gem armor |
| 94 | 2 | enchanting table |

## Mobs & extras (res/images/mobs.png) - sprite palette

Frame order per mob: front (facing down), back (facing up), side (facing right), walk A, walk B -
the same order the zombie code uses (left = side frames flipped; up/down walking = front/back flipped).
Sprite tile = 744 + cell * 4. New sprite palette rows 8-11 keep the hurt-flash layout of row 5, so
`palette = 5` while hurt still flashes white.

| Cells | First tile | Palette | Sprite |
|---|---:|---:|---|
| 0-4 | 744 | 8 | cow |
| 5-9 | 764 | 8 | pig |
| 10-14 | 784 | 8 | chicken |
| 15-19 | 804 | 9 | sheep |
| 20-24 | 824 | 9 | sheep (sheared) |
| 25-29 | 844 | 9 | wolf |
| 30-34 | 864 | 8 | skeleton |
| 35-39 | 884 | 10 | spider |
| 40-44 | 904 | 10 | bloater |
| 45-49 | 924 | 11 | magma golem |
| 50 | 944 | 9 | arrow (horizontal) |
| 51 | 948 | 9 | arrow (vertical) |
| 52 | 952 | 11 | ember orb (XP) |
| 53 | 956 | 6 | bed |
| 54 | 960 | 6 | enchanting table |

Notes:
- Bed and enchanting table use sprite row 6 like all furniture (red added to row 6 for the blanket).
  Furniture tiles are computed as `148 + 4 * index` today; new furniture needs its tile from this sheet.
- Sheared sheep is a separate set of frames: swap frame base when the sheep is sheared.
- Arrows: horizontal points right (flip for left), vertical points up (flip vertically for down).

## HUD icons (res/images/gui.png) - BG palette 10

| Tile | Icon |
|---:|---|
| 104 | hunger full |
| 105 | hunger empty |
| 106 | armor full |
| 107 | armor empty |

Same pattern as hearts/stamina: `BG3_TILEMAP[...] = (104 + (hunger <= i)) | 10 << 12;`

## Palette additions (free slots only - every existing sprite looks the same)
- background.png rows 1, 5, 7, 10 (blocks + HUD greys)
- items.png rows 0, 1, 3
- sprites.png: row 6 + red, new rows 8-11

## Source
`source/` regenerates everything: `blocks.py` (blocks), `art_items.py` + `art_mobs.py` (pixel grids),
`build_art.py` (assembles sheets, adds colours, verifies palette rows), `preview_art.py`.
Edit a grid string, rerun, and the palette check tells you if a colour breaks a tile.

All sprites are original pixel art made for this fork.
