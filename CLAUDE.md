# Minicraft Expanded Edition (Minicraft GBA mod fork)

Fork of Vulcalien's **Minicraft for GBA** (github.com/Vulcalien/minicraft-gba, GPLv3, v2.0 + later commits).
Goal: add Minecraft-style content (blocks, items, mobs, player systems) **without** increasing RAM use.
Original art and sounds are Markus Persson's (2011, no explicit licence) – all new art in this fork is original.

## Build & run
```sh
git submodule update --init          # libsimplegba + res2gba
make -C lib/libsimplegba
make res && make                     # res2gba converts res/ PNGs -> src/res/*.c
mgba-qt bin/minicraft-expanded-edition.gba            # or: make run
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
"/c/Program Files/mGBA/mGBA.exe" bin/minicraft-expanded-edition.gba
```
- `OS=Linux` overrides the `Windows_NT` env var so the Makefile uses arm-none-eabi-* and `mkdir -p`.
- `make res` fails silently on Windows: res2gba runs its extensionless converter scripts through
  `os.system` (cmd.exe). `tools/res2gba-win.py` re-routes those calls through the Python interpreter.
- Repo is checked out with CRLF (`core.autocrlf=true`), so shebang scripts can't run directly; call them
  with `python`. `LINK_MAP=1` writes `bin/output.map`.
- mGBA locks the ROM it has open: close it before building, or `objcopy` can't write `bin/minicraft-expanded-edition.gba`
  (the link still succeeds, so the old ROM silently stays). mGBA keys: X = A, Z = B, Enter = Start.

### Smoke test
`make test` (Windows: `make test OS=Linux PYTHON=py`) runs `tools/test/smoke.py`: it plays the built normal ROM
in mGBA through the GDB stub (minimized window, no keyboard input) and checks block placement and its messages,
the enchanting table, night lighting, bed sleep + autosave, split-stack crafting, and an 89 KB save written and
loaded back. Addresses come from the ELF, so it survives rebuilds. Needs `arm-none-eabi-gdb`; set `MGBA` if mGBA
isn't in `C:\Program Files\mGBA`. Nothing else may hold GDB port 2345. `--keep` keeps the work folder and logs.
`tools/test/screenshots.py` uses the same harness with the debug ROM to retake `docs/screenshots/` (it
composes frames from VRAM/OAM/IO dumps). Human-facing build docs: BUILDING.md.

### CI and releases
`.github/workflows/build.yml` builds both ROMs with Arm GNU 14.2.Rel1 on every push and checks the headers
(`tools/check-header.py`). A pushed tag `ee-vX.Y` creates the GitHub release (ROMs + newest CHANGELOG.md
section). Version: "EE V1.0" in `src/scene/start.c`, Software Version byte in `src/header.s` (0 = 1.0; the
header checksum must change with it). Checklist: `doc/release-checklist.md`.

### Debug version
`make build OS=Linux DEBUG_KIT=1` -> `bin/minicraft-expanded-edition-debug.gba` ("DEBUG BUILD" on the title
screen). It uses its own object dir (`obj-debug/`) and save file, so it never mixes with the normal build,
which contains none of this code. `src/debug-kit.c`, active only with `-DDEBUG_KIT`:
- Performance overlay (L+R, Select) also shows "S" + hex: stack bytes never used since boot.
- New game starts with gem tools, 99 of every mod item, furnace/oven/anvil/lantern, crafting materials, 300 XP.
- Next to the spawn: a pond (fishing, buckets), sheep, a wolf and other animals, and stairs down. The stairs
  chain straight down (3 tiles apart) through lit test rooms: level 2 has a skeleton, spider and bloater,
  level 1 a spawner, level 0 a lava pool (water bucket -> obsidian) and the magma golem at the far end.

## Hard constraints (measured from the ELF, GCC 14.2.Rel1, 26.10.2026, all content implemented)
- **EWRAM 97.8 % full** (256 480 B used, ~5 660 B free): 5 levels 143 290 B, collision table
  `level_solid_entities` 100 352 B, 32 chests 12 352 B. Do NOT grow world size, level count, chest count or entity cap.
- **IWRAM 92.9 %** (30 436 B, 30.10.2026). Data ends at `0x030076E4`, user stack starts at `0x03007F00` (top
  256 B reserved for IRQ stacks) -> **~2 070 B of stack**, no overflow check (the debug build shows the
  stack high-water mark). Upstream HEAD was 97.9 % / 436 B:
  the fork freed IWRAM by moving `player_attack`/`player_use` to ROM and shrinking `struct Tile` to 8 B
  (`interact` pointers now live in the ROM table `tile_interact_list`).
- Any plain global/static goes to IWRAM `.bss`: put new ones in EWRAM with `SBSS_SECTION`.
- New code goes in ROM. `ETICK`/`EDRAW` put entity code in IWRAM – mod entities use `ETICK_ROM`/`EDRAW_ROM`.
- `level_tick`/`level_draw` are `IWRAM_SECTION` and `tick/tiles.c` + `draw/tiles.c` are `#include`d into
  level.c as `static inline`, so every tile `case` there lands in IWRAM. Mod tiles go through the `default:`
  branch into `src/mod-tiles.c` (ROM). Largest IWRAM users: `level_draw` 10.8 KB, `game_draw` 1.6 KB.
