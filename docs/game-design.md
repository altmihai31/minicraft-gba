# Game design spec – new content

## Existing breaking model (src/tile.c)
- Damage accumulates in the tile data byte; block breaks at its HP (max 255).
- Right tool: `10 + 5*tier + random(10)` per hit, costs `4 - tier` stamina. Wrong tool / hand: `1 + random(3)`.
- Tiers: 0 wood, 1 rock, 2 iron, 3 gold, 4 gem.
- Existing: cactus 10 HP (hand), tree 20 (axe), rock 50 (pick; stone + 50 % coal), hard rock 200 (gem pick only),
  ores pick-only, break after 3–12 hits, drop per hit +2 on break. Shovel/hoe act instantly.

## New blocks
| Block | Breaks with | Min tier | HP | Drops | Leaves | Source |
|---|---|---|---|---|---|---|
| Coal ore | Pick | Wood | ore-style | coal/hit, +2 | dirt | surface rock + underground 1 (lower rock coal chance to ~25 %) |
| Obsidian | Pick | Gem | 200 | obsidian | dirt | water bucket poured on lava (deepest level) |
| Planks | Axe fast, any slow | – | 20 | planks | dirt | placed |
| Stone bricks | Pick | Wood | 50 | stone bricks | dirt | placed |
| Clay bricks | Pick | Wood | 40 | brick | dirt | placed |
| Door | Axe/pick | – | 20 | door | dirt | placed; any other item/hand toggles open/closed; zombies can't open |
| Pumpkin | Axe/hand | – | 10 | pumpkin | grass | surface patches |
| Melon | Axe/hand | – | 10 | 3–5 melon slices | grass | surface patches |
| Sugar cane | Any | – | 1 | 1–2 sugar cane | grass | next to water, grows |
| Mushrooms | Any | – | 1 | 1–2 mushrooms | dirt | underground 1–2 dirt |
| Torch | Any | – | 1 | torch | dirt | placed on grass/dirt/sand |
| Spawner | Pick | Iron | 150 | XP orbs | dirt | dungeons underground 2–3, spawns near player |
| Ice | Pick | Wood | 20 | – | water | frozen ponds, slippery |

## Items
- Bucket: empty on liquid -> water (lava on deepest level). Water can't be emptied on deepest level;
  water on lava -> obsidian. Lava only emptied on deepest level (or add a separate lava tile – no RAM cost).
- Shears on sheep: 1–3 wool, sheep -> sheared frames, regrows.
- Bow: needs + consumes arrows. Fishing rod: facing water, raw fish after random delay, ~30 % nothing.
- Clay: shovel on sand next to water, 25 %. Furnace: clay -> brick.
- Food HP (max 10; apple 1, bread 2): raw meat/raw fish/melon slice 1, cooked fish 2,
  cooked meat/stew 3, cake 4. With hunger: food fills hunger, HP regenerates while hunger > 0.
- Armor (1 slot, 1 byte): chance to fully block a hit – leather 20 %, iron 35 %, gold 50 %, gem 65 %.

## Recipes
| Station | Recipes |
|---|---|
| Workbench | 1 wood -> 2 planks · 6 planks -> door · 1 wood + 1 coal -> 4 torches · 3 sugar cane -> 3 paper · 4 stone -> stone bricks · 5 wood + 3 string -> bow · 1 wood + 1 stone + 1 feather -> 4 arrows · 3 wood + 2 string -> fishing rod · 5 wood + 3 wool -> bed · 8 paper -> map · 8 leather -> leather armor |
| Furnace | clay + coal -> brick |
| Oven | raw meat + coal -> cooked meat · raw fish + coal -> cooked fish · 2 mushrooms + 1 wood -> stew · 3 wheat + 1 egg + 1 sugar cane -> cake |
| Anvil | 3 iron -> bucket · 2 iron -> shears · 8 iron / 8 gold / 15 gem -> armor · 4 obsidian + 2 gem + 6 paper -> enchanting table |

## Mob drops (suggested)
cow: raw meat 1–2, leather 0–2 · pig: raw meat 1–3 · chicken: feather 0–2, raw meat 1, lays eggs ·
sheep: raw meat 1, wool 1 (unsheared) · wolf: tameable with bone · skeleton: bone 0–2, arrows 0–2 ·
spider: string 0–2 · bloater: spore powder 0–2, explodes (destroys nearby non-hard tiles) ·
magma golem (deepest-level boss): gem 5+, obsidian.

## Out of scope (needs RAM rework first)
Nether/End levels, bigger world, many more chests, signs/books with text. Path if ever needed: replace the
100 KB `level_solid_entities` table with a chunked/linked structure (~85 KB freed).
