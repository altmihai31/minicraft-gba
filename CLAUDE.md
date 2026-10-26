# Minicraft GBA – mod fork

Fork of Vulcalien's **Minicraft for GBA** (github.com/Vulcalien/minicraft-gba, GPLv3, v2.0 + later commits).
Goal: add Minecraft-style content (blocks, items, mobs, player systems) **without** increasing RAM use.
Original art and sounds are Markus Persson's (2011, no explicit licence) – all new art in this fork is original.

## Build & run
```sh
git submodule update --init          # libsimplegba + res2gba
make -C lib/libsimplegba
make res && make                     # res2gba converts res/ PNGs -> src/res/*.c
mgba-qt bin/minicraft.gba            # or: make run
```
Toolchain: `arm-none-eabi-gcc` (macOS: `brew install --cask gcc-arm-embedded`), Python 3 + Pillow, mGBA.
Performance overlay in game: hold L+R, press Select.

### Windows (native, from Git Bash)
Installed via winget: `Arm.GnuArmEmbeddedToolchain` (14.2.Rel1), `Python.Python.3.12` + `pip install pillow`,
`ezwinports.make`, `JeffreyPfau.mGBA`. New PATH entries only reach new terminals.
```sh
make -C lib/libsimplegba AR=arm-none-eabi-ar                       # host `ar` doesn't exist
mkdir -p src/res/images src/res/palettes src/res/sounds
python tools/res2gba-win.py tools/res2gba/res2gba res/resources.json   # replaces `make res`
make build OS=Linux                                                # Makefile's Windows branch is a stub
"/c/Program Files/mGBA/mGBA.exe" bin/minicraft.gba
```
- `OS=Linux` overrides the `Windows_NT` env var so the Makefile uses arm-none-eabi-* and `mkdir -p`.
- `make res` fails silently on Windows: res2gba runs its extensionless converter scripts through
  `os.system` (cmd.exe). `tools/res2gba-win.py` re-routes those calls through the Python interpreter.
- Repo is checked out with CRLF (`core.autocrlf=true`), so shebang scripts can't run directly; call them
  with `python`. `LINK_MAP=1` writes `bin/output.map`.

## Hard constraints (measured from the ELF, GCC 14.2.Rel1, 26.10.2026)
- **EWRAM 97.8 % full** (256 384 B used, 5 760 B free): 5 levels 143 290 B, collision table
  `level_solid_entities` 100 352 B, 32 chests 12 352 B. Do NOT grow world size, level count, chest count or entity cap.
- **IWRAM 97.89 %** (32 076 B: `.iwram` 31 340 B code + `.bss` 732 B; identical on pristine upstream HEAD –
  the old 93.7 % figure came from an older compiler). Data ends at `0x03007D4C`, user stack starts at
  `0x03007F00` (top 256 B reserved for IRQ stacks) -> **only ~436 B of stack**, with no overflow check:
  a deep call chain silently corrupts the end of `.bss`. Any IWRAM growth comes straight out of the stack.
- New code goes in ROM. `ETICK`/`EDRAW` macros put entity code in IWRAM by default – new entities must opt out.
- `level_tick`/`level_draw` are `IWRAM_SECTION` and `tick/tiles.c` + `draw/tiles.c` are `#include`d into
  level.c as `static inline`, so every new tile `case` lands in IWRAM. Route new tiles through one
  `NO_INLINE` ROM dispatcher (e.g. the switch's `default:`). Largest IWRAM users: `level_draw` 10.8 KB,
  `player_tick` 3.8 KB, `game_draw` 1.6 KB.
- ROM: 146 280 B with the art pack (upstream 133 456 B).
- Tile = 1 B type + 1 B data. Data byte holds damage/growth/etc., so a placed block can't remember what was
  under it -> broken placed blocks become dirt (plants -> grass).
- Entity state: 8 bytes (`data[8]`). Limit 255 entities per level shared by mobs, items, particles.
- **Saves:** RLE into 64 KB Flash and `storage.c` silently truncates on overflow. First change of the fork:
  switch to `BACKUP_FLASH_128KB` + `STORAGE_SIZE` 128 KB and add a save-version byte (breaks old saves).
- Liquid tile is water everywhere except the deepest level (index 0), where it is lava.

## Palette rule (most common mistake)
res2gba stores each tile's colours relative to the **first** palette row that contains all of its colours.
Code must draw with that same row. Every new sprite's row is listed in `docs/art-pack.md`.
Note: dirt reuses the grass tiles drawn with palette 8; all three ores share cell 13 with palette 3.

## Art already done (installed in res/, see docs/art-pack.md + docs/previews/)
- 14 blocks in `res/images/level.png` cells 22–35 (coal ore, obsidian, planks, stone bricks, clay bricks,
  door closed/open, pumpkin, melon, sugar cane, mushrooms, torch, spawner, ice).
- 40 item icons in `res/images/items.png` icons 55–94. Icons 30–54 are tool tiers, so new item types
  (33+) use **icon = type + 22**. HUD tile = 128 + icon, sprite tile = 256 + icon.
- `res/images/mobs.png` (new tileset, OBJ tile 744 + cell*4): cow, pig, chicken, sheep, sheared sheep, wolf,
  skeleton, spider, bloater (exploding fungus), magma golem (boss) – 5 frames each in zombie order
  (front, back, side-right, walk A, walk B) – plus arrows, ember orb (XP), bed, enchanting table.
  New sprite palette rows 8–11 keep row 5's hurt-flash layout. Furniture uses sprite row 6.
- HUD: hunger full/empty, armor full/empty = BG tiles 104–107, palette 10.
- `code-changes.patch`: loads mobs tileset at 744; player light 320->352, lantern 336->368,
  level light 192->224 (VRAM room for the extra icons). **Already applied** – do not re-apply. The patch
  missed `clear_light()` in `src/level.c` (base dark tile 192 -> 224); that is fixed in the tree.
- Regenerate/edit art: `tools/art-source/` (pixel grids as strings; build_art.py verifies palette rows).

## Design spec
See `docs/game-design.md` for break rules, drops, item behaviour, food, armor and recipes.
Implement breaking as ONE data table + one generic interact function, not one function per block.

## Suggested order
1. 128 KB save + version byte
2. Coal ore, planks, door, torch (blocks + break table)
3. Passive animals + drops + cooked food
4. Skeleton + bow/arrows
5. Hunger / armor HUD, then the rest of the list in docs/game-design.md

## Owner preferences
Technically precise, no-preamble answers; direct pushback when something is a bad idea; metric units;
dates DD.MM.YYYY.