- ROM: 177 632 B (upstream 133 456 B).
- Tile = 1 B type + 1 B data. Data byte holds damage/growth/etc., so a placed block can't remember what was
  under it -> broken placed blocks become dirt (plants -> grass). Exception: torches are three tile types by
  ground (`TORCH_TILE`, `TORCH_GRASS_TILE` 36, `TORCH_SAND_TILE` 37; `torch_tile_on`/`is_torch` in tile.h),
  because neighbour rules (`connects_to`) are per tile type. Mod tile IDs 36+ don't map to cells 36+: torches
  on grass/sand draw border pieces from cells 51-58 (`torch_edge_draw`).
- Entity state: 8 bytes (`data[8]`); mobs get 2 bytes after `struct mob_Data`. Limit 255 entities per level
  shared by mobs, items, particles. Item types go up to 72: bitfields holding one need 7 bits.
- **Saves:** 128 KB Flash (`BACKUP_FLASH_128KB`), save version byte at 0x1C (`SAVE_VERSION` in storage.c, now 2
  – bump it whenever the layout changes; other versions show as no save). The backup driver takes 16-bit
  offsets, so storage.c switches the two 64 KB banks itself (`flash_read_at`/`flash_write_at`). The game
  code is written last; a save that doesn't fit is left invalid and the pause menu says "SAVE FAILED".
  A fresh world uses ~20 KB. mGBA starts every save as 64 KB Flash and switches to 128 KB on the first bank-1
  access (loading a save, or saving past 64 KB), so a 64 KB `.sav` file is normal. Tested 28.10.2026: an
  89 KB save writes, verifies and loads back byte-exact; a 20 KB save loads from a 64 KB file.
- Liquid tile is water everywhere except the deepest level (index 0), where it is lava.
- `options.keep_inventory` defaults to true: death keeps inventory, armor and XP.

## Palette rule (most common mistake)
res2gba stores each tile's colours relative to the **first** palette row that contains all of its colours.
Code must draw with that same row. Every new sprite's row is listed in `docs/art-pack.md`.
Note: dirt reuses the grass tiles drawn with palette 8; all three ores share cell 13 with palette 3.

## Art already done (installed in res/, see docs/art-pack.md + docs/previews/)
- 14 blocks in `res/images/level.png` cells 22–35 (coal ore, obsidian, planks, stone bricks, clay bricks,
  door closed/open, pumpkin, melon, sugar cane, mushrooms, torch, spawner, ice). Cells 36–50: connected
  (rock-style) tiles for placed planks, stone bricks and clay bricks, drawn by `connected_draw` in mod-tiles.c.
- 40 item icons in `res/images/items.png` icons 55–94. Icons 30–54 are tool tiers, so new item types
  (33+) use **icon = type + 22**. HUD tile = 128 + icon, sprite tile = 256 + icon.
- `res/images/mobs.png` (new tileset, OBJ tile 744 + cell*4): cow, pig, chicken, sheep, sheared sheep, wolf,
  skeleton, spider, bloater (exploding fungus), magma golem (boss) – 5 frames each in zombie order
  (front, back, side-right, walk A, walk B) – plus arrows, ember orb (XP), bed, enchanting table.
  New sprite palette rows 8–11 keep row 5's hurt-flash layout. Furniture uses sprite row 6.
- HUD: hunger full/empty, armor full/empty = BG tiles 104–107, palette 10.
- VRAM: mobs tileset at OBJ tile 744; player light 352, lantern 368, level light 224 (room for the extra
  icons). The original art patch is applied and removed from the tree.
- Edit art: `tools/art-source/` grids, then `python tools/art-source/update_art.py` (in place, checks palette rows;
  the one-off build_art.py is gone). Style and rules: "Source" section of docs/art-pack.md. Refresh docs/previews with
  `python tools/art-source/make_previews.py`.
- BG palette 10 indices 5/6 are rewritten every frame (armor pip colour = worn armor tier); index 12 blinks
  stamina. Don't give other palette-10 tiles those colours.

## Design spec
See `docs/game-design.md` for break rules, drops, item behaviour, food, armor and recipes. Everything in it is
implemented; its "Implementation notes" section lists where the code differs from the spec.

## Where the mod code lives
- `src/tile.c`: `block_breaks` table + `block_interact` (the one break rule for mod blocks), doors.
- `src/mod-tiles.c`: drawing, ticking (sugar cane, spawners) and torch light of mod tiles.
- `src/entity/mod-mobs.c`: all 9 mobs – one `mod_mobs` table, shared tick/draw/touch, drops, spawn choice.
- `src/entity/projectiles.c`: arrows and XP orbs. Bed/enchanting table: `src/entity/furniture.c`.
- `src/item-use.c`: bucket, shears, bow, fishing rod, map, armor, bone, bed, enchanting table.
- `src/survival.c`: hunger, armor, XP, bed spawn point, ice sliding, fishing, HUD, status-bar messages
  (`survival_show_message`), day/night (`day_time`, saved; at night the surface uses the light layer BG2,
  alpha-blended, so player/lantern/torch light works; `light_layer_on` replaces the old `level < &levels[3]`).
- `src/scene/map.c`: map item. World generation of mod content: end of `src/generator.c`.

## Owner preferences
Technically precise, no-preamble answers; direct pushback when something is a bad idea; metric units;
dates DD.MM.YYYY.
