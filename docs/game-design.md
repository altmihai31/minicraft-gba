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

## Implementation notes (26.10.2026, updated 28.10.2026)
Everything above is implemented. Where the spec was open or the code differs:
- **Break rule:** "Pick, min tier Wood" works like rock: hand/other tools still punch (1–3 dmg). Blocks with a
  higher min tier (obsidian = gem, spawner = iron) take no damage from anything else, like hard rock.
- **Ice** drops an ice item (spec: nothing) so the icon has a use: ice can be placed on water as a bridge
  (not on lava). Standing on ice keeps the player sliding ~16 px after letting go.
- **Doors:** hand and tools other than axe/pick open/close a door; so do materials, placeables, furniture
  and the power glove. Food, armor, bow, map etc. keep their own action. A door can't close on a mob.
- **Placeables:** planks, stone bricks, brick, door and torch on grass/dirt/sand/flower/farmland/wheat (a flower
  or crop underneath drops as if harvested); pumpkin on grass; sugar cane on grass/sand; mushroom on dirt.
  Placed planks, stone bricks and clay bricks join their own kind like rock (raised look, dark south face);
  doors count as part of the wall. A solid block can't be placed on a mob, furniture or a dropped item
  (BLOCKED); the wrong ground says CANT PLACE.
- **Status messages** (2 s, in place of the XP counter): placement, enchanting table, SAVED / NOT SAVED (bed),
  FULL / CANT POUR (buckets), FISHING... / FISH! / GOT AWAY.
- **Furniture reach:** B reaches the centre of the tile in front, so furniture right next to the player works.
- **Crafting:** a resource split over two inventory slots counts in full (upstream only checked the first
  slot) and is taken from both. Recipes making several items show the count ("PLANKS X2").
- **Recipe counts** (spec gave none): 1 wood -> 2 planks, 4 stone -> 4 stone bricks, 1 clay + 1 coal -> 1 brick.
- **Coal** from plain rock dropped from 50 % to 25 % now that coal ore exists.
- **Hunger:** 10 points, -1 every 40 s. While hunger > 0, +1 HP every 10 s. No starvation damage. Food heals
  its HP value and fills 2 hunger per HP; it can be eaten at full HP when hungry.
- **Armor:** equip with A (swaps with the worn piece). HUD shows 2/4/5/7 points for leather/iron/gold/gem, the
  pips coloured like the worn armor (brown, grey, yellow, pink).
- **XP:** orbs from spawners (5–10), hostile mobs (1–2), the golem (20) and mined ore (coal/iron 1, gold 2,
  gem 3); 1 XP each.
- **Enchanting table:** B with a tool in hand. Level I costs 30 XP: half stamina, swords/axes +50 %. Level II
  costs 60 more: a quarter of the stamina, +100 %. Enchanted tools are written in yellow with '+' (I) or '*'
  (II) between tier and name. (Level in the high byte of `item_Data.count`, so it's saved.)
- **Bed:** B on it heals fully, sets the respawn point (any level) to where the player stands, skips to the
  morning if it is dusk or night, and saves the game.
- **Day and night:** 12-minute cycle (day 8 min, dusk 30 s, night 3 min, dawn 30 s), saved. Only the surface
  gets dark: the light layer is blended over the level at up to 10/16, so the player, lanterns and torches
  light it. Surface spawns at night are monsters only (skeleton, spider, zombie).
- **Map:** A with the map shows the current level, one pixel per tile, player blinking.
- **Fishing:** A facing water casts; after 1–5 s, 70 % chance of a raw fish. Switching item cancels it.
- **Buckets:** water can be poured on hole/dirt/grass/sand (not on the deepest level). Lava only on the
  deepest level, onto hole/dirt. Filling a bucket leaves a hole (which refills from neighbouring liquid).
- **Mobs:** passive animals wander and flee when hit; chickens lay eggs (~1 per 40 s), sheep regrow wool
  (~1 min). Wolves: neutral, angry for 4 s when hit; a bone has a 1/3 chance to tame (always consumed);
  tamed wolves follow the player and bite hostiles. Skeletons keep distance, shoot every ~2 s within 5 tiles
  (1 dmg, 2 on the two deepest levels). Spiders chase within 4 tiles at full speed. Bloaters walk up and
  explode after ~0.75 s near the player (5 + depth dmg, 3x3 soft tiles -> dirt). Golem: 600 HP, spark ring
  every 2.5 s, generated once on the deepest level, never despawns.
- **Spawning:** surface spawns are 50 % animals (wolf 1 in 15), 20 % skeleton/spider, 30 % zombie/slime;
  underground 3/8 skeleton/spider/bloater, the rest zombie/slime; the sky is unchanged. 30 animals at world creation. Hostile stats scale with depth.
- **Generation:** coal ore clusters (surface rock + first underground), 8 pumpkin + 8 melon patches, sugar cane
  along surface water, mushrooms on dirt in underground 1–2, 4 frozen ponds, 3 dungeons (7x7 stone-brick room,
  spawner in the middle) on each of the two deepest levels.

## Out of scope (needs RAM rework first)
Nether/End levels, bigger world, many more chests, signs/books with text. Path if ever needed: replace the
100 KB `level_solid_entities` table with a chunked/linked structure (~85 KB freed).
